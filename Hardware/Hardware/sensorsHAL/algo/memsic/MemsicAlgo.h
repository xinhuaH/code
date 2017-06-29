/*****************************************************************************
* Copyright (C), 2014, MEMSIC Inc.
* File Name		: CompassAlgo.h
* Author		: Yan Guopu		Version: 1.0	Data: 2014/11/09
* Description	: This file is the head file of CompassAlgo.lib. It provides the 
* 			      interface function declarations of the lib. 
* History		: 1. Data			: 
* 			  	  2. Author			: 
*			  	  3. Modification	:	
*****************************************************************************/
#include "MemsicConfig.h"

#ifndef _COMPASSALGO_H_
#define _COMPASSALGO_H_

/*******************************************************************************************
* Function Name	: AlgoInitial
* Description	: Initialize the algorithm.
* Input			: None
* Output		: None.
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int AlgoInitial(void);

/*******************************************************************************************
* Function Name	: MemsicAlgo
* Description	: Pass the Acc raw data, Mag raw data and SET flag bit to the algorithm library.
* Input			: fRawData[0-2] --- Magnetic raw data of three axis;
*				  fRawData[3-5] --- Acceleration raw data of three axis; 				  
*		          flagTemp[0] --- Calibration status. 0 = no need to calibrate.
*													  1 = need to calibrate
*				  flagTemp[1] --- SET and RESET flag bit. 0 = S&R finish.
*														  1 = S&R not finish.
*				  flagTemp[2] --- 0 = Orientation sensor is closed.
*								  1 = Orientation sensor is opened.
*				  flagTemp[3] --- 0 = Mag or Ori sensor's status is stable.
*								  1 = Re-start the Mag or Ori sensor.
*				  samplingTnterval --- sampling interval.
* Output		: MemsicCorMag[0-2] --- calibrated magnetic sensor data;
* 				: bStatus[0] - calibration status. = 0, calibration is done, quality = 3 = 100%.
*												   = 1, the compass is been calibrating, quality = 0 = 0%.
*		 		  bStatus[1] - SET flag bit. 	   = 0, no need to do SET;
*		  			   		   				       = 1, need to do SET.
*		 		  bStatus[2] - calibration quality = 0, quality = 0%.
*		  			   		   					   = 3, quality = 100%.
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int MemsicAlgo(float *fRawData, uint8 *flagTemp, uint8 samplingTnterval, float *MemsicCorMag, uint8 *bStatus);

/*******************************************************************************************
* Function Name	: MemsicOrientationSensor
* Description	: Get the yaw, pitch, roll of the orientation sensor.
* Input			: None;
* Output		: dataOut[0] --- yaw;
*				  dataOut[1] --- pitch;
*				  dataOut[2] --- roll;
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int MemsicOrientationSensor(float *dataOut);

/*******************************************************************************************
* Function Name	: CheckSensorStatus
* Description	: Check if there is interference when start the magnetic sensor or change
*                 the sampling rate.
* Input			: None;
* Output		: int type
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int CheckSensorStatus(float *fRawData);

/*******************************************************************************************
* Function Name	: AlgoMagParaLoad
* Description	: Read the mag para from the file in cell phone.
* Input			: None
* Output		: None.
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int AlgoMagParaLoad(void);

/*******************************************************************************************
* Function Name	: AlgoMagParaStore
* Description	: Save the mag para in the file in cell phone.
* Input			: None
* Output		: None.
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int AlgoMagParaStore(void);

#endif // _COMPASSALGO_H_






