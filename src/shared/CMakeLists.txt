set(MIR_GENERATED_INCLUDE_DIRECTORIES)
add_subdirectory(protobuf/)

set(PREFIX "${CMAKE_INSTALL_PREFIX}")
set(EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
set(INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include/mircommon")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/mircommon.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/mircommon.pc
)

set(
  MIR_GENERATED_INCLUDE_DIRECTORIES
  ${MIR_GENERATED_INCLUDE_DIRECTORIES}
  PARENT_SCOPE)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/shared/mir ${CMAKE_SOURCE_DIR}/include/shared/mir_toolkit
  DESTINATION "include/mircommon"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mircommon.pc
  DESTINATION lib/pkgconfig
)
