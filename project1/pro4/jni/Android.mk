LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := BurgerBuddies.c
LOCAL_MODULE := BurgerBuddies
LOCAL_CFLAGS += -pie -fPIE -I/home/manifold/kernel/goldfish
LOCAL_LDFLAGS += -pie -fPIE
LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)
