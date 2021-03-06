include_directories(${CMAKE_SOURCE_DIR})

set(
  INTEGRATION_TESTS_SRCS
  test_surfaceloop.cpp
  test_error_reporting.cpp
  test_display_info.cpp
)

add_subdirectory(client/)
add_subdirectory(compositor/)
add_subdirectory(frontend/)
add_subdirectory(shell/)
add_subdirectory(process/)
if (NOT MIR_DISABLE_INPUT)
add_subdirectory(input/)
endif()

if (MIR_PLATFORM STREQUAL "android")
  include_directories(${PROJECT_SOURCE_DIR}/include/accessory)

  add_subdirectory(graphics/android)
endif()

if(MIR_PLATFORM STREQUAL "gbm")
  include_directories(
    ${DRM_INCLUDE_DIRS}
    ${GBM_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GLESv2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
  )
  add_subdirectory(graphics/gbm)
  list(APPEND INTEGRATION_TESTS_SRCS test_drm_auth_magic.cpp)
endif()

add_subdirectory(cucumber)

add_executable(
  integration-tests
  ${INTEGRATION_TESTS_SRCS})
uses_android_input(integration-tests)

target_link_libraries(
  integration-tests

  mir-test-framework
  mir-test-doubles
  mir-test-cucumber

  mirserver
  mirclient
  mirdraw
  mirtestdraw

  3rd_party

  ${PROTOBUF_LIBRARIES}
  ${Boost_LIBRARIES}
  ${GTEST_BOTH_LIBRARIES}
  ${GMOCK_LIBRARY}
  ${GMOCK_MAIN_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT} # Link in pthread.
  ${ANDROID_STDLIB}
)

mir_discover_tests(integration-tests)
