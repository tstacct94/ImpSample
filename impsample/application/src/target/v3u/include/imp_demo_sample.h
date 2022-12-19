/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021-2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : imp_demo_sample.h
 * Version      : -
 * Description  : IMP Sample Code (Setting for impdemo sample)
 * Device(s)    : -
 * Description  : -
 *********************************************************************************************************************/
/*********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "rcar-xos/impfw/r_impfw.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef IMPFWDEMOSAMPLE_H_
#define IMPFWDEMOSAMPLE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* declare use core */
#define IMPFWDEMO_USECORES \
        { IMPFW_CORE_TYPE_IMP,    0u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_IMP,    1u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_IMP,    2u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_IMP,    3u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_OCV,    0u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_OCV,    1u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_OCV,    2u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_OCV,    3u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_OCV,    4u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_OCV,    5u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_OCV,    6u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_OCV,    7u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_DMAC,   0u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_DMAC,   1u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_DMAC,   2u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_DMAC,   3u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_PSCEXE, 0u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_PSCEXE, 1u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_CNN,    0u }, /* valid for instaance 0,1,2,6 */ \
        { IMPFW_CORE_TYPE_CNN,    1u }, /* valid for instaance 3,4,5,6 */ \
        { IMPFW_CORE_TYPE_CNN,    2u }  /* valid for instaance 0,1,2,6 */ 

/* declare num of core listed in IMPFWDEMO1_USECORES */
#define IMPFWDEMO_NUMOFMAXCORE  21u

/* declare use instance */
#define IMPFWDEMO_INSTANCE  IMPFW_INSTANCE_6

/* declare max queue num : max requests */
#define IMPFWDEMO_MAXQUEUENUM   (1U * IMPFWDEMO_NUMOFMAXCORE)

/* declare max msg num : sum of API call and response from IMPDRV */
#define IMPFWDEMO_MAXMSGNUM     (2U * IMPFWDEMO_NUMOFMAXCORE)

/* declare max core num */
#define IMPFWDEMO_MAXCORENUM_IMP    (4U)
#define IMPFWDEMO_MAXCORENUM_OCV    (8U)
#define IMPFWDEMO_MAXCORENUM_DMAC   (4U)
#define IMPFWDEMO_MAXCORENUM_PSCEXE (2U)
#define IMPFWDEMO_MAXCORENUM_CNN    (3U)
#define IMPFWDEMO_MAXCORENUM_IMP_SLIM  (0U)
#define IMPFWDEMO_MAXCORENUM_DMAC_SLIM (0U)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* IMPFWDEMO_H_ */

