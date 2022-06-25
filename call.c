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


extern int tcp_connect_flag;
extern int ppp_state[10];

extern void nwy_data_cb_fun(
    int hndl,
    nwy_data_call_state_t ind_state);




// static void nwy_data_cb_fun(
//     int hndl,
//     nwy_data_call_state_t ind_state)
// {
//   OSI_LOGI(0, "=DATA= hndl=%d,ind_state=%d", hndl,ind_state);
//   if (hndl > 0 && hndl <= 8)
//   {
//     ppp_state[hndl-1] = ind_state;
//     nwy_ext_echo("\r\nData call status update, handle_id:%d,state:%d\r\n",hndl,ind_state);
//   }
// }
nwy_osiSemaphore_t      *s_Call_OK_semaphore = NULL;

extern void nwy_test_cli_ble_open();
extern void nwy_test_cli_ble_client_scan_Para(char *scan_TTT,char *scan_type);

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


nwy_osiThread_t *g_app_Call_thread = NULL;

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
