/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "platform.h"
#include "c_string.h"
#include "c_stdlib.h"
#include "c_stdio.h"
#include "flash_api.h"

#include "osapi.h"

#include "user_interface.h"
#include "user_config.h"

#include "ets_sys.h"
#include "driver/uart.h"
#include "driver/relay.h"
#include "mem.h"

#include "dns.h"
#include "serial_number.h"

#include "http/app.h"
#include "mqtt/app.h"

#include "sensor/sensors.h"

#ifdef DEVELOP_VERSION
os_timer_t heapTimer;

static void heapTimerCb(void *arg){

    NODE_DBG("FREE HEAP: %d",system_get_free_heap_size());

}

#endif




static void config_wifi(){
    NODE_DBG("Putting AP UP");

    platform_key_led(0);    
    
    wifi_station_set_auto_connect(1); 
    wifi_set_opmode(0x03); // station+ap mode                       

    struct softap_config config;
    wifi_softap_get_config(&config);

    char ssid[]="SmartRelay"SERIAL_NUMBER;

    strcpy(config.ssid,ssid);
    memset(config.password,0,64);
    config.ssid_len=strlen(ssid);
    config.channel=11;
    config.authmode=AUTH_OPEN;
    config.max_connection=4;
    config.ssid_hidden=0;

    wifi_softap_set_config(&config);

    
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{   
    
    system_update_cpu_freq(160); //overclock :)

    uart_init(BIT_RATE_115200,BIT_RATE_115200);

    NODE_DBG("User Init");

    uint32_t size = flash_get_size_byte();
    NODE_DBG("Flash size %d",size);
   
    config_wifi();
    
	relay_init();   
    
    init_dns();
    init_http_server();

    //uncomment to send data to mqtt broker
    //mqtt_app_init();    

    //uncomment if you have sensors intalled
    //sensors_init();

    #ifdef DEVELOP_VERSION

    //arm timer
    os_memset(&heapTimer,0,sizeof(os_timer_t));
    os_timer_disarm(&heapTimer);
    os_timer_setfn(&heapTimer, (os_timer_func_t *)heapTimerCb, NULL);
    os_timer_arm(&heapTimer, 5000, 1);

    #endif
}
