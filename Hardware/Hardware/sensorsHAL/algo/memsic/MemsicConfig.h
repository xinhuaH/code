/*****************************************************************************
* Copyright (C), 2014, MEMSIC Inc.
* File Name		: Config.h
* Author		: Yan Guopu		Version: 1.0	Data: 2014/12/11
* Description	: This file is the head file of config.c. It provides the 
* 			      interface function declarations of the lib. 
* History		: 1. Data			: 
* 			  	  2. Author			: 
*			  	  3. Modification	:	
*****************************************************************************/

#ifndef _MEMSICCONFIG_H_
#define _MEMSICCONFIG_H_

//Create a new name for existing types
#ifndef ALGORITHM_TYPES
#define ALGORITHM_TYPES
typedef   signed char  int8; 	// signed 8-bit number    (-128 to +127)
typedef unsigned char  uint8; 	// unsigned 8-bit number  (+0 to +255)

typedef   signed int  int16; 	// signed 32-bt number    (-2,147,483,648 to +2,147,483,647)
typedef unsigned int  uint16; 	// unsigned 32-bit number (+0 to +4,294,967,295)
#endif

#define MMC328XMA	1
#define MMC3416XPJ	2
#define MMC3516XPJ	3
#define MMC3524XPJ	4
#define MAGSENSOR	MMC3516XPJ

#define PI	      			3.14159265	//pi

#define NVM_SIZE        	1024 
#define NVM_PATH			"/data/misc/magpara"	
//#define NVM_PATH			"/data/misc/sensors/magpara"


#define MMC3416X_IOM			'm'

/* IOCTLs for MMC3416X device */
#define MMC3416X_IOC_TM				_IO (MMC3416X_IOM, 0x00)
#define MMC3416X_IOC_SET			_IO (MMC3416X_IOM, 0x01)
#define MMC3416X_IOC_READ			_IOR(MMC3416X_IOM, 0x02, int[3])
#define MMC3416X_IOC_READXYZ		_IOR(MMC3416X_IOM, 0x03, int[3])
#define MMC3416X_IOC_RESET          _IO (MMC3416X_IOM, 0x04)
#define MMC3416X_IOC_NOBOOST 		_IO (MMC3416X_IOM, 0x05)
#define MMC3416X_IOC_ID      		_IOR(MMC3416X_IOM, 0x06, short)
#define MMC3416X_IOC_DIAG    		_IOR(MMC3416X_IOM, 0x14, int[1])

/*******************************************************************************************
* Function Name	: SetAlgoPara
* Description	: Set the magnetic sensor parameter
* Input			: Algorithm parameter.
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int SetMagSensorPara(float *pa);

/*******************************************************************************************
* Function Name	: SetOriSensorPara
* Description	: Set the orientation sensor parameter
* Input			: Algorithm parameter.
* Return		: 0 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
int SetOriSensorPara(float *pa, float *pb, uint8 *pc);

#endif // _MEMSICCONFIG_H_
