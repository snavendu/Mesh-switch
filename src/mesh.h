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



enum proto_t
{
    JSON,
    MQTT,
    HTTP
};



class Mesh:Wifi{

public:
    Mesh(mesh_type_t);
    void sendData(mesh_addr_t,mesh_data_t,proto_t);
    void recvData(proto_t);
    void start();
    void init();
    void set_router(wifi_config_t*);
    static void callback(mesh_event_t event);
    static void event_loop();

private:
    static mesh_addr_t addr;
    mesh_cfg_t cfg;
    bool isRoot=false;
};

#endif