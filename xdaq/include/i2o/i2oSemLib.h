/* i2oSemLib.h - sem library header file */

/****************************************************************
Copyright 1999 I2O Special Interest Group (I2O SIG).  All rights reserved.

TERMS AND CONDITIONS OF USE

This header file, and any modifications of this header file, are provided
contingent upon your agreement and adherence to the here-listed terms and
conditions.  By accepting and/or using this header file, you agree to
abide by these terms and conditions and that these terms and conditions will
be construed and governed in accordance with the laws of the State of
California, without reference to conflict-of-law provisions.  If you do not
agree to these terms and conditions, please delete this file, and any
copies, permanently, without making any use thereof.

THIS HEADER FILE IS PROVIDED FREE OF CHARGE ON AN AS-IS BASIS WITHOUT
WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  I2O SIG DOES NOT WARRANT THAT THIS HEADER FILE WILL MEET THE
USER'S REQUIREMENTS OR THAT ITS OPERATION WILL BE UNINTERRUPTED OR
ERROR-FREE.

I2O SIG DISCLAIMS ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF
ANY PROPRIETARY RIGHTS, RELATING TO THE IMPLEMENTATION OF THE I2O
SPECIFICATIONS.  I2O SIG DOES NOT WARRANT OR REPRESENT THAT SUCH
IMPLEMENTATIONS WILL NOT INFRINGE SUCH RIGHTS.

THE USER OF THIS HEADER FILE SHALL HAVE NO RECOURSE TO I2O SIG FOR ANY
ACTUAL OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT LIMITED TO, LOST DATA
OR LOST PROFITS ARISING OUT OF THE USE OR INABILITY TO USE THIS PROGRAM.

I2O SIG grants the user of this header file a license to copy, distribute,
and modify it, for any purpose, under the following terms.  Any copying,
distribution, or modification of this header file must not delete or alter
the copyright notice of I2O SIG or any of these Terms and Conditions.

Any distribution of this header file must not include a charge for the
header file (unless such charges are strictly for the physical acts of
copying or transferring copies).  However, distribution of a product in
which this header file is embedded may include a charge so long as any
such charge does not include any charge for the header file itself.

Any modification of this header file constitutes a derivative work based
on this header file.  Any distribution of such derivative work: (1) must
include prominent notices that the header file has been changed from the
original, together with the dates of any changes; (2) automatically
includes this same license to the original header file from I2O SIG, without
any restriction thereon from the distributing user; and (3) must include a
grant of license of the modified file under the same terms and conditions as
these Terms and Conditions.

****************************************************************

The I2O SIG Web site can be found at: http://www.i2osig.org

The I2O SIG encourages you to deposit derivative works based on this
header file at the I2O SIG Web site.  Furthermore, to become a Registered
Developer of the I2O SIG, sign up at the Web site or call 415.750.8352
(United States).

****************************************************************/

#ifndef __INCi2oSemLibh
#define __INCi2oSemLibh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "i2o/shared/i2otypes.h"
#include "i2o/i2oErrorLib.h"
#include "i2o/i2oObjLib.h"

/* sem typedefs */

typedef struct i2oSem *  	I2O_SEM_ID;		/* sem ID */
typedef int			I2O_SEM_OPTIONS; 	/* sem options */

/* semaphore options */

#define I2O_SEM_OPT_Q_FIFO	0x0    	/* 0x0 first in first out queue */
#define I2O_SEM_OPT_Q_PRIORITY	0x1   	/* 0x1 priority sorted queue */
#define I2O_SEM_OPT_DELETE_SAFE	0x4 	/* 0x4 holder del safe (mutx) */
#define I2O_SEM_OPT_INVERSION_SAFE 0x8 	/* 0x8 no pri invn (mutx)*/

/* semaphore initial states */

typedef enum
    {
    I2O_SEM_EMPTY	= 0,
    I2O_SEM_FULL	= 1
    } I2O_SEM_B_STATE;

/* function declarations */


extern I2O_SEM_ID 	i2oSemBCreate (I2O_OWNER_ID  ownerId, 
				       I2O_SEM_OPTIONS semOptions, 
				       I2O_SEM_B_STATE intialState,
				       I2O_STATUS * pStatus);
extern I2O_SEM_ID 	i2oSemCCreate (I2O_OWNER_ID  ownerId, 
				       I2O_SEM_OPTIONS semOptions, 
				       I2O_COUNT intialCount,
				       I2O_STATUS * pStatus);
extern I2O_SEM_ID 	i2oSemMCreate (I2O_OWNER_ID  ownerId, 
				       I2O_SEM_OPTIONS semOptions, 
				       I2O_STATUS * pStatus);
extern void		i2oSemTake (I2O_SEM_ID semId, I2O_USECS timeout, 
				    I2O_STATUS * pStatus);
extern void		i2oSemGive (I2O_SEM_ID semId, I2O_STATUS * pStatus);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCi2oSemLibh */

