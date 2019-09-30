#ifndef MESH_H
#define MESH_h

#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"
#include "Wifi.h"



// enum proto_t
// {
//     JSON,
//     MQTT,
//     HTTP
// };


typedef struct{
    uint8_t id;
    uint8_t value;
}packet;



class Mesh:Wifi{

public:
    Mesh(mesh_type_t);
    static EventGroupHandle_t mesh_event ;
    static const  uint8_t PARENT_CONNECTED = BIT0;

    void sendData();
    void recvData();
    void start();
    void init();
    void set_router(wifi_config_t*);
    static void callback(mesh_event_t event);
    static void event_loop();
    bool isRoot=false;
private:
    static mesh_addr_t addr;
    mesh_cfg_t cfg;
    
    packet on;
};

#endif