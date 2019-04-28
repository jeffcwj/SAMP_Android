LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := samp
LOCAL_LDLIBS += -llog
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/RakNet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/RenderWare/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../imgui/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/RakNet/SAMP
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../imgui

LOCAL_WHOLE_STATIC_LIBRARIES += android_support
 
LOCAL_CPPFLAGS += -D_ARM_ -DRAKSAMP_CLIENT -D_RAKNET_THREADSAFE
LOCAL_CPPFLAGS += -w -pthread -fpack-struct=1 -Wall -fdiagnostics-color=auto -O2 -ffast-math -std=c++11

include $(BUILD_SHARED_LIBRARY)
 
$(call import-module,android/support)