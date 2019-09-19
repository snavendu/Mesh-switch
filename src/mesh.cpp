#include "mesh.h"

Mesh::Mesh(mesh_type_t node)
{   

            Wifi::init(WIFI_MODE_STA);
            
            // 
            //set_router(); 
            esp_mesh_set_self_organized(true,true);
            xEventGroupWaitBits(wifi_event_group,INIT_BIT,false,true,portMAX_DELAY);
            init();
            set_router(sta_cfg);
            if(isRoot){
                //memset(sta_cfg->sta.bssid, '\0', 6);
                esp_mesh_set_parent(sta_cfg,&cfg.mesh_id,MESH_ROOT,MESH_ROOT_LAYER);
             }

            else{

                
               // ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));
                esp_mesh_set_type(MESH_NODE);
                
                esp_mesh_fix_root(1);
                
            }

     
}

void Mesh::start()
{
    ESP_ERROR_CHECK(esp_mesh_start());
   
}



void Mesh::callback(mesh_event_t event)
{
    ESP_LOGI("MESH","event triggered id : %d",event.id);
    switch(event.id)
    {
        case MESH_EVENT_STARTED:
            esp_mesh_get_id(&addr);
            ESP_LOGI("MESH","<MESH_EVENT_STARTED>ID:"MACSTR"", MAC2STR(addr.addr));
            break;
        case MESH_EVENT_ROOT_ADDRESS:
            ESP_LOGI("MESH","<MESH_EVENT_ROOT_ADDRESS>root address:"MACSTR"",
            MAC2STR(event.info.root_addr.addr));
            break;
        case MESH_EVENT_ROOT_GOT_IP:
        /* root starts to connect to server */
            ESP_LOGI("MESH",
                    "<MESH_EVENT_ROOT_GOT_IP>sta ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
                    IP2STR(&event.info.got_ip.ip_info.ip),
                    IP2STR(&event.info.got_ip.ip_info.netmask),
                    IP2STR(&event.info.got_ip.ip_info.gw));
            break;
        case MESH_EVENT_SCAN_DONE:


           // esp_mesh_connect();
           
            break;

        case MESH_EVENT_FIND_NETWORK:
             //esp_mesh_set_parent();
             break;


        default:
            break;

    }
}


void Mesh::init()
{

    /*  wifi initialization */
    tcpip_adapter_init();
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA));
   
   // ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
 
    
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(4));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));

    cfg.channel=sta_cfg->sta.channel;
    cfg.mesh_id = {0x00,0x00,0x00,0x00,0x00,0x01};

    cfg.mesh_ap.max_connection=2;
    esp_mesh_set_ap_authmode((wifi_auth_mode_t)2);
    strcpy((char*)cfg.mesh_ap.password, "mesh is real");
    
    cfg.event_cb=&callback;
    
    
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

}


mesh_addr_t Mesh::addr={0x00,0x00,0x00,0x00,0x00,0x02};

void Mesh::set_router(wifi_config_t* sta_cfg)
{
    mesh_router_t newinfo = {0};
    strcpy((char*)newinfo.ssid, (char*)sta_cfg->sta.ssid);
    strcpy((char*)newinfo.password,(char*)sta_cfg->sta.password);
    newinfo.ssid_len = strlen((char*) newinfo.ssid);
    strcpy((char*)newinfo.bssid, (char*)sta_cfg->sta.bssid);
    newinfo.allow_router_switch = true;
    //ESP_LOGI(HOMEIOT_TAG, "About to call set router org ssid[%s] password[%s] len[%d]", newinfo.ssid, newinfo.password, newinfo.ssid_len);

    esp_err_t err = esp_mesh_set_router(&newinfo);
    //ESP_LOGI(HOMEIOT_TAG, "Finished set router err [%d = %s]", err, esp_err_to_name(err))
}



