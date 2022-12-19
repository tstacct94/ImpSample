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
 * File Name    : imp_demo2.c
 * Version      : -
 * Description  : IMP Sample Code (How to use IMP Framework with IMP Driver)
 * Device(s)    : -
 * Description  : -
 *********************************************************************************************************************/
/*********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* OSAL API*/
#include "rcar-xos/osal/r_osal.h"

/* IMP Framework */
#include "rcar-xos/impfw/r_impfw.h"

/* Atomic Library */
#include "rcar-xos/atmlib/r_atmlib_prot.h"
#include "rcar-xos/atmlib/r_atmlib_types.h"

/* Demo */
#include "imp_demo.h"
#include "imp_demo_sample.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/
#define IMPFW_WORKSIZE  (65536U)

/* macro used to log print */
#define LOGPRINT(ARG)   (void)(printf ARG, fflush(stdout))

#define CL_CNN_NOP      0x80000020U
#define CL_CNN_TRAP     0x88000000U


#define NOP_SMALL_CLSIZE     (128U)
#define NOP_BIG_CLSIZE    (102400U)

/**********************************************************************************************************************
 * Local Typedef definitions
 *********************************************************************************************************************/
typedef char char_t;

/* declare typedef for CL */
typedef struct {
    uint32_t cl_nop_small[NOP_SMALL_CLSIZE];
    uint32_t cl_nop_large[NOP_BIG_CLSIZE];
} st_cldata_t;

typedef struct {
#if IMPFWDEMO_MAXCORENUM_IMP > 0
    st_cldata_t imp[IMPFWDEMO_MAXCORENUM_IMP];
#endif
#if IMPFWDEMO_MAXCORENUM_IMP_SLIM > 0
    st_cldata_t impslim[IMPFWDEMO_MAXCORENUM_IMP_SLIM];
#endif
#if IMPFWDEMO_MAXCORENUM_OCV > 0
    st_cldata_t ocv[IMPFWDEMO_MAXCORENUM_OCV];
#endif
#if IMPFWDEMO_MAXCORENUM_DMAC > 0
    st_cldata_t dmac[IMPFWDEMO_MAXCORENUM_DMAC];
#endif
#if IMPFWDEMO_MAXCORENUM_DMAC_SLIM > 0
    st_cldata_t dmac[IMPFWDEMO_MAXCORENUM_DMAC_SLIM];
#endif
#if IMPFWDEMO_MAXCORENUM_PSCEXE > 0
    st_cldata_t pscexe[IMPFWDEMO_MAXCORENUM_PSCEXE];
#endif
#if IMPFWDEMO_MAXCORENUM_CNN > 0
    st_cldata_t cnn[IMPFWDEMO_MAXCORENUM_CNN];
#endif
} st_impdemo_buffer_t;

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/


extern int32_t imp_demo2 (void);

/**********************************************************************************************************************
 * Function Name: imp_demo2_createCL_nop
 * Description  : return cldate to test queue
 * Arguments    : st_impfw_core_info_t core_type
 *              : uint32_t             *data
 *              : uint32_t             clsize
 * Return Value : 0     ..  no error
 *              : other ..  error
 *********************************************************************************************************************/
static uint32_t imp_demo2_createCL_nop(e_impfw_core_type_t core_type, uint32_t *data, uint32_t clsize)
{
    R_ATMLIB_RETURN_VALUE    atomic_ret;
    R_ATMLIB_CLData          cldata;
    uint32_t                 ret = 0U;
    uint32_t                 loop;
    uint32_t                 nop_count;

    if (NOP_SMALL_CLSIZE == clsize)
    {
        nop_count = 1U;
    }
    else
    {
        nop_count = clsize - 8U;
    }

    /* set cldata for small CL */
    switch(core_type)
    {
        case IMPFW_CORE_TYPE_IMP:       /* fall through */
        case IMPFW_CORE_TYPE_IMP_SLIM:
            atomic_ret = r_atmlib_InitializeIMPCL(&cldata, data, clsize);
            if (R_ATMLIB_E_OK != atomic_ret)
            {
                LOGPRINT(("<< [ERROR] r_atmlib_InitializeIMPCL  : code %d>>\n",
                    (int32_t)atomic_ret)); /* cast to int32_t */
                ret = 1U;
            }
            else
            {
                loop = 0U;
                while (nop_count > loop)
                {
                    loop++;
                    atomic_ret = r_atmlib_IMP_NOP( &cldata, 1U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_IMP_NOP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                        break;
                    }
                }
                if (0U == ret)
                {
                    atomic_ret = r_atmlib_IMP_TRAP( &cldata );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_IMP_TRAP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                    }
                    else
                    {
                        atomic_ret = r_atmlib_FinalizeIMPCL(&cldata);
                        if (R_ATMLIB_E_OK != atomic_ret)
                        {
                            ret = 1U;
                            LOGPRINT(("<< [ERROR] r_atmlib_FinalizeIMPCL  : code %d>>\n",
                                (int32_t)atomic_ret)); /* cast to int32_t */
                        }
                    }
                }
            }
            break;

        case IMPFW_CORE_TYPE_OCV:
            atomic_ret = r_atmlib_InitializeOCVCL(&cldata, data, clsize);
            if (R_ATMLIB_E_OK != atomic_ret)
            {
                ret = 1U;
                LOGPRINT(("<< [ERROR] r_atmlib_InitializeOCVCL  : code %d>>\n",
                    (int32_t)atomic_ret)); /* cast to int32_t */
            }
            else
            {
                loop = 0U;
                while (nop_count > loop)
                {
                    loop++;
                    atomic_ret = r_atmlib_OCV_NOP( &cldata, 1U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_OCV_NOP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                        break;
                    }
                }
                if (0U == ret)
                {
                    atomic_ret = r_atmlib_OCV_TRAP( &cldata, 0U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_OCV_TRAP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                    }
                    else
                    {
                        atomic_ret = r_atmlib_FinalizeOCVCL(&cldata);
                        if (R_ATMLIB_E_OK != atomic_ret)
                        {
                            ret = 1U;
                            LOGPRINT(("<< [ERROR] r_atmlib_FinalizeOCVCL  : code %d>>\n",
                                (int32_t)atomic_ret)); /* cast to int32_t */
                        }
                    }
                }
            }
            break;

        case IMPFW_CORE_TYPE_DMAC:      /* fall through */
        case IMPFW_CORE_TYPE_DMAC_SLIM:
            atomic_ret = r_atmlib_InitializeDMACL(&cldata, data, clsize);
            if (R_ATMLIB_E_OK != atomic_ret)
            {
                ret = 1U;
                LOGPRINT(("<< [ERROR] r_atmlib_InitializeDMACL  : code %d>>\n",
                    (int32_t)atomic_ret)); /* cast to int32_t */
            }
            else
            {
                loop = 0U;
                while (nop_count > loop)
                {
                    loop++;
                    atomic_ret = r_atmlib_DMA_NOP( &cldata, 1U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_DMA_NOP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                        break;
                    }
                }
                if (0U == ret)
                {
                    atomic_ret = r_atmlib_DMA_TRAP( &cldata, 0U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_DMA_TRAP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                    }
                    else
                    {
                        atomic_ret = r_atmlib_FinalizeDMACL(&cldata);
                        if (R_ATMLIB_E_OK != atomic_ret)
                        {
                            ret = 1U;
                            LOGPRINT(("<< [ERROR] r_atmlib_FinalizeDMACL  : code %d>>\n",
                                (int32_t)atomic_ret)); /* cast to int32_t */
                        }
                    }
                }
            }
            break;

        case IMPFW_CORE_TYPE_PSCEXE:
            atomic_ret = r_atmlib_InitializePSCCL(&cldata, data, clsize);
            if (R_ATMLIB_E_OK != atomic_ret)
            {
                ret = 1U;
                LOGPRINT(("<< [ERROR] r_atmlib_InitializePSCCL  : code %d>>\n",
                    (int32_t)atomic_ret)); /* cast to int32_t */
            }
            else
            {
                loop = 0U;
                while (nop_count > loop)
                {
                    loop++;
                    atomic_ret = r_atmlib_PSC_NOP( &cldata, 1U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_PSC_NOP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                        break;
                    }
                }
                if (0U == ret)
                {
                    atomic_ret = r_atmlib_PSC_TRAP( &cldata, 0U );
                    if (R_ATMLIB_E_OK != atomic_ret)
                    {
                        ret = 1U;
                        LOGPRINT(("<< [ERROR] r_atmlib_PSC_NOP  : code %d>>\n",
                            (int32_t)atomic_ret)); /* cast to int32_t */
                    }
                    else
                    {
                        atomic_ret = r_atmlib_FinalizePSCCL(&cldata);
                        if (R_ATMLIB_E_OK != atomic_ret)
                        {
                            ret = 1U;
                            LOGPRINT(("<< [ERROR] r_atmlib_FinalizePSCCL  : code %d>>\n",
                                (int32_t)atomic_ret)); /* cast to int32_t */
                        }
                    }
                }
            }
            break;

        case IMPFW_CORE_TYPE_CNN:
            loop = 0U;
            while (nop_count > loop)
            {
                data[loop] = CL_CNN_NOP; /* NOP */
                loop++;
            }
            data[loop] = CL_CNN_TRAP; /* TRAP */
            ret = 0U;
            break;

        default:
            ret = 1U;
            break;
    }
    return ret;
}
/**********************************************************************************************************************
 * End of function imp_demo2_createCL_nop
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: imp_demo2_createdata
 * Description  : allocate osal buffer and return cldate to do sample program.
 * Arguments    : st_buffer_t *cl_buffers
 * Return Value : 0     success
 *              : other error
 *********************************************************************************************************************/
static uint32_t imp_demo2_createdata(st_buffer_t *cl_buffers)
{
    uint32_t core_num;
    uint32_t ret;
    e_impfw_core_type_t core_type;
    st_impdemo_buffer_t *cldata;

    ret    = 0;
    cldata = NULL;

    /* OSAL */
    e_osal_return_t ret_osal = OSAL_RETURN_OK;

    ret_osal = buffer_alloc(cl_buffers, sizeof(st_impdemo_buffer_t));

    if (OSAL_RETURN_OK == ret_osal)
    {
        cldata = (st_impdemo_buffer_t *) cl_buffers->cpu_addr;
#if 0
        LOGPRINT(("imga_virt:    0x%p\n",   cl_buffers->cpu_addr));
        LOGPRINT(("imga_phys:    0x%08x\n",
            (uint32_t)cl_buffers->hw_addr)); /* cast to uint32_t */
#endif
#if IMPFWDEMO_MAXCORENUM_IMP > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_IMP;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_IMP > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->imp[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->imp[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for IMP>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
#if IMPFWDEMO_MAXCORENUM_IMP_SLIM > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_IMP_SLIM;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_IMP_SLIM > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->impslim[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->impslim[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for IMP>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
#if IMPFWDEMO_MAXCORENUM_OCV > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_OCV;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_OCV > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->ocv[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->ocv[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for OCV>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
#if IMPFWDEMO_MAXCORENUM_DMAC > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_DMAC;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_DMAC > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->dmac[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->dmac[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for DMA>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
#if IMPFWDEMO_MAXCORENUM_DMAC_SLIM > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_DMAC_SLIM;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_DMAC_SLIM > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->dmacslim[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->dmacslim[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for DMA>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
#if IMPFWDEMO_MAXCORENUM_PSCEXE > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_PSCEXE;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_PSCEXE > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->pscexe[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->pscexe[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for PSCEXE>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
#if IMPFWDEMO_MAXCORENUM_CNN > 0
        if (0 == ret)
        {
            core_type = IMPFW_CORE_TYPE_CNN;
            core_num = 0U;
            while (IMPFWDEMO_MAXCORENUM_CNN > core_num)
            {
                ret = imp_demo2_createCL_nop(core_type, cldata->cnn[core_num].cl_nop_small, NOP_SMALL_CLSIZE);
                if (0 == ret)
                {
                    ret = imp_demo2_createCL_nop(core_type, cldata->cnn[core_num].cl_nop_large, NOP_BIG_CLSIZE);
                }
                if (0 != ret)
                {
                    LOGPRINT(("<< [ERROR] error create NOP for CNN>>\n"));
                    break;
                }
                core_num++;
            }
        }
#endif
    }
    else
    {
        ret = 1;
    }

    if (0 == ret)
    {
        ret_osal = buffer_sync(cl_buffers);
        if (OSAL_RETURN_OK != ret_osal)
        {
            ret = 1;
            LOGPRINT(("<< [ERROR] buffer_sync  : e_osal_return_t %d>>\n",
                (int32_t)ret_osal)); /* cast to int32_t */
        }
    }

    if ((0 != ret) && (NULL != cl_buffers->cpu_addr))
    {
        cldata = NULL;
        ret_osal = buffer_free(cl_buffers);
        if (OSAL_RETURN_OK != ret_osal)
        {
            ret = 1;
            LOGPRINT(("<< [ERROR] buffer_free  : e_osal_return_t %d>>\n",
                (int32_t)ret_osal)); /* cast to int32_t */
        }
    }
    return ret;
}
/**********************************************************************************************************************
 * End of function imp_demo2_createdata
 ********************************************************************************************************************/

uint32_t imp_demo2_gethw_addr(st_buffer_t *cl_buffers, st_impfw_core_info_t *core, uint32_t type, uint32_t *p_claddr_phys, uint32_t *p_clsize)
{
    uint32_t    claddr_phys;
    uint32_t    clsize;
    uint32_t    *target_buffer;
    uint32_t    ret;
    uintptr_t   buffer_top;
    uintptr_t   buffer_end;
    uintptr_t   target_addr;
    uint32_t    core_num;
    st_impdemo_buffer_t      *cl_data;

    target_buffer = NULL;
    core_num      = core->core_num;
    cl_data       = (st_impdemo_buffer_t *)cl_buffers->cpu_addr;

    if (type == 0U)
    {
        /* small buffer */
        switch(core->core_type)
        {
            case IMPFW_CORE_TYPE_IMP:
                target_buffer = &cl_data->imp[core_num].cl_nop_small[0];
                clsize        = sizeof(cl_data->imp[core_num].cl_nop_small);
                break;
#if IMPFWDEMO_MAXCORENUM_IMP_SLIM > 0
            case IMPFW_CORE_TYPE_IMP_SLIM:
                target_buffer = &cl_data->impslim[core_num].cl_nop_small[0];
                clsize        = sizeof(cl_data->impslim[core_num].cl_nop_small);
                break;
#endif
            case IMPFW_CORE_TYPE_OCV:
                target_buffer = &cl_data->ocv[core_num].cl_nop_small[0U];
                clsize        = sizeof(cl_data->ocv[core_num].cl_nop_small);
                break;
            case IMPFW_CORE_TYPE_DMAC:
                target_buffer = &cl_data->dmac[core_num].cl_nop_small[0U];
                clsize        = sizeof(cl_data->dmac[core_num].cl_nop_small);
                break;
#if IMPFW_CORE_TYPE_DMAC_SLIM > 0
            case IMPFW_CORE_TYPE_DMAC_SLIM:
                target_buffer = &cl_data->dmacslim[core_num].cl_nop_small[0U];
                clsize        = sizeof(cl_data->dmacslim[core_num].cl_nop_small);
                break;
#endif
            case IMPFW_CORE_TYPE_PSCEXE:
                target_buffer = &cl_data->pscexe[core_num].cl_nop_small[0U];
                clsize        = sizeof(cl_data->pscexe[core_num].cl_nop_small);
                break;
            case IMPFW_CORE_TYPE_CNN:
                target_buffer = &cl_data->cnn[core_num].cl_nop_small[0U];
                clsize      = sizeof(cl_data->cnn[core_num].cl_nop_small);
                break;
            default:
                LOGPRINT(("<< [ERROR] invalid core.core_type>>\n"));
                break;
        }
    }
    else
    {
        /* small buffer */
        switch(core->core_type)
        {
            case IMPFW_CORE_TYPE_IMP:
                target_buffer = &cl_data->imp[core_num].cl_nop_large[0];
                clsize        = sizeof(cl_data->imp[core_num].cl_nop_large);
                break;
            case IMPFW_CORE_TYPE_OCV:
                target_buffer = &cl_data->ocv[core_num].cl_nop_large[0U];
                clsize        = sizeof(cl_data->ocv[core_num].cl_nop_large);
                break;
            case IMPFW_CORE_TYPE_DMAC:
                target_buffer = &cl_data->dmac[core_num].cl_nop_large[0U];
                clsize        = sizeof(cl_data->dmac[core_num].cl_nop_large);
                break;
            case IMPFW_CORE_TYPE_PSCEXE:
                target_buffer = &cl_data->pscexe[core_num].cl_nop_large[0U];
                clsize        = sizeof(cl_data->pscexe[core_num].cl_nop_large);
                break;
            case IMPFW_CORE_TYPE_CNN:
                target_buffer = &cl_data->cnn[core_num].cl_nop_large[0U];
                clsize      = sizeof(cl_data->cnn[core_num].cl_nop_large);
                break;
            default:
                LOGPRINT(("<< [ERROR] invalid core.core_type>>\n"));
                break;
        }
    }

    ret = 0;
    if (NULL != target_buffer)
    {
        target_addr = (uintptr_t) target_buffer;
        buffer_top  = (uintptr_t) cl_buffers->cpu_addr;
        buffer_end  = buffer_top + cl_buffers->buffersize;
        if (buffer_top > target_addr || target_addr >= buffer_end)
        {
            LOGPRINT(("<< [ERROR] can not calculate hw_addr>>\n"));
            ret = 1;
        }
        else
        {
            /* offset */
            claddr_phys = (uint32_t) (target_addr - buffer_top);
            claddr_phys += (uint32_t) cl_buffers->hw_addr;

            *p_claddr_phys = claddr_phys;
            *p_clsize      = clsize;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

/**********************************************************************************************************************
 * Function Name: imp_demo2
 * Description  : sample of imp core
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
/* declare parameters to execute imp framework */
typedef struct
{
    /* measure time */
    st_osal_time_t             time_queue;
    st_osal_time_t             time_start;
    st_osal_time_t             time_callback;
    
    st_impfw_core_info_t       core;
    /* attr parameter is valid after R_IMPFW_AttrSetCl untill START-callback. */
    impfw_attr_handle_t        attr;
    /* parameter for R_IMPFW_AttrSetCl */
    uint32_t                   claddr_phys;
    uint32_t                   clsize;
    e_impfw_req_priority_t     priority;
    /* parameter for R_IMPFW_AttrSetPair */
    int32_t                    pair_id;
    uint32_t                   pair_num;
    st_impfw_core_info_t       pair_core_info[IMPFWDEMO_NUMOFMAXCORE];
} st_impdemo_request_t;


e_impfw_api_retcode_t imp_demo2_execute_pair(
    impfw_ctrl_handle_t        impfwctl, 
    st_impfw_core_info_t       *p_core,
    st_buffer_t                *p_cl_buffers,
    e_impfw_req_priority_t     priority,
    uint32_t                   pair_id,
    const st_impfw_core_info_t *p_pair_core_target,
    uint32_t                   num_pair_core_target
)
{
    impfw_attr_handle_t      impfw_attr;
    e_impfw_api_retcode_t    impfw_ret;
    uint32_t                 ret;

    uint32_t                 claddr_phys;
    uint32_t                 clsize;

    uint32_t                 pair_num;
    st_impfw_core_info_t     pair_core_info[IMPFWDEMO_NUMOFMAXCORE];

    uint32_t                 loop_core;

    impfw_ret = IMPFW_EC_OK;
    claddr_phys = 0UL;

    /* prepare parameter for cl */
    ret = imp_demo2_gethw_addr(p_cl_buffers, p_core, 0U, &claddr_phys, &clsize);
    if (0 != ret)
    {
        impfw_ret = IMPFW_EC_NG_PARAM;
    }

    /* prepare parameter for pair */
    pair_num = 0;
    for (loop_core = 0; loop_core < num_pair_core_target; loop_core++ )
    {
        if (p_pair_core_target[loop_core].core_type != p_core->core_type || p_pair_core_target[loop_core].core_num != p_core->core_num)
        {
            pair_core_info[pair_num] = p_pair_core_target[loop_core];
            pair_num++;
        }
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = R_IMPFW_AttrInit(impfwctl, p_core, &impfw_attr);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_AttrInit  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = R_IMPFW_AttrSetCl(impfw_attr, claddr_phys, clsize, priority);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_AttrSetCL  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = R_IMPFW_AttrSetPair(impfw_attr, pair_id, pair_num, pair_core_info);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_AttrSetPair  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = imp_demo_impfw_execute(impfwctl, p_core, impfw_attr);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_Execute  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    return impfw_ret;
}

e_impfw_api_retcode_t imp_demo2_execute(
    impfw_ctrl_handle_t        impfwctl, 
    st_impfw_core_info_t       *p_core,
    st_buffer_t                *p_cl_buffers,
    e_impfw_req_priority_t     priority
)
{
    impfw_attr_handle_t      impfw_attr;
    e_impfw_api_retcode_t    impfw_ret;
    uint32_t                 ret;

    uint32_t                 claddr_phys;
    uint32_t                 clsize;

    impfw_ret = IMPFW_EC_OK;
    claddr_phys = 0UL;

    /* prepare parameter for cl */
    ret = imp_demo2_gethw_addr(p_cl_buffers, p_core, 1U, &claddr_phys, &clsize);
    if (0 != ret)
    {
        impfw_ret = IMPFW_EC_NG_PARAM;
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = R_IMPFW_AttrInit(impfwctl, p_core, &impfw_attr);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_AttrInit  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = R_IMPFW_AttrSetCl(impfw_attr, claddr_phys, clsize, priority);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_AttrSetCL  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    if (IMPFW_EC_OK == impfw_ret)
    {
        impfw_ret = imp_demo_impfw_execute(impfwctl, p_core, impfw_attr);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] R_IMPFW_Execute  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }
    }

    return impfw_ret;
}

int32_t imp_demo2(void)
{
    impfw_ctrl_handle_t      impfwctl;
    int32_t                  result;

    /* IMP Framework */
    e_impfw_api_retcode_t      impfw_ret;
    uint32_t                 ret;

    st_impfw_core_info_t     core;

    st_buffer_t              cl_buffers;

    e_impfw_req_priority_t   priority;

    const st_impfw_core_info_t pair_exec_core[] = {
        IMPFWDEMO_USECORES
    };
    uint32_t                 pair_id;

    uint32_t                 loop_paircore;

    result = IMPDEMO_OK;

    /* OSAL */
    e_osal_return_t ret_osal = OSAL_RETURN_OK;

    /* create CL */
    ret = imp_demo2_createdata(&cl_buffers);
    if (0 != ret)
    {
        result = IMPDEMO_NG;
    }
    else
    {
        /* initialize IMPFW */
        impfw_ret = imp_demo_impfw_init(&impfwctl);
        if (IMPFW_EC_OK == impfw_ret)
        {
            LOGPRINT(("Execute R_IMPFW_Execute Start\n"));

            /* sample of pair execution */
            for (loop_paircore = 0; loop_paircore < sizeof(pair_exec_core) / sizeof(pair_exec_core[0]); loop_paircore++ )
            {
                core.core_type = pair_exec_core[loop_paircore].core_type;
                core.core_num  = pair_exec_core[loop_paircore].core_num;

                pair_id  = 0u;
                priority = IMPFW_REQ_PRIORITY_1;
                impfw_ret = imp_demo2_execute_pair(impfwctl, &core, &cl_buffers, priority, pair_id, pair_exec_core, sizeof(pair_exec_core)/sizeof(pair_exec_core[0]));
                if (IMPFW_EC_OK != impfw_ret)
                {
                    result = IMPDEMO_NG;
                }
                priority = IMPFW_REQ_PRIORITY_0;
                impfw_ret = imp_demo2_execute(impfwctl, &core, &cl_buffers, priority);
                if (IMPFW_EC_OK != impfw_ret)
                {
                    result = IMPDEMO_NG;
                }
            }

            LOGPRINT(("Execute R_IMPFW_Execute End\n\n"));
        }

        if (IMPFW_EC_OK == impfw_ret)
        {
            /* Wait for CL end before r_impfw_Quit, or inifinity process loop */
            ret_osal = imp_demo_impfw_waitcallback();
            if (OSAL_RETURN_OK != ret_osal)
            {
                impfw_ret = IMPFW_EC_NG_TIMEOUT;
            }

            /*----- IMP Framework Finalization -----*/
            if (IMPFW_EC_OK == impfw_ret)
            {
                impfw_ret = R_IMPFW_Quit(impfwctl);
                if (IMPFW_EC_OK != impfw_ret)
                {
                    LOGPRINT(("<< [ERROR] r_impfw_Quit  : code %d>>\n",
                        (int32_t)impfw_ret)); /* cast to int32_t */
                }
            }
        }
        if (IMPFW_EC_OK != impfw_ret)
        {
            result = IMPDEMO_NG;
        }
    }

    if (NULL != cl_buffers.cpu_addr)
    {
        ret_osal = buffer_free(&cl_buffers);
        if (OSAL_RETURN_OK != ret_osal)
        {
            result = IMPDEMO_NG;
        }
    }

    return result;
}
/**********************************************************************************************************************
 * End of function imp_demo2
 ********************************************************************************************************************/

