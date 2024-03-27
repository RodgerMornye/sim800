#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "i2c-lcd.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"

//=========================================================================
#define SSID "SUTOR 8419"  //Укажите свой имя роутера
#define PASS "12345678"    //Укажите пароль роутера
char APIKEY[] = "7LIB73VNAPQBTJKL"; //Укажите ключ апи thingspeak
#define REQUESTTIME  15     //Укажите интервал в сек для отправки данных на сервер
//=========================================================================


char thingspeak[] = "http://api.thingspeak.com/update?api_key=";
char field1[] = "&field1=";
char field2[] = "&field2=";
char field3[] = "&field3=";


#define LED_GPIO GPIO_NUM_26

static esp_adc_cal_characteristics_t adc1_chars;

static const char *TAG = "i2c-simple-example";

char get_request[100];

int SECOND = 0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_STA_START:printf("WiFi connecting ... \n");break;
        case WIFI_EVENT_STA_CONNECTED:printf("WiFi connected ... \n");break;
        case WIFI_EVENT_STA_DISCONNECTED:printf("WiFi lost connection ... \n");break;
        case IP_EVENT_STA_GOT_IP:printf("WiFi got IP ... \n\n");break;
        default:break;
    }
}

void wifi_connection()
{
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {.sta = {.ssid = SSID,.password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_connect();
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
       case HTTP_EVENT_ON_DATA:printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);break;
       default:break;
    }
    return ESP_OK;
}

static void rest_get(char *str)
{
    esp_http_client_config_t config_get = {
        .url = str,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

int stringFormat(int ldr, float voltage, float temp)
{
	char buf_v[10];
    sprintf(buf_v, "%.2f", voltage);
	char buf_t[10];
    sprintf(buf_t, "%.2f", temp);
	char buf_l[10];
    sprintf(buf_l, "%d", ldr);

	memset(get_request, 0, 100);
	strncat (get_request, thingspeak,strlen(thingspeak));
	strncat (get_request, APIKEY,strlen(APIKEY));
	strncat (get_request, field1,strlen(field1));
	strncat (get_request, buf_l,strlen(buf_l));
	strncat (get_request, field2,strlen(field2));
	strncat (get_request, buf_v,strlen(buf_v));
	strncat (get_request, field3,strlen(field3));
	strncat (get_request, buf_t,strlen(buf_t));
	return strlen(get_request);
}
/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}
unsigned int adc_read(unsigned char ch)
{
	unsigned long res=0;
	for(int i=0;i<10;i++)
	{
		switch(ch)
		{
		  case 0: res+=adc1_get_raw(ADC1_CHANNEL_4);break;
		  case 1: res+=adc1_get_raw(ADC1_CHANNEL_5);break;
		  case 2: res+=adc1_get_raw(ADC1_CHANNEL_6);break;
		}

	}
	return res/10;
}

float voltage()
{
	float f = 0;
	f = ((3.3/4095)*adc_read(0))*7.58;
	return f;
}
float temp()
{
	float t = 0;
	t = (( adc_read(1)/4096.0 )*3.3) *1000/10;
	return t;
}
int ldr()
{
	return adc_read(2);
}
void app_main(void)
{

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_BITWIDTH_12, 0, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_BITWIDTH_12));

    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));

    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");


    lcd_init();
    lcd_clear();

	gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    nvs_flash_init();
    wifi_connection();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n\n");

    while (true)
    {
        gpio_set_level(LED_GPIO, 1);
        usleep(500000);
        gpio_set_level(LED_GPIO, 0);
        usleep(500000);

        float v = voltage();
		float t = temp();
		int l = ldr();

        lcd_send_string(0,0,"LDR");lcd_send_string(4,0,"      "); lcd_send_int(4,0,l);
        lcd_send_string(0,1,"V"); lcd_send_string(2,1,"      "); lcd_send_float(2,1,v);
        lcd_send_string(8,1,"T"); lcd_send_string(10,1,"     "); lcd_send_float(10,1,t);

        lcd_send_int(13,0,stringFormat(l, v, t));

        if(SECOND>REQUESTTIME)
        {
	       rest_get(get_request);
           printf(get_request);printf("\r\n");
           SECOND=0;
        }
        SECOND++;

    }
}
