list(APPEND UNIT_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/mock_drm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mock_gbm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbm_platform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbm_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbm_buffer_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbm_display.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbm_display_multi_monitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbm_display_configuration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_kms_output.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_kms_page_flipper.cpp
)

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
