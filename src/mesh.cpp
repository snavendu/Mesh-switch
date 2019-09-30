#include "mesh.h"

Mesh::Mesh(mesh_type_t node)
{   

            Wifi::init(WIFI_MODE_STA);
            esp_mesh_set_self_organized(true,true);
            xEventGroupWaitBits(wifi_event_group,INIT_BIT,false,true,portMAX_DELAY);
            init();
            //set_router(sta_cfg);   
            if(node==MESH_ROOT){
                memset(sta_cfg->sta.bssid, '\0', 6);
                esp_mesh_set_type(MESH_ROOT);
                isRoot=true;
                esp_mesh_set_parent(sta_cfg,&cfg.mesh_id,MESH_ROOT,MESH_ROOT_LAYER);
             }

            else{
                //ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));
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
    //ESP_LOGI("MESH","event triggered id : %d",event.id);
    switch(event.id)
    {


// ******************  mesh main events **********************************************
        case MESH_EVENT_STARTED:
            esp_mesh_get_id(&addr);
            ESP_LOGI("MESH","<MESH_EVENT_STARTED>ID:"MACSTR"", MAC2STR(addr.addr));
            break;

        case MESH_EVENT_SCAN_DONE:
            ESP_LOGI("MESH","wifi scan done the number of AP's  found is %d",event.info.scan_done.number); 
            break;

        case MESH_EVENT_PARENT_CONNECTED:
            ESP_LOGI("MESH","parent connected and the ssid of ap is %s",event.info.connected.connected.ssid);
            xEventGroupSetBits(mesh_event,PARENT_CONNECTED);
            break;

        case MESH_EVENT_PARENT_DISCONNECTED:
            ESP_LOGI("MESH","parent disconnected and the ssid is %s",event.info.disconnected.ssid);
            xEventGroupClearBits(mesh_event,PARENT_CONNECTED);
            break;

        case MESH_EVENT_NO_PARENT_FOUND:
            ESP_LOGI("MESH","no parent found");
            break;

        case MESH_EVENT_STOPPED:
            ESP_LOGI("MESH","mesh has stopped");
            break;

//********************************  Mesh network updates events ***************************************************

        case MESH_EVENT_CHILD_CONNECTED:
            ESP_LOGI("MESH", "child connected mac addr is :" MACSTR, MAC2STR(event.info.child_connected.mac));
            break;

        case MESH_EVENT_CHILD_DISCONNECTED:
            ESP_LOGI("MESH","child is disconnected and mac addr ofchild is :" MACSTR , MAC2STR(event.info.child_disconnected.mac));

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
      
        case MESH_EVENT_FIND_NETWORK:

            ESP_LOGI("MESH","network is found %d",event.info.find_network.channel);
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
    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));
   
   // ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
 
    
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(4));
    //ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));

    cfg.channel=10;//sta_cfg->sta.channel;
    cfg.mesh_id = {0x00,0x00,0x00,0x00,0x00,0x01};
    ESP_LOGI("MESH","the current cahnnesl received is %d",sta_cfg->sta.channel);
    strcpy((char*)cfg.router.ssid, (char*)sta_cfg->sta.ssid);
    strcpy((char*)cfg.router.password,(char*)sta_cfg->sta.password);
    cfg.router.ssid_len = strlen((char*) cfg.router.ssid);
    memset(cfg.router.bssid, '\0', 6);

    cfg.mesh_ap.max_connection=2;
    esp_mesh_set_ap_authmode((wifi_auth_mode_t)2);
    strcpy((char*)cfg.mesh_ap.password, "mesh is real");
    
    cfg.event_cb=&callback;
    
    
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

}


mesh_addr_t Mesh::addr={0x00,0x00,0x00,0x00,0x00,0x02};

void Mesh::sendData()
{
    mesh_data_t data;
    uint8_t send_buffer[1472];
    data.proto = MESH_PROTO_BIN;
    on.id=1;
    on.value=1;
    data.size=sizeof(on);
    
    memcpy(send_buffer,(uint8_t*)&on,sizeof(on));
    data.data=send_buffer;

    if(!esp_mesh_send(NULL,&data,NULL,NULL,0)){
        ESP_LOGI("MESH","sent successfully");

    }

    else{
        ESP_LOGI("MESH","error occured");
    }

}

void Mesh::recvData()
{
    mesh_addr_t from={0};
    mesh_data_t data={};
    uint8_t pkt[1500];
    data.data=pkt;
    if(!esp_mesh_recv(&from,&data,portMAX_DELAY,0,NULL,1)){
        ESP_LOGI("MESH","received error");
    }

   packet  off = *((packet*)pkt);
    ESP_LOGI("MESH","the id of received message is %d",off.id);
}


EventGroupHandle_t Mesh::mesh_event = xEventGroupCreate();



