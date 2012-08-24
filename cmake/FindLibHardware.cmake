# Variables defined by this module:
#   LIBHARDWARE_FOUND
#   LIBHARDWARE_INCLUDE_DIRS
#   LIBHARDWARE_LIBRARIES

INCLUDE(FindPackageHandleStandardArgs)

find_path(LIBHARDWARE_INCLUDE_DIR
   NAMES         hardware/hardware.h
                 hardware/gralloc.h
                 cutils/native_handle.h
                 system/graphics.h
                 system/window.h
   HINTS         ${ANDROID_STANDALONE_TOOLCHAIN}/sysroot
   )

find_path(LIBHARDWARE_LIBRARY
   NAMES         libhardware.so 
   HINTS         ${ANDROID_STANDALONE_TOOLCHAIN}/sysroot/usr/lib
                 ${ANDROID_STANDALONE_TOOLCHAIN}/sysroot/lib
   )


# handle the QUIETLY and REQUIRED arguments and set LIBHARDWARE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBHARDWARE DEFAULT_MSG
                                  LIBHARDWARE_LIBRARY LIBHARDWARE_INCLUDE_DIR)

mark_as_advanced(LIBHARDWARE_INCLUDE_DIR LIBHARDWARE_LIBRARY )

