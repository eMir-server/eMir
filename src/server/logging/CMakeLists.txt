set(
  LOGGING_SOURCES

  dumb_console_logger.cpp
  glog_logger.cpp
  session_mediator_report.cpp
  message_processor_report.cpp
  display_report.cpp
)

add_library(
  mirlogging STATIC

  ${LOGGING_SOURCES}
)
