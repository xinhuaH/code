/*--------------------------------------------------------------------------
Copyright (c) 2014, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <utils/Log.h>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "CalibrationModule.h"

//++add by memsic
#include "MemsicAlgo.h"
#include "MemsicConfig.h"

#define LOG_TAG "memsic"


#define SENSOR_CAL_ALGO_VERSION 	1
#define ARRAY_SIZE(a) 		(sizeof(a)/sizeof(a[0]))
static uint8 sampleIntv = 20;
static uint8 algoInit = 0;
static uint8 bCalStatusTemp = 0;
static int oriFlag = 0;
static float arrAccData[3] = {0.0};                                 

static uint8 setFlag = 0;
static int setFinish = 0;
static int sensorRestart = 0;
static int needCheckRailed = 0;

struct sensor_vec {
	union {
		struct {
			float data[4];
		};
		struct {
			float x;
			float y;
			float z;
		};
	};
};

struct sensor_cal_module_t SENSOR_CAL_MODULE_INFO;
static struct sensor_cal_algo_t algo_list[];

void MsensorSet(void);

static int convert_magnetic(sensors_event_t *raw, sensors_event_t *result,struct sensor_algo_args *args)
{
	static uint8 outStatusFlag[4];
	static uint8 algoStatus[3];
	static float fRawData[6];	
	float fMagCorData[3];
	int sensorStatus;
	
	int i = 0;
	static float fPreRawData[3] = {0.0};
	ALOGE("Convert Mag Raw MAG: %f, %f, %f",
		raw->magnetic.x, raw->magnetic.y, raw->magnetic.z);

	// Input raw data,follow ENU coordinate
	fRawData[0] = -raw->magnetic.x / 100.0f;
	fRawData[1] =  raw->magnetic.y / 100.0f;
	fRawData[2] = -raw->magnetic.z / 100.0f;
	
	//remove the first data after SET
	if (setFinish == 1) {
		setFinish = 0;
		for (i = 0; i < 3; i++)
			fRawData[i] = fPreRawData[i];
	}	
	for (i = 0; i < 3; i++) {
		fPreRawData[i] = fRawData[i];
	}
	
	//get the acc raw data 
	fRawData[3] = arrAccData[0];
	fRawData[4] = arrAccData[1];
	fRawData[5] = arrAccData[2];
		
	if (bCalStatusTemp == 1) {
		//if read file failed!!! or sensor is railed	
		outStatusFlag[0] = bCalStatusTemp;
		bCalStatusTemp = 0;
	}
	outStatusFlag[1] = setFlag;	
	outStatusFlag[2] = oriFlag;
	outStatusFlag[3] = sensorRestart;
	//ALOGE("memsic AlgoMainFunction outStatusFlag %d %d\n",outStatusFlag[0],outStatusFlag[1]);
	
    MemsicAlgo(fRawData,outStatusFlag,sampleIntv,fMagCorData,algoStatus);
	//ALOGE("memsicrawmag000 data %f %f %f\n",fRawData[0],fRawData[1],fRawData[2]);
	//ALOGE("memsicrawmagacc data %f %f %f %f %f %f\n",fRawData[0],fRawData[1],fRawData[2],fRawData[3],fRawData[4],fRawData[5]);
	
	if (oriFlag == 1)	
		oriFlag = 0; 
	if (sensorRestart == 1) {						
		sensorRestart = 0;
		needCheckRailed = 1;
	}
	if((needCheckRailed == 1)&&(algoStatus[2] == 3)){
		sensorStatus = CheckSensorStatus(fRawData);
		switch(sensorStatus){
			case 0:
				//not enough data;
				break;			
			case 1:
				//no need to re-cal
				needCheckRailed = 0;
				break;			
			case 2:
				//need re-cal
				needCheckRailed = 0;
				bCalStatusTemp = 1;
				break;
		 }  
	}
       
	outStatusFlag[0] = algoStatus[0];		//algoStatus[0]--cal status
	setFlag = algoStatus[1];				//S&R flag

	// Decide when to do SET/RESET
	if (setFlag == 1) {
		MsensorSet();
		setFlag = 0;	
		setFinish = 1;
	}

	result->magnetic.x = fMagCorData[0] * 100.0f;
	result->magnetic.y = fMagCorData[1] * 100.0f;
    result->magnetic.z = fMagCorData[2] * 100.0f;
    result->magnetic.status=algoStatus[2];

	return 0;
}

static int convert_orientation(sensors_event_t *raw, sensors_event_t *result, struct sensor_algo_args *args)
{
    static struct sensor_vec mag, acc;
    const float rad2deg = 180 / M_PI;	
	float pitch, roll, azimuth;
	float av;	
   
    float dataOut[3] = {0.0};
	ALOGE("convert_orientation %f  %f  %f\n",raw->acceleration.x,raw->acceleration.y,raw->acceleration.z);			
	if (raw->type == SENSOR_TYPE_MAGNETIC_FIELD) {
		mag.x = raw->magnetic.x;
		mag.y = raw->magnetic.y;
		mag.z = raw->magnetic.z;
	}

	if (raw->type == SENSOR_TYPE_ACCELEROMETER) {
		acc.x = raw->acceleration.x;
		acc.y = raw->acceleration.y;
		acc.z = raw->acceleration.z;
		
		arrAccData[0] = acc.x / 9.80f;
		arrAccData[1] = acc.y / 9.80f;
		arrAccData[2] = acc.z / 9.80f;
	
		oriFlag = 1;
	}

	av = sqrtf(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z);
	if(av!=0){
		pitch = asinf(-acc.y / av);
		roll = asinf(acc.x / av);
		result->orientation.pitch = pitch * rad2deg;
		result->orientation.roll = roll * rad2deg;
		
		//azimuth = atan2(-(mag.x) * cosf(roll) + mag.z * sinf(roll),
		//		mag.x*sinf(pitch)*sinf(roll) + mag.y*cosf(pitch) + mag.z*sinf(pitch)*cosf(roll));
		//tempazimuth=result->orientation.azimuth =  azimuth * rad2deg;
		//if((result->orientation.azimuth>-180)&&(result->orientation.azimuth<0))
		//{
		// tempazimuth=result->orientation.azimuth+360;
		//}		
		//YawFilter(tempazimuth,&outputyaw);
		//result->orientation.azimuth=outputyaw;
		
		MemsicOrientationSensor(dataOut);
		result->orientation.azimuth=dataOut[0];
	    result->orientation.status = raw->magnetic.status; 	
	    ALOGE("memsicyaw algo %f %d\n",dataOut[0],raw->magnetic.status);		    
	}
	
	ALOGE("Convert Ori: %f, %f, %f. type%d",
		result->orientation.azimuth,
		result->orientation.pitch,
		result->orientation.roll, raw->type);

	if (raw->type != SENSOR_TYPE_MAGNETIC_FIELD)
		return -EAGAIN;

	return 0;

}

void MsensorSet(void)
{
	static int fd = -1;	
	if(fd < 0){
		fd = open("/dev/mmc3416x", O_RDWR);
		if(fd < 0){
			ALOGE("memsicset open dev node faild!\n");
	}}
	//////////////do set////////////////////
	if(ioctl(fd, MMC3416X_IOC_SET)){
		ALOGE("mmalgoset Doing SET failed!\n");
	}else{
		ALOGE("memsicset Doing SET successed!\n");		
	}
}

static int config_magnetic(int cmd, struct sensor_algo_args *args)
{
	struct compass_algo_args *param = (struct compass_algo_args*)args;

	switch (cmd) {
		case CMD_ENABLE:
			if(param->common.enable == 1)
			{	
				sensorRestart = 1;
	   			MsensorSet();//do set when start a new apk which use the mag sensor					
			}
			//ALOGD("memsicconfig Enable status changed to %d\n", param->common.enable);			
			break;
		case CMD_DELAY: 
			sampleIntv = param->common.delay_ms;
			//ALOGD("memsicconfig Polling rate changed to %d\n", param->common.delay_ms);		
			break;
		case CMD_BATCH:
			break;
	}

	return 0;
}

static int cal_init(const struct sensor_cal_module_t *module)
{
	ALOGE("memsicalgo init!!!");
    if(algoInit == 0){	
    	///////////do once when starting up///////////
    	algoInit = 1;    	
	    AlgoInitial();	////Pass the init cal para to the algo lib.  
	    	
		if(AlgoMagParaLoad() == -1){			
			ALOGE("memsicalgo read file failed!!!");
			bCalStatusTemp = 1;		////If read cal data failed, re-cal.					
		}else{
			ALOGE("memsicalgo read file succeed!!!");
		}
	}else{
		ALOGE("memsicalgo Init OK!!!");	
	}
	return 0;
}

static void cal_deinit()
{
	//ALOGI("%s called\n", __func__);
}

static int cal_get_algo_list(const struct sensor_cal_algo_t **algo)
{
	*algo = algo_list;
	return 0;
}

static struct sensor_algo_methods_t compass_methods = {
	.convert = convert_magnetic,
	.config = config_magnetic,
};

static const char* compass_match_table[] = {
	"compass",
	NULL
};

static struct sensor_algo_methods_t orientation_methods = {
	.convert = convert_orientation,
	.config = NULL,
};

static const char* orientation_match_table[] = {
	"oem-orientation",
	"compass-orientation",
	NULL
};

static struct sensor_cal_algo_t algo_list[] = {
	{
		.tag = SENSOR_CAL_ALGO_TAG,
		.version = SENSOR_CAL_ALGO_VERSION,
		.type = SENSOR_TYPE_MAGNETIC_FIELD,
		.compatible = compass_match_table,
		.module = &SENSOR_CAL_MODULE_INFO,
		.methods = &compass_methods,
	},
	{
		.tag = SENSOR_CAL_ALGO_TAG,
		.version = SENSOR_CAL_ALGO_VERSION,
		.type = SENSOR_TYPE_ORIENTATION,
		.compatible = orientation_match_table,
		.module = &SENSOR_CAL_MODULE_INFO,
		.methods = &orientation_methods,
	},
};

static struct sensor_cal_methods_t cal_methods = {
	.init = cal_init,
	.deinit = cal_deinit,
	.get_algo_list = cal_get_algo_list,
};

struct sensor_cal_module_t SENSOR_CAL_MODULE_INFO = {
	.tag = SENSOR_CAL_MODULE_TAG,
	.id = "cal_module_memsic",
	.version = SENSOR_CAL_MODULE_VERSION,
	.vendor = "memsic",
	.dso = NULL,
	.number = ARRAY_SIZE(algo_list),
	.methods = &cal_methods,
	.reserved = {0},
};
