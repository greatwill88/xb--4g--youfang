#include "nwy_test_cli\comm\nwy_test_cli_utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "nwy_ble.h"
#include "nwy_ble_client.h"
#include "nwy_wifi.h"
#include "prj.h"

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
    int time = 3879582;
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
    Str_4_Cat(msg, "\"sn\":","\"",xb_sim.nImei,"\",");   
    Str_4_Cat(msg, "\"iccid\":","\"",xb_sim.iccid,"\",");  
    strcat(msg, "}");

}

