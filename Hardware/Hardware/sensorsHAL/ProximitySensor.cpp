/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include "ProximitySensor.h"
#include "sensors.h"

#define EVENT_TYPE_PROXIMITY		ABS_DISTANCE

#define PROXIMITY_THRESHOLD                    5.0f

#define ARRAY	3

/*****************************************************************************/

enum input_device_name {
    GENERIC_PSENSOR = 0,
    LEGACY_PSENSOR,
    CM36283_PS,
    SUPPORTED_PSENSOR_COUNT,
};

static const char *data_device_name[SUPPORTED_PSENSOR_COUNT] = {
   [GENERIC_PSENSOR] = "proximity",
    [LEGACY_PSENSOR] = "proximity",
        [CM36283_PS] = "cm36283-ps",
};

static const char *input_sysfs_path_list[SUPPORTED_PSENSOR_COUNT] = {
   /* This is not used by generic HAL. Just for back compatibility */
   [GENERIC_PSENSOR] = "/sys/class/input/%s/device/",
    [LEGACY_PSENSOR] = "/sys/class/input/%s/device/",
        [CM36283_PS] = "/sys/class/input/%s/device/",
};

static const char *input_sysfs_enable_list[SUPPORTED_PSENSOR_COUNT] = {
   [GENERIC_PSENSOR] = "enable",
    [LEGACY_PSENSOR] = "enable",
        [CM36283_PS] = "enable",
};


ProximitySensor::ProximitySensor()
    : SensorBase(NULL, NULL),
      mInputReader(4),
      mHasPendingEvent(false),
      sensor_index(-1),
      mThreshold_h(0),
      mThreshold_l(0),
      mBias(0),
      res(PROXIMITY_THRESHOLD)
{
    int i;
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = SENSORS_PROXIMITY_HANDLE;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

    for(i = 0; i < SUPPORTED_PSENSOR_COUNT; i++) {
        data_name = data_device_name[i];

        // data_fd is not initialized if data_name passed
        // to SensorBase is NULL.
        data_fd = openInput(data_name);
        if (data_fd > 0) {
            sensor_index = i;
            break;
        }
    }

    if (data_fd > 0) {
            snprintf(input_sysfs_path, sizeof(input_sysfs_path),
                            input_sysfs_path_list[i], input_name);
        input_sysfs_path_len = strlen(input_sysfs_path);
        enable(0, 1);
    }

    ALOGI("The proximity sensor path is %s",input_sysfs_path);
}

ProximitySensor::ProximitySensor(struct SensorContext *context)
: SensorBase(NULL, NULL, context),
        mInputReader(4),
        mHasPendingEvent(false),
        sensor_index(GENERIC_PSENSOR),
        mThreshold_h(0),
        mThreshold_l(0),
        mBias(0),
        res(context->sensor->resolution)
{
        mPendingEvent.version = sizeof(sensors_event_t);
        mPendingEvent.sensor = context->sensor->handle;
        mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
        memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

        data_fd = context->data_fd;
        strlcpy(input_sysfs_path, context->enable_path, sizeof(input_sysfs_path));
        input_sysfs_path_len = strlen(input_sysfs_path);
}

ProximitySensor::ProximitySensor(char *name)
: SensorBase(NULL, data_device_name[GENERIC_PSENSOR]),
        mInputReader(4),
        mHasPendingEvent(false),
        sensor_index(GENERIC_PSENSOR),
        mThreshold_h(0),
        mThreshold_l(0),
        mBias(0),
        res(PROXIMITY_THRESHOLD)
{
        mPendingEvent.version = sizeof(sensors_event_t);
        mPendingEvent.sensor = SENSORS_PROXIMITY_HANDLE;
        mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
        memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

        if (data_fd) {
                strlcpy(input_sysfs_path, SYSFS_CLASS, sizeof(input_sysfs_path));
                strlcat(input_sysfs_path, name, sizeof(input_sysfs_path));
                strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
                input_sysfs_path_len = strlen(input_sysfs_path);
                ALOGI("The proximity sensor path is %s",input_sysfs_path);
                enable(0, 1);
        }
}
ProximitySensor::~ProximitySensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int ProximitySensor::enable(int32_t, int en) {
    int flags = en ? 1 : 0;
    char propBuf[PROPERTY_VALUE_MAX];
    property_get("sensors.proxymity.loopback", propBuf, "0");
    if (strcmp(propBuf, "1") == 0) {
        mEnabled = flags;
        ALOGE("sensors.proxymity.loopback is set");
        return 0;
    }

    if (flags != mEnabled) {
        int fd;
        if (sensor_index >= 0) {
            strlcpy(&input_sysfs_path[input_sysfs_path_len], input_sysfs_enable_list[sensor_index],
                            sizeof(input_sysfs_path) - input_sysfs_path_len);
        } else {
            ALOGE("invalid sensor index:%d\n", sensor_index);
            return -1;
        }
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
            char buf[2];
            buf[1] = 0;
            if (flags) {
                buf[0] = '1';
            } else {
                buf[0] = '0';
            }
            write(fd, buf, sizeof(buf));
            close(fd);
            mEnabled = flags;
            return 0;
        } else {
            ALOGE("open %s failed.(%s)\n", input_sysfs_path, strerror(errno));
            return -1;
        }
    } else if (flags) {
            mHasPendingEvent = true;
    }
    return 0;
}

bool ProximitySensor::hasPendingEvents() const {
    return mHasPendingEvent || mHasPendingMetadata;
}

int ProximitySensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

    if (mHasPendingMetadata) {
            mHasPendingMetadata--;
            meta_data.timestamp = getTimestamp();
            *data = meta_data;
            return mEnabled ? 1 : 0;
    }

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            if (event->code == EVENT_TYPE_PROXIMITY) {
                if (event->value != -1) {
                    // FIXME: not sure why we're getting -1 sometimes
                    mPendingEvent.distance = indexToValue(event->value);
                    ALOGE("ProximitySensor: distance is %d\n", event->value);
                }
            }
        } else if (type == EV_SYN) {
                switch ( event->code ) {
                        case SYN_TIME_SEC:
                                mUseAbsTimeStamp = true;
                                report_time = event->value * 1000000000LL;
                                break;
                        case SYN_TIME_NSEC:
                                mUseAbsTimeStamp = true;
                                mPendingEvent.timestamp = report_time + event->value;
                                break;
                        case SYN_REPORT:
                                if(mUseAbsTimeStamp != true) {
                                        mPendingEvent.timestamp = timevalToNano(event->time);
                                }
                                if (mEnabled) {
                                        *data++ = mPendingEvent;
                                        count--;
                                        numEventReceived++;
                                }
                                break;
                }
        } else {
            ALOGE("ProximitySensor: unknown event (type=%d, code=%d)",
                    type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

int ProximitySensor::setDelay(int32_t, int64_t ns)
{
        int fd;
        char propBuf[PROPERTY_VALUE_MAX];
        char buf[80];
        int len;

        property_get("sensors.light.loopback", propBuf, "0");
        if (strcmp(propBuf, "1") == 0) {
                ALOGE("sensors.light.loopback is set");
                return 0;
        }
        int delay_ms = ns / 1000000;
        strlcpy(&input_sysfs_path[input_sysfs_path_len],
                        SYSFS_POLL_DELAY, SYSFS_MAXLEN);
        fd = open(input_sysfs_path, O_RDWR);
        if (fd < 0) {
                ALOGE("open %s failed.(%s)\n", input_sysfs_path, strerror(errno));
                return -1;
        }
        snprintf(buf, sizeof(buf), "%d", delay_ms);
        len = write(fd, buf, ssize_t(strlen(buf)+1));
        if (len < ssize_t(strlen(buf) + 1)) {
                ALOGE("write %s failed\n", buf);
                close(fd);
                return -1;
        }

        close(fd);
        return 0;
}

float ProximitySensor::indexToValue(size_t index) const
{
	if(index<=500)
		index=1;
	else if(index>500)
		index=0;
	return  index*PROXIMITY_THRESHOLD;
}
