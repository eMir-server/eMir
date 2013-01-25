# Copyright © 2012 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Kevin DuBois <kevin.dubois@canonical.com>

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

set(
  TEST_UTILS_SRCS

  test_protobuf_client.cpp
  event_factory.cpp
)

if (NOT MIR_DISABLE_INPUT)
list(APPEND TEST_UTILS_SRCS
  fake_event_hub.cpp
)
endif()

if (MIR_TRANSPORT STREQUAL "socket")
list(APPEND TEST_UTILS_SRCS
  test_protobuf_socket_server.cpp
)
elseif (MIR_TRANSPORT STREQUAL "binder")
list(APPEND TEST_UTILS_SRCS
  test_protobuf_binder_server.cpp
)
endif()

add_library(
  mir-test-doubles STATIC
  ${TEST_UTILS_SRCS})

target_link_libraries(
  mir-test-doubles

  mir
  miroptions
  mircompositor
  mirfrontend
  mirgraphics
  mirinput
  mirsurfaces
  mirplatformgraphics
  mirprotobuf

  3rd_party

  ${PROTOBUF_LIBRARIES}
  ${Boost_LIBRARIES}
  ${GTEST_BOTH_LIBRARIES}
  ${GMOCK_LIBRARY}
  ${GMOCK_MAIN_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT} # Link in pthread.
)