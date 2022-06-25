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
/*add by neoway start*/
#include "nwy_osi_api.h"
#include "nwy_config.h"
#ifdef FEATURE_NWY_AT_HX_GNSS
#include "nwy_loc_hx.h"
#else
#include "nwy_loc.h"
#endif
#include "nwy_voice.h"
#include "nwy_sim.h"
#include "nwy_wifi.h"
#include "nwy_vir_at.h"
#include "osi_log.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "nwy_sms.h"
#include "nwy_data.h"
#include "nwy_usb_serial.h"
#include "nwy_adc.h"
#include "nwy_led.h"
#include "nwy_gpio.h"
#include "nwy_gpio_open.h"
#include "nwy_fota.h"
#include "nwy_fota_api.h"
#include "nwy_uart.h"
#include "nwy_spi.h"
#include "nwy_i2c.h"
#include "nwy_pm.h"
#include "nwy_keypad.h"
#include "stdio.h"
#include "nwy_network.h"
#include "nwy_file.h"
#include "nwy_audio_api.h"
#include "nwy_socket.h"
//#include "nwy_alimqtt.h"
#include "mqtt_api.h"
#include "nwy_osi_api.h"
#include "nwy_ftp.h"
#include "nwy_http.h"
#include "MQTTClient.h"
#include "nwy_mqtt.h"
/* added by wangchen for N58 ussd api to test 20200826 begin */
#include "nwy_ussd.h"
/* added by wangchen for N58 ussd api to test 20200826 end */
#include "nwy_pwm.h"
#include "nwy_dm.h"
#include "nwy_ble.h"
#ifdef FEATURE_NWY_OPEN_ZZD_SDK
#include "nwy_oem_zzd_api.h"
#endif
#include "nwy_ip_packet.h"
#include "nwy_poc_dsds.h"
/* added by wangchen for stk openapi 2021.08.26 begin */
#ifdef FEATURE_NWY_STK_COMMAND
#include "nwy_sat.h"
#endif
/* added by wangchen for stk openapi 2021.08.26 end */

#include "prj.h"




char File_Name_White_1[]="White_Name_1";
char File_Name_White_2[]="White_Name_2";
char File_Name_White_3[]="White_Name_3";
#define SN_LEN 8 
typedef struct {
   // char Num[6];///num00=
    char sn[SN_LEN];
}White_Name_Format;


White_Name_Format File_White_Num[100];


int IsValide_Sn(char *name) {
    return 1 ;
}


int add_name(char *name) {
	int ls = nwy_sdk_vfs_ls();
    char *pt;
    int fd = nwy_sdk_fopen(File_Name_White_1, NWY_CREAT | NWY_RDWR | NWY_TRUNC);
    int len = nwy_sdk_fread(fd, File_White_Num, sizeof(File_White_Num));

    pt = (char *)File_White_Num;
    int index = 0;

    while((*pt) != 0) {
        if(strcmp(pt,name) == 0) {
            nwy_ext_echo("\r\nAlready exist\r\n"); 
            return -1;             
        } else {
            pt+= SN_LEN;
            index++;
        }
    }
    strcat(File_White_Num,name);
    len = nwy_sdk_fwrite(fd, File_White_Num, strlen(File_White_Num));
    nwy_sdk_fclose(fd);
    if(len != strlen(File_White_Num))
        nwy_ext_echo("\r\nfile write: len=%d, return len=%d\r\n",strlen(File_White_Num), len);
    else
        nwy_ext_echo("\r\nfile write success and close\r\n");
    return index;
}


int Del_Name(char *name) {
	int ls = nwy_sdk_vfs_ls();
    char *pt;
    int fd = nwy_sdk_fopen(File_Name_White_1, NWY_CREAT | NWY_RDWR | NWY_TRUNC);
    int len = nwy_sdk_fread(fd, File_White_Num, sizeof(File_White_Num));

    pt = (char *)File_White_Num;
    int index = 0;

    char *pt_Last;

    while(*pt != 0) {
        if(strcmp(pt,name) == 0) {
            pt_Last = pt + SN_LEN;

            memset(pt, 0, SN_LEN);

            strcat(File_White_Num,pt_Last);

            nwy_ext_echo("\r\nDel succ--\r\n"); 
            len = nwy_sdk_fwrite(fd, File_White_Num, strlen(File_White_Num));
            nwy_sdk_fclose(fd);
            if(len != strlen(File_White_Num))
                nwy_ext_echo("\r\nfile write: len=%d, return len=%d\r\n",strlen(File_White_Num), len);
            else
                nwy_ext_echo("\r\nfile write success and close\r\n");
            return 0;          
        } else {
            pt+= SN_LEN;
            index++;
        }
    }
    nwy_ext_echo("\r\nNot exist\r\n"); 
    return -1;    
}


void nwy_file_test_mine(void)
{
    char* sptr = nwy_ext_sio_recv_buff;
    char file_name[NWY_FILE_NAME_MAX + 1] = {0};
    char context[NWY_UART_RECV_SINGLE_MAX + 1] = {0};
	int ls = nwy_sdk_vfs_ls();
    int len = 0,fd =-1, size =0;
    nwy_ext_echo("\r\nfilesystem statistics:%d\r\n", ls);

    while(1) {
        nwy_ext_input_gets("\r\nPlease input filename(len <= 32): ");
        if (strlen(sptr) != SN_LEN) {
            nwy_ext_echo("\r\nErro_Length");
            break;
        }
        add_name(sptr);
    }

}

