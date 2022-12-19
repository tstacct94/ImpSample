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
 * Copyright (C) 2017-2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : impDemo.h
 * Version      : -
 * Description  : IMP Sample Code (How to use IMP Framework with IMP Driver)
 * Device(s)    : -
 * Description  : -
 *********************************************************************************************************************/
/*********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "rcar-xos/osal/r_osal.h"
#include "rcar-xos/impfw/r_impfw.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef IMPFWDEMO_H_
#define IMPFWDEMO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* declare osal resorce id used by main.c */
#define IMP_RESOURCE_ID                           (0x5000u)
#define OSAL_MQ_IMP_FW_TOTASK                     (IMP_RESOURCE_ID +  6u)
#define OSAL_MQ_IMP_FW_FROMTASK                   (IMP_RESOURCE_ID + 13u)
#define OSAL_MUTEX_IMP_FW_STATE_LOCK              (IMP_RESOURCE_ID + 10u)
#define OSAL_MUTEX_IMP_FW_ATTR_LOCK               (IMP_RESOURCE_ID + 17u)
#define OSAL_MUTEX_IMP_FW_LOCK                    (IMP_RESOURCE_ID + 24u)
#define OSAL_THREAD_IMP_FW                        (IMP_RESOURCE_ID +  2u)

#define OSAL_MUTEX_IMP_DRV_LOCK                   (IMP_RESOURCE_ID +  2u)

#define MQ_ID_NO                                  (IMP_RESOURCE_ID + 20u)

#define    IMPDEMO_OK                                (0)
#define    IMPDEMO_NG                                (-1)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/* declare typedef */
typedef struct {
    osal_memory_buffer_handle_t    buffer;
    uint32_t                       buffersize;
    void                           * cpu_addr;
    uintptr_t                      hw_addr;
} st_buffer_t;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: fillmemory
 * Description  : fill memory
 * Arguments    : void * mem
 *                uint8_t data
 *                uint32_t size
 * Return Value : None
 *********************************************************************************************************************/
void fillmemory (void * mem, uint8_t data, uint32_t size);

/**********************************************************************************************************************
 * Function Name: outputmemory
 * Description  : output memory
 * Arguments    : void * mem
 *                uint32_t xsize
 *                uint32_t ysize
 *                uint32_t bytepp
 * Return Value : None
 *********************************************************************************************************************/
void outputmemory (void * mem, uint32_t xsize, uint32_t ysize, uint32_t bytepp);

/**********************************************************************************************************************
 * Function Name: mmngr_open
 * Description  : init osal mmngr
 * Arguments    : None
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t mmngr_open (void);

/**********************************************************************************************************************
 * Function Name: mmngr_close
 * Description  : quit osal mmngr
 * Arguments    : None
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t mmngr_close (void);

/**********************************************************************************************************************
 * Function Name: buffer_alloc
 * Description  : allocate buffer
 * Arguments    : st_buffer_t * buffer
 *              : uint32_t size
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t buffer_alloc (st_buffer_t * buffer, uint32_t size);

/**********************************************************************************************************************
 * Function Name: buffer_free
 * Description  : free buffer
 * Arguments    : st_buffer_t * buffer
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t buffer_free (st_buffer_t * buffer);

/**********************************************************************************************************************
 * Function Name: buffer_sync
 * Description  : cache flush and invalidate to sync memory
 * Arguments    : st_buffer_t * buffer
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t buffer_sync (st_buffer_t * buffer);

/**********************************************************************************************************************
 * Function Name: imp_demo_impfw_init
 * Description  : initialize impfw_ctrl_handle
 * Arguments    : impfw_ctrl_handle_t * p_impfwctl
 * Return Value : e_impfw_api_retcode_t
 *********************************************************************************************************************/
e_impfw_api_retcode_t imp_demo_impfw_init(impfw_ctrl_handle_t * p_impfwctl);

/**********************************************************************************************************************
 * Function Name: imp_demo_impfw_execute
 * Description  : initialize impfw_ctrl_handle
 * Arguments    : impfw_ctrl_handle_t p_impfwctl
 *              : st_impfw_core_info_t *p_core
 *              : impfw_attr_handle_t impfw_attr
 * Return Value : e_impfw_api_retcode_t
 *********************************************************************************************************************/
e_impfw_api_retcode_t imp_demo_impfw_execute(impfw_ctrl_handle_t impfwctl, st_impfw_core_info_t *p_core, impfw_attr_handle_t impfw_attr);

/**********************************************************************************************************************
 * Function Name: imp_demo_impfw_waitcallback
 * Description  : wait all callback during 5sec.
 * Arguments    : None
 * Return Value : e_impfw_api_retcode_t
 *********************************************************************************************************************/
e_osal_return_t imp_demo_impfw_waitcallback(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* IMPFWDEMO_H_ */

