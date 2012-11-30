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
 * Authored by: Alan Griffiths <alan@octopull.co.uk>
 */


#include "mir_socket_rpc_channel.h"

#include "mir_protobuf.pb.h"  // For Buffer frig
#include "mir_protobuf_wire.pb.h"

#include "ancillary.h"

#include <boost/bind.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/null.hpp>

#include <cstdlib>
#include <iostream>

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

namespace mcl = mir::client;
namespace mcld = mir::client::detail;

mcld::PendingCallCache::PendingCallCache(std::shared_ptr<Logger> const& log) :
    log(log)
{
}

mcld::SendBuffer& mcld::PendingCallCache::save_completion_details(
    mir::protobuf::wire::Invocation& invoke,
    google::protobuf::Message* response,
    std::shared_ptr<google::protobuf::Closure> const& complete)
{
    std::unique_lock<std::mutex> lock(mutex);

    auto& current = pending_calls[invoke.id()] = PendingCall(response, complete);
    log->debug() << "save_completion_details " << invoke.id() << " response " << response << " complete " << complete << std::endl;
    return current.send_buffer;
}

void mcld::PendingCallCache::complete_response(mir::protobuf::wire::Result& result)
{
    std::unique_lock<std::mutex> lock(mutex);
    log->debug() << "complete_response for result " << result.id() << std::endl;
    auto call = pending_calls.find(result.id());
    if (call == pending_calls.end())
    {
        log->error() << "orphaned result: " << result.ShortDebugString() << std::endl;
    }
    else
    {
        auto& completion = call->second;
        log->debug() << "complete_response for result " << result.id() << " response " << completion.response << " complete " << completion.complete << std::endl;
        completion.response->ParseFromString(result.response());
        completion.complete->Run();
        pending_calls.erase(call);
    }
}


mcl::MirSocketRpcChannel::MirSocketRpcChannel() :
    pending_calls(std::shared_ptr<Logger>()), work(io_service), socket(io_service)
{
}

mcl::MirSocketRpcChannel::MirSocketRpcChannel(std::string const& endpoint, std::shared_ptr<Logger> const& log) :
    log(log), next_message_id(0), pending_calls(log), work(io_service), endpoint(endpoint), socket(io_service)
{
    socket.connect(endpoint);

    auto run_io_service = boost::bind(&boost::asio::io_service::run, &io_service);

    for (int i = 0; i != threads; ++i)
    {
        io_service_thread[i] = std::move(std::thread(run_io_service));
    }
}

mcl::MirSocketRpcChannel::~MirSocketRpcChannel()
{
    io_service.stop();

    for (int i = 0; i != threads; ++i)
    {
        if (io_service_thread[i].joinable())
        {
            io_service_thread[i].join();
        }
    }
}


void mcl::MirSocketRpcChannel::receive_file_descriptors(google::protobuf::Message* response,
    google::protobuf::Closure* complete)
{
    log->debug() << __PRETTY_FUNCTION__ << std::endl;

    auto buffer = dynamic_cast<mir::protobuf::Buffer*>(response);
    if (!buffer)
    {
        auto surface = dynamic_cast<mir::protobuf::Surface*>(response);
        if (surface && surface->has_buffer())
            buffer = surface->mutable_buffer();
    }

    if (buffer)
    {
        buffer->clear_fd();

        if (buffer->fds_on_side_channel() > 0)
        {
            log->debug() << __PRETTY_FUNCTION__ << " expect " << buffer->fds_on_side_channel() << " file descriptors" << std::endl;

            std::vector<int32_t> buf(buffer->fds_on_side_channel());

            int received = 0;
            while ((received = ancil_recv_fds(socket.native_handle(), buf.data(), buf.size())) == -1)
                /* TODO avoid spinning forever */;

            log->debug() << __PRETTY_FUNCTION__ << " received " << received << " file descriptors" << std::endl;

            for (int i = 0; i != received; ++i)
                buffer->add_fd(buf[i]);
        }
    }

    auto platform = dynamic_cast<mir::protobuf::Platform*>(response);
    if (!platform)
    {
        auto connection = dynamic_cast<mir::protobuf::Connection*>(response);
        if (connection && connection->has_platform())
            platform = connection->mutable_platform();
    }

    if (platform)
    {
        platform->clear_fd();

        if (platform->fds_on_side_channel() > 0)
        {
            log->debug() << __PRETTY_FUNCTION__ << " expect " << platform->fds_on_side_channel() << " file descriptors" << std::endl;

            std::vector<int32_t> buf(platform->fds_on_side_channel());

            int received = 0;
            while ((received = ancil_recv_fds(socket.native_handle(), buf.data(), buf.size())) == -1)
                /* TODO avoid spinning forever */;

            log->debug() << __PRETTY_FUNCTION__ << " received " << received << " file descriptors" << std::endl;

            for (int i = 0; i != received; ++i)
                platform->add_fd(buf[i]);
        }
    }


    complete->Run();
}

void mcl::MirSocketRpcChannel::CallMethod(
    const google::protobuf::MethodDescriptor* method,
    google::protobuf::RpcController*,
    const google::protobuf::Message* parameters,
    google::protobuf::Message* response,
    google::protobuf::Closure* complete)
{
    mir::protobuf::wire::Invocation invocation = invocation_for(method, parameters);
    std::ostringstream buffer;
    invocation.SerializeToOstream(&buffer);

    std::shared_ptr<google::protobuf::Closure> callback(
        google::protobuf::NewPermanentCallback(this, &MirSocketRpcChannel::receive_file_descriptors, response, complete));

    // Only save details after serialization succeeds
    auto& send_buffer = pending_calls.save_completion_details(invocation, response, callback);

    // Only send message when details saved for handling response
    send_message(buffer.str(), send_buffer);
}

mir::protobuf::wire::Invocation mcl::MirSocketRpcChannel::invocation_for(
    const google::protobuf::MethodDescriptor* method,
    const google::protobuf::Message* request)
{
    std::ostringstream buffer;
    request->SerializeToOstream(&buffer);

    mir::protobuf::wire::Invocation invoke;

    invoke.set_id(next_id());
    invoke.set_method_name(method->name());
    invoke.set_parameters(buffer.str());

    return invoke;
}

int mcl::MirSocketRpcChannel::next_id()
{
    int id = next_message_id.load();
    while (!next_message_id.compare_exchange_weak(id, id + 1)) std::this_thread::yield();
    return id;
}

void mcl::MirSocketRpcChannel::send_message(const std::string& body, detail::SendBuffer& send_buffer)
{
    const size_t size = body.size();
    const unsigned char header_bytes[2] =
    {
        static_cast<unsigned char>((size >> 8) & 0xff),
        static_cast<unsigned char>((size >> 0) & 0xff)
    };

    send_buffer.resize(sizeof header_bytes + size);
    std::copy(header_bytes, header_bytes + sizeof header_bytes, send_buffer.begin());
    std::copy(body.begin(), body.end(), send_buffer.begin() + sizeof header_bytes);

    boost::asio::async_write(
        socket,
        boost::asio::buffer(send_buffer),
        boost::bind(&MirSocketRpcChannel::on_message_sent, this,
            boost::asio::placeholders::error));
}

void mcl::MirSocketRpcChannel::on_message_sent(boost::system::error_code const& error)
{
    log->debug() << __PRETTY_FUNCTION__ << std::endl;
    if (error)
    {
        log->error() << error.message() << std::endl;
        return;
    }

    read_message();
}


void mcl::MirSocketRpcChannel::read_message()
{
    log->debug() << __PRETTY_FUNCTION__ << std::endl;
    const size_t body_size = read_message_header();

    log->debug() << __PRETTY_FUNCTION__ << " body_size:" << body_size << std::endl;

    mir::protobuf::wire::Result result = read_message_body(body_size);

    log->debug() << __PRETTY_FUNCTION__ << " result.id():" << result.id() << std::endl;

    pending_calls.complete_response(result);
}

size_t mcl::MirSocketRpcChannel::read_message_header()
{
    unsigned char header_bytes[2];
    boost::system::error_code error;
    boost::asio::read(socket, boost::asio::buffer(header_bytes), boost::asio::transfer_exactly(sizeof header_bytes), error);
    if (error)
        log->error() << error.message() << std::endl;

    const size_t body_size = (header_bytes[0] << 8) + header_bytes[1];
    return body_size;
}

mir::protobuf::wire::Result mcl::MirSocketRpcChannel::read_message_body(const size_t body_size)
{
    boost::system::error_code error;
    boost::asio::streambuf message;
    boost::asio::read(socket, message, boost::asio::transfer_exactly(body_size), error);
    if (error)
        log->error() << error.message() << std::endl;

    std::istream in(&message);
    mir::protobuf::wire::Result result;
    result.ParseFromIstream(&in);
    return result;
}

std::ostream& mcl::ConsoleLogger::error()
{
    return std::cerr  << "ERROR: ";
}

std::ostream& mcl::ConsoleLogger::debug()
{
    static char const* const debug = getenv("MIR_CLIENT_DEBUG");

    if (debug) return std::cerr  << "DEBUG: ";

    static boost::iostreams::stream<boost::iostreams::null_sink> null((boost::iostreams::null_sink()));
    return null;
}

std::ostream& mcl::NullLogger::error()
{
    static boost::iostreams::stream<boost::iostreams::null_sink> null((boost::iostreams::null_sink()));
    return null;
}

std::ostream& mcl::NullLogger::debug()
{
    static boost::iostreams::stream<boost::iostreams::null_sink> null((boost::iostreams::null_sink()));
    return null;
}