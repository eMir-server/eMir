list(APPEND UNIT_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_aging_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_buffer_depository.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_platform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_client_mir_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mir_connection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_wait_handle.cpp
)

if(MIR_PLATFORM STREQUAL "android")
  add_subdirectory("android")
endif()

if(MIR_PLATFORM STREQUAL "gbm")
  add_subdirectory("gbm")
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
