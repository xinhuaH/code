/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	  http://www.apache.org/licenses/LICENSE-2.0
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
#include <stdlib.h>
#include "AccelSensor.h"
#include "sensors.h"

#define FETCH_FULL_EVENT_BEFORE_RETURN	1
#define IGNORE_EVENT_TIME				10000000

#define	EVENT_TYPE_ACCEL_X	ABS_X
#define	EVENT_TYPE_ACCEL_Y	ABS_Y
#define	EVENT_TYPE_ACCEL_Z	ABS_Z

#define ACCEL_CONVERT		((GRAVITY_EARTH) / 4096) /* (4 * 1G / 2^12) */
#define CONVERT_ACCEL_X		ACCEL_CONVERT
#define CONVERT_ACCEL_Y		ACCEL_CONVERT
#define CONVERT_ACCEL_Z		ACCEL_CONVERT

#define SYSFS_I2C_SLAVE_PATH	"/device/device/"
#define SYSFS_INPUT_DEV_PATH	"/device/"

#define ARRAY	3

/*****************************************************************************/

AccelSensor::AccelSensor()
	: SensorBase(NULL, "accelerometer"),
	  mInputReader(4),
	  mHasPendingEvent(false),
	  mEnabledTime(0)
{
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = SENSORS_ACCELERATION_HANDLE;
	mPendingEvent.type = SENSOR_TYPE_ACCELEROMETER;
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
	mPendingEvent.acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;

	if (data_fd) {
		strlcpy(input_sysfs_path, "/sys/class/input/", sizeof(input_sysfs_path));
		strlcat(input_sysfs_path, input_name, sizeof(input_sysfs_path));
		strlcat(input_sysfs_path, SYSFS_I2C_SLAVE_PATH, sizeof(input_sysfs_path));
		input_sysfs_path_len = strlen(input_sysfs_path);
#ifdef TARGET_8610
		if (access(input_sysfs_path, F_OK)) {
			input_sysfs_path_len -= strlen(SYSFS_I2C_SLAVE_PATH);
			strlcpy(&input_sysfs_path[input_sysfs_path_len],
					SYSFS_INPUT_DEV_PATH, SYSFS_MAXLEN);
			input_sysfs_path_len += strlen(SYSFS_INPUT_DEV_PATH);
		}
#endif
		enable(0, 1);
	}
}

AccelSensor::AccelSensor(char *name)
	: SensorBase(NULL, "accelerometer"),
	  mInputReader(4),
	  mHasPendingEvent(false),
	  mEnabledTime(0)
{
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = SENSORS_ACCELERATION_HANDLE;
	mPendingEvent.type = SENSOR_TYPE_ACCELEROMETER;
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
	mPendingEvent.acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;

	if (data_fd) {
		strlcpy(input_sysfs_path, SYSFS_CLASS, sizeof(input_sysfs_path));
		strlcat(input_sysfs_path, name, sizeof(input_sysfs_path));
		strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
		input_sysfs_path_len = strlen(input_sysfs_path);
		ALOGI("The accel sensor path is %s",input_sysfs_path);
		enable(0, 1);
	}
}

AccelSensor::AccelSensor(SensorContext *context)
	: SensorBase(NULL, NULL, context),
	  mInputReader(4),
	  mHasPendingEvent(false),
	  mEnabledTime(0)
{
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = context->sensor->handle;
	mPendingEvent.type = SENSOR_TYPE_ACCELEROMETER;
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
	mPendingEvent.acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;

	strlcpy(input_sysfs_path, context->enable_path, sizeof(input_sysfs_path));
	input_sysfs_path_len = strlen(input_sysfs_path);
	data_fd = context->data_fd;
	ALOGI("The accel sensor path is %s",input_sysfs_path);
	mUseAbsTimeStamp = false;
	enable(0, 0);
}

AccelSensor::~AccelSensor() {
	if (mEnabled) {
		enable(0, 0);
	}
}

int AccelSensor::enable(int32_t, int en) {
	int flags = en ? 1 : 0;
	char propBuf[PROPERTY_VALUE_MAX];
	property_get("sensors.accel.loopback", propBuf, "0");
	if (strcmp(propBuf, "1") == 0) {
		ALOGE("sensors.accel.loopback is set");
		mEnabled = flags;
		return 0;
	}

	if (flags != mEnabled) {
		int fd;
		strlcpy(&input_sysfs_path[input_sysfs_path_len],
				SYSFS_ENABLE, SYSFS_MAXLEN);
		fd = open(input_sysfs_path, O_RDWR);
		if (fd >= 0) {
			char buf[2];
			int err;
			buf[1] = 0;
			if (flags) {
				buf[0] = '1';
				mEnabledTime = getTimestamp() + IGNORE_EVENT_TIME;
			} else {
				buf[0] = '0';
			}
			err = write(fd, buf, sizeof(buf));
			close(fd);
			mEnabled = flags;
			return 0;
		}
		ALOGE("AccelSensor: failed to open %s", input_sysfs_path);
		return -1;
	}
	return 0;
}

bool AccelSensor::hasPendingEvents() const {
	return mHasPendingEvent || mHasPendingMetadata;
}

int AccelSensor::setDelay(int32_t, int64_t delay_ns)
{
	int fd;
	char propBuf[PROPERTY_VALUE_MAX];
	property_get("sensors.accel.loopback", propBuf, "0");
	if (strcmp(propBuf, "1") == 0) {
		ALOGE("sensors.accel.loopback is set");
		return 0;
	}
	int delay_ms = delay_ns / 1000000;
	strlcpy(&input_sysfs_path[input_sysfs_path_len],
			SYSFS_POLL_DELAY, SYSFS_MAXLEN);
	fd = open(input_sysfs_path, O_RDWR);
	if (fd >= 0) {
		char buf[80];
		snprintf(buf, sizeof(buf), "%d", delay_ms);
		write(fd, buf, strlen(buf)+1);
		close(fd);
		return 0;
	}
	return -1;
}

int AccelSensor::readEvents(sensors_event_t* data, int count)
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

#if FETCH_FULL_EVENT_BEFORE_RETURN
again:
#endif
	while (count && mInputReader.readEvent(&event)) {
		int type = event->type;
		if (type == EV_ABS) {
			float value = event->value;
			if (event->code == EVENT_TYPE_ACCEL_X) {
				mPendingEvent.data[0] = value * CONVERT_ACCEL_X;
			} else if (event->code == EVENT_TYPE_ACCEL_Y) {
				mPendingEvent.data[1] = value * CONVERT_ACCEL_Y;
			} else if (event->code == EVENT_TYPE_ACCEL_Z) {
				mPendingEvent.data[2] = value * CONVERT_ACCEL_Z;
			}
		} else if (type == EV_SYN) {
			switch (event->code){
				case SYN_TIME_SEC:
					{
						mUseAbsTimeStamp = true;
						report_time = event->value*1000000000LL;
					}
					break;
				case SYN_TIME_NSEC:
					{
						mUseAbsTimeStamp = true;
						mPendingEvent.timestamp = report_time+event->value;
					}
					break;
				case SYN_REPORT:
					{
						if(mUseAbsTimeStamp != true) {
							mPendingEvent.timestamp = timevalToNano(event->time);
						}
						if (mEnabled) {
							if(mPendingEvent.timestamp >= mEnabledTime) {
								*data++ = mPendingEvent;
								numEventReceived++;
							}
							count--;
						}
					}
					break;
			}
		} else {
			ALOGE("AccelSensor: unknown event (type=%d, code=%d)",
					type, event->code);
		}
		mInputReader.next();
	}

#if FETCH_FULL_EVENT_BEFORE_RETURN
	/* if we didn't read a complete event, see if we can fill and
	   try again instead of returning with nothing and redoing poll. */
	if (numEventReceived == 0 && mEnabled == 1) {
		n = mInputReader.fill(data_fd);
		if (n)
			goto again;
	}
#endif

	return numEventReceived;
}

