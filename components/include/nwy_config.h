/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _NWY_CONFIG_H_
#define _NWY_CONFIG_H_

// Auto generated. Don't edit it manually!

#define FEATURE_NWY_AT
#define FEATURE_NWY_PROJ_N58

#define FEATURE_NWY_AT_NEMA_UART
#define FEATURE_NWY_AT_NEMA_USB
//#define CFW_VOLTE_SUPPORT
#define FEATURE_NWY_PROJ_N716

#if defined(FEATURE_NWY_PROJ_N58)

#define IVSN_STR "N716-R08-STD-OE-002"    /*modify by lijintao in 2021.07.07*/
#define CGMR_STR "N716-R08-STD-OE-002"
#define ATI_STR  "NEOWAY\r\nN716\r\nV002"
#define CGMI_STR "Neoway Corp Ltd"
#define CGMM_STR "N716"
#define OPEN_SDK_VER_STR "N716-OPEN-SDK-03A"
#define nwy_hw_version "V2.0"
#define nwy_sfv_time "041221"
#define nwy_hwv_time "020919"
#define LINEVER_STR  "8910_Module_V1_4_W21.42.2"


// ------EJTTS ------
/* #undef CONFIG_NWY_AT_EJTTS_SUPPORT */
#ifdef CONFIG_NWY_AT_EJTTS_SUPPORT
#define FEATURE_NWY_TTS
#define FEATURE_NWY_AT_EJTTS_SUPPORT
#endif

// ------IVTTS ------
/* #undef CONFIG_NWY_AT_IVTTS_SUPPORT */
#ifdef CONFIG_NWY_AT_IVTTS_SUPPORT
#define FEATURE_NWY_TTS
#define FEATURE_NWY_AT_IVTTS_SUPPORT
#endif

// ------NWY STD AT ------
#define CONFIG_NWY_AT_STD_SUPPORT
#ifdef CONFIG_NWY_AT_STD_SUPPORT
#define FEATURE_NWY_AT_STD
#endif/*CONFIG_NWY_AT_STD_SUPPORT*/

// ------NWY NGNS AT ------
/* #undef CONFIG_NWY_AT_NGNS_SUPPORT */
#ifdef CONFIG_NWY_AT_NGNS_SUPPORT
#define FEATURE_NWY_AT_NGNS
#endif/*CONFIG_NWY_AT_NGNS_SUPPORT*/

// ------NWY FILESYS AT ------
#define CONFIG_NWY_AT_FILESYS
#ifdef CONFIG_NWY_AT_FILESYS
#define FEATURE_NWY_AT_FILESYS
#endif

// ------NWY OPEN SDK ------
#define CONFIG_NWY_OPEN_SDK_SUPPORT
#ifdef CONFIG_NWY_OPEN_SDK_SUPPORT
#define FEATURE_NWY_OPEN_SDK
#define FEATURE_NWY_WEFOTA_ENABLE
#define FEATURE_NWY_NO_SIM_DETECT   //add by lijintao for close sim hotswap
#endif/*CONFIG_NWY_OPEN_SDK_SUPPORT*/

// -----NWY NETWORK AT-----
#ifdef FEATURE_NWY_AT
#define FEATURE_NWY_AT_BANDLOCK
#define	FEATURE_NWY_AT_NBANDLOCK
#define FEATURE_NWY_AT_NFREQLOCK
#define FEATURE_NWY_AT_MYDATAONLY
#define FEATURE_NWY_SIM_CACHE
#endif

// ------BT ------
#define CONFIG_NWY_AT_BT_SUPPORT
#ifdef CONFIG_NWY_AT_BT_SUPPORT
#define FEATURE_NWY_AT_BT
#endif

// ------PAHO MQTT ------
/* #undef CONFIG_NWY_AT_MQTT_SUPPORT */
//#ifdef CONFIG_NWY_AT_MQTT_SUPPORT
//#define FEATURE_NWY_PAHO_MQTT
//#endif

// -----HP-----
#define CONFIG_NWY_HEADPHONE_DETECT
#ifdef CONFIG_NWY_HEADPHONE_DETECT
#define FEATURE_NWY_HEADPHONE_DETECT
#endif

#define FEATURE_NWY_OPEN_LITE

#endif/*FEATURE_NWY_PROJ_N58*/
#endif/*_NWY_CONFIG_H_*/

