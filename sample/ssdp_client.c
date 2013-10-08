#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lightssdpc.h>

int search_device(device_type_t type) {
    printf("search device type: %s ...\n", convert_type_to_string(type));

    ssdp_result_t* result = ssdp_search_device(type, 1000);
    if (result) {
        int i = 0;
        if (result->count) {
            for (i = 0; i < result->count; i++) {
                device_t* dev = result->devices[i];
                printf("device info:name=%s, ip=%s, mac=%s, rtspport=%d\n", 
                        dev->name, dev->ip, dev->mac, ((ipc_device_t*)dev)->rtsp_port);
            }
        }
        ssdp_result_free(result);
    }
}

void print_usage(void) {
    printf("Usage:ssdp_client (IPC|IHOME)\n");
}

int main (int argc, char* argv[]) {
    device_type_t type = DEVICE_TYPE_UNKNOWN;
    if (argc < 2) {
        print_usage();
        exit(-1);
    } else {
        if (!strcasecmp(argv[1], "IPC")) {
            type = DEVICE_TYPE_IPC;
        } else if (!strcasecmp(argv[1], "IHOME")) {
            type = DEVICE_TYPE_IHOME;
        }
    }

    if (type == DEVICE_TYPE_UNKNOWN) {
        print_usage();
        exit(-1);
    }

    search_device(type);
}

