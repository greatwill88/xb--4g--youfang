#ifndef __MY_PRJ__
#define __MY_PRJ__

#include "nwy_config.h"
#include "nwy_sim.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define TEST_HELLO_WORLD 

#define NWY_EXT_SIO_RX_MAX          (2 * 1024)

#define MAX_NUM 32
#define NWY_UART_RECV_SINGLE_MAX   512
#define NWY_FILE_NAME_MAX          32
#define MQTT_TOPIC_SND "test_mqtt_123456789"
#define MQTT_TOPIC_REC "/test/RecMqtt88"
#define MQTT_TOPIC_WILL "xb/testEMQ"



#define LED_YELLOW 0
#define LED_RED 1
#define LED_GREEN 2 
#define LED_ON 1
#define LED_OFF 0

typedef struct nwy_file_ftp_info_s
{
  int is_vaild;
  char filename[256];
  int pos;
  //int length;
  //int file_size;
}nwy_file_ftp_info_s;
typedef enum
{
  NWY_CUSTOM_IP_TYPE_OR_DNS_NONE = -1,
  NWY_CUSTOM_IP_TYPE_OR_DNS_IPV4 = 0,
  NWY_CUSTOM_IP_TYPE_OR_DNS_IPV6 = 1,
  NWY_CUSTOM_IP_TYPE_OR_DNS_DNS = 2
}nwy_ip_type_or_dns_enum;

extern int nwy_ext_sio_len;
extern char nwy_ext_sio_recv_buff[NWY_EXT_SIO_RX_MAX + 1];
extern int get_CCID_Fun(void);
extern int vsnprintf (char *, size_t, const char *, va_list);
extern void test_main(int id);
extern void nwy_xb_mqtt_test(void);
extern void prvThreadEntry_xb_Connect(void *param);
extern void nwy_app_timer_xb(void);
extern void prvThreadEntry_Get_Value(void *param);
extern void nwy_ext_echo(char* fmt, ...);
extern nwy_osiSemaphore_t *s_Call_OK_semaphore;
extern nwy_osiThread_t *g_app_thread_xb;
extern int nwy_get_ip_str(char *url_or_ip, char *ip_str, int *isipv6);
extern void nwy_tcp_test_Mine(int step,char *para1,char *para2,char *para3,char *para4);
extern void nwy_paho_mqtt_test_mine(int step, char *para1, char *para2, char *para3 ,char *para4, char *para5 ,char *para6);
extern int Snd_Mqtt(char *topic,char *qos, char *retain,char *msg);
extern void nwy_tcp_recv_func(void *param);
extern nwy_osiThread_t *tcp_recv_thread;
extern int nwy_port_get(char *port_str, int *port);
extern int nwy_hostname_check(char *hostname);
extern int SubMqtt(char *topic,char *qos);
extern void nwy_wifi_test_xb(void);
extern void nwy_test_cli_ble_client_scan_Para(char *scan_TTT,char *scan_type);
extern void start_Ble_Scan_Thread(void);
extern void nwy_file_test_mine(void);
extern void SET_Led_On_off(uint8_t led_name,uint8_t valu);
extern nwy_sim_result_type xb_sim;

extern void Gernerate_Topic_ctrl(char *topic,int len);
extern void Gernerate_Topic_status(char *topic,int len);

#ifdef __cplusplus
}
#endif


#endif/*__NWY_GPIO_H__*/
