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
LOCAL_SHARED_LIBRARIES := libcutils libhitvwrapper libutils libhi_msp
LOCAL_LDFLAGS := -L$(TOP)/device/mstar/common/libraries/mi/lib/generic \

LOCAL_SRC_FILES :=  \
   hdmi_cec.cpp   \
  hs_hal_cec.c

LOCAL_C_INCLUDES := \
    $(TOP)/hardware/libhardware/include/hardware \
    $(TOP)/system/core/include/cutils \
    $(TOP)/vendor/hisense/middleware/elements/middleware/include \
    $(TOP)/vendor/hisense/til_layer/tilwrapper \
    $(TOP)/vendor/hisilicon/bigfish/sdk/source/msp/include \
    $(TOP)/vendor/hisilicon/bigfish/sdk/source/common/include \
    $(TOP)/vendor/hisense/middleware/testTil/testTilCase/hardware/til
LOCAL_MODULE := hdmi_cec.default
#LOCAL_CFLAGS:= -DLOG_TAG=\"hdmi_cec\"
ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_MODULE)
include $(BUILD_SHARED_LIBRARY)
