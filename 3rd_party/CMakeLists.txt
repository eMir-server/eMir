
# Warnings in 3rd party code are not to be considered errors. :(
string (REPLACE " -Werror " " " CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

list(
  APPEND MIR_3RD_PARTY_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/libancillary
  ${CMAKE_CURRENT_SOURCE_DIR}/glm
  ${CMAKE_CURRENT_SOURCE_DIR}/android-deps
  )

list(
  APPEND MIR_3RD_PARTY_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/libancillary/fd_send.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/libancillary/fd_recv.c)

add_library(
  3rd_party 

  ${MIR_3RD_PARTY_SOURCES}
)

set(MIR_3RD_PARTY_INCLUDE_DIRECTORIES ${MIR_3RD_PARTY_INCLUDE_DIRECTORIES} PARENT_SCOPE)
set(MIR_INPUT_ANDROID_COMPILE_FLAGS ${MIR_INPUT_ANDROID_COMPILE_FLAGS}
  PARENT_SCOPE)

if (MIR_PLATFORM STREQUAL "android")
  add_subdirectory(android-fbtype)
endif()

if (NOT MIR_DISABLE_INPUT)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/android-deps)
  add_subdirectory(android-input)
endif()

set(MIR_ANDROID_INCLUDE_DIRECTORIES ${MIR_ANDROID_INCLUDE_DIRECTORIES} PARENT_SCOPE)
set(
  MIR_ANDROID_INPUT_COMPILE_FLAGS
  ${MIR_ANDROID_INPUT_COMPILE_FLAGS}
  PARENT_SCOPE)
  
if (NOT MIR_DISABLE_INPUT)
target_link_libraries(
  3rd_party

  android-input)
endif()
