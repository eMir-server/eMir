/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Guest <thomas.guest@canonical.com>
 */

#include "protobuf_socket_communicator.h"
#include "mir/frontend/application_proxy.h"
#include "mir/frontend/protobuf_ipc_factory.h"
#include "mir/frontend/resource_cache.h"
#include "mir/thread/all.h"

#include "mir_protobuf.pb.h"
#include "mir_protobuf_wire.pb.h"

#include "ancillary.h"

#include <google/protobuf/descriptor.h>

#include <map>
#include <exception>

namespace
{
// Too clever? The idea is to ensure protbuf version is verified once (on
// the first google_protobuf_guard() call) and memory is released on exit.
struct google_protobuf_guard_t
{
    google_protobuf_guard_t() { GOOGLE_PROTOBUF_VERIFY_VERSION; }
    ~google_protobuf_guard_t() { google::protobuf::ShutdownProtobufLibrary(); }
};

void google_protobuf_guard()
{
    static google_protobuf_guard_t guard;
}
bool force_init{(google_protobuf_guard(), true)};
}


namespace mf = mir::frontend;
namespace mfd = mir::frontend::detail;
namespace ba = boost::asio;
namespace bs = boost::system;

struct mfd::AsioSession
{
    AsioSession(
        boost::asio::io_service& io_service,
        int id_,
        ConnectedSessions<AsioSession>* connected_sessions,
        std::shared_ptr<protobuf::DisplayServer> const& display_server,
        std::shared_ptr<ResourceCache> const& resource_cache);

    int id() const
    {
        return id_;
    }

    void read_next_message();
    void on_response_sent(boost::system::error_code const& error, std::size_t);
    void send_response(::google::protobuf::uint32 id, google::protobuf::Message* response);
    void on_new_message(const boost::system::error_code& ec);
    void on_read_size(const boost::system::error_code& ec);

    template<class ResultMessage>
    void send_response(::google::protobuf::uint32 id, ResultMessage* response)
    {
        send_response(id, static_cast<google::protobuf::Message*>(response));
    }

    // TODO detecting the message type to see if we send FDs seems a bit of a frig.
    // OTOH until we have a real requirement it is hard to see how best to generalise.
    void send_response(::google::protobuf::uint32 id, mir::protobuf::Buffer* response)
    {
        const auto& fd = extract_fds_from(response);
        send_response(id, static_cast<google::protobuf::Message*>(response));
        send_fds(fd);
        resource_cache->free_resource(response);
    }

    // TODO detecting the message type to see if we send FDs seems a bit of a frig.
    // OTOH until we have a real requirement it is hard to see how best to generalise.
    void send_response(::google::protobuf::uint32 id, mir::protobuf::Platform* response)
    {
        const auto& fd = extract_fds_from(response);
        send_response(id, static_cast<google::protobuf::Message*>(response));
        send_fds(fd);
        resource_cache->free_resource(response);
    }

    // TODO detecting the message type to see if we send FDs seems a bit of a frig.
    // OTOH until we have a real requirement it is hard to see how best to generalise.
    void send_response(::google::protobuf::uint32 id, mir::protobuf::Connection* response)
    {
        const auto& fd = response->has_platform() ?
            extract_fds_from(response->mutable_platform()) :
            std::vector<int32_t>();

        send_response(id, static_cast<google::protobuf::Message*>(response));
        send_fds(fd);
        resource_cache->free_resource(response);
    }

    // TODO detecting the message type to see if we send FDs seems a bit of a frig.
    // OTOH until we have a real requirement it is hard to see how best to generalise.
    void send_response(::google::protobuf::uint32 id, mir::protobuf::Surface* response)
    {
        const auto& fd = response->has_buffer() ?
            extract_fds_from(response->mutable_buffer()) :
            std::vector<int32_t>();

        send_response(id, static_cast<google::protobuf::Message*>(response));
        send_fds(fd);
        resource_cache->free_resource(response);
    }

    template<class Response>
    std::vector<int32_t> extract_fds_from(Response* response)
    {
        std::vector<int32_t> fd(response->fd().data(), response->fd().data() + response->fd().size());
        response->clear_fd();
        response->set_fds_on_side_channel(fd.size());
        return fd;
    }

    void send_fds(std::vector<int32_t> const& fd)
    {
        if (fd.size() > 0)
        {
            ancil_send_fds(socket.native_handle(), fd.data(), fd.size());
        }
    }

    template<class ParameterMessage, class ResultMessage>
    void invoke(
        void (protobuf::DisplayServer::*function)(
            ::google::protobuf::RpcController* controller,
            const ParameterMessage* request,
            ResultMessage* response,
            ::google::protobuf::Closure* done),
        mir::protobuf::wire::Invocation const& invocation)
    {
        ParameterMessage parameter_message;
        parameter_message.ParseFromString(invocation.parameters());
        ResultMessage result_message;

        try
        {
            std::unique_ptr<google::protobuf::Closure> callback(
                google::protobuf::NewPermanentCallback(this,
                    &AsioSession::send_response,
                    invocation.id(),
                    &result_message));

            (display_server.get()->*function)(
                0,
                &parameter_message,
                &result_message,
                callback.get());
        }
        catch (std::exception const& x)
        {
            result_message.set_error(x.what());
            send_response(invocation.id(), &result_message);
        }
    }

    boost::asio::local::stream_protocol::socket socket;
    boost::asio::streambuf message;
    int const id_;
    ConnectedSessions<AsioSession>* connected_sessions;
    std::shared_ptr<protobuf::DisplayServer> const display_server;
    mir::protobuf::Surface surface;
    unsigned char message_header_bytes[2];
    std::vector<char> whole_message;
    std::shared_ptr<ResourceCache> const resource_cache;
};


mf::ProtobufSocketCommunicator::ProtobufSocketCommunicator(
    std::string const& socket_file,
    std::shared_ptr<ProtobufIpcFactory> const& ipc_factory)
:   socket_file((std::remove(socket_file.c_str()), socket_file)),
    acceptor(io_service, socket_file),
    ipc_factory(ipc_factory),
    next_session_id(0)
{
    start_accept();
}

void mf::ProtobufSocketCommunicator::start_accept()
{
    auto session = std::make_shared<detail::AsioSession>(
        io_service,
        next_id(),
        &connected_sessions,
        ipc_factory->make_ipc_server(),
        ipc_factory->resource_cache());

    acceptor.async_accept(
        session->socket,
        boost::bind(
            &ProtobufSocketCommunicator::on_new_connection,
            this,
            session,
            ba::placeholders::error));
}

int mf::ProtobufSocketCommunicator::next_id()
{
    int id = next_session_id.load();
    while (!next_session_id.compare_exchange_weak(id, id + 1)) std::this_thread::yield();
    return id;
}


void mf::ProtobufSocketCommunicator::start()
{
    auto run_io_service = boost::bind(&ba::io_service::run, &io_service);
    io_service_thread = std::move(std::thread(run_io_service));
}

mf::ProtobufSocketCommunicator::~ProtobufSocketCommunicator()
{
    io_service.stop();

    if (io_service_thread.joinable())
    {
        io_service_thread.join();
    }

    connected_sessions.clear();

    std::remove(socket_file.c_str());
}

void mf::ProtobufSocketCommunicator::on_new_connection(
    std::shared_ptr<detail::AsioSession> const& session,
    const boost::system::error_code& ec)
{
    if (!ec)
    {
        connected_sessions.add(session);

        session->read_next_message();
    }
    start_accept();
}

mfd::AsioSession::AsioSession(
    boost::asio::io_service& io_service,
    int id_,
    ConnectedSessions<AsioSession>* connected_sessions,
    std::shared_ptr<protobuf::DisplayServer> const& display_server,
    std::shared_ptr<ResourceCache> const& resource_cache)
    : socket(io_service),
    id_(id_),
    connected_sessions(connected_sessions),
    display_server(display_server),
    resource_cache(resource_cache)
{
}

void mfd::AsioSession::read_next_message()
{
    boost::asio::async_read(socket,
        boost::asio::buffer(message_header_bytes),
        boost::bind(&mfd::AsioSession::on_read_size,
            this, ba::placeholders::error));
}

void mfd::AsioSession::on_read_size(const boost::system::error_code& ec)
{
    if (!ec)
    {
        size_t const body_size = (message_header_bytes[0] << 8) + message_header_bytes[1];
        // Read newline delimited messages for now
        ba::async_read(
             socket,
             message,
             boost::asio::transfer_exactly(body_size),
             boost::bind(&AsioSession::on_new_message,
                         this, ba::placeholders::error));
    }
}

void mfd::AsioSession::on_new_message(const boost::system::error_code& ec)
{
    if (!ec)
    {
        std::istream in(&message);
        mir::protobuf::wire::Invocation invocation;

        invocation.ParseFromIstream(&in);

        // TODO comparing strings in an if-else chain isn't efficient.
        // It is probably possible to generate a Trie at compile time.
        if ("connect" == invocation.method_name())
        {
            invoke(&protobuf::DisplayServer::connect, invocation);
        }
        else if ("create_surface" == invocation.method_name())
        {
            invoke(&protobuf::DisplayServer::create_surface, invocation);
        }
        else if ("next_buffer" == invocation.method_name())
        {
            invoke(&protobuf::DisplayServer::next_buffer, invocation);
        }
        else if ("release_surface" == invocation.method_name())
        {
            invoke(&protobuf::DisplayServer::release_surface, invocation);
        }
        else if ("test_file_descriptors" == invocation.method_name())
        {
            invoke(&protobuf::DisplayServer::test_file_descriptors, invocation);
        }
        else if ("disconnect" == invocation.method_name())
        {
            invoke(&protobuf::DisplayServer::disconnect, invocation);
            // Careful about what you do after this - it deletes this
            connected_sessions->remove(id());
            return;
        }
        else
        {
            /*log->error()*/
            std::cerr << "Unknown method:" << invocation.method_name() << std::endl;
        }
    }

    read_next_message();
}

void mfd::AsioSession::on_response_sent(bs::error_code const& error, std::size_t)
{
    if (error)
        std::cerr << "ERROR sending response: " << error.message() << std::endl;
}

void mfd::AsioSession::send_response(
    ::google::protobuf::uint32 id,
    google::protobuf::Message* response)
{
    std::ostringstream buffer1;
    response->SerializeToOstream(&buffer1);

    mir::protobuf::wire::Result result;
    result.set_id(id);
    result.set_response(buffer1.str());

    std::ostringstream buffer2;
    result.SerializeToOstream(&buffer2);

    const std::string& body = buffer2.str();
    const size_t size = body.size();
    const unsigned char header_bytes[2] =
    {
        static_cast<unsigned char>((size >> 8) & 0xff),
        static_cast<unsigned char>((size >> 0) & 0xff)
    };

    whole_message.resize(sizeof header_bytes + size);
    std::copy(header_bytes, header_bytes + sizeof header_bytes, whole_message.begin());
    std::copy(body.begin(), body.end(), whole_message.begin() + sizeof header_bytes);

    ba::async_write(
        socket,
        ba::buffer(whole_message),
        boost::bind(&AsioSession::on_response_sent, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}