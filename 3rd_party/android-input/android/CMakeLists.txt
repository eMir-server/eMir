include_directories(
  external/kernel-headers/original
  frameworks/base/include
  frameworks/native/include
  hardware/libhardware_legacy/include
  system/core/include
  system/extras/ext4_utils
  /usr/include
  )

add_definitions(
  -DANDROID_SMP=1
  )

add_library(
  android-input
  # The stuff that we want
  frameworks/base/services/input/EventHub.cpp
  frameworks/base/services/input/InputApplication.cpp
  frameworks/base/services/input/InputDispatcher.cpp
  frameworks/base/services/input/InputListener.cpp
  frameworks/base/services/input/InputManager.cpp
  frameworks/base/services/input/InputReader.cpp
  frameworks/base/libs/androidfw/InputTransport.cpp
  frameworks/base/services/input/InputWindow.cpp
  frameworks/base/services/input/PointerController.cpp
  # The stuff that is used internally by the implementation above.
  # We're not directly interested in those:
  frameworks/base/libs/androidfw/Input.cpp
  frameworks/base/libs/androidfw/InputDevice.cpp
  frameworks/base/libs/androidfw/Keyboard.cpp
  frameworks/base/libs/androidfw/KeyCharacterMap.cpp
  frameworks/base/libs/androidfw/KeyLayoutMap.cpp
  frameworks/base/libs/androidfw/VelocityControl.cpp
  frameworks/base/libs/androidfw/VelocityTracker.cpp
  frameworks/base/libs/androidfw/VirtualKeyMap.cpp
  frameworks/native/libs/utils/BufferedTextOutput.cpp
  frameworks/native/libs/utils/Debug.cpp
  frameworks/native/libs/utils/FileMap.cpp
  frameworks/native/libs/utils/Looper.cpp
  frameworks/native/libs/utils/PropertyMap.cpp
  frameworks/native/libs/utils/String16.cpp
  frameworks/native/libs/utils/String8.cpp
  frameworks/native/libs/utils/RefBase.cpp
  frameworks/native/libs/utils/SharedBuffer.cpp
  frameworks/native/libs/utils/Static.cpp
  frameworks/native/libs/utils/TextOutput.cpp
  frameworks/native/libs/utils/Threads.cpp
  frameworks/native/libs/utils/Timers.cpp
  frameworks/native/libs/utils/Tokenizer.cpp
  frameworks/native/libs/utils/Unicode.cpp
  frameworks/native/libs/utils/VectorImpl.cpp
  hardware/libhardware_legacy/power/power.c
  system/core/libcutils/atomic.c
  system/core/libcutils/properties.c
  system/core/libcutils/sched_policy.c
  system/core/libcutils/sockets.c
  system/core/libcutils/threads.c
  system/core/liblog/event_tag_map.c
  system/core/liblog/fake_log_device.c
  system/core/liblog/logd_write.c
  system/core/liblog/logprint.c
  system/extras/ext4_utils/sha1.c
  )

set_target_properties(
  android-input
  PROPERTIES
  COMPILE_FLAGS ${ANDROID_INPUT_COMPILE_FLAGS}
  )