#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"
#include "nvs_flash.h"
#include "mesh.h"

extern "C" {
 int app_main(void);
}


int app_main()
{   
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    Mesh mesh(MESH_ROOT);
    
    // mesh.mesh_init()
    mesh.start();

    while(1)
    {
        if(!mesh.isRoot)
        {
            xEventGroupWaitBits(mesh.mesh_event,mesh.PARENT_CONNECTED,false,true,portMAX_DELAY);
            mesh.sendData();
            vTaskDelay(1000);
        }

        else{
            mesh.recvData();
        }
    }
    
    // mesh.send(addr,data);
    // mesh.recv(addr);
    
    return 0;
}



