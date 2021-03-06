include_directories(
  ${PROTOBUF_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR})

set(
  MIR_PROTOBUF_PROTOCOL_SPEC
  mir_protobuf_wire.proto
  mir_protobuf.proto
)

protobuf_generate_cpp(
  GENERATED_PROTOBUF_SRCS GENERATED_PROTOBUF_HDRS
  ${MIR_PROTOBUF_PROTOCOL_SPEC}
)

add_library(
  mirprotobuf SHARED
   
  google_protobuf_guard.cpp
  ${GENERATED_PROTOBUF_SRCS}
  ${GENERATED_PROTOBUF_HDRS})

set_target_properties(
  mirprotobuf
  PROPERTIES
  VERSION ${MIR_VERSION_MAJOR}.${MIR_VERSION_MINOR}.${MIR_VERSION_PATCH}
  SOVERSION ${MIR_VERSION_MAJOR})

target_link_libraries(
  mirprotobuf

  ${PROTOBUF_LIBRARIES}
)

# Export the include directories
list(APPEND MIR_GENERATED_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})

set(
  MIR_GENERATED_INCLUDE_DIRECTORIES
  ${MIR_GENERATED_INCLUDE_DIRECTORIES}
  PARENT_SCOPE)

# Install the protobuf library
install(
  TARGETS mirprotobuf
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(
  FILES ${MIR_PROTOBUF_PROTOCOL_SPEC}
  DESTINATION share/mir/protocol
)

