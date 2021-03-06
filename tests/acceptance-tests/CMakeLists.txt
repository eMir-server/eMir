include_directories(
  ${CMAKE_SOURCE_DIR}
)

set(
  SOURCES

  test_client_library.cpp
  test_surfaceloop.cpp
  test_test_framework.cpp
  test_focus_management_api.cpp
  test_focus_selection.cpp
)

list(APPEND SOURCES
  test_server_startup.cpp
)

add_executable(
  acceptance-tests

  ${SOURCES}
)


target_link_libraries(
  acceptance-tests

  mirserver
  mirclient

  mir-test-framework
  mir-test-doubles
  3rd_party

  ${PROTOBUF_LIBRARIES}
  ${Boost_LIBRARIES}
  ${GTEST_BOTH_LIBRARIES}
  ${GMOCK_LIBRARY}
  ${GMOCK_MAIN_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT} # Link in pthread.
  ${ANDROID_STDLIB}
)

mir_discover_tests(acceptance-tests)
