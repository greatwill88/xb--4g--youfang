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
nwy_time_t xb_time= {2020, 3,19,20,33,30};
char xb_timezone =0;
float temp_IC;
float voltage_Xb;

static nwy_osiTimer_t   *s_nwy_test_timer = NULL;
nwy_osiThread_t *g_Key_thread;
uint16_t value_zone_0 = 0xff;
uint16_t value_zone_1 = 0x58;
float temp_chip = 56.1;
float voltage_Input = 220.5;
extern MQTTClient paho_mqtt_client;

/* void nwy_app_timer_xb(void)
{
    uint32_t adc_vol;
    static uint32_t Last_AD;
     static uint32_t Cnt = 0;
    char buf[32];
    OSI_LOGI(0, "xb_Timer_Handle_333=%d",1);
    //nwy_ext_echo("\r\n Timer test ok");
    test_main(Cnt++);
    if(0 == nwy_dm_get_rftemperature(&temp_IC)) {
        //OSI_LOGI(0,"temp_ic==%f",temp_IC);
        snprintf (buf, sizeof(buf), "%f",temp_IC);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "temp_ic---%s\n", buf);
    } else {
        OSI_LOGI(0,"temp_ic_fail=%d",2);
    }

    adc_vol =  nwy_adc_get(1,NWY_ADC_SCALE_3V233);
    if((adc_vol < 100) && (Last_AD > 2000)){
        OSI_LOGI(0,"Power_fail",0); 
    }
    Last_AD = adc_vol;
    OSI_LOGI(0,"Power_Voltage_Is==%d",adc_vol);

    nwy_get_time(&xb_time, &xb_timezone);
    nwy_ext_echo("\r\n%d-%d-%d %d:%d:%d", xb_time.year,xb_time.mon,xb_time.day, xb_time.hour,xb_time.min,xb_time.sec);
} */

void test_main(int id) {
    OSI_LOGI(0,"test_main_fun==%d",id);
}


void prvThreadEntry_Get_Value(void *param) {
    uint32_t adc_vol;
    static uint32_t Last_AD;
    static int i = 0;
    char buf[32];
    OSI_LOGI(0,"prvThreadEntry_Get_Value==%f",temp_IC);

    while(1) {
        i++;

      //  nwy_ext_echo("\r\n Run_Sample_Thread.");
        if(0 == nwy_dm_get_rftemperature(&temp_IC)) {
            //OSI_LOGI(0,"temp_ic==%f",temp_IC);
            snprintf (buf, sizeof(buf), "%f",temp_IC);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "temp_ic---%s\n", buf);
        } else {
            OSI_LOGI(0,"temp_ic_fail=%d",2);
        }

        adc_vol =  nwy_adc_get(1,NWY_ADC_SCALE_3V233);
        if((adc_vol < 100) && (Last_AD > 2000)){
            OSI_LOGI(0,"Power_fail",0); 
        }
        Last_AD = adc_vol;
        OSI_LOGI(0,"Power_Voltage_Is33==%d",adc_vol);

        nwy_get_time(&xb_time, &xb_timezone);
        OSI_LOGI(0,"\r\nTime_Zone===%d-%d-%d %d:%d:%d", xb_time.year,xb_time.mon,xb_time.day, xb_time.hour,xb_time.min,xb_time.sec);

        UINT8 *ble_mac = NULL;
        ble_mac = nwy_ble_get_mac_addr();
        if(NULL != ble_mac)
        {
        //    nwy_ext_echo("\r\n Ble mac addr:%02x:%02x:%02x:%02x:%02x:%02x",ble_mac[5],ble_mac[4],ble_mac[3],ble_mac[2],ble_mac[1],ble_mac[0]);
        }
        else
        {
         //   nwy_ext_echo("\r\n ble get mac addr is null.");
        }

        static int nn = 0;

        nn++;

        if(nn > 20) {
            nn = 0;
           // test_get_imei();
            get_CCID_Fun();
           // Generate_Report_WG_Info();

           // nwy_wifi_test_xb();
        }


        if(nn%2) {
            SET_Led_On_off(LED_YELLOW,LED_ON);            
            SET_Led_On_off(LED_RED,LED_ON);            
            SET_Led_On_off(LED_GREEN,LED_ON);

        } else {
            SET_Led_On_off(LED_YELLOW,LED_OFF); 
            SET_Led_On_off(LED_RED,LED_OFF);            
            SET_Led_On_off(LED_GREEN,LED_OFF);           
        }

        nwy_sleep(500);
    
    }
}




void nwy_wifi_test_xb(void)
{
    int i;
	int ret;
    nwy_wifi_scan_list_t scan_list;

    return ;

    memset(&scan_list, 0, sizeof(scan_list));

	nwy_ext_echo("enter wifi test.\r\n ");
  
	ret = nwy_wifi_scan(&scan_list);

	if( 0 == ret )
	{
		if ( 0 == scan_list.num )
		{
			nwy_ext_echo("\r\n Wifi scan nothing");
		}
		else
		{
            nwy_ext_echo("\r\n Wifi Total_Num=====%d",scan_list.num);
			for (i = 0; i< scan_list.num; i++)
			{
		        OSI_LOGI(0, "nwy wifi ap mac is %02x:%02x:%02x:%02x:%02x:%02x",
		        scan_list.ap_list[i].mac[5],scan_list.ap_list[i].mac[4],scan_list.ap_list[i].mac[3],scan_list.ap_list[i].mac[2],
		        scan_list.ap_list[i].mac[1],scan_list.ap_list[i].mac[0]);
		        OSI_LOGI(0, "nwy channel = %d", scan_list.ap_list[i].channel);
		        OSI_LOGI(0, "nwy rssi = %d", scan_list.ap_list[i].rssival);


		        nwy_ext_echo("\r\nwifi ap mac is %02x:%02x:%02x:%02x:%02x:%02x",
		        scan_list.ap_list[i].mac[5],scan_list.ap_list[i].mac[4],scan_list.ap_list[i].mac[3],scan_list.ap_list[i].mac[2],
		        scan_list.ap_list[i].mac[1],scan_list.ap_list[i].mac[0]);
		        nwy_ext_echo("\r\nchannel = %d", scan_list.ap_list[i].channel);
		        nwy_ext_echo("\r\nrssi = %d", scan_list.ap_list[i].rssival);
			}
		}
	}
}





static nwy_test_cli_timer_cb()
{
    nwy_ext_echo("\r\nKey_press_Down_After=50");
    snd_key_Event();

}
static void _gpioisropen(int param)
{
    nwy_ext_echo("\r\ngpio isr set success--start-timer--");
    if(true == nwy_start_timer(s_nwy_test_timer, 50)) {
        nwy_ext_echo("\r\nTimer_Start_Succ");       
    }
}



void Key_Init_Fun(void) {
    int port = 27;
    int data;
    int nn = 0;
    nwy_close_gpio(port);
    while(nn < 5) {
        nn++;
        nwy_sleep(1000);
        nwy_ext_echo("\r\nInit_Key=%d",nn);  
    }
    data = nwy_open_gpio_irq_config(port,1,_gpioisropen);///falling edge ,
    if (s_nwy_test_timer == NULL) {
        s_nwy_test_timer = nwy_timer_init(nwy_get_current_thread(), nwy_test_cli_timer_cb, NULL);
    } 
}





void prvThreadEntry_Key_Thread(void *param) {
    
    Key_Init_Fun();
    while(1) {
        nwy_osiEvent_t event; 
        memset(&event, 0, sizeof(event));
        nwy_wait_thead_event(g_Key_thread, &event, 0);
        if(event.id) {
            nwy_ext_echo("\r\nKey_Event--");
        }
    }
}

 void Start_Key_Thread(void){
    g_Key_thread = nwy_create_thread("PollKeyThread", prvThreadEntry_Key_Thread, NULL, NWY_OSI_PRIORITY_NORMAL, 1024*1, 16);
 }


 
void Str_2_Cat(char *pDst,char *p1,char *p2) {
    strcat(pDst,p1);
    strcat(pDst,p2);
}

void Str_3_Cat(char *pDst,char *p1,char *p2,char *p3) {
    Str_2_Cat(pDst,p1,p2);
    strcat(pDst,p3);
}

void Str_4_Cat(char *pDst,char *p1,char *p2,char *p3,char *p4) {
    Str_3_Cat(pDst,p1,p2,p3);
    strcat(pDst,p4);
}

void pase_cmd(char *msg) {

}


void reply_Cmd_Cloud(char *msg) {

    char domain[32] = "www.zxy168.cn";
    char user_name[32]= "public";
    char password[32]= "xb1555895588";
    char time[32]= "";
    int value = 30;
    char LastWill[32]= "will";
    char LastWill_Msg[32]= "off";
    char Time_Stamp[32]= "";
    int tme_Run_Cnt = 144;
    char Time_Run[32]= "";

    snprintf(time,sizeof(time),"%d",value);
    snprintf(Time_Stamp,sizeof(time),"%d",time);
    snprintf(Time_Run,sizeof(time),"%d",tme_Run_Cnt);
    msg[0] = '{';
    strcat(msg, "\"cmd\":\"cloud\",");
    strcat(msg, "\"mqttip\":\"183.6.101.117\",");
    strcat(msg, "\"mqttport\":1882,");
    Str_4_Cat(msg, "\"mqttdomain\":","\"",domain,"\",");
    Str_4_Cat(msg, "\"mqttuser\":","\"",user_name,"\",");
    Str_4_Cat(msg, "\"mqttpsw\":","\"",password,"\",");
    Str_3_Cat(msg, "\"mqtttime\":",time,",");
    Str_4_Cat(msg, "\"lastwill\":","\"",LastWill,"\",");
    Str_4_Cat(msg, "\"lastwillsay\":","\"",LastWill_Msg,"\",");
    Str_3_Cat(msg, "\"ts\":",Time_Stamp,","); 
    Str_3_Cat(msg, "\"runtime\":",Time_Run,",");
    Str_4_Cat(msg, "\"sn\":","\"xbwg",xb_sim.nImei,"\",");   
    Str_4_Cat(msg, "\"iccid\":","\"",xb_sim.iccid,"\",");  
    strcat(msg, "}");

}




/////
void Generate_Report_WG_Info(void) {
    char msg[512];
    char temp[32];
     
    nwy_ext_echo("\r\nReport_WG_Info--every 20s\r\n"); 
    memset(msg,0,sizeof(msg));
    Str_2_Cat(msg,"0," ,xb_sim.iccid); 
    Str_2_Cat(msg, ",","100"); ////TODO,
    Str_2_Cat(msg, ",","2022-07-05-15:32:25"); ////TODO,
   
    memset(temp, 0 ,sizeof(temp));
    snprintf(temp,32,"%02x",value_zone_0);
    Str_2_Cat(msg, ",",temp);// 0区域数据，

    memset(temp, 0 ,sizeof(temp));
    snprintf(temp,32,"%02x",value_zone_1);
    Str_2_Cat(msg, ",",temp);// 1区域数据

    memset(temp, 0 ,sizeof(temp));
    snprintf(temp,32,"%.1f",temp_chip);
    Str_2_Cat(msg, ",",temp);//芯片温度。

    memset(temp, 0 ,sizeof(temp));
    snprintf(temp,32,"%.1f",voltage_Input);
    Str_2_Cat(msg, ",",temp);//进线电压



    // for(int i = 0;i < 2;i++){
    // memset(temp, 0 ,sizeof(temp));

    // Str_2_Cat(msg, ",",temp);//进线电压
    // }
    Str_3_Cat(msg, ",","111234567890","-");//white mac
    Str_2_Cat(msg,"221234567890"," ");///white mac

    Str_3_Cat(msg, ",","331234567890","-");//all mac
    Str_2_Cat(msg,"441234567890","-");//all mac 
    Str_2_Cat(msg,"551234567890"," ");//all mac 

    Str_2_Cat(msg, ",","1");//posiotn,
    Str_2_Cat(msg, ",","115.112--116.223"); ////gps--position,
    Str_2_Cat(msg, ",","5");//num
    Str_2_Cat(msg, ",","11011");

    Str_3_Cat(msg, ",","661234567890","-");//white mac
    Str_2_Cat(msg,"771234567890"," ");///white mac

    strcat(msg, "}");
    

    memset(mqtt_report_Msg,0,sizeof(mqtt_report_Msg));
    mqtt_report_Len = strlen(msg);
    memcpy(mqtt_report_Msg,msg,mqtt_report_Len);
      nwy_ext_echo("\r\n=======Mqtt_SND:==%s",mqtt_report_Msg);



/*     if(MQTTIsConnected(&paho_mqtt_client)) {
        nwy_ext_echo("\r\nMqtt——-connectted\r\n");

        if(mqtt_Snd_task_id) {
            nwy_ext_echo("\r\nSnd_Signal--wg\r\n");
            nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_WG_MSG); 
        } else {
            nwy_ext_echo("\r\nNo taskkk=====\r\n");            
        }

    } */
  
}




