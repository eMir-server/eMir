include_directories(
  external/kernel-headers/original
  frameworks/base/include
  frameworks/native/include
  hardware/libhardware_legacy/include
  system/core/include
  system/extras/ext4_utils
)

add_definitions(
  -DANDROID_SMP=1
  )

# This is stuff for which we want non-android versions
set(
  UTIL_SOURCES

  frameworks/native/libs/utils/Looper.cpp     # used by InputDispatcher.cpp
  frameworks/native/libs/utils/RefBase.cpp    # used a lot
  frameworks/native/libs/utils/Timers.cpp     # used by KeyCharacterMap.cpp, InputReader.cpp, InputDispatcher.cpp, Looper.cpp
)

if (MIR_INPUT_USE_ANDROID_TYPES)
list(APPEND UTIL_SOURCES
  frameworks/native/libs/utils/String16.cpp
  frameworks/native/libs/utils/String8.cpp
  frameworks/native/libs/utils/Threads.cpp
  frameworks/native/libs/utils/Unicode.cpp
  frameworks/native/libs/utils/BufferedTextOutput.cpp
  frameworks/native/libs/utils/VectorImpl.cpp
  frameworks/native/libs/utils/SharedBuffer.cpp
  frameworks/native/libs/utils/PropertyMap.cpp
  frameworks/native/libs/utils/Static.cpp
  frameworks/native/libs/utils/TextOutput.cpp
  frameworks/native/libs/utils/Debug.cpp
  hardware/libhardware_legacy/power/power.c
  system/core/libcutils/threads.c
  system/core/libcutils/sched_policy.c
  system/core/libcutils/sockets.c
  system/core/libcutils/atomic.c
  system/core/liblog/event_tag_map.c
  system/core/liblog/fake_log_device.c
  system/core/liblog/logprint.c
  system/core/liblog/logd_write.c
  system/extras/ext4_utils/sha1.c
  system/core/libcutils/properties.c
)
endif()


add_library(
  android-input SHARED
  # The stuff that we want
  frameworks/base/services/input/EventHub.cpp
  frameworks/base/services/input/InputApplication.cpp
  frameworks/base/services/input/InputDispatcher.cpp
  frameworks/base/services/input/InputListener.cpp
  frameworks/base/services/input/InputManager.cpp
  frameworks/base/services/input/InputReader.cpp
  frameworks/base/services/input/InputTransport.cpp
  frameworks/base/services/input/InputWindow.cpp
  frameworks/base/services/input/PointerController.cpp
  # Keyboard/keymap handling
  frameworks/base/services/input/GenericKeyMap.cpp
  # The stuff that is used internally by the implementation above.
  # We're not directly interested in those:
  frameworks/base/services/input/Input.cpp
  frameworks/base/services/input/InputDevice.cpp
  frameworks/base/services/input/Keyboard.cpp
  frameworks/base/services/input/KeyCharacterMap.cpp
  frameworks/base/services/input/KeyLayoutMap.cpp
  frameworks/base/services/input/VelocityControl.cpp
  frameworks/base/services/input/VelocityTracker.cpp
  frameworks/base/services/input/VirtualKeyMap.cpp
  frameworks/native/libs/utils/Tokenizer.cpp
  frameworks/native/libs/utils/FileMap.cpp
  ${UTIL_SOURCES}
  )

#we don't have binder or skia available on hybris/linux platforms
if(MIR_USE_BIONIC)
  target_link_libraries(
    android-input
    binder
    skia)
endif()

target_link_libraries(
  android-input

  ${Boost_LIBRARIES}
  -lpthread
)

set_target_properties(
  android-input
  PROPERTIES
  COMPILE_FLAGS ${ANDROID_INPUT_COMPILE_FLAGS}
)

install(
  TARGETS android-input
  LIBRARY DESTINATION lib)
