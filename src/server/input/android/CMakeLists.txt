list(
  APPEND INPUT_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/android_input_manager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/android_input_reader_policy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/android_pointer_controller.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/android_input_lexicon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/android_input_channel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rudimentary_input_reader_policy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/event_filter_dispatcher_policy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/default_android_input_configuration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/android_input_application_handle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/android_input_window_handle.cpp
)

set(
  INPUT_SOURCES
  ${INPUT_SOURCES}
  PARENT_SCOPE
)

