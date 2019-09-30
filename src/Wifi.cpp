#include "Wifi.h"

Wifi::Wifi( )
{
    tcpip_adapter_init();
}

EventGroupHandle_t Wifi::wifi_event_group=xEventGroupCreate();

void Wifi::init(wifi_mode_t type)
{   
    
    init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    esp_event_loop_init(NULL,NULL);
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));
    esp_wifi_start();
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
    
    //mode_init(type);
    //esp_wifi_connect();
    
}

void  Wifi::mode_init(wifi_mode_t mode)
{
    switch(mode){
        case WIFI_MODE_STA:
            
            ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
            esp_wifi_set_config(WIFI_IF_STA,sta_cfg);
            ESP_LOGI("wifi","ssid:%s  and password:%s",sta_cfg->sta.ssid,sta_cfg->sta.password);
            ESP_LOGI("wifi","i am here to configure");
            break;

        case WIFI_MODE_AP:
            esp_event_loop_init(handler,NULL);
            ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
            wifi_config_t ap_cfg ; 
            //strcpy((char*)ap_cfg.ap.ssid,ap_ssid);
            //strcpy((char*)ap_cfg.ap.password,ap_password);
            ap_cfg.ap.authmode=WIFI_AUTH_WPA2_ENTERPRISE;
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_cfg));
            break;

        default:
        break;
             

    }

}

esp_err_t Wifi::reinit()
{   
    esp_wifi_stop();
	vTaskDelay(200 / portTICK_PERIOD_MS);

	esp_wifi_deinit();
	vTaskDelay(200 / portTICK_PERIOD_MS);

    init(WIFI_MODE_STA);
    start();
    
// init(WIFI_MODE_STA);
return ESP_OK;   
}


esp_err_t Wifi::scan()
{
        uint16_t num=5;
        
        wifi_scan_config_t scan_config ;
        scan_config.ssid = 0;
        scan_config.bssid = 0;
        scan_config.channel = 0;
        scan_config.show_hidden = true;
            
        esp_wifi_scan_start(&scan_config,1);
        wifi_ap_record_t ap_records[5];
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num, ap_records));
        for(int i=0;i<5;i++)
        {
            ESP_LOGI("wifi","ssids %s",ap_records[i].ssid);
        }

        return ESP_OK;
}

esp_err_t Wifi::handler(void*ctx,system_event_t*event)
{
      switch(event->event_id)
  {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI("wifi","wifi started ");
        break;

    case SYSTEM_EVENT_SCAN_DONE:
        ESP_LOGI("wifi","scan done ");
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI("wifi","wifi connected  %s",event->event_info.connected.ssid);
        break;
   
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI("wifi","ip allocated ");
        xEventGroupSetBits(wifi_event_group,CONNECTED_BIT);
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI("wifi", "Wi-Fi Reason: (%d)", event->event_info.disconnected.reason);
        ESP_LOGI("wifi","disconnected ");
        xEventGroupClearBits(wifi_event_group,CONNECTED_BIT);
        esp_wifi_connect();
        break;

    default:
        break;
  }

      return ESP_OK;

}


void Wifi::sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status) {
        case SC_STATUS_WAIT:
            ESP_LOGI("scan", "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI("scan", "SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI("scan", "SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
            ESP_LOGI("scan", "SC_STATUS_LINK");
            sta_cfg = (wifi_config_t*)pdata;
            xEventGroupSetBits(wifi_event_group, INIT_BIT);
            break;
        case SC_STATUS_LINK_OVER:
            ESP_LOGI("scan", "SC_STATUS_LINK_OVER");
            if (pdata != NULL) {
                uint8_t phone_ip[4] = { 0 };
                memcpy(phone_ip, (uint8_t* )pdata, 4);
                ESP_LOGI("wifi", "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            
            esp_smartconfig_stop();
            break;
        default:
            break;
    }

}

void Wifi::wait_for_connect()
{
    xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,false,true,portMAX_DELAY);
}


wifi_config_t* Wifi::sta_cfg;