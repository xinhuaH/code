# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

# HAL module implemenation stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MULTILIB := 64
LOCAL_SHARED_LIBRARIES := libcutils libhitvwrapper libutils
LOCAL_LDFLAGS := -L$(TOP)/device/mstar/common/libraries/mi/lib/generic \

LOCAL_SRC_FILES :=  \
    hal_power_control.cpp

LOCAL_C_INCLUDES := \
    $(TOP)/hardware/libhardware/include/hardware \
    $(TOP)/hardware/libhardware/include/hardware/til \
    $(TOP)/system/core/include/cutils \
    $(TOP)/vendor/hisense/middleware/elements/middleware/include \
    $(TOP)/vendor/hisense/til_layer/tilwrapper \
    $(TOP)/vendor/hisense/middleware/dtv/testTil/testTilCase/hardware/til

LOCAL_MODULE := hal_power_control.default
ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_MODULE)
include $(BUILD_SHARED_LIBRARY)
