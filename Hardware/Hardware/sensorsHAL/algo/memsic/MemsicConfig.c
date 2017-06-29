/*****************************************************************************
* Copyright (C), 2014, MEMSIC Inc.
* File Name		: CompassAlgo.c
* Author		: MEMSIC Inc.		
* Version		: 1.0	Data: 2014/12/12
* Description	: This file is the configuration file for MEMSIC algorithm. 
* History		: 1.Data		: 
* 			  	  2.Author		: 
*			      3.Modification:	
*****************************************************************************/
#include <utils/Log.h>
#include "MemsicConfig.h"

static float fPassMagPara[9] = {1.0,0.0,0.0,  0.0,1.0,0.0,  0.0,0.0,1.0};	//Mag cal para unit matrix	

/*******************************************************************************************
* Function Name	: SetMagSensorPara
* Description	: Set the magnetic sensor parameter.
********************************************************************************************/
int SetMagSensorPara(float *pa)
{
	int i;
	
	for(i=0;i<9;i++){
		pa[i] = fPassMagPara[i];
		//ALOGE("memsicalgoinit pa[%d] = %f\n",i,pa[i]);
	}
	
	return 0;
}

/*******************************************************************************************
* Function Name	: SetOriSensorPara
* Description	: Set the orientation sensor parameter
********************************************************************************************/
int SetOriSensorPara(float *pa, float *pb, uint8 *pc)
{
	pa[0] = 75;		//uPointBufSizeLevelThr	= can not be more than 100
	pa[1] = 0.04;	//uThresholdPdistance	= ..........
	pa[2] = 30;		//uThrowOffPoint		= ..........
	pa[3] = 1.20;	//GetBufferPointThrh	= fMaxX-fMinX
	pa[4] = 0.70;	//UpdateMinThreshold	= when figure 8 calibration
	pa[5] = 1.40;   //UpdateMaxThreshold	= when figure 8 calibration
	pa[6] = 0.025;	//TrhldPdis 			= for circle offset calibration

	pb[0] = 1.60;	//GetQuaThrh 	= non-calibration Max-Min
	pb[1] = 10.0;	//GetQuaThrhTcl = non-calibration Max-Min
	pb[2] = 0.001;	//fAccVar 		= judge if the acc sensor is moving
	pb[3] = 0.0005;	//fMagNoiVarX 	= judge if the mag X-Axis is moving
	pb[4] = 0.0005;	//fMagNoiVarY 	= judge if the mag Y-Axis is moving
	pb[5] = 0.001;	//fMagNoiVarZ 	= judge if the mag Z-Axis is moving
	pb[6] = 0.001;	//fMagSatuVar 	= judge if the mag sensor is saturation 
	
	pc[0] = 21;		//yawFirstLen	= can not be more than 41, must be odd number.
	pc[1] = 10;		//yawFirstLen	= can not be more than 40
	pc[2] = 2;		//corMagLen		= can not be more than 30
	
	#if 0
	int i;
	for(i=0;i<7;i++){
		ALOGE("memsicalgoinit pa[%d] = %f\n",i,pa[i]);
	}
	for(i=0;i<7;i++){
		ALOGE("memsicalgoinit pb[%d] = %f\n",i,pb[i]);
	}
	for(i=0;i<3;i++){
		ALOGE("memsicalgoinit pc[%d] = %d\n",i,pc[i]);
	}
	#endif
	
	return 0;
}
