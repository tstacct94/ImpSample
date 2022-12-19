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
 * File Name    : main.c
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
#include "rcar-xos/impfw/r_impfw_version.h"

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
#define IMPFW_MAIN_WORKSIZE  (16384U + 3000U)
#define IMPFW_ATTR_WORKSIZE  ((568U + 80U) * IMPFWDEMO_MAXQUEUENUM)
#define IMPFW_QUE_WORKSIZE   (248U * IMPFWDEMO_MAXQUEUENUM)

#define OSAL_MQ_APPLICATIONID    (MQ_ID_NO)
#define OSAL_MQ_TIMEOUT          (5000UL)

/* macro used to log print */
#define LOGPRINT(ARG)   (void)(printf ARG, fflush(stdout))

/**********************************************************************************************************************
 * Local Typedef definitions
 *********************************************************************************************************************/
typedef char char_t;

typedef struct
{
    uint32_t                  request_num;
    bool                      complete;
    st_impfw_core_info_t      core;
} st_additional_data_t;

typedef struct
{
    e_impfw_callback_reason_t reason;      /* callback reason */
    int32_t                   code;        /* callback code   */
    uint32_t                  request_num; /* index of st_additional_data_t */
    uint32_t                  reserved[5]; /* reserved */
} st_callbackmessage_t;

typedef struct
{
    uint32_t enable;
    int32_t  (*sample_main)(void);
    const    char_t *text;
} st_samplelist_t;

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/
static uint64_t  s_workmain_impfwinit[IMPFW_MAIN_WORKSIZE / sizeof(uint64_t)];
static uint64_t  s_workattr_impfwinit[IMPFW_ATTR_WORKSIZE / sizeof(uint64_t)];
static uint64_t  s_workque_impfwinit[IMPFW_QUE_WORKSIZE / sizeof(uint64_t)];

static osal_mq_handle_t     s_queuecallback = OSAL_MQ_HANDLE_INVALID;
static uint32_t             s_request_num;
static st_additional_data_t s_additional_data[ IMPFWDEMO_MAXQUEUENUM ];

static int32_t callbackfunc (e_impfw_callback_reason_t reason, const st_impfw_core_info_t * const core,
            int32_t code, void * const callbackargs);
static void callbackfunc_fatal (e_impfw_fatalcode_t  reason, uint32_t value);
static const char_t * reason_tostr (e_impfw_callback_reason_t reason);
static const char_t * fatal_code_tostr(e_impfw_fatalcode_t code);
static int32_t imp_demo (void);
static void    memory_barrier(void);
static int32_t impfw_version(void);
int32_t imp_demo2 (void);

static st_samplelist_t     s_sample_list[3] =
{
	{ 1U,             impfw_version, "Version" },
	{ 1U,             imp_demo,  "Execute IMP Core0" },
	/* set ENABLE_SAMPLE2 by CMakeList.txt. Simulator can not guarantee operation. */
	{ 0 & ENABLE_SAMPLE2, imp_demo2, "Execute Multiple Core" }
};

/**********************************************************************************************************************
 * Function Name: reason_tostr
 * Description  : return string of reason
 * Arguments    : e_impfw_callback_reason_t reason
 * Return Value : string
 *********************************************************************************************************************/
static const char_t * reason_tostr(e_impfw_callback_reason_t reason)
{
    char_t * p_msg;

    switch (reason)
    {
        case IMPFW_CB_STARTED:
            p_msg = "STARTED       ";
            break;
        case IMPFW_CB_TRAP:
            p_msg = "TRAP          ";
            break;
        case IMPFW_CB_INT:
            p_msg = "INT           ";
            break;
        case IMPFW_CB_INT_PBCOVF:
            p_msg = "INT_PBCOVF    ";
            break;
        case IMPFW_CB_ERROR_ILLEGAL:
            p_msg = "ERROR_ILLEGAL ";
            break;
        case IMPFW_CB_ERROR_INTERNAL:
            p_msg = "ERROR_INTERNAL";
            break;
        case IMPFW_CB_USIER:
            p_msg = "USIER         ";
            break;
        case IMPFW_CB_INT_SBO0ME:
            p_msg = "INT_SBO0ME    ";
            break;
        case IMPFW_CB_TRAP_SBO0ME:
            p_msg = "TRAP_SBO0ME   ";
            break;
        case IMPFW_CB_END:
            p_msg = "END           ";
            break;
        case IMPFW_CB_WUPCOV:
            p_msg = "WUPCOV        ";
            break;
        case IMPFW_CB_PBCOVF:
            p_msg = "PBCOVF        ";
            break;
        case IMPFW_CB_HPINT:
            p_msg = "HPINT         ";
            break;
        case IMPFW_CB_APIPINT:
            p_msg = "APIPINT       ";
            break;
        case IMPFW_CB_MSCO:
            p_msg = "MSCO          ";
            break;
        case IMPFW_CB_DRVERR:
            p_msg = "DRVERR        ";
            break;
        case IMPFW_CB_RTTERR:
            p_msg = "RTTERR        ";
            break;
        case IMPFW_CB_RESOURCEFULL:
            p_msg = "RESOURCEFULL  ";
            break;
        default:
            p_msg = "UNDEFINED     ";
            break;
    }
    return p_msg;
}
/**********************************************************************************************************************
 * End of function reason_tostr
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: fatal_code_tostr
 * Description  : return string of code
 * Arguments    : e_impfw_fatalcode_t code
 * Return Value : string
 *********************************************************************************************************************/
static const char_t * fatal_code_tostr(e_impfw_fatalcode_t code)
{
    char_t * p_msg;

    switch (code)
    {
        case IMPFW_FATALERR_FW_ERROR:
            p_msg = "FATALERR_FW          ";
            break;
        case IMPFW_FATALERR_DRV_ERROR:
            p_msg = "FATALERR_DRV         ";
            break;
        case IMPFW_FATALERR_UNEXPECT_INT:
            p_msg = "FATALERR_UNEXPECT_INT";
            break;
        default:
            p_msg = "UNDEFINED            ";
            break;
    }
    return p_msg;
}
/**********************************************************************************************************************
 * End of function fatal_code_tostr
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: memory_barrier
 * Description  : call OSAL API to wait pending memory access.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
static void memory_barrier(void)
{
    e_osal_return_t  ret_osal;

    ret_osal = R_OSAL_ThsyncMemoryBarrier();
    if (OSAL_RETURN_OK != ret_osal)
    {
        LOGPRINT(("<< [ERROR] R_OSAL_ThsyncMemoryBarrier  : code %d>>\n",
            (int32_t)ret_osal)); /* cast to int32_t */
    }
    return;
}
/**********************************************************************************************************************
 * End of function memory_barrier
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: impfw_version
 * Description  : print version
 * Arguments    : None
 * Return Value : return 0 if success
 *              : return -1 if not success
 *********************************************************************************************************************/
static int32_t impfw_version(void)
{
    int   result;
    const st_impfw_version_t     * p_fw_version = R_IMPFW_GetVersion();
    result = IMPDEMO_OK;

    LOGPRINT(("IMPFW_VERSION_MAJOR:%u\n", IMPFW_VERSION_MAJOR));
    LOGPRINT(("IMPFW_VERSION_MINOR:%u\n", IMPFW_VERSION_MINOR));
    LOGPRINT(("IMPFW_VERSION_PATCH:%u\n", IMPFW_VERSION_PATCH));
    if (NULL != p_fw_version)
    {
        LOGPRINT(("R_IMPFW_GetVersion:%u.%u.%u\n",
            p_fw_version->major, p_fw_version->minor, p_fw_version->patch));
        p_fw_version = NULL;
    }
    else
    {
        result = IMPDEMO_NG;
    }
    return result;
}
/**********************************************************************************************************************
 * End of function impfw_version
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: imp_demo_impfw_init
 * Description  : sample of imp core
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
e_impfw_api_retcode_t imp_demo_impfw_init(impfw_ctrl_handle_t *p_impfwctl)
{
    e_impfw_api_retcode_t    impfw_ret;
    st_impfw_initdata_t      impfwinit;
    st_impfw_core_info_t     impfw_usecore[] = {
        IMPFWDEMO_USECORES
    };

    /*----- IMP Framework Initialization -----*/
    memset(&impfwinit,        0, sizeof(impfwinit));
    memset(&s_workmain_impfwinit, 0, sizeof(s_workmain_impfwinit));
    memset(&s_workattr_impfwinit, 0, sizeof(s_workattr_impfwinit));
    memset(&s_workque_impfwinit, 0, sizeof(s_workque_impfwinit));

    impfwinit.work_area_info[IMPFW_WORKAREA_TYPE_MAIN].p_work_addr = (void *)&s_workmain_impfwinit; /* cast to void * */
    impfwinit.work_area_info[IMPFW_WORKAREA_TYPE_MAIN].work_size   = sizeof(s_workmain_impfwinit);
    impfwinit.work_area_info[IMPFW_WORKAREA_TYPE_ATTR].p_work_addr = (void *)&s_workattr_impfwinit; /* cast to void * */
    impfwinit.work_area_info[IMPFW_WORKAREA_TYPE_ATTR].work_size   = sizeof(s_workattr_impfwinit);
    impfwinit.work_area_info[IMPFW_WORKAREA_TYPE_QUE] .p_work_addr = (void *)&s_workque_impfwinit; /* cast to void * */
    impfwinit.work_area_info[IMPFW_WORKAREA_TYPE_QUE] .work_size   = sizeof(s_workque_impfwinit);

    impfwinit.instance_num  = IMPFWDEMO_INSTANCE;
    impfwinit.use_core_num  = (sizeof(impfw_usecore)) / (sizeof(impfw_usecore[0]));
    impfwinit.core_info     = impfw_usecore;            
    impfwinit.callback_func_fatal = callbackfunc_fatal;
    impfwinit.fw_resource.max_queue_num  = IMPFWDEMO_MAXQUEUENUM;
    impfwinit.fw_resource.max_msg_num    = IMPFWDEMO_MAXMSGNUM;
    impfwinit.fw_resource.msg_id[0]      = OSAL_MQ_IMP_FW_TOTASK;
    impfwinit.fw_resource.msg_id[1]      = OSAL_MQ_IMP_FW_FROMTASK;
    impfwinit.fw_resource.mutex_id[0]    = OSAL_MUTEX_IMP_FW_STATE_LOCK;
    impfwinit.fw_resource.mutex_id[1]    = OSAL_MUTEX_IMP_FW_ATTR_LOCK;
    impfwinit.fw_resource.mutex_id[2]    = OSAL_MUTEX_IMP_FW_LOCK;
    impfwinit.fw_resource.task_id[0]     = OSAL_THREAD_IMP_FW;
    impfwinit.fw_resource.timeout        = 10000u;
    impfwinit.fw_resource.task_priority  = OSAL_THREAD_PRIORITY_TYPE6;
    impfwinit.fw_resource.task_stacksize = 16384u;
    impfwinit.drv_resource.mutex_id      = OSAL_MUTEX_IMP_DRV_LOCK;
    impfwinit.drv_resource.mutex_timeout = 10000u;
    impfwinit.drv_resource.int_priority  = OSAL_INTERRUPT_PRIORITY_TYPE7;

    impfw_ret = R_IMPFW_Init(&impfwinit, p_impfwctl);
    if (IMPFW_EC_OK != impfw_ret)
    {
        LOGPRINT(("<< [ERROR] r_impfw_Init  : code %d>>\n",
            (int32_t)impfw_ret)); /* cast to int32_t */
    }

    return impfw_ret;
}
/**********************************************************************************************************************
 * End of function imp_demo_impfw_init
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: imp_demo_impfw_execute
 * Description  : initialize impfw_ctrl_handle
 * Arguments    : impfw_ctrl_handle_t p_impfwctl
 *              : st_impfw_core_info_t *p_core
 *              : impfw_attr_handle_t impfw_attr
 * Return Value : e_impfw_api_retcode_t
 *********************************************************************************************************************/
e_impfw_api_retcode_t imp_demo_impfw_execute(impfw_ctrl_handle_t impfwctl, st_impfw_core_info_t *p_core, impfw_attr_handle_t impfw_attr)
{
    e_impfw_api_retcode_t    impfw_ret;
    void *                   callbackargs = &s_additional_data[s_request_num];

    s_additional_data[s_request_num].complete       = false;
    s_additional_data[s_request_num].request_num    = s_request_num;
    s_additional_data[s_request_num].core.core_type = p_core->core_type;
    s_additional_data[s_request_num].core.core_num  = p_core->core_num;

    impfw_ret = R_IMPFW_Execute(impfwctl, p_core, impfw_attr,
                                callbackfunc, callbackargs);
    if (IMPFW_EC_OK == impfw_ret)
    {
        s_request_num++;
    }

    return impfw_ret;
}

/**********************************************************************************************************************
 * End of function imp_demo_impfw_execute
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: imp_demo_impfw_waitcallback
 * Description  : wait all callback during 5sec.
 * Arguments    : None
 * Return Value : e_impfw_api_retcode_t
 *********************************************************************************************************************/
e_osal_return_t imp_demo_impfw_waitcallback(void)
{
    e_osal_return_t       ret_osal = OSAL_RETURN_FAIL;
    st_callbackmessage_t  message;
    st_additional_data_t  *data;
    uint32_t index;
    uint32_t complete_count;
    
    complete_count = 0U;
    while(complete_count < s_request_num)
    {
        ret_osal = R_OSAL_MqReceiveForTimePeriod(s_queuecallback, OSAL_MQ_TIMEOUT, &message, sizeof(message));
        if (OSAL_RETURN_OK != ret_osal)
        {
            LOGPRINT(("<< [ERROR] R_OSAL_MqReceiveForTimePeriod  : code %d>>\n",
                (int32_t)ret_osal)); /* cast to int32_t */
            break;
        }
        else
        {
            index = message.request_num;
            data  = &s_additional_data[index];
            LOGPRINT(("CallBack : status %s, Core type %d, Core num %d, callbackargs s_additional_data[%d] %d\n",
                reason_tostr(message.reason),
                (int32_t)(data->core.core_type), /* cast to int32_t */
                (int32_t)(data->core.core_num),  /* cast to int32_t */
                index, message.code));

            switch (message.reason)
            {
                case IMPFW_CB_TRAP:           /* fall through */
                case IMPFW_CB_INT:            /* fall through */
                case IMPFW_CB_INT_PBCOVF:     /* fall through */
                case IMPFW_CB_ERROR_ILLEGAL:  /* fall through */
                case IMPFW_CB_INT_SBO0ME:     /* fall through */
                case IMPFW_CB_TRAP_SBO0ME:
                    complete_count ++;
                    break;
                default:
                    break;
            }
        }
    }

    if (OSAL_RETURN_OK != ret_osal)
    {
        index = 0U;
        while ( index < s_request_num)
        {
            st_additional_data_t *data = &s_additional_data[index];
            if (false == data->complete)
            {
                LOGPRINT(("<< [ERROR] no callback Core type %d, Core num %d>>\n",
                    (int32_t)data->core.core_type,  /* cast to int32_t */
                    (int32_t)data->core.core_num)); /* cast to int32_t */
            }
            index ++;
        }
    }
    s_request_num = 0U;
    return ret_osal;
}
/**********************************************************************************************************************
 * End of function imp_demo_impfw_waitcallback
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: main
 * Description  : main
 * Arguments    : int argc
 *                char *argv[]
 * Return Value : int
 *********************************************************************************************************************/
int main(int argc, char * argv[])
{
    e_osal_return_t          ret_osal = OSAL_RETURN_FAIL;
    st_osal_mq_config_t      mq_config;
    st_osal_mq_config_t      mq_config_verify;
    int32_t                  result;
    uint32_t                 cnt;

    (void)argc;
    (void)argv;
    result = IMPDEMO_OK;

    /* init */
    ret_osal = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != ret_osal)
    {
        LOGPRINT(("<< [ERROR] R_OSAL_Initialize  : code %d>>\n",
            (int32_t)ret_osal)); /* cast to int32_t */
    }
    else
    {
        ret_osal = mmngr_open();

        if (OSAL_RETURN_OK == ret_osal)
        {
            mq_config.max_num_msg = IMPFWDEMO_MAXMSGNUM;
            mq_config.msg_size    = sizeof(st_callbackmessage_t);
            ret_osal = R_OSAL_MqCreate(&mq_config, OSAL_MQ_APPLICATIONID, &s_queuecallback);
            if (OSAL_RETURN_OK != ret_osal)
            {
                LOGPRINT(("<< [ERROR] R_OSAL_MqCreate  : code %d>>\n",
                    (int32_t)ret_osal)); /* cast to int32_t */
            }
            else
            {
                ret_osal = R_OSAL_MqGetConfig(s_queuecallback, &mq_config_verify);
                if (OSAL_RETURN_OK != ret_osal)
                {
                    LOGPRINT(("<< [ERROR] R_OSAL_MqGetConfig  : code %d>>\n",
                        (int32_t)ret_osal)); /* cast to int32_t */
                }
                else
                {
                    if ((mq_config.max_num_msg >  mq_config_verify.max_num_msg) ||
                        (mq_config.msg_size    != mq_config_verify.msg_size))
                    {
                        LOGPRINT(("<< [ERROR] Requested Message queue invalid config  : %d, %d [ %d, %d]>>\n",
                            (int32_t)mq_config_verify.max_num_msg,  /* cast to int32_t */
                            (int32_t)mq_config_verify.msg_size,     /* cast to int32_t */
                            (int32_t)mq_config.max_num_msg,         /* cast to int32_t */
                            (int32_t)mq_config.msg_size));          /* cast to int32_t */
                        ret_osal = OSAL_RETURN_CONF;
                    }
                }
            }
        }
    }

    if (OSAL_RETURN_OK != ret_osal)
    {
        result = IMPDEMO_NG;
    }
    else
    {
        LOGPRINT(("IMP Framework Demo start\n\n"));

        cnt = 0U;
        while((sizeof(s_sample_list)/sizeof(s_sample_list[0])) > cnt)
        {
            if (0U != s_sample_list[cnt].enable)
            {
                LOGPRINT(("\n%s\n", s_sample_list[cnt].text));
                result = s_sample_list[cnt].sample_main();
                if (result != IMPDEMO_OK)
                {
                    break;
                }
            }
            cnt++;
        }

       LOGPRINT(("IMP Framework Demo end\n\n"));
    }

    /* quit */
    ret_osal = R_OSAL_MqDelete(s_queuecallback);
    if (OSAL_RETURN_OK != ret_osal)
    {
        result = IMPDEMO_NG;
        LOGPRINT(("<< [ERROR] R_OSAL_MqDelete  : code %d>>\n",
            (int32_t)ret_osal)); /* cast to int32_t */
    }

    ret_osal = mmngr_close();
    if (OSAL_RETURN_OK != ret_osal)
    {
        result = IMPDEMO_NG;
    }
    ret_osal = R_OSAL_Deinitialize();
    if (OSAL_RETURN_OK != ret_osal)
    {
        LOGPRINT(("<< [ERROR] R_OSAL_Deinitialize  : code %d>>\n",
            (int32_t)ret_osal)); /* cast to int32_t */
        result = IMPDEMO_NG;
    }
    printf("return %d\n", result);
    return result;
}
/**********************************************************************************************************************
 * End of function main
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: imp_demo
 * Description  : sample of imp core
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
static int32_t imp_demo(void)
{
    /* IMP Framework */
    impfw_ctrl_handle_t      impfwctl;
    int32_t                  result;

    impfw_attr_handle_t        impfw_imp0attr;
    e_impfw_api_retcode_t      impfw_ret;
    e_impfw_req_priority_t     priority = IMPFW_REQ_PRIORITY_0;


    st_impfw_core_info_t         core;
    st_buffer_t              cl_buffer;
    st_buffer_t              imga_buffer;
    uint32_t                       cl_size;
    uint32_t                       hwAddr;

    /* IMP Atomic */
    R_ATMLIB_RETURN_VALUE    atomic_ret;

    uint32_t                 xsize;
    uint32_t                 ysize;
    R_ATMLIB_CLData          cldata;
    R_ATMLIB_IMPSubsetParam2 param;
    R_ATMLIB_ImageParam      imgparam;

    /* OSAL */
    e_osal_return_t ret_osal = OSAL_RETURN_OK;

    /*----- CL Initialization -----*/
    /* Allocate Image memory */

    result = IMPDEMO_OK;
    memset(&imga_buffer, 0, sizeof(imga_buffer));
    memset(&cl_buffer, 0, sizeof(cl_buffer));

    xsize = 16u;
    ysize = 16u;

    ret_osal = buffer_alloc(&imga_buffer,
        xsize * ysize * (uint32_t)sizeof(uint32_t)); /* cast to uint32_t */
    if (OSAL_RETURN_OK == ret_osal)
    {
        fillmemory((void *)imga_buffer.cpu_addr, /* cast to void * */
            0, (uint32_t)(xsize * ysize * sizeof(uint32_t)));

        hwAddr = (uint32_t)imga_buffer.hw_addr; /* cast to uint32_t */
#if 0
        LOGPRINT(("imga_virt:    0x%p\n",   imga_buffer.cpu_addr));
        LOGPRINT(("imga_phys:    0x%08x\n", hwAddr));
#endif
        ret_osal = buffer_sync(&imga_buffer);
    }

    if (OSAL_RETURN_OK != ret_osal)
    {
        result = IMPDEMO_NG;
    }
    else
    {
        cl_size = 256u * sizeof(uint32_t);
        ret_osal = buffer_alloc(&cl_buffer, cl_size);
        if (OSAL_RETURN_OK == ret_osal)
        {
            fillmemory(cl_buffer.cpu_addr, 0, cl_size);

            hwAddr = (uint32_t)cl_buffer.hw_addr; /* cast to uint32_t */
#if 0
            LOGPRINT(("cl_addr_virt:\t0x%p\n", cl_buffer.cpu_addr));
            LOGPRINT(("cl_addr_phys:\t0x%08x\n", hwAddr));
#endif
        }

        if (OSAL_RETURN_OK == ret_osal)
        {
            ret_osal = OSAL_RETURN_FAIL;

            /*----- Atomic Initialization -----*/
            LOGPRINT(("Execute Atomic processing Start\n"));

            /* === Initialize memories === */
            memset(&cldata, 0, sizeof(R_ATMLIB_CLData));
            memset(&param, 0, sizeof(R_ATMLIB_IMPSubsetParam2));
            memset(&imgparam, 0, sizeof(R_ATMLIB_ImageParam));

            atomic_ret = r_atmlib_InitializeIMPCL(&cldata,
                (uint32_t *)cl_buffer.cpu_addr, /* cast to uint32_t * */
                cl_size / sizeof(uint32_t));
            if (R_ATMLIB_E_OK != atomic_ret)
            {
                LOGPRINT(("<< [ERROR] r_atmlib_InitializeIMPCL  : code %d>>\n",
                    (int32_t)atomic_ret)); /* cast to int32_t */
            }

            if (R_ATMLIB_E_OK == atomic_ret)
            {
                /*----- Atomic CL Creation -----*/
                /* Set IMP Subset */
                param.leng        = (ysize << 16u) | xsize;
                param.mag         = ((uint32_t)1u << 16u) | (uint32_t)1u; /* cast to uint32_t */
                param.scale       = 0u;
                param.const_a     = 0xAA;
                param.const_b     = 0x00;
                param.binthr_min  = 0x00;
                param.binthr_max  = 0x00;
                param.opt         = 0u;
                param.srca_unpack = R_ATMLIB_DISABLE;
                param.srcb_unpack = R_ATMLIB_DISABLE;
                param.dst_pack    = R_ATMLIB_DISABLE;

                atomic_ret = r_atmlib_SetDataIMPCL2(&cldata, R_ATMLIB_IMP_CONST, &param);
                if (R_ATMLIB_E_OK != atomic_ret)
                {
                    LOGPRINT(("<< [ERROR] r_atmlib_SetDataIMPCL  : code %d>>\n",
                        atomic_ret)); /* cast to int32_t */
                }
            }

            if (R_ATMLIB_E_OK == atomic_ret)
            {
                /* Set Image */
                imgparam.srca_type   = R_ATMLIB_IMG_8U;
                imgparam.dst_type    = R_ATMLIB_IMG_8U;
                imgparam.srca_stride = (int32_t)xsize;
                imgparam.dst_stride  = (int32_t)xsize;
                imgparam.srca_addr   = (uint32_t)imga_buffer.hw_addr; /* cast to uint32_t */
                imgparam.dst_addr    = (uint32_t)imga_buffer.hw_addr; /* cast to uint32_t */

                atomic_ret = r_atmlib_SetImageIMPCL(&cldata, &imgparam);
                if (R_ATMLIB_E_OK != atomic_ret)
                {
                    LOGPRINT(("<< [ERROR] r_atmlib_SetImageIMPCL  : code %d>>\n",
                        atomic_ret)); /* cast to int32_t */
                }
            }

            if (R_ATMLIB_E_OK == atomic_ret)
            {
                /*----- Atomic Finalization -----*/
                atomic_ret = r_atmlib_FinalizeIMPCL(&cldata);
                if (R_ATMLIB_E_OK != atomic_ret)
                {
                    LOGPRINT(("<< [ERROR] r_atmlib_FinalizeIMPCL  : code %d>>\n",
                        atomic_ret)); /* cast to int32_t */
                }
                else
                {
                    ret_osal = buffer_sync(&cl_buffer);
                }
            }
            LOGPRINT(("Execute Atomic processing End\n\n"));
        }
    }

    if (OSAL_RETURN_OK != ret_osal)
    {
        result = IMPDEMO_NG;
    }
    else
    {
        /*----- IMP Framework Initialization -----*/
        impfw_ret = imp_demo_impfw_init(&impfwctl);
        if (IMPFW_EC_OK != impfw_ret)
        {
            LOGPRINT(("<< [ERROR] r_impfw_Init  : code %d>>\n",
                (int32_t)impfw_ret)); /* cast to int32_t */
        }

        /*----- IMP Framework Execution -----*/
        if (IMPFW_EC_OK == impfw_ret)
        {
            LOGPRINT(("Execute R_IMPFW_Execute Start\n"));

            core.core_type = IMPFW_CORE_TYPE_IMP;
            core.core_num  = 0;

            impfw_ret = R_IMPFW_AttrInit(impfwctl, &core, &impfw_imp0attr);
            if (IMPFW_EC_OK != impfw_ret)
            {
                LOGPRINT(("<< [ERROR] R_IMPFW_AttrInit  : code %d>>\n",
                    (int32_t)impfw_ret)); /* cast to int32_t */
            }
        }

        if (IMPFW_EC_OK == impfw_ret)
        {
            impfw_ret = R_IMPFW_AttrSetCl(impfw_imp0attr, hwAddr, cl_size, priority);
            if (IMPFW_EC_OK != impfw_ret)
            {
                LOGPRINT(("<< [ERROR] R_IMPFW_AttrSetCL  : code %d>>\n",
                    (int32_t)impfw_ret)); /* cast to int32_t */
            }
        }

        if (IMPFW_EC_OK == impfw_ret)
        {
            impfw_ret = imp_demo_impfw_execute(impfwctl, &core, impfw_imp0attr);
            if (IMPFW_EC_OK != impfw_ret)
            {
                LOGPRINT(("<< [ERROR] R_IMPFW_Execute  : code %d>>\n",
                    (int32_t)impfw_ret)); /* cast to int32_t */
            }
        }
        LOGPRINT(("Execute R_IMPFW_Execute End\n\n"));

        if (IMPFW_EC_OK == impfw_ret)
        {
            /* Wait for CL end before r_impfw_Quit */
            ret_osal = imp_demo_impfw_waitcallback();
            if (OSAL_RETURN_OK == ret_osal)
            {
                outputmemory(
                    (void *)imga_buffer.cpu_addr, /* cast to void * */
                    xsize, ysize, 1);
            }
            else
            {
                impfw_ret = IMPFW_EC_NG_TIMEOUT;
            }
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

        if (IMPFW_EC_OK != impfw_ret)
        {
            result = IMPDEMO_NG;
        }
    }

    if (NULL != cl_buffer.cpu_addr)
    {
        ret_osal = buffer_free(&cl_buffer);
        if (OSAL_RETURN_OK != ret_osal)
        {
            result = IMPDEMO_NG;
        }
    }

    if (NULL != imga_buffer.cpu_addr)
    {
        ret_osal = buffer_free(&imga_buffer);
        if (OSAL_RETURN_OK != ret_osal)
        {
            result = IMPDEMO_NG;
        }
    }

    return result;
}
/**********************************************************************************************************************
 * End of function imp_demo
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: callbackfunc
 * Description  : callback of IMPFW
 * Arguments    : e_impfw_callback_reason_t reason
 *                const st_impfw_core_info_t * const core
 *                int32_t code
 *                void const * callbackargs
 * Return Value : None
 *********************************************************************************************************************/
static int32_t callbackfunc(e_impfw_callback_reason_t reason,        /* CL execution Callback status     */
    const st_impfw_core_info_t * const core, /* Core ID related to this callback */
    int32_t code,                            /* INT code or TRAP code or -1(unavailable) */
    void * const callbackargs)               /* The pointer to user data */
{
    /* OSAL */
    e_osal_return_t ret_osal;
    st_additional_data_t *data;
    uint32_t             index;
    st_callbackmessage_t message;

    data = (st_additional_data_t *)callbackargs;

    if ((data->core.core_type != core->core_type) || (data->core.core_num != core->core_num))
    {
        LOGPRINT(("<< [ERROR] callbackargs invalid>>\n"));
    }
    else
    {
        switch (reason)
        {
            case IMPFW_CB_STARTED:
                memory_barrier();
                break;
            case IMPFW_CB_TRAP:           /* fall through */
            case IMPFW_CB_INT:            /* fall through */
            case IMPFW_CB_INT_PBCOVF:     /* fall through */
            case IMPFW_CB_ERROR_ILLEGAL:  /* fall through */
            case IMPFW_CB_INT_SBO0ME:     /* fall through */
            case IMPFW_CB_TRAP_SBO0ME:
                data->complete = true;
                break;

            case IMPFW_CB_ERROR_INTERNAL: /* fall through */
            case IMPFW_CB_USIER:
                ; /* fatal error */
                break;

            case IMPFW_CB_END:            /* fall through */
            case IMPFW_CB_WUPCOV:         /* fall through */
            case IMPFW_CB_PBCOVF:         /* fall through */
            case IMPFW_CB_HPINT:          /* fall through */
            case IMPFW_CB_APIPINT:        /* fall through */
            case IMPFW_CB_MSCO:           /* fall through */
            case IMPFW_CB_DRVERR:         /* fall through */
            case IMPFW_CB_RTTERR:         /* fall through */
            case IMPFW_CB_RESOURCEFULL:   /* fall through */
            default:
                ; /* nothing to do */
                break;
        }

        index = data->request_num;

        message.request_num = index;
        message.reason      = reason;
        message.code        = code;

        ret_osal = R_OSAL_MqSendForTimePeriod(s_queuecallback, OSAL_MQ_TIMEOUT, &message, sizeof(message));
        if (OSAL_RETURN_OK != ret_osal)
        {
            LOGPRINT(("<< [ERROR] R_OSAL_MqSendForTimePeriod  : code %d>>\n",
            (int32_t)ret_osal)); /* cast to int32_t */
        }
    }
    return ((int32_t)0);
}
/**********************************************************************************************************************
 * End of function callbackfunc
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: callbackfunc_fatal
 * Description  : callback of fatal
 * Arguments    : e_impfw_fatalcode_t  reason
 *                uint32_t value
 * Return Value : None
 *********************************************************************************************************************/
static void callbackfunc_fatal(e_impfw_fatalcode_t error, uint32_t code)
{
    LOGPRINT(("callbackfunc_fatal : fatalcond:%s, code:%u\n",
        fatal_code_tostr(error), code));
}
/**********************************************************************************************************************
 * End of function callbackfunc_fatal
 ********************************************************************************************************************/
