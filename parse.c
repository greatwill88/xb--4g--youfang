#include "nwy_test_cli\comm\nwy_test_cli_utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "nwy_ble.h"
#include "nwy_ble_client.h"
#include "nwy_wifi.h"
#include "prj.h"

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
    Str_3_Cat(msg, "\"sn\":\"XBwg", xb_sim.nImei,"\",");
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
    if(pt) {
        memcpy(pt, buf, len);
        para = strtok(pt, "\",");
        if(para == NULL) return ;
        do {
            if(temp =strstr(para, "\"mqttip\": \"") != NULL) {
                temp += strlen("\"mqttip\": \"");
                memcpy(Net_Info.mqttip,temp,sizeof(Net_Info.mqttip,temp));
            } else if(temp =strstr(para, "\"mqttport\": \"") != NULL) {
                temp += strlen("\"mqttport\": \"");
                memcpy(Net_Info.mqttport,temp,sizeof(Net_Info.mqttport));
            } else if(temp =strstr(para, "\"mqttdomain\": \"") != NULL) {
                temp += strlen("\"mqttdomain\": \"");
                memcpy(Net_Info.domain,temp,sizeof(Net_Info.domain));
            } else if(temp =strstr(para, "\"mqttuser\": \"") != NULL) {
                temp += strlen("\"mqttuser\": \"");
                memcpy(Net_Info.user,temp,sizeof(Net_Info.user));
            }else if(temp =strstr(para, "\"mqttpsw\": \"") != NULL) {
                temp += strlen("\"mqttpsw\": \"");
                memcpy(Net_Info.pw,temp,sizeof(Net_Info.pw));
            }else if(temp =strstr(para, "\"mqtttime\": \"") != NULL) {
                temp += strlen("\"mqtttime\": \"");
                memcpy(Net_Info.hold_Time,temp,sizeof(Net_Info.hold_Time));
            }else if(temp =strstr(para, "\"laswill\": \"") != NULL) {
                temp += strlen("\"laswill\": \"");
                memcpy(Net_Info.lastWill,temp,sizeof(Net_Info.lastWill));
            }
// while(para != NULL)

        }while((para = strtok(NULL, "\",")) != NULL);







    }


}


void Reply_Cloud_Cmd(char *msg ,int len){
    memset(msg,0 ,len);
    strcat(msg,"\"cmd\": \"cloud\""); 
    Str_3_Cat(msg, "\"sn\":\"XBwg", xb_sim.nImei,"\",");
    Str_3_Cat(msg, "\"mqttip\":\"", Net_Info.mqttip,"\",");
    Str_3_Cat(msg, "\"mqttport\":\"", Net_Info.mqttport,"\",");
    Str_3_Cat(msg, "\"mqttdomain\":\"", Net_Info.domain,"\",");
    Str_3_Cat(msg, "\"mqttuser\":\"", Net_Info.user,"\",");

    Str_3_Cat(msg, "\"mqttpsw\":\"XBwg", Net_Info.pw,"\",");
    Str_3_Cat(msg, "\"mqtttime\":\"XBwg", Net_Info.hold_Time,"\",");
    Str_3_Cat(msg, "\"laswill\":\"XBwg", Net_Info.lastWill,"\",");
    Str_3_Cat(msg, "\"ts\":\"XBwg", "12345678","\",");////TODO,
    Str_3_Cat(msg, "\"iccid\":\"XBwg", xb_sim.iccid,"\"");

}

/* void Reply_Set_Cmd(char *msg ,int len){
    memset(msg,0 ,len);
    strcat(msg,"\"cmd\": \"cloud\""); 
    Str_3_Cat(msg, "\"sn\":\"XBwg", xb_sim.nImei,"\",");
    Str_3_Cat(msg, "\"mqttip\":\"", Net_Info.mqttip,"\",");
    Str_3_Cat(msg, "\"mqttport\":\"", Net_Info.mqttport,"\",");
    Str_3_Cat(msg, "\"mqttdomain\":\"", Net_Info.domain,"\",");
    Str_3_Cat(msg, "\"mqttuser\":\"", Net_Info.user,"\",");

    Str_3_Cat(msg, "\"mqttpsw\":\"XBwg", Net_Info.pw,"\",");
    Str_3_Cat(msg, "\"mqtttime\":\"XBwg", Net_Info.hold_Time,"\",");
    Str_3_Cat(msg, "\"laswill\":\"XBwg", Net_Info.lastWill,"\",");

}
 */


void handle_Net_Cmd(char *buf) {
    char *pt;
    pt = buf;

    if(strstr(pt,"cloud=?")) {
       Reply_Cloud_Cmd(mqtt_report_Msg, MSG_REPLY_LEN);

    } else if(strstr(pt,"restart")) {
        Reply_Restart(mqtt_report_Msg, MSG_REPLY_LEN);
    }  else if((strstr(pt,"\"cmd\": \"sets\","))) {
        pt += strlen("\"cmd\": \"sets\",");
        Handle_Set_Cmd(pt);
        Reply_Set_Cmd(mqtt_report_Msg, MSG_REPLY_LEN);
    }



}