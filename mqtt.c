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

int xb_Mqtt_Step = 1;
//extern char nwy_ext_sio_recv_buff[NWY_EXT_SIO_RX_MAX + 1];

char mqtt_buf[MAX_NUM];


#ifndef TEST_HELLO_WORLD  // 0///def FEATURE_NWY_PAHO_MQTT
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
MQTTClient paho_mqtt_client;
unsigned char* g_nwy_paho_readbuf = NULL;
unsigned char* g_nwy_paho_writebuf = NULL;
nwy_osiThread_t *nwy_paho_task_id = NULL;
nwy_osiThread_t *mqtt_Snd_task_id = NULL;
Network *paho_network = NULL;
#define NWY_EXT_SIO_PER_LEN 1024
char echo_buff[NWY_EXT_SIO_PER_LEN + 1] = {0};
nwy_paho_mqtt_at_param_type paho_mqtt_at_param = {0};
nwy_osiMutex_t *ext_mutex = NULL;

void messageArrived(MessageData* md)
{
  char topic_name[PAHO_TOPIC_LEN_MAX] = {0};
  int i = 0;
  unsigned int remain_len = 0;

  strncpy(topic_name, md->topicName->lenstring.data, md->topicName->lenstring.len);
  ext_mutex = nwy_create_mutex();
  nwy_lock_mutex(ext_mutex, 0);
  nwy_ext_echo("\r\n===messageArrived======");
  nwy_ext_echo("\r\npayloader len is %d", md->message->payloadlen);
  nwy_ext_echo("\r\n%s:\r\n", topic_name);
  remain_len = md->message->payloadlen;
  if(md->message->payloadlen > NWY_EXT_SIO_PER_LEN)
  {
    for(i = 0 ;i < ((md->message->payloadlen / NWY_EXT_SIO_PER_LEN ) + 1); i++)
    {
      memset(echo_buff, 0, sizeof(echo_buff));
      strncpy(echo_buff, md->message->payload + i*NWY_EXT_SIO_PER_LEN,
              remain_len > NWY_EXT_SIO_PER_LEN ? NWY_EXT_SIO_PER_LEN : remain_len);
      remain_len = md->message->payloadlen- (i+1)*NWY_EXT_SIO_PER_LEN;
      nwy_ext_echo(echo_buff);
    }
  }
  else
  {
    memset(echo_buff, 0, sizeof(echo_buff));
    strncpy(echo_buff, md->message->payload, md->message->payloadlen);
    nwy_ext_echo(echo_buff);
  }
  nwy_unlock_mutex(ext_mutex);
}

void nwy_paho_cycle(void)
{
  static int cnt = 0;
  char buf[128];

  SubMqtt(MQTT_TOPIC_REC,"2");
  while(1)
  {
    static int _10_Cnt = 0;
    while (MQTTIsConnected(&paho_mqtt_client))
    {
      MQTTYield(&paho_mqtt_client, 500);
      nwy_sleep(100);
    }
    nwy_ext_echo("\r\nMQTT disconnect ,Out paho cycle");
    nwy_suspend_thread(nwy_paho_task_id);
    cnt = 0;
  }
  nwy_sleep(200);
}


void mqtt_Snd_Thread(void)
{
  static int cnt = 0;
  char buf[128];


  while(1)
  {
    static int _10_Cnt = 0;
    while (MQTTIsConnected(&paho_mqtt_client))
    {
      nwy_sleep(1000);
      _10_Cnt++;
      if(_10_Cnt >= 10) {
        _10_Cnt = 0;
        cnt++;
        snprintf(buf,128,"hello===test---%d",cnt);
        nwy_ext_echo("\r\nRun_nwy_paho_cycle======%d",cnt);
        Snd_Mqtt(MQTT_TOPIC_SND,"2", "0",buf);
      }

    }
    nwy_ext_echo("\r\nMQTT disconnect ,Not_Snd");
    nwy_suspend_thread(mqtt_Snd_task_id);
    cnt = 0;
  }
  nwy_sleep(200);
}


nwy_osiThread_t * nwy_paho_yeild_task_init(void)
{
  if (nwy_paho_task_id == NULL)
  {
    nwy_osiThread_t * task_id = nwy_create_thread("neo_paho_yeild_task",nwy_paho_cycle,
                   NULL, NWY_OSI_PRIORITY_NORMAL,1024*2,4);
  
    nwy_paho_task_id = task_id;

    mqtt_Snd_task_id = nwy_create_thread("Mqtt_Snd_task",mqtt_Snd_Thread,
                   NULL, NWY_OSI_PRIORITY_NORMAL,1024*2,4);

  }
  else
    nwy_resume_thread(nwy_paho_task_id);
  return nwy_paho_task_id;
}

void nwy_paho_mqtt_test_mine(int step, char *para1, char *para2, char *para3 ,char *para4, char *para5 ,char *para6)
{
  char* sptr = nwy_ext_sio_recv_buff;
  MQTTPacket_connectData* options;
  int ssl_mode = 0, auth_mode, qos, retained;
  MQTTMessage pubmsg = {0};
  int rc, i;
  //ext_mutex = nwy_create_mutex();

  //while (1)
  {
  //  nwy_lock_mutex(ext_mutex, 0);
   // nwy_unlock_mutex(ext_mutex);


    switch(step)
    {
      case 1:
        if (MQTTIsConnected(&paho_mqtt_client) == 1)
        {
          nwy_ext_echo("\r\npaho mqtt already connect");
          break;
        }
        memset(&paho_mqtt_at_param, 0, sizeof(nwy_paho_mqtt_at_param_type));
        sptr = para1 ;
        //nwy_ext_input_gets("\r\nPlease input url/ip: ");
        strncpy(paho_mqtt_at_param.host_name, sptr, strlen(sptr));
       // nwy_ext_input_gets("\r\nPlease input port: ");
        sptr = para2 ;
        paho_mqtt_at_param.port = atoi(sptr);

        //nwy_ext_input_gets("\r\nPlease input client_id: ");
        sptr = para3 ;
        strncpy(paho_mqtt_at_param.clientID, sptr, strlen(sptr));
       // nwy_ext_input_gets("\r\nPlease input usrname: ");

        sptr = para4 ;
        strncpy(paho_mqtt_at_param.username, sptr, strlen(sptr));
        
        sptr = para5 ;
        //nwy_ext_input_gets("\r\nPlease input password: ");
        strncpy(paho_mqtt_at_param.password, sptr, strlen(sptr));

        sptr = para6 ;
        //nwy_ext_input_gets("\r\nPlease input sslmode(1-ssl,0-no ssl): ");
        if(atoi(sptr)>1 || atoi(sptr)<0)
        {
            nwy_ext_echo("\r\ninput sslmode error");
            break;
        }
        paho_mqtt_at_param.paho_ssl_tcp_conf.sslmode = atoi(sptr);
        if(g_nwy_paho_writebuf!=NULL)
        {
          free(g_nwy_paho_writebuf);
          g_nwy_paho_writebuf=NULL;
        }
        if (NULL == (g_nwy_paho_writebuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX))) {
            nwy_ext_echo("\r\nmalloc buffer g_nwy_paho_writebuf error");
            break;
        }
        if(g_nwy_paho_readbuf!=NULL)
        {
          free(g_nwy_paho_readbuf);
          g_nwy_paho_readbuf=NULL;
        }
        if (NULL == (g_nwy_paho_readbuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX))) {
            nwy_ext_echo("\r\nmalloc buffer g_nwy_paho_readbuf error");
           break;
        }
        memset(g_nwy_paho_readbuf, 0, sizeof(g_nwy_paho_readbuf));
        memset(g_nwy_paho_writebuf, 0, sizeof(g_nwy_paho_writebuf));
        if(paho_network != NULL)
        {
          free(paho_network);
          paho_network = NULL;
        }
        if (NULL == (paho_network = (unsigned char *)malloc(sizeof(Network)))) {
            nwy_ext_echo("\r\nmalloc buffer paho_network error");
           break;
        }
        memset(paho_network, 0, sizeof(Network));
        NetworkInit(paho_network);
        if (paho_mqtt_at_param.paho_ssl_tcp_conf.sslmode == 1)
        {
         // nwy_ext_input_gets("\r\nPlease input auth_mode(0/1/2): ");
          //paho_mqtt_at_param.paho_ssl_tcp_conf.authmode = atoi(sptr);
          paho_mqtt_at_param.paho_ssl_tcp_conf.authmode = 0;
          if(paho_mqtt_at_param.paho_ssl_tcp_conf.authmode == 0)
          {
            paho_network->tlsConnectParams.pRootCALocation = NULL;
            paho_network->tlsConnectParams.pDeviceCertLocation = NULL;
            paho_network->tlsConnectParams.pDevicePrivateKeyLocation = NULL;
          }
          else if(paho_mqtt_at_param.paho_ssl_tcp_conf.authmode == 1)
          {
            nwy_ext_input_gets("\r\nPlease input ca: ");
            strncpy( paho_mqtt_at_param.paho_ssl_tcp_conf.cacert.cert_name, sptr, strlen(sptr));
            paho_network->tlsConnectParams.pRootCALocation = paho_mqtt_at_param.paho_ssl_tcp_conf.cacert.cert_name;
            paho_network->tlsConnectParams.pDeviceCertLocation = NULL;
            paho_network->tlsConnectParams.pDevicePrivateKeyLocation = NULL;
          }
          else
          {
            nwy_ext_input_gets("\r\nPlease input ca: ");
            strncpy( paho_mqtt_at_param.paho_ssl_tcp_conf.cacert.cert_name, sptr, strlen(sptr));
            paho_network->tlsConnectParams.pRootCALocation = paho_mqtt_at_param.paho_ssl_tcp_conf.cacert.cert_name;
            nwy_ext_input_gets("\r\nPlease input clientcert: ");
            strncpy(paho_mqtt_at_param.paho_ssl_tcp_conf.clientcert.cert_name, sptr, strlen(sptr));
            nwy_ext_input_gets("\r\nPlease input clientkey: ");
            strncpy(paho_mqtt_at_param.paho_ssl_tcp_conf.clientkey.cert_name, sptr, strlen(sptr));
            paho_network->tlsConnectParams.pDeviceCertLocation =  paho_mqtt_at_param.paho_ssl_tcp_conf.clientcert.cert_name;
            paho_network->tlsConnectParams.pDevicePrivateKeyLocation = paho_mqtt_at_param.paho_ssl_tcp_conf.clientkey.cert_name;
          }
          paho_network->tlsConnectParams.ServerVerificationFlag = paho_mqtt_at_param.paho_ssl_tcp_conf.authmode;
          paho_network->is_SSL = 1;
          paho_network->tlsConnectParams.timeout_ms = 5000;
        }
         else
           nwy_ext_echo( "\r\nis no-SSL NetworkConnect");
         ///nwy_ext_input_gets("\r\nPlease input clean_flag(0/1): ");
         //if(atoi(sptr)>1 || atoi(sptr)<0)
         //{
         //  nwy_ext_echo("\r\ninput clean_flag error");
         //  break;
         //}
        // paho_mqtt_at_param.cleanflag = (sptr[0] - 0x30);
         paho_mqtt_at_param.cleanflag = 0;
         //nwy_ext_input_gets("\r\nPlease input keep_alive: ");
        // paho_mqtt_at_param.keepalive = atoi(sptr);
         paho_mqtt_at_param.keepalive = 60;
         nwy_ext_echo("\r\nip:%s, port :%d",paho_mqtt_at_param.host_name, paho_mqtt_at_param.port);
         rc = NetworkConnect(paho_network, paho_mqtt_at_param.host_name, paho_mqtt_at_param.port);
         if (rc < 0)
         {
           nwy_ext_echo("\r\nNetworkConnect err rc=%d", rc);
           break;
         }
         nwy_ext_echo("\r\nNetworkConnect ok");

         MQTTClientInit(&paho_mqtt_client, paho_network, 10000, g_nwy_paho_writebuf, PAHO_PLAYOAD_LEN_MAX,
                        g_nwy_paho_readbuf, PAHO_PLAYOAD_LEN_MAX);
         MQTTClientInit_defaultMessage(&paho_mqtt_client, messageArrived);
         data.clientID.cstring = paho_mqtt_at_param.clientID;
         if (0 != strlen((char*)paho_mqtt_at_param.username)
             && 0 != strlen((char*)paho_mqtt_at_param.password))
         {
           data.username.cstring = paho_mqtt_at_param.username;
           data.password.cstring = paho_mqtt_at_param.password;
         }
         data.keepAliveInterval = paho_mqtt_at_param.keepalive;
         data.cleansession = paho_mqtt_at_param.cleanflag;
        
        
         memcpy(paho_mqtt_at_param.willtopic,MQTT_TOPIC_WILL,sizeof(MQTT_TOPIC_WILL) );//TODO,kuang,
        memcpy(paho_mqtt_at_param.willmessage,"offine",sizeof("offine") );//TODO,kuang,
        paho_mqtt_at_param.willmessage_len  = strlen((char*)paho_mqtt_at_param.willmessage);//TODO,kuang,
         if (0 != strlen((char*)paho_mqtt_at_param.willtopic))
         {
           nwy_ext_echo("\r\nSet_Will_Msg");
           memset(&data.will, 0x0, sizeof(data.will));
           data.willFlag = 1;
           data.will.retained = paho_mqtt_at_param.willretained;
           data.will.qos = paho_mqtt_at_param.willqos;
           if (paho_mqtt_at_param.willmessage_len != 0)
           {
             data.will.topicName.lenstring.data = paho_mqtt_at_param.willtopic;
             data.will.topicName.lenstring.len = strlen((char*)paho_mqtt_at_param.willtopic);
             data.will.message.lenstring.data = paho_mqtt_at_param.willmessage;
             data.will.message.lenstring.len = paho_mqtt_at_param.willmessage_len;
           }
           else
           {
             data.will.topicName.cstring = paho_mqtt_at_param.willtopic;
             data.will.message.cstring = paho_mqtt_at_param.willmessage;
           }
           nwy_ext_echo("\r\nMQTT will ready");
         }
         nwy_ext_echo("\r\nConnecting MQTT");
        if (rc = nwy_MQTTConnect(&paho_mqtt_client, &data))
            nwy_ext_echo("\r\nFailed to create client, return code %d", rc);
        else
        {
            nwy_ext_echo("\r\nMQTT connect ok");
            nwy_osiTimer_t *task_id = nwy_paho_yeild_task_init();
            if (task_id == NULL)
              nwy_ext_echo("\r\npaho yeid task create failed ");
            else
              nwy_ext_echo("\r\npaho yeid task create ok ");
        }
      break;
      case 2:
        if (MQTTIsConnected(&paho_mqtt_client))
        {
            memset(paho_mqtt_at_param.topic , 0, sizeof(paho_mqtt_at_param.topic));
           // nwy_ext_input_gets("\r\nPlease input topic: ");
           sptr = para1;
            strncpy(paho_mqtt_at_param.topic, sptr, strlen(sptr));
            //nwy_ext_input_gets("\r\nPlease input qos: ");
            sptr = para2;
            paho_mqtt_at_param.qos = atoi(sptr);
          //  nwy_ext_input_gets("\r\nPlease input retained: ");
            sptr = para3;
            paho_mqtt_at_param.retained = atoi(sptr);
            memset(paho_mqtt_at_param.message , 0, sizeof(paho_mqtt_at_param.message));
#ifdef FEATURE_NWY_N58_OPEN_NIPPON
            nwy_ext_input_gets("\r\nPlease input message(<= 2048): ");
            if (nwy_ext_sio_len > NWY_EXT_SIO_RX_MAX) {
              nwy_ext_echo("\r\nNo more than 2048 bytes at a time ");
              break;
            }
#else
           // nwy_ext_input_gets("\r\nPlease input message(<= 512): ");
            if (nwy_ext_sio_len > 512) {
              nwy_ext_echo("\r\nNo more than 512 bytes at a time ");
              break;
            }
#endif
            sptr = para4;
            strncpy(paho_mqtt_at_param.message, sptr, strlen(sptr));
            nwy_ext_echo("\r\nmqttpub param retained = %d, qos = %d, topic = %s, msg = %s",
	                          paho_mqtt_at_param.retained,
	                          paho_mqtt_at_param.qos,
	                          paho_mqtt_at_param.topic,
	                          paho_mqtt_at_param.message);
	        memset(&pubmsg, 0, sizeof(pubmsg));
            pubmsg.payload =(void *)paho_mqtt_at_param.message;
            pubmsg.payloadlen = strlen(paho_mqtt_at_param.message);
            pubmsg.qos = paho_mqtt_at_param.qos;
            pubmsg.retained = paho_mqtt_at_param.retained;
            pubmsg.dup = 0;
            rc = nwy_MQTTPublish(&paho_mqtt_client, paho_mqtt_at_param.topic, &pubmsg);
            nwy_ext_echo("\r\nmqtt publish rc:%d", rc );
        }
        else
            nwy_ext_echo("\r\nMQTT not connect");
        break;
      case 3:
        if (MQTTIsConnected(&paho_mqtt_client))
        {
          memset(paho_mqtt_at_param.topic , 0, sizeof(paho_mqtt_at_param.topic));
          //nwy_ext_input_gets("\r\nPlease input topic: ");
          sptr = para1;
          strncpy(paho_mqtt_at_param.topic, sptr, strlen(sptr));

          sptr = para2;
          //nwy_ext_input_gets("\r\nPlease input qos: ");
          paho_mqtt_at_param.qos = atoi(sptr);
          rc = MQTTSubscribe(&paho_mqtt_client,
                    (char*)paho_mqtt_at_param.topic,
                    paho_mqtt_at_param.qos,
                    messageArrived);
          if (rc ==SUCCESS)
            nwy_ext_echo("\r\nMQTT Sub ok");
          else
            nwy_ext_echo("\r\nMQTT Sub error:%d", rc);
        }
        else
          nwy_ext_echo("\r\nMQTT no connect");
        break;
      case 4:
        if (MQTTIsConnected(&paho_mqtt_client))
        {
          memset(paho_mqtt_at_param.topic , 0, sizeof(paho_mqtt_at_param.topic));
          nwy_ext_input_gets("\r\nPlease input topic: ");
          strncpy(paho_mqtt_at_param.topic, sptr, strlen(sptr));
          rc = MQTTUnsubscribe(&paho_mqtt_client, paho_mqtt_at_param.topic);
          if (rc ==SUCCESS)
            nwy_ext_echo("\r\nMQTT Unsub ok");
          else
            nwy_ext_echo("\r\nMQTT Unsub error:%d", rc);
        }
        else
          nwy_ext_echo("\r\nMQTT no connect");
        break;
      case 5:
        if(MQTTIsConnected(&paho_mqtt_client))
        {
            MQTTDisconnect(&paho_mqtt_client);
            NetworkDisconnect(paho_network);
            if (NULL != g_nwy_paho_writebuf) {
              free(g_nwy_paho_writebuf);
              g_nwy_paho_writebuf=NULL;
            }
            if (NULL != g_nwy_paho_readbuf) {
              free(g_nwy_paho_readbuf);
              g_nwy_paho_readbuf=NULL;
            }
            if(NULL != paho_network )
            {
              free(paho_network);
              paho_network = NULL;
            }
        }
        nwy_ext_echo("\r\nMQTT disconnect ok");
        break;
#ifdef FEATURE_NWY_N58_OPEN_NIPPON
      case 6:
        if (MQTTIsConnected(&paho_mqtt_client))
        {
          memset(paho_mqtt_at_param.topic , 0, sizeof(paho_mqtt_at_param.topic));
          nwy_ext_input_gets("\r\nPlease input topic: ");
          strncpy(paho_mqtt_at_param.topic, sptr, strlen(sptr));
          nwy_ext_input_gets("\r\nPlease input qos: ");
          paho_mqtt_at_param.qos = atoi(sptr);
          nwy_ext_input_gets("\r\nPlease input retained: ");
          paho_mqtt_at_param.retained = atoi(sptr);
          memset(paho_mqtt_at_param.message , 0, sizeof(paho_mqtt_at_param.message));
          nwy_ext_input_gets("\r\nPlease input 1k str,msg(10k) consists of 10 str");
          if (nwy_ext_sio_len != 1024) {
            nwy_ext_echo("\r\nmust be 1k message");
            break;
          }
          for( i = 0; i < 10; i++)
              strncpy(paho_mqtt_at_param.message + i*strlen(sptr), sptr, strlen(sptr));
          nwy_ext_echo("\r\nmqttpub param retained = %d, qos = %d, topic = %s, strlen is %d",
                        paho_mqtt_at_param.retained,
                        paho_mqtt_at_param.qos,
                        paho_mqtt_at_param.topic,
                        strlen(paho_mqtt_at_param.message));
          memset(&pubmsg, 0, sizeof(pubmsg));
          pubmsg.payload =(void *)paho_mqtt_at_param.message;
          pubmsg.payloadlen = strlen(paho_mqtt_at_param.message);
          pubmsg.qos = paho_mqtt_at_param.qos;
          pubmsg.retained = paho_mqtt_at_param.retained;
          pubmsg.dup = 0;
          rc = nwy_MQTTPublish(&paho_mqtt_client, paho_mqtt_at_param.topic, &pubmsg);
          nwy_ext_echo("\r\nmqtt publish rc:%d", rc );
        }
        else
            nwy_ext_echo("\r\nMQTT not connect");
        break;
#endif
     case 7:
     //   nwy_delete_mutex(ext_mutex);
        return ;
      default:
        break;
    }
  }
}

int Snd_Mqtt(char *topic,char *qos, char *retain,char *msg) {
  nwy_paho_mqtt_test_mine(2, topic, qos, retain ,msg, NULL ,NULL);
}


int SubMqtt(char *topic,char *qos) {

    nwy_paho_mqtt_test_mine(3, topic, qos, NULL ,NULL, NULL ,NULL);
}

 void prvThreadEntry_xb_Connect(void *param) {
   nwy_time_t xb_time;
   char xb_timezone;

#if 1
  char ip_addr[]="183.6.101.117";
  char port_Num[]="1882";
  char Client_ID[64]="test_Xb_iot";
  char User_Name[]="kuang";
  char password[]="kuang";
  char ssl_Setting[]="0";
#else
  char ip_addr[]="183.6.101.117";
  char port_Num[]="1882";
  char Client_ID[]="test_Xb_iot";
  char User_Name[]="kuang";
  char password[]="kuang";

#endif 
    static int i = 0;
    char time_buf[32];
    while(1) {

        xb_Mqtt_Step = 1;
        if(MQTTIsConnected(&paho_mqtt_client)){
            nwy_sleep(3000);
            i++;
            nwy_ext_echo("\r\nMqtt_On_Line_Time:%d",i*3);
        }else{
            i = 0;
            nwy_ext_echo("\r\nExecute_Mqtt_Now---setup_tcp:%d",i);
            nwy_get_time(&xb_time, &xb_timezone);
            memset(Client_ID,0,sizeof(Client_ID));
            
          //  memcpy(Client_ID,"test_Xb_iot",sizeof(Client_ID));
            while(strlen(xb_sim.nImei) == 0) {
                nwy_sleep(3000);
            }
            memcpy(Client_ID, xb_sim.nImei,sizeof(Client_ID));


            snprintf(time_buf,32,"%d_%d_%d",xb_time.hour,xb_time.min,xb_time.sec);
            strcat(Client_ID,time_buf);
            nwy_ext_echo("\r\nClient--ID:%s",Client_ID);        
            nwy_ext_echo("\r\n%d-%d-%d %d:%d:%d", xb_time.year,xb_time.mon,xb_time.day, xb_time.hour,xb_time.min,xb_time.sec);    
            nwy_tcp_test_Mine(1,ip_addr,port_Num,NULL,NULL);
            nwy_paho_mqtt_test_mine(1, ip_addr, port_Num, Client_ID ,User_Name, password ,ssl_Setting);

        }
      
    }
}



void Gernerate_Topic_ctrl(char *topic,int len) {
  memset(topic, 0 , len);
  strcat(topic, "Gateway/");
  strcat(topic, xb_sim.nImei);
  strcat(topic, "/ctrraw");
}


void Gernerate_Topic_status(char *topic,int len) {
  memset(topic, 0 , len);
  strcat(topic, "Gateway/");
  strcat(topic, xb_sim.nImei);
  strcat(topic, "/stateraw");
}

#endif
