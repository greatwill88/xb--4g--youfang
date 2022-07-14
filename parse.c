#include "nwy_test_cli\comm\nwy_test_cli_utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "nwy_ble.h"
#include "nwy_ble_client.h"
#include "nwy_wifi.h"
#include "prj.h"
#include "nwy_loc.h"
typedef struct {
    char mqttip[64];
    char mqttport[16];   
    char domain[96];
    char user[32];
    char pw[32];
    char hold_Time[8];
    char lastWill[32];
} Net_Set_Typedef;

Net_Set_Typedef Net_Info;


void Reply_Restart(char *msg,int len){
    memset(msg,0 ,len);
    Str_3_Cat(msg, "\"sn\":\"XBWG", xb_sim.nImei,"\",");
    strcat(msg,"\"cmd\": \"restart\"");

}


void Reply_Set_Cmd(char *msg,int len){
    memset(msg,0 ,len);
    Str_3_Cat(msg, "\"mqttip\":\"", xb_sim.nImei,"\",");


}




void Handle_Set_Cmd(char *buf) {
    int len ;
    len = strlen(buf);
    char *pt;
    char *para;
    char *temp;

    pt = malloc(len);
    nwy_ext_echo("\r\nHandle_Set_Cmd==%d,%s",len,buf);
    if(pt) {
        memcpy(pt, buf, len);
        para = strtok(pt, ",");
        if(para == NULL) {
            nwy_ext_echo("\r\nHandle_Set_Cmd--error");
            return ;
        }
        do {

            len = strlen(para);
            para[len -1] = 0;
            nwy_ext_echo("\r\nPara===%s",para);            

            if((temp =strstr(para, "\"mqttip\":\""))!= NULL) {
                temp += strlen("\"mqttip\":\"");
                
                len = strlen(temp);
                nwy_ext_echo("\r\nMqtt_Temp===%s,%d",temp,len);
                memcpy(Net_Info.mqttip,temp,len);
                nwy_ext_echo("\r\nMqtt--ip==%s", Net_Info.mqttip);

            } else if((temp =strstr(para, "\"mqttport\":\"")) != NULL) {
                temp += strlen("\"mqttport\":\"");
                memcpy(Net_Info.mqttport,temp,len);
                nwy_ext_echo("\r\nmqttport==%s", Net_Info.mqttport);
            } else if((temp =strstr(para, "\"mqttdomain\":\"")) != NULL) {
                temp += strlen("\"mqttdomain\":\"");
                memcpy(Net_Info.domain,temp,sizeof(Net_Info.domain));
                nwy_ext_echo("\r\ndomain==%s", Net_Info.domain);
            } else if((temp =strstr(para, "\"mqttuser\":\"")) != NULL) {
                temp += strlen("\"mqttuser\":\"");
                memcpy(Net_Info.user,temp,sizeof(Net_Info.user));
                nwy_ext_echo("\r\nuser==%s", Net_Info.user);
            }else if((temp =strstr(para, "\"mqttpsw\":\"")) != NULL) {
                temp += strlen("\"mqttpsw\":\"");
                memcpy(Net_Info.pw,temp,sizeof(Net_Info.pw));
                nwy_ext_echo("\r\n password==%s", Net_Info.pw);
            }else if((temp =strstr(para, "\"mqtttime\":\"")) != NULL) {
                temp += strlen("\"mqtttime\":\"");
                memcpy(Net_Info.hold_Time,temp,sizeof(Net_Info.hold_Time));
                nwy_ext_echo("\r\n hold_Time==%s", Net_Info.hold_Time);
            }else if((temp =strstr(para, "\"laswill\":\"")) != NULL) {
                temp += strlen("\"laswill\":\"");
                memcpy(Net_Info.lastWill,temp,sizeof(Net_Info.lastWill));
                nwy_ext_echo("\r\n lastWill==%s", Net_Info.lastWill);
            } else {
                nwy_ext_echo("\r\nFound__none");
            }
// while(para != NULL)
        }while((para = strtok(NULL, ",")) != NULL);

    }


}


void Reply_Cloud_Cmd(char *msg ,int len) {
    memset(msg,0 ,len);
    strcat(msg,"\"cmd\": \"cloud\""); 
    Str_3_Cat(msg, "\"sn\":\"XBWG", xb_sim.nImei,"\",");
    Str_3_Cat(msg, "\"mqttip\":\"", Net_Info.mqttip,"\",");
    Str_3_Cat(msg, "\"mqttport\":\"", Net_Info.mqttport,"\",");
    Str_3_Cat(msg, "\"mqttdomain\":\"", Net_Info.domain,"\",");
    Str_3_Cat(msg, "\"mqttuser\":\"", Net_Info.user,"\",");

    Str_3_Cat(msg, "\"mqttpsw\":\"", Net_Info.pw,"\",");
    Str_3_Cat(msg, "\"mqtttime\":\"", Net_Info.hold_Time,"\",");
    Str_3_Cat(msg, "\"laswill\":\"", Net_Info.lastWill,"\",");
    Str_3_Cat(msg, "\"ts\":\":\"", "12345678","\",");////TODO,
    Str_3_Cat(msg, "\"iccid\":\"", xb_sim.iccid,"\"");

}



int IsRelay_Cmd(char *msg)
{
    uint8_t buf[16];

    uint8_t i = 0;

    for(i = 0; i < 16;i+=2) {
        buf[i/2] = *(msg +i)-'0';
        buf[i/2] <<= 4;
        buf[i/2] += *(msg +i +1)-'0';
    }

    nwy_ext_echo(" \r\ndecode--cmd=%x,%x,%x,%x\r\n", buf[0], buf[1], buf[2], buf[3]);

    if((buf[0] == 0)  && (buf[1] == 0x44)){
        if((buf[3] == 0x55)  && (buf[2] == 0x00)) {
            return 0;
        }
        if((buf[3] == 0x55)  && (buf[2] == 0x11)) {
            return 1;
        }
        if((buf[3] == 0x55)  && (buf[2] == 0x22)) {
            return 2;
        }
    }

    return -1;
}

void Reply_Zone(char *msg, uint16_t value,uint8_t zone) {
    memset(msg, 0 ,sizeof(mqtt_report_Msg));
    strcat(msg,"result");
    if(zone == 0)
        strcat(msg,"0:");
    else if(zone == 1)
        strcat(msg,"1:");
    char tmp_buf[8];
    uint8_t result[2];
    result[0] = value >> 8;
    result[1] = value & 0xff;    
    memset(tmp_buf, 0 , 8);
    convert_hex_Asc(result,2, tmp_buf);              
    strcat(msg,tmp_buf);
}


void conver_Crc(uint16_t crc ,uint8_t *buf) {

    buf[0]  = crc >>8;    
    buf[1] = crc & 0xff;    
}

void conver_Data(uint16_t crc ,uint8_t *buf) {
    buf[0]  = value_zone_0 >>8; 
    buf[1] = value_zone_0 & 0xff;
   
}

uint16_t conver_u8_u16(char *buf) {
    uint16_t tmp;
    tmp = buf[1];
    tmp <<= 8;
    tmp += buf[0];

    return tmp; 
}

void handle_Net_Cmd(char *buf , int len) {
    char *pt;
    pt = buf;
    nwy_ext_echo(" \r\nHandle--Net_Cmd=");
    for(int i = 0;i <len ;i++) {
        nwy_ext_echo("%x-",buf[i]);
    }


    if(buf[0] == 0){
        if(buf[1] == 0x01) {
            mqtt_report_Msg[0] = 0x00;
            mqtt_report_Msg[1] = 0x01;            
            mqtt_report_Msg[2] = value_zone_0 & 0xff;
            mqtt_report_Msg[3] = value_zone_0 >>8;

            uint16_t crc;
            crc = N_CRC16(mqtt_report_Msg,4);
            conver_Crc(crc ,&mqtt_report_Msg[4]);
            mqtt_report_Len = 6;

        } else  if(buf[1] == 0x05) {
            uint16_t tmp;
            tmp = mqtt_report_Msg[5];
            tmp <<= 8;
            tmp +=  mqtt_report_Msg[4];


            for(int j = 0; j < 8;j++) {
              mqtt_report_Msg[j] = buf[j]; 
            }
            mqtt_report_Len = 8;
        } else  if(buf[1] == 0x02) {
            mqtt_report_Msg[0] = 0x00;
            mqtt_report_Msg[1] = 0x02;            

            conver_Data(value_zone_1 ,&mqtt_report_Msg[2]);
            uint16_t crc;
            crc = N_CRC16(mqtt_report_Msg,4);
            conver_Crc(crc ,&mqtt_report_Msg[4]);
            mqtt_report_Len = 6;
        }  else  if(buf[1] == 0x44) {

            uint16_t cmd = 0; 
            cmd =conver_u8_u16(&buf[2]); 
            memcpy(mqtt_report_Msg,buf, 6);     
            mqtt_report_Len = 6;
            int i = 0;
            if(cmd == 0x5500) {i = 0;}
            else  if(cmd == 0x5511) {i = 1;}
            else  if(cmd == 0x5522) {i = 2;}
            else  if(cmd == 0x5533) {i = 3;}
            Waiting_Mqtt(fg_Snding_485);
            nwy_ext_send_sig(g_RS485_Ctrl_thread,EVENT_SND_485_CTRL+i);                     
          //  nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);           

        } 
        nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);
    }





/*     if(strstr(pt,"setzone")){
        pt += strlen("setzone");
        uint8_t zone_num = 0;
        zone_num = *pt;
        if((zone_num =='0') ||(zone_num =='2')) {
            //pt++;
            //pt++;
            pt += strlen("0-");
            char cmd[10];
            uint8_t Result[5];
            memset(cmd, 0 , sizeof(cmd));
            memset(Result, 0 , sizeof(Result));
            memcpy(cmd, pt, 4);
            if(Conver_Asc_Hex(cmd,Result)) {
                 nwy_ext_echo(" \r\nZone0--Net_Cmd");
                 for(int i = 0; i< 5;i++) {
                    nwy_ext_echo("%x-",Result[i]);                     
                 } 
                 uint16_t tmp;
                 tmp = Result[0];
                 tmp <<= 8;
                 tmp +=  Result[1];

                 if(zone_num == '0') {
                    value_zone_0 = tmp;
                    Reply_Zone(mqtt_report_Msg,value_zone_0, 0); 
                    nwy_ext_echo(" \r\nValue0===%x",value_zone_0);
                 } else if(zone_num == '2') {
                    strcat(mqtt_report_Msg,"2:");
                     int i = 0;
                     if(tmp == 0x5500) {i = 0;}
                     else  if(tmp == 0x5511) {i = 1;}
                     else  if(tmp == 0x5522) {i = 2;}
                     Waiting_Mqtt(fg_Snding_485);
                    nwy_ext_send_sig(g_RS485_Ctrl_thread,EVENT_SND_485_CTRL+i);                     
                 }

                 nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);             

            }
        }
    } else if(strstr(pt,"getzone")){
        pt += strlen("getzone");
        if(*pt =='0') {
            Reply_Zone(mqtt_report_Msg,value_zone_0,0);
        } else if(*pt =='1') {
            Reply_Zone(mqtt_report_Msg,value_zone_1,1);
        }
        nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);
    }else if(strstr(pt,"cloud=?")) {
        nwy_ext_echo(" \r\nCloud_Cmd==Query");
       Reply_Cloud_Cmd(mqtt_report_Msg, MSG_REPLY_LEN);
       nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);
    } else if(strstr(pt,"restart")) {
        Reply_Restart(mqtt_report_Msg, MSG_REPLY_LEN);
        nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);
    }  else if((strstr(pt,"\"cmd\": \"sets\","))) {
        pt += strlen("\"cmd\": \"sets\",");
        Handle_Set_Cmd(pt);
        Reply_Set_Cmd(mqtt_report_Msg, MSG_REPLY_LEN);
        nwy_ext_send_sig(mqtt_Snd_task_id,REPORT_MQTT_CTRL_CMD);
    } */


}

nwy_log_cipgsmloc_result_t xb_position;
static void nwy_cipgsmloc_cb(char *text)
{
    nwy_log_cipgsmloc_result_t *param = (nwy_log_cipgsmloc_result_t *)text;
    if(NULL == param) return;
/*     nwy_ext_echo("\r\n cipgsmloc info\r\n");
    if(0 == param->result)
    {
        nwy_ext_echo("lat %lf \r\n", param->info.data.lat);
        nwy_ext_echo("lng %lf \r\n", param->info.data.lng);
        nwy_ext_echo("accuracy %lf \r\n", param->info.data.alt);
    }
    else
    {
        nwy_ext_echo(" %s\r\n", param->info.errmsg);
    } */

    memcpy(&xb_position, param, sizeof(xb_position));

    return ;
}



void Open_Pos_Location(bool fg_On_Off ) {
   nwy_lbs_info_t databuf;   
   nwy_ncell_lbs_info_t data_info;
   bool loc_base_on;
   int result1 = -1;

   loc_base_on = fg_On_Off;
    nwy_ext_echo("\r\n Test_Location--");
	    	//nwy_ext_virtual_at_test();
    	if (1 == nwy_ext_check_data_connect())
		{
			nwy_pdp_set_status(NWY_PDP_CONNECTED);
		}
		else
		{
			nwy_ext_echo("\r\n need module dail at+xiic = 1");
	    	//nwy_ext_virtual_at_test();
		}

		//nwy_sleep(50);

	   nwy_ext_echo("\r\n lbs_mode:%d", 1);

		nwy_loc_get_lbs_info(&databuf);
	//	nwy_ext_echo("\r\n cell_id:%d lac:%d mcc:%d mnc:%d csq:%d imei:%s",databuf.cell_id,databuf.lac,databuf.mcc,databuf.mnc,
	//		databuf.rssi_csq,databuf.imei_str);

		memset(&data_info,0x00,sizeof(data_info));
        nwy_loc_get_ncell_lbs_info(&data_info);
	//	nwy_ext_echo("\r\n cid num: %d ",data_info.num);
/* 		for(int i = 0; i < data_info.num; i++)
		{
		   nwy_ext_echo("\r\n index:%d pci:%d lac:%d cell_id:%d mcc:%d mnc:%d arcfn:%d rsrp:%d",i,data_info.ncell_lbs_info[i].pci,data_info.ncell_lbs_info[i].lac,
		   	data_info.ncell_lbs_info[i].cell_id,data_info.ncell_lbs_info[i].mcc,data_info.ncell_lbs_info[i].mnc,data_info.ncell_lbs_info[i].arcfn,data_info.ncell_lbs_info[i].rsrp);
		} */

		result1 = nwy_loc_cipgsmloc_open(loc_base_on, nwy_cipgsmloc_cb);
        if (result1) {
          nwy_ext_echo("\r\n lbs  success");
        } else {
          nwy_ext_echo("\r\n lbs fail");
        }
	  //  nwy_sleep(1000);
}


