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
 * File Name    : imp_demo_sub.c
 * Version      : -
 * Description  : IMP Sample Code (How to use IMP Framework with IMP Driver)
 * Device(s)    : -
 * Description  : -
 *********************************************************************************************************************/
/*********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "imp_demo.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static osal_memory_manager_handle_t   s_osal_mmngr         = OSAL_MEMORY_MANAGER_HANDLE_INVALID;
static osal_axi_bus_id_t              s_imp_dev_axi_bus_id = OSAL_AXI_BUS_ID_INVALID;

/*****************************************************************************
* Function Name: fillmemory
* Description  : fill memory
* Arguments    : void * mem
*              : uint8_t data
*              : uint32_t size
* Return Value : None
*****************************************************************************/
void fillmemory(void *mem, uint8_t data, uint32_t size)
{
    uint8_t * p_m8 = (uint8_t *)mem; /* cast to uint8_t * */
    size_t i;

    for (i = 0u; i < size; i++ )
    {
        p_m8[i] = data;
    }
}
/*****************************************************************************
* End of function fillmemory
*****************************************************************************/


/*****************************************************************************
* Function Name: outputmemory
* Description  : output memory
* Arguments    : void * mem
*              : uint32_t xsize
*              : uint32_t ysize
*              : uint32_t bytepp
* Return Value : None
*****************************************************************************/
void outputmemory(void *mem, uint32_t xsize, uint32_t ysize, uint32_t bytepp)
{
    uint32_t  i;
    uint32_t  j;
    uint8_t   * p_m8;
    uint16_t  * p_m16;
    uint32_t  * p_m32;

    printf("-- OutPutMemory --\n");

    switch (bytepp)
    {
        default:
        case 1U:
            p_m8 = (uint8_t *)mem; /* cast to uint8_t * */
            for (i = 0u; i < ysize; i++ )
            {
                for (j = 0u; j < xsize; j++ )
                {
                    printf("%02X ", p_m8[j+(i*xsize)]);
                }
                printf("\n");
            }
            break;

        case 2U:
            p_m16 = (uint16_t *)mem; /* cast to uint16_t * */
            for (i = 0u; i < ysize; i++ )
            {
                for (j = 0u; j < xsize; j++ )
                {
                    printf("%04X ", p_m16[j+(i*xsize)]);
                }
                printf("\n");
            }
            break;

        case 4U:
            p_m32 = (uint32_t *)mem; /* cast to uint32_t* */
            for (i = 0u; i < ysize; i++ )
            {
                for (j = 0u; j < xsize; j++ )
                {
                    printf("%08X ", p_m32[j+(i*xsize)]);
                }
                printf("\n");
            }
            break;
    }

    return ;
}
/*****************************************************************************
* End of function outputmemory
*****************************************************************************/

/*****************************************************************************
* Function Name: mmngr_open
* Description  : init osal mmngr
* Arguments    : None
* Return Value : None
*****************************************************************************/
e_osal_return_t mmngr_open(void)
{
    e_osal_return_t          ret_osal = OSAL_RETURN_FAIL;
    st_osal_mmngr_config_t   osal_mmngr_cfg;
    int32_t                  val;

    ret_osal = R_OSAL_MmngrGetOsalMaxConfig(&osal_mmngr_cfg);
    if (OSAL_RETURN_OK != ret_osal)
    {
        val = (int32_t)ret_osal; /* cast to int32_t */
        printf("<< [ERROR] R_OSAL_MmngrGetOsalMaxConfig  : code %d>>\n", val);
    }
    else
    {
       /* osal_mmngr_cfg.memory_limit                = 0x2000000;  32MB */
       /* osal_mmngr_cfg.max_allowed_allocations     = 256u;
        osal_mmngr_cfg.max_registered_monitors_cbs = 0u;*/

        ret_osal = R_OSAL_MmngrOpen(&osal_mmngr_cfg, &s_osal_mmngr);
        if (OSAL_RETURN_OK != ret_osal)
        {
            val = (int32_t)ret_osal; /* cast to int32_t */
            printf("<< [ERROR] R_OSAL_MmngrOpen  : code %d>>\n", val);
        }
        else
        {
            s_imp_dev_axi_bus_id = OSAL_AXI_BUS_ID_IPA_MAIN_MEMORY;
            ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName("imp_top_00", &s_imp_dev_axi_bus_id);
            if (OSAL_RETURN_OK != ret_osal)
            {
                val = (int32_t)ret_osal; /* cast to int32_t */
                printf("<< [ERROR] R_OSAL_IoGetAxiBusIdFromDeviceName : code %d>>\n", val);
            }
        }
    }
    return ret_osal;
}
/*****************************************************************************
* End of function mmngr_open
*****************************************************************************/

/**********************************************************************************************************************
 * Function Name: mmngr_close
 * Description  : quit osal mmngr
 * Arguments    : None
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t mmngr_close(void)
{
    e_osal_return_t          ret_osal = OSAL_RETURN_OK;
    int32_t                  val;

    if (OSAL_MEMORY_MANAGER_HANDLE_INVALID != s_osal_mmngr)
    {
        ret_osal = R_OSAL_MmngrClose(s_osal_mmngr);
        if (OSAL_RETURN_OK != ret_osal)
        {
            val = (int32_t)ret_osal; /* cast to int32_t */
            printf("<< [ERROR] R_OSAL_MmngrClose  : code %d>>\n", val);
        }
        else
        {
            s_osal_mmngr = OSAL_MEMORY_MANAGER_HANDLE_INVALID;
        }
    }
    return ret_osal;
}
/*****************************************************************************
* End of function mmngr_close
*****************************************************************************/

/**********************************************************************************************************************
 * Function Name: buffer_alloc
 * Description  : allocate buffer
 * Arguments    : st_buffer_t * buffer
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t buffer_alloc(st_buffer_t * buffer, uint32_t size)
{
    e_osal_return_t          ret_osal = OSAL_RETURN_OK;
    int32_t                  val;

    /* init */
    buffer->buffer = OSAL_MEMORY_BUFFER_HANDLE_INVALID;
    buffer->buffersize = size;
    buffer->cpu_addr = NULL;
    buffer->hw_addr = 0uL;

    ret_osal = R_OSAL_MmngrAlloc(s_osal_mmngr, size, 128u, &buffer->buffer);
    if (OSAL_RETURN_OK != ret_osal)
    {
        val = (int32_t)ret_osal; /* cast to int32_t */
        printf("<< [ERROR] R_OSAL_MmngrAlloc  : code %d>>\n", val);
    }
    else
    {
        ret_osal = R_OSAL_MmngrGetCpuPtr(buffer->buffer, &buffer->cpu_addr);
        if (OSAL_RETURN_OK != ret_osal)
        {
            val = (int32_t)ret_osal; /* cast to int32_t */
            printf("<< [ERROR] R_OSAL_MmngrGetCpuPtr  : code %d>>\n", val);
        }
        else
        {
            ret_osal = R_OSAL_MmngrGetHwAddr(buffer->buffer, s_imp_dev_axi_bus_id, &buffer->hw_addr);
            if (OSAL_RETURN_OK != ret_osal)
            {
                val = (int32_t)ret_osal; /* cast to int32_t */
                printf("<< [ERROR] R_OSAL_MmngrGetHwAddr  : code %d>>\n", val);
            }
        }
    }
    return ret_osal;
}
/*****************************************************************************
* End of function buffer_alloc
*****************************************************************************/

/**********************************************************************************************************************
 * Function Name: buffer_free
 * Description  : free buffer
 * Arguments    : st_buffer_t * buffer
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t buffer_free(st_buffer_t * buffer)
{
    e_osal_return_t          ret_osal = OSAL_RETURN_OK;
    int32_t                  val;

    ret_osal = R_OSAL_MmngrDealloc(s_osal_mmngr, buffer->buffer);
    if (OSAL_RETURN_OK != ret_osal)
    {
        val = (int32_t)ret_osal; /* cast to int32_t */
        printf("<< [ERROR] R_OSAL_MmngrDealloc  : code %d>>\n", val);
    }
    else
    {
        buffer->buffer = OSAL_MEMORY_BUFFER_HANDLE_INVALID;
    }
    return ret_osal;
}
/*****************************************************************************
* End of function buffer_free
*****************************************************************************/

/**********************************************************************************************************************
 * Function Name: buffer_sync
 * Description  : cache flush and invalidate
 * Arguments    : st_buffer_t * buffer
 * Return Value : e_osal_return_t
 *********************************************************************************************************************/
e_osal_return_t buffer_sync(st_buffer_t * buffer)
{
    e_osal_return_t          ret_osal = OSAL_RETURN_OK;
    int32_t                  val;

    ret_osal = R_OSAL_MmngrFlush(buffer->buffer, 0, buffer->buffersize);
    if (OSAL_RETURN_OK != ret_osal)
    {
        val = (int32_t)ret_osal; /* cast to int32_t */
        printf("<< [ERROR] R_OSAL_MmngrDealloc  : code %d>>\n", val);
    }
    else
    {
        ret_osal = R_OSAL_MmngrInvalidate(buffer->buffer, 0, buffer->buffersize);
        if (OSAL_RETURN_OK != ret_osal)
        {
            val = (int32_t)ret_osal; /* cast to int32_t */
            printf("<< [ERROR] R_OSAL_MmngrDealloc  : code %d>>\n", val);
        }
    }

    return ret_osal;
}
/*****************************************************************************
* End of function buffer_sync
*****************************************************************************/
