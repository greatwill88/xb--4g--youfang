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

#include "prj.h"
#include "nwy_test_cli_func_def.h"


uint8_t Poll_Addr_id = 1;


nwy_osiSemaphore_t      *s_Call_OK_semaphore = NULL;


nwy_osiThread_t *g_app_Call_thread = NULL;
nwy_osiThread_t *g_app_Poll_Addr_thread = NULL;
nwy_osiThread_t *g_RS485_Ctrl_thread = NULL;
static void nwy_485_recv_handle_1 (const char *str, uint32_t length);
static void nwy_485_recv_handle_2 (const char *str, uint32_t length);

#define  RS485_RTS_1 23 
#define  RS485_RTS_2 19 
int RS485_hd[2]={-1,-1};

char xb_SubDev_SN[32][12];
#define EVENT_REC_485 0x55AA

uint8_t poll_Cmd[5];
#define RS_485_DEV 2 
#define RS_485_OUT 1 
uint8_t poll_id = 1;
uint8_t poll_Ctrl_Cmd[5];
uint8_t thread_Fg = 0 ;

typedef struct {
  uint8_t type;
  uint8_t sn[12];
}Sub_Dev_Info;
uint8_t Dev_Num = 0;

/* extern void nwy_data_cb_fun(
    int hndl,
    nwy_data_call_state_t ind_state); */




static void nwy_data_cb_fun(
    int hndl,
    nwy_data_call_state_t ind_state)
{
  OSI_LOGI(0, "=DATA= hndl=%d,ind_state=%d", hndl,ind_state);
  if (hndl > 0 && hndl <= 8)
  {
    ppp_state[hndl-1] = ind_state;
    nwy_ext_echo("\r\nData call status update, handle_id:%d,state:%d\r\n",hndl,ind_state);
  }
}



void prvThreadEntry_Call(void *param)
{
  int ret = -1;
  char* sptr = nwy_ext_sio_recv_buff;
  nwy_data_profile_info_t profile_info;

    static int wait = 0;

    while(wait < 18) {
        wait++;
        nwy_sleep(1000);
        nwy_ext_echo("Call_Thread_Wait == %d", wait);
    }
  start_Ble_Scan_Thread();
    // nwy_test_cli_ble_open();

/*     if(nwy_ble_client_set_enable(1))
        nwy_ext_echo("Ble Client Open");
    else
        nwy_ext_echo("Ble Client Close");
     nwy_test_cli_ble_client_scan_Para("49","1"); */


  while(1) {


    int hndl = nwy_data_get_srv_handle(nwy_data_cb_fun);
    OSI_LOGI(0, "=DATA=  hndl= %d", hndl);
/* Begin: Add by yjj for TD83000 in 2021.05.12*/
    if ((hndl > 0) &&(hndl <= NWY_DATA_CALL_MAX_NUM))
        nwy_ext_echo("\r\nCreate a Resource Handle id: %d success\r\n",hndl);
    else
        nwy_ext_echo("\r\nCreate a Resource Handle failed, result<%d>\r\n",hndl);
/* End: Add by yjj for TD83000 in 2021.05.12*/


//////step :3
    memset(&profile_info,0,sizeof(nwy_data_profile_info_t));
    //nwy_ext_input_gets("\r\nPlease input profile info: profile_id <1-7>");
    int profile_id =  1;//atoi(sptr);
    //nwy_ext_input_gets("\r\nPlease input profile info: auth_proto <0-2>\r\n0:NONE, 1:PAP, 2:CHAP\r\n");
    int auth_proto =  0;

    profile_info.auth_proto = auth_proto;

    //nwy_ext_input_gets("\r\nPlease input profile info: pdp_type <1-3,6>\r\n1:IPV4, 2:IPV6, 3:IPV4V6, 6:PPP\r\n");
    int pdp_type = 1;//atoi(sptr);
    profile_info.pdp_type = pdp_type;


    memcpy(profile_info.apn,"3gnet",sizeof(profile_info.apn));

    memset(nwy_ext_sio_recv_buff,0,sizeof(nwy_ext_sio_recv_buff));
    memcpy(nwy_ext_sio_recv_buff,"kuang",sizeof(nwy_ext_sio_recv_buff));

   // nwy_ext_sio_recv_buff[0]= "kuang";//'\"';
    //nwy_ext_sio_recv_buff[1]= 0;//'\"';
   // nwy_ext_sio_recv_buff[2]= 0;
    memcpy(profile_info.user_name,nwy_ext_sio_recv_buff,sizeof(profile_info.user_name));
    memcpy(profile_info.pwd,nwy_ext_sio_recv_buff,sizeof(profile_info.pwd));

    nwy_ext_echo("Set_Profile_Profile:%d",1);
    ret = nwy_data_set_profile(profile_id,NWY_DATA_PROFILE_3GPP,&profile_info);
    nwy_ext_echo("=DATA=  nwy_data_set_profile ret= %d", ret);
    if (ret != NWY_RES_OK)
        nwy_ext_echo("\r\nSet profile %d info fail, result<%d>\r\n",profile_id,ret);
    else
        nwy_ext_echo("\r\nSet profile %d info success\r\n",profile_id);

  //  nwy_sleep(5000);
////step 4: start call, 
    nwy_data_start_call_v02_t param_t;
    memset(&param_t,0,sizeof(nwy_data_start_call_v02_t));

  //  hndl = 1;
    param_t.profile_idx = 1;
    param_t.is_auto_recon = 1;
    param_t.auto_re_max = 0;
    param_t.auto_interval_ms = 6000;
   ret = nwy_data_start_call(hndl,&param_t);
    nwy_ext_echo("DATA=  nwy_data_start_call ret= %d", ret);
    if (ret != NWY_RES_OK)
        nwy_ext_echo("\r\nStart data call fail, result<%d>\r\n",ret);
    else
        nwy_ext_echo("\r\nStart data call ...\r\n");

    while(ppp_state[hndl-1]!= 1) {
      nwy_sleep(100);
      nwy_ext_echo("\r\nWait_Connect...\r\n");
    }

/////step 5:get info
    nwy_data_addr_t_info addr_info;
    // nwy_ext_input_gets("\r\nPlease select resource handle id <1-%d>",NWY_DATA_CALL_MAX_NUM);
//     hndl = 1;
#if 1//TODO
    int len = 0;
    memset(&addr_info,0,sizeof(nwy_data_addr_t_info));
    OSI_LOGI(0, "=DATA=  addr_info size= %d",sizeof(nwy_data_addr_t_info));
    ret = nwy_data_get_ip_addr(hndl, &addr_info, &len);
    OSI_LOGI(0, "=DATA=  nwy_data_get_ip_addr = %d|len%d", ret,len);
    if (ret != NWY_RES_OK)
    {
        nwy_ext_echo("\r\nGet data info fail, result<%d>\r\n",ret);
        
    }
#endif
    nwy_ext_echo("\r\nGet data info success\r\nIface address: %s,%s\r\n",
        nwy_ip4addr_ntoa(&addr_info.iface_addr_s.ip_addr.addr),
        nwy_ip6addr_ntoa(&addr_info.iface_addr_s.ip6_addr.addr));
    nwy_ext_echo("Dnsp address: %s,%s\r\n",
        nwy_ip4addr_ntoa(&addr_info.dnsp_addr_s.ip_addr.addr),
        nwy_ip6addr_ntoa(&addr_info.dnsp_addr_s.ip6_addr.addr));
    nwy_ext_echo("Dnss address: %s,%s\r\n",
        nwy_ip4addr_ntoa(&addr_info.dnss_addr_s.ip_addr.addr),
        nwy_ip6addr_ntoa(&addr_info.dnss_addr_s.ip6_addr.addr));

       // nwy_sleep(60000);
       // nwy_sleep(60000);
        // s_Call_OK_semaphore = nwy_semaphore_create(10, 1);
        g_app_thread_xb = nwy_create_thread("mqttthread", prvThreadEntry_xb_Connect, NULL, NWY_OSI_PRIORITY_NORMAL, 1024*10, 16);
       break ;
    }

    nwy_exit_thread();
            
#if 0
  while (1)
  {

      switch(atoi(sptr))
      {
          case 1:
          {
              int hndl = nwy_data_get_srv_handle(nwy_data_cb_fun);
              OSI_LOGI(0, "=DATA=  hndl= %d", hndl);
/* Begin: Add by yjj for TD83000 in 2021.05.12*/
              if ((hndl > 0) &&(hndl <= NWY_DATA_CALL_MAX_NUM))
                nwy_ext_echo("\r\nCreate a Resource Handle id: %d success\r\n",hndl);
              else
                nwy_ext_echo("\r\nCreate a Resource Handle failed, result<%d>\r\n",hndl);
/* End: Add by yjj for TD83000 in 2021.05.12*/
              break;
          }
          case 2:
          {
            nwy_ext_input_gets("\r\nPlease input profile index: (1-7)");
            int profile_id = atoi(sptr);
            if ((profile_id <= 0) || (profile_id > 7)){
              nwy_ext_echo("\r\nInvaild profile id: %d\r\n",profile_id);
              break;
            }
            memset(&profile_info,0,sizeof(nwy_data_profile_info_t));
            ret = nwy_data_get_profile(profile_id,NWY_DATA_PROFILE_3GPP,&profile_info);
            OSI_LOGI(0, "=DATA=  nwy_data_get_profile ret= %d", ret);
            OSI_LOGI(0, "=DATA=  profile= %d|%d", profile_info.pdp_type,profile_info.auth_proto);
            OSI_PRINTFI("=DATA=   profile= %s|%s|%s", profile_info.apn,profile_info.user_name,profile_info.pwd);
            if (ret != NWY_RES_OK){
              nwy_ext_echo("\r\nRead profile %d info fail, result%d\r\n",profile_id,ret);
            }
            else {
                nwy_ext_echo("\r\nProfile %d info: <pdp_type>,<auth_proto>,<apn>,<user_name>,<password>\r\n%d,%d,%s,%s,%s\r\n",profile_id,profile_info.pdp_type,
                profile_info.auth_proto,profile_info.apn,profile_info.user_name,profile_info.pwd);
            }
            break;
          }
          case 3:
          {
            memset(&profile_info,0,sizeof(nwy_data_profile_info_t));
            nwy_ext_input_gets("\r\nPlease input profile info: profile_id <1-7>");
            int profile_id = atoi(sptr);
            if ((profile_id <= 0) || (profile_id > 7)){
              nwy_ext_echo("\r\nInvaild profile id: %d\r\n",profile_id);
              break;
            }

            nwy_ext_input_gets("\r\nPlease input profile info: auth_proto <0-2>\r\n0:NONE, 1:PAP, 2:CHAP\r\n");
            int auth_proto = atoi(sptr);
            if ((auth_proto < 0) || (auth_proto > 2)){
              nwy_ext_echo("\r\nInvaild auth_proto value: %d\r\n",auth_proto);
              break;
            }
            profile_info.auth_proto = auth_proto;

            nwy_ext_input_gets("\r\nPlease input profile info: pdp_type <1-3,6>\r\n1:IPV4, 2:IPV6, 3:IPV4V6, 6:PPP\r\n");
            int pdp_type = atoi(sptr);
            if ((pdp_type != 1) && (pdp_type !=2) && (pdp_type !=3) && (pdp_type !=6)){
              nwy_ext_echo("\r\nInvaild pdp_type value: %d\r\n",pdp_type );
              break;
            }
            profile_info.pdp_type = pdp_type;

            nwy_ext_input_gets("\r\nPlease input profile info: apn (length 0-%d)\r\n",NWY_APN_MAX_LEN);
            if (strlen(nwy_ext_sio_recv_buff) > NWY_APN_MAX_LEN) {
              nwy_ext_echo("\r\nInvaild apn len\r\n");
              break;
            }
            if (strlen(nwy_ext_sio_recv_buff) > 0)
              memcpy(profile_info.apn,nwy_ext_sio_recv_buff,sizeof(profile_info.apn));

            nwy_ext_input_gets("\r\nPlease input profile info: user name (length 0-%d)\r\n",NWY_APN_USER_MAX_LEN);
            if (strlen(nwy_ext_sio_recv_buff) > NWY_APN_USER_MAX_LEN) {
              nwy_ext_echo("\r\nInvaild user name len\r\n");
              break;
            }
            if (strlen(nwy_ext_sio_recv_buff) > 0)
              memcpy(profile_info.user_name,nwy_ext_sio_recv_buff,sizeof(profile_info.user_name));

            nwy_ext_input_gets("\r\nPlease input profile info: password(length 0-%d)\r\n",NWY_APN_PWD_MAX_LEN);
            if (strlen(nwy_ext_sio_recv_buff) > NWY_APN_PWD_MAX_LEN) {
              nwy_ext_echo("\r\nInvaild password len\r\n");
              break;
            }
            if (strlen(nwy_ext_sio_recv_buff) > 0)
              memcpy(profile_info.pwd,nwy_ext_sio_recv_buff,sizeof(profile_info.pwd));

            ret = nwy_data_set_profile(profile_id,NWY_DATA_PROFILE_3GPP,&profile_info);
            OSI_LOGI(0, "=DATA=  nwy_data_set_profile ret= %d", ret);
            if (ret != NWY_RES_OK)
              nwy_ext_echo("\r\nSet profile %d info fail, result<%d>\r\n",profile_id,ret);
            else
              nwy_ext_echo("\r\nSet profile %d info success\r\n",profile_id);
            break;
          }
          case 4:
          {
            nwy_data_start_call_v02_t param_t;
            memset(&param_t,0,sizeof(nwy_data_start_call_v02_t));
            nwy_ext_input_gets("\r\nPlease select resource handle id <1-%d>",NWY_DATA_CALL_MAX_NUM);
            int hndl = atoi(sptr);
            if ((hndl <= 0) || (hndl > NWY_DATA_CALL_MAX_NUM)){
              nwy_ext_echo("\r\nInvaild resource handle id: %d\r\n",hndl);
              break;
            }

            nwy_ext_input_gets("\r\nPlease select profile: profile_id <1-7>");
            int profile_id = atoi(sptr);
            if ((profile_id <= 0) || (profile_id > 7)){
              nwy_ext_echo("\r\nInvaild profile id: %d\r\n",profile_id);
              break;
            }
            param_t.profile_idx = profile_id;

            /* Begin: Add by YJJ for support auto re_connect in 2020.05.20*/
            nwy_ext_input_gets("\r\nPlease set auto_connect: 0 Disable, 1 Enable");
            int is_auto_recon = atoi(sptr);
            if ((is_auto_recon != 0) && (is_auto_recon != 1)){
              nwy_ext_echo("\r\nInvaild auto_connect: %d\r\n",is_auto_recon);
              break;
            }
            param_t.is_auto_recon = is_auto_recon;

            if(is_auto_recon ==1)
            {
              nwy_ext_input_gets("\r\nPlease set auto_connect maximum times: 0 Always Re_connect, [1-65535] maximum_times");
              int auto_re_max = atoi(sptr);
              if ((auto_re_max < 0) || (auto_re_max > 65535)){
                nwy_ext_echo("\r\nInvaild auto_connect maximum times : %d\r\n",auto_re_max);
                break;
              }
              param_t.auto_re_max = auto_re_max;

              nwy_ext_input_gets("\r\nPlease set auto_connect interval (ms): [100,86400000]");
              int auto_interval_ms = atoi(sptr);
              if ((auto_interval_ms < 100) || (auto_interval_ms > 86400000)){
                nwy_ext_echo("\r\nInvaild auto_connect interval: %d\r\n",auto_interval_ms);
                break;
              }
              param_t.auto_interval_ms = auto_interval_ms;
            }

            /* End: Add by YJJ for auto re_connect in 2020.05.20*/

            ret = nwy_data_start_call(hndl,&param_t);
            OSI_LOGI(0, "=DATA=  nwy_data_start_call ret= %d", ret);
            if (ret != NWY_RES_OK)
              nwy_ext_echo("\r\nStart data call fail, result<%d>\r\n",ret);
            else
              nwy_ext_echo("\r\nStart data call ...\r\n");
            break;
          }
          case 5:
          {
            int len = 0;
            nwy_data_addr_t_info addr_info;
            nwy_ext_input_gets("\r\nPlease select resource handle id <1-%d>",NWY_DATA_CALL_MAX_NUM);
            int hndl = atoi(sptr);
            if ((hndl <= 0) || (hndl > NWY_DATA_CALL_MAX_NUM)){
              nwy_ext_echo("\r\nInvaild resource handle id: %d\r\n",hndl);
              break;
            }

            memset(&addr_info,0,sizeof(nwy_data_addr_t_info));
            OSI_LOGI(0, "=DATA=  addr_info size= %d",sizeof(nwy_data_addr_t_info));
            ret = nwy_data_get_ip_addr(hndl, &addr_info, &len);
            OSI_LOGI(0, "=DATA=  nwy_data_get_ip_addr = %d|len%d", ret,len);
            if (ret != NWY_RES_OK)
            {
              nwy_ext_echo("\r\nGet data info fail, result<%d>\r\n",ret);
              break;
            }
            nwy_ext_echo("\r\nGet data info success\r\nIface address: %s,%s\r\n",
                nwy_ip4addr_ntoa(&addr_info.iface_addr_s.ip_addr.addr),
                nwy_ip6addr_ntoa(&addr_info.iface_addr_s.ip6_addr.addr));
            nwy_ext_echo("Dnsp address: %s,%s\r\n",
                nwy_ip4addr_ntoa(&addr_info.dnsp_addr_s.ip_addr.addr),
                nwy_ip6addr_ntoa(&addr_info.dnsp_addr_s.ip6_addr.addr));
            nwy_ext_echo("Dnss address: %s,%s\r\n",
                nwy_ip4addr_ntoa(&addr_info.dnss_addr_s.ip_addr.addr),
                nwy_ip6addr_ntoa(&addr_info.dnss_addr_s.ip6_addr.addr));
            break;
          }
          case 6:
          {
            nwy_ext_input_gets("\r\nPlease select resource handle id <1-%d>",NWY_DATA_CALL_MAX_NUM);
            int hndl = atoi(sptr);
            if ((hndl <= 0) || (hndl > NWY_DATA_CALL_MAX_NUM)){
              nwy_ext_echo("\r\nInvaild resource handle id: %d\r\n",hndl);
              break;
            }
            ret = nwy_data_stop_call(hndl);
            OSI_LOGI(0, "=DATA=  nwy_data_stop_call ret= %d", ret);
            if (ret != NWY_RES_OK)
              nwy_ext_echo("\r\nStop data call fail, result<%d>\r\n",ret);
            else
              nwy_ext_echo("\r\nStop data call ...\r\n");
            break;
          }
          case 7:
          {
            nwy_ext_input_gets("\r\nPlease select resource handle id <1-%d>",NWY_DATA_CALL_MAX_NUM);
            int hndl = atoi(sptr);
            if ((hndl < 0) || (hndl > NWY_DATA_CALL_MAX_NUM)){
              nwy_ext_echo("\r\nInvaild resource handle id: %d\r\n",hndl);
              break;
            }

            nwy_data_relealse_srv_handle(hndl);
            OSI_LOGI(0, "=DATA=  nwy_data_relealse_srv_handle hndl= %d", hndl);
            nwy_ext_echo("\r\nRelease resource handle id %d\r\n",hndl);
            break;
          }
          case 8:
          {
            char rsp[128] = {0};
            nwy_data_flowcalc_info_t flowcalc_info = {0};
            nwy_data_get_flowcalc_info(&flowcalc_info);
            snprintf (rsp, sizeof(rsp), "tx_bytes:%llu,rx_bytes:%llu\r\ntx_packets:%lu,rx_packets:%lu",
                      flowcalc_info.tx_bytes,flowcalc_info.rx_bytes,
                      flowcalc_info.tx_packets,flowcalc_info.rx_packets);
            nwy_ext_echo("\r\nGet data traffic statistics success\r\n%s\r\n",rsp);
            break;
          }
          case 9:
          {
            nwy_ext_input_gets("\r\nPlease select resource handle id <1-%d>",NWY_DATA_CALL_MAX_NUM);
            int hndl = atoi(sptr);
            if ((hndl < 0) || (hndl > NWY_DATA_CALL_MAX_NUM)){
              nwy_ext_echo("\r\nInvaild resource handle id: %d\r\n",hndl);
              break;
            }
            nwy_data_auto_reconn_status_t auto_reconn_status = {0};
            ret = nwy_data_get_auto_reconnect_status(hndl,&auto_reconn_status);
            OSI_LOGI(0, "=DATA=  nwy_data_get_auto_reconnect_status hndl= %d, ret=%d", hndl,ret);
            if (ret == NWY_RES_OK)
              nwy_ext_echo("\r\nhandle id %d auto reconnect status query success, state=%d,cnt=%d\r\n",hndl,
                    auto_reconn_status.auto_reconn_state,auto_reconn_status.auto_re_cnt);
            else
              nwy_ext_echo("\r\nhandle id %d auto reconnect status query failed, ret=%d\r\n",hndl,ret);
            break;
          }
          case 10:
          {
            return;
          }
          default:
              nwy_ext_echo("\r\nPlease input vaild value");
              break;
      }
  }
#endif
}






 void start_Call_Thread(void){
    g_app_Call_thread = nwy_create_thread("callThread", prvThreadEntry_Call, NULL, NWY_OSI_PRIORITY_NORMAL, 1024*10, 16);
 }

 
void nwy_tcp_test_Mine(int step,char *para1,char *para2,char *para3,char *para4)
{
    static int s = 0;
    char url_or_ip[256] = {0};
    char ip_buf[256] = {0};
    int port=0;
    nwy_ip_type_or_dns_enum ip_dns_type =-1;
    char* sptr = nwy_ext_sio_recv_buff;
    int af_inet_flag = AF_INET;
    char *ip_str =NULL;
    int isipv6 =0;
    char buff[NWY_UART_RECV_SINGLE_MAX + 1] = {0};
    int on = 1;
    int opt = 1,ret = 0, send_len;
    uint64_t start ;
    ip_addr_t addr = {0};

    struct sockaddr_in sa_v4;
    struct sockaddr_in6 sa_v6;

    //while (1)
    {
       // nwy_ext_tcp_menu();
        //nwy_ext_input_gets("\r\nPlease input option: ");
        switch(step)
        {
            case 1:
            {
                if (0 == nwy_ext_check_data_connect()) {
                    nwy_ext_echo("\r\ndata call not setup");
                    break;
                }
                if(tcp_connect_flag)
                {
                    nwy_ext_echo("\r\ntcp alreay connect");
                    break;
                }
                memset(url_or_ip, 0, sizeof(url_or_ip));
                memset(ip_buf, 0, sizeof(ip_buf));
               // nwy_ext_input_gets("\r\nPlease input url: ");
               sptr = para1;
                strncpy(url_or_ip, sptr, strlen(sptr));
                sptr = para2;
              //  nwy_ext_input_gets("\r\nPlease input port: ");
                if (nwy_port_get(sptr, &port)!= NWY_SUCESS) {
                    nwy_ext_echo("\r\nInput port error");
                    break;
                }

                nwy_ext_echo("\r\nurl test start:%s:%d\r\n", url_or_ip, port);
                ret = nwy_get_ip_str(url_or_ip, ip_buf, &isipv6);
                if (ret != NWY_SUCESS) {
                    break;
                }
                OSI_LOGXI(OSI_LOGPAR_SI,0, "nwy ip_buf= %s isipv6 = %d", ip_buf,isipv6);
                if (isipv6) {
                    if (ip6addr_aton( ip_buf,&addr.u_addr.ip6) == 0) {
                        nwy_ext_echo("\r\ninput ip or url is invalid");
                        break;
                    } else {

                        inet6_addr_from_ip6addr(&sa_v6.sin6_addr, ip_2_ip6(&addr));
                        sa_v6.sin6_len = sizeof(struct sockaddr_in);
                        sa_v6.sin6_family = AF_INET6;
                        sa_v6.sin6_port = htons(port);
                        af_inet_flag = AF_INET6;
                    }
                } else {
                    ret = nwy_hostname_check(ip_buf);
                    if (ret != NWY_SUCESS) {
                        nwy_ext_echo("\r\ninput ip or url is invalid");
                        break;
                    }

                    if (ip4addr_aton(ip_buf, &addr.u_addr.ip4) == 0)
                    {
                        nwy_ext_echo("\r\ninput ip error:\r\n");
                        break;
                    }
                    inet_addr_from_ip4addr(&sa_v4.sin_addr, ip_2_ip4(&addr));
                    sa_v4.sin_len = sizeof(struct sockaddr_in);
                    sa_v4.sin_family = AF_INET;
                    sa_v4.sin_port = htons(port);
                    af_inet_flag = AF_INET;
                }

                if (s == 0) {
                    s= nwy_socket_open(af_inet_flag, SOCK_STREAM, IPPROTO_TCP);
                }
                nwy_socket_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));

                nwy_socket_setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
                if (0 != nwy_socket_set_nonblock(s))
                {
                    nwy_ext_echo("\r\nsocket set err\r\n");
                    break;
                }
                start = nwy_get_ms();
                do
                {

                    if (af_inet_flag == AF_INET6) {
                        ret = nwy_socket_connect(s, (struct sockaddr *)&sa_v6, sizeof(sa_v6));

                    } else {
                        ret = nwy_socket_connect(s, (struct sockaddr *)&sa_v4, sizeof(sa_v4));

                    }
                    if(ret)
                    {
                    }
                    if((nwy_get_ms()-start)>= 10000 )
                    {
                        nwy_ext_echo("\r\nsocket connect timeout\r\n");
                        nwy_socket_shutdown(s, SHUT_RD);
                        nwy_socket_close(s);
                        s = 0;
                        break;
                    }
                    if(ret == -1)
                    {
                        //nwy_ext_echo("errno=%d\r\n", nwy_socket_errno());
                        if( EISCONN == nwy_socket_errno() )
                        {
                            nwy_ext_echo( "\r\nnwy_net_connect_tcp connect ok-----");
                            tcp_connect_flag = 1;
                            break;
                        }
                        if (EINPROGRESS != nwy_socket_errno() && EALREADY != nwy_socket_errno() )
                        {
                            nwy_ext_echo("\r\nconnect errno = %d", nwy_socket_errno());
                            nwy_socket_close(s);
                            s = 0;
                            tcp_connect_flag = 0;
                            break;
                        }
                        }
                    nwy_sleep(100);
                }while(1);
                if(tcp_connect_flag)
                {
                    tcp_recv_thread = nwy_create_thread("tcp_recv_thread",nwy_tcp_recv_func, (void*)&s, NWY_OSI_PRIORITY_NORMAL, 2048, 4);
                    if(tcp_recv_thread ==NULL)
                    {
                        nwy_ext_echo("\r\ncreate tcp recv thread failed, close connect");
                        nwy_socket_close(s);
                        s = 0;
                        tcp_connect_flag = 0;
                    }
                }
                break;
            }
            case 2:
            {
                if(!tcp_connect_flag)
                {
                    nwy_ext_echo("\r\ntcp not setup");
                    break;
                }
                memset(buff, 0, sizeof(buff));
                nwy_ext_input_gets("\r\nPlease input send data(<=512): ");
                if (nwy_ext_sio_len > NWY_UART_RECV_SINGLE_MAX) {
                    nwy_ext_echo("\r\nNo more than 512 bytes at a time to send ");
                    break;
                }
                memcpy(buff, sptr, nwy_ext_sio_len);
                send_len = nwy_socket_send(s, buff, nwy_ext_sio_len, 0);
                if(send_len != nwy_ext_sio_len)
                    nwy_ext_echo("\r\nsend len=%d, return len=%d", strlen(buff),send_len);
                else
                    nwy_ext_echo("\r\nsend ok");
                break;
            }
            case 3:
            {
                if(tcp_connect_flag)
                {
                    nwy_sleep(100);
                    nwy_socket_close(s);
                    s = 0;
                    tcp_connect_flag = 0;
                }
                nwy_ext_echo("\r\nclose tcp success");
                // kill phread
                break;
            }
            case 4:
                return;
            default:
                break;
        }
    }
}


void SET_Led_On_off(uint8_t led_name,uint8_t valu) {
    uint8_t port = 4;

    if(led_name == LED_YELLOW) 
        port = 4;
    else  if(led_name == LED_RED) 
        port = 28;
    else  if(led_name == LED_GREEN) 
        port = 24;
    nwy_gpio_set_direction(port,nwy_output);
    nwy_gpio_set_value(port,valu);    
}


void Set_RTS_on_off(uint8_t valu){
    uint8_t port = 23; 
    nwy_gpio_set_direction(port,nwy_output);
    nwy_gpio_set_value(port,valu);    
}





void Snd_485_Msg(char *msg , int num,int len ) {
    int value = 1;
    int port;
    int hd;

    if(num == 1) {
      Snd_485_Msg_Uart1( msg, len);
    } else if(num == 2) {
      Snd_485_Msg_Uart2( msg, len);
    }

}

void Snd_485_Msg_Uart1(char *msg , int len ) {
    int value = 1;
    int port;
    int hd;


    port = 19;
    hd = nwy_uart_init(NWY_NAME_UART1,1);   
    nwy_uart_reg_recv_cb(hd,nwy_485_recv_handle_1);


    if(nwy_uart_set_baud(hd,115200)) {
      nwy_ext_echo("\r\n Snd_485_--115200---chn ===%d", hd);  
    }

    nwy_gpio_set_direction(port,nwy_output);
    value = 1;
    nwy_gpio_set_value(port,value); 
    nwy_ext_echo("\r\n rs485--Port_id==%d,%d",port,value);
    nwy_uart_send_data(hd, (uint8_t *)msg, len);

    nwy_sleep(2);

    nwy_gpio_set_direction(port,nwy_output);
    value = 0;
    nwy_gpio_set_value(port,value); 

    RS485_hd[0]= hd;
}



void Snd_485_Msg_Uart2(char *msg , int len ) {
    int value = 1;
    int port;
    int hd;

    port = 23;//RS485_RTS_2;
    hd= nwy_uart_init(NWY_NAME_UART2,1);
    nwy_uart_reg_recv_cb(hd,nwy_485_recv_handle_2);


   if(nwy_uart_set_baud(hd,115200)) {
     nwy_ext_echo("\r\n Snd_485_=-115200---chn ===%d", hd);  
   }
    nwy_gpio_set_direction(port,nwy_output);
    nwy_gpio_set_value(port,value); 
    nwy_ext_echo("\r\n rs485--Port_id==%d,%d",port,value);
    nwy_uart_send_data(hd, (uint8_t *)msg, len);
    nwy_sleep(2);
    nwy_gpio_set_direction(port,nwy_output);

    value = 0;
    nwy_gpio_set_value(port,value); 
    RS485_hd[1]= hd;
  //  nwy_uart_deinit(RS485_hd[num-1]);

}
void Snd_N_ISO_485(char *msg , int len ) {
  nwy_ext_echo("\r\n Snd_N_ISO_48522");
  Snd_485_Msg_Uart1(msg , len);  
}


void Snd_OUT_ISO_485(char *msg , int len ) {
  nwy_ext_echo("\r\n Snd_OUT_ISO_485");
  Snd_485_Msg_Uart2(msg , len);  
}






void Set_Poll_Addr_Pin_Low(void) {
      int port;
     port = 14  ; 
    nwy_gpio_set_direction(port,nwy_output);
    nwy_gpio_set_value(port,0);  

     port = 15 ; 
    nwy_gpio_set_direction(port,nwy_output);
    nwy_gpio_set_value(port,0); 
}


void handle_n_Iso(const char *str, uint32_t length )
{
    char temp_buf[4];
    nwy_ext_echo("\r\nPoll_Length==%d---dev=%d,Total=%d",length,Poll_Addr_id,Dev_Num);
    memset(mqtt_report_Msg,0,sizeof(mqtt_report_Msg));

    strcat(mqtt_report_Msg, "6,");
    strcat(mqtt_report_Msg, "6,");


    for(int i = 0; i < length;i++) {
      nwy_ext_echo("%x-", *(str+i));
      memset(temp_buf, 0 , 4);
      //snprintf(&mqtt_report_Msg[i*2],256,"%02x", *(str+i));
      snprintf(temp_buf,4,"%02x", *(str+i));
      strcat(mqtt_report_Msg, temp_buf);
    }
    mqtt_report_Len = length * 2;

    nwy_ext_echo("\r\nData_Report==%s",&mqtt_report_Msg[0]); 
    
    //Generate_Report_WG_Info();
    Waiting_Mqtt(Fg_Snding_Mqtt);
    nwy_ext_send_sig(mqtt_Snd_task_id,EVENT_SND_485_CTRL);
  
}


void handle_rec(int hd,const char *str, uint32_t length ) {
  int crc;
  char id;

  if(length < 2) {
    return ;  
  }
  crc = N_CRC16(str,length-2);
  nwy_ext_echo("\r\nRs-485-handle--1-crc=%x,length = %d,uart_id = %d",crc,length,hd);
  if(((crc >> 8) == *(str + length -2)) && ((crc & 0x0ff) == *(str + length - 1))) {
    if(thread_Fg == 1){
      id = str[0];
      if(id < 0) id = -id;
      if(id < 32) {
        if(id >= 1)
        memcpy(&xb_SubDev_SN[id - 1][0],&str[2],12);
      }
      nwy_ext_echo("\r\nCrc_check--ok--poll");
    } else {

    }

  } else {
    id = str[0];
    if(id < 0) id = -id;
    if(id <4) {
      memcpy(&xb_SubDev_SN[id][0],&str[2],12);
    }    
     nwy_ext_echo("\r\nCrc_check--error"); 
  } 

  if(thread_Fg == 1) {
    nwy_ext_send_sig(g_app_Poll_Addr_thread, EVENT_REC_485);
  } else {
    if(hd == 0) {
      handle_n_Iso(str, length);
    } else {
      
    }
  }

}


void Snd_Ctrl_Cmd(char id, uint16_t cmd) {
  char buf[6];
  uint16_t crc;
  buf[0] = id;
  buf[1] = 0x44;
  buf[2] = cmd >> 8;
  buf[3] = cmd & 0x0ff;
  crc = N_CRC16(buf,4);
  buf[4] = crc >> 8;
  buf[5] = crc & 0x0ff;
  Snd_N_ISO_485(buf,sizeof(buf));

}



static void nwy_485_recv_handle_1 (const char *str, uint32_t length) {

  handle_rec(RS485_hd[0],str, length );

}

static void nwy_485_recv_handle_2 (const char *str, uint32_t length) {
  handle_rec(RS485_hd[1],str, length );
}

void Init_485(void) {
  int i;
  for(i = 0; i < 2;i++) {
    if(i == 0)        
      RS485_hd[i] = nwy_uart_init(NWY_NAME_UART1,1);
    else
      RS485_hd[i] = nwy_uart_init(NWY_NAME_UART2,1);
   if(nwy_uart_set_baud(RS485_hd[i],115200)) {
     nwy_ext_echo("\r\n Snd_485_==%d--115200",RS485_hd[i]);  
   }

   if(i == 0) 
      nwy_uart_reg_recv_cb(RS485_hd[i],nwy_485_recv_handle_1);
    else if(i == 1) 
      nwy_uart_reg_recv_cb(RS485_hd[i],nwy_485_recv_handle_2);
  }

  nwy_ext_echo("\r\n Uart_handle_id ==%d,%d",RS485_hd[0], RS485_hd[1]);  



}



void test_485_snd() {

    while(1) {
      Snd_485_Msg("\r\nI am RS485---1", 1, strlen("\r\nI am RS485---1"));
      nwy_sleep(1000);
      Snd_485_Msg("\r\nI am RS485---2" , 2,strlen("\r\nI am RS485---2") );
      nwy_sleep(1000);
    }

}




unsigned char auchCRCHi[]=
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};
unsigned  auchCRCLo[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

unsigned int N_CRC16(unsigned char *updata,unsigned int len)
{
  unsigned char uchCRCHi=0xff;
  unsigned char uchCRCLo=0xff;
  unsigned int  uindex;
    
  while(len--)
  {
    uindex=uchCRCHi^*updata++;
    uchCRCHi=uchCRCLo^auchCRCHi[uindex];
    uchCRCLo=auchCRCLo[uindex];
  }
  return (uchCRCLo<<8|uchCRCHi);  // 位在前
}




void Poll_Addr_Thread(void *param) {

  unsigned int crc ;
  int nn = 0;
  nwy_osiEvent_t event;



  while(nn < 3) {
    nn++;
    nwy_sleep(1000);
    nwy_ext_echo("\r\n Test_485_Addr_Poll==%d",nn); 
  }
  Init_485();
  //  thread_Fg  = 1;  
  while(1) {

    thread_Fg  = 1;


    Set_Poll_Addr_Pin_Low(); ///TODO, 
    poll_Cmd[0] = 0;
    poll_Cmd[1] = 0;     
    poll_Cmd[2] = Poll_Addr_id;
    crc = N_CRC16(poll_Cmd,3);
    poll_Cmd[3] = crc>>8;
    poll_Cmd[4] = crc & 0x0ff;
    nwy_ext_echo("\r\n Run_Poll_Addr_Thread--1"); 

    Snd_N_ISO_485(poll_Cmd ,5);

    Snd_OUT_ISO_485(poll_Cmd ,5);

    memset(&event, 0, sizeof(event));
  nwy_wait_thead_event(g_app_Poll_Addr_thread, &event, 3000);
   //  nwy_wait_thead_event(g_app_Poll_Addr_thread, &event, 0);///TODO,
  #if 1///def TODO_NOW
    if (event.id == EVENT_REC_485){
        nwy_ext_echo("\r\n Rec_event=%x", event.id);
        Poll_Addr_id++;
    } 
    else 
    {
      Dev_Num = Poll_Addr_id;
        nwy_ext_echo("\r\n Over_Time==%d",Dev_Num);        
        Start_Ctrl_Thread(); 
        nwy_exit_thread();     
    }

    if(Poll_Addr_id >= 4) {
      nwy_ext_echo("\r\n exit_thread--");
      Start_Ctrl_Thread(); 
      nwy_exit_thread(); 

    } 
#endif
   // nwy_sleep(2000);


  }


}

 void Start_Poll_Addr_Thread(void){
    g_app_Poll_Addr_thread = nwy_create_thread("PollAddrThread", Poll_Addr_Thread, NULL, NWY_OSI_PRIORITY_NORMAL, 1024*4, 16);
 }


void Waiting_Mqtt(uint8_t fg) {
  nwy_ext_echo("\r\nWait_mqtt_Sing--%x",nwy_get_current_thread());
  while(fg) {
    nwy_sleep(100);
  }  




  nwy_ext_echo("\r\nGet_mqtt_Sing--%x",nwy_get_current_thread());
}

uint8_t volatile fg_Snding_485 = 0;
 void Rs485_Ctrl_Thread(void *param) {
  uint16_t crc;
  nwy_osiEvent_t event;


   while(1) {
      thread_Fg = 2;
      poll_Ctrl_Cmd[0] = poll_id;
      poll_Ctrl_Cmd[1] = 0x4D;
      if(MQTTIsConnected(&paho_mqtt_client))
        poll_Ctrl_Cmd[2] = 1;
      else
        poll_Ctrl_Cmd[2] = 0;
      poll_Ctrl_Cmd[2] = 0;
      crc = N_CRC16(poll_Ctrl_Cmd,3);
      poll_Ctrl_Cmd[3] = crc>>8;
      poll_Ctrl_Cmd[4] = crc & 0x0ff;
      memset(&event, 0, sizeof(event));
      nwy_wait_thead_event(g_RS485_Ctrl_thread, &event, 200);

      fg_Snding_485 = 1;
      if((event.id == EVENT_SND_485_ALL_ON) || (event.id == EVENT_SND_485_ALL_OFF) || (event.id == EVENT_SND_485_ALL_RS) ||(event.id == EVENT_SND_485_CTRL)  ){
        poll_id = 1;
        while(poll_id <= Dev_Num) {
          if(event.id == EVENT_SND_485_ALL_ON)
            Snd_Ctrl_Cmd(poll_id, RELAY_ALL_ON);
          else if(event.id == EVENT_SND_485_ALL_OFF)
            Snd_Ctrl_Cmd(poll_id, RELAY_ALL_OFF);
          else if(event.id == EVENT_SND_485_ALL_RS)
            Snd_Ctrl_Cmd(poll_id, RELAY_ALL_RS);
          else if(event.id == EVENT_SND_485_CTRL) {
            Snd_Ctrl_Cmd(poll_id, RELAY_ALL_ON);   
          }
       
          
          nwy_ext_echo("\r\nSnd_Ctrl_Cmd_App_Event==%,id=%d=%d",RELAY_ALL_ON,poll_id,Dev_Num); 
          poll_id++;
          if(poll_id < Dev_Num)
          nwy_sleep(100);
        }

      } else {
        poll_id++;
        if(poll_id > Dev_Num) {
          poll_id = 1;
        }
        static uint16_t kkk=0;
        kkk++;

        Snd_N_ISO_485(poll_Ctrl_Cmd , sizeof(poll_Ctrl_Cmd));

        //kkk = 0;
        if(kkk > 5* 2) {
          kkk = 0;
          Generate_Report_WG_Info();
          nwy_ext_echo("\r\nSnd_mqtt_thread_task_id==%x\r\n",mqtt_Snd_task_id); 
          Open_Pos_Location(1);
          Waiting_Mqtt(Fg_Snding_Mqtt);
          nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_WG_MSG);

        //  Generate_Report_WG_Info();
        }
      }
      fg_Snding_485 = 0;


   }

 }

  void Start_Ctrl_Thread(void) {
    g_RS485_Ctrl_thread = nwy_create_thread("RS485_Ctrl_Thread", Rs485_Ctrl_Thread, NULL, NWY_OSI_PRIORITY_NORMAL, 1024*5, 16);
 }




  void snd_key_Event(void) {
  nwy_osiEvent_t event;
  event.id = EVENT_SND_485_CTRL;        
  Waiting_Mqtt(fg_Snding_485);
  nwy_send_thead_event(g_RS485_Ctrl_thread, &event, 0);
  }