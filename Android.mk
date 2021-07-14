#---------------------------------------------------------------------
# Copyright (c) 2013-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#---------------------------------------------------------------------

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := procenv
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES:= procenv.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_STATIC_LIBRARIES := libc libdl
LOCAL_SHARED_LIBRARIES := libcutils

include $(BUILD_EXECUTABLE)
