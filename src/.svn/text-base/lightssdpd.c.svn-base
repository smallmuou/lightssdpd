/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file lightssdpd.c
 * @brief ssdp server implement
 *
 * @history
 *  2013-07-30 create by xuwf
 */

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "ssdp.h"
#include "daemonize.h"
#include "ssdp_log.h"
#include "ssdp_packet.h"
#include "device.h"
#include "net.h"

static device_type_t server_type = DEVICE_TYPE_UNKNOWN;
static char server_type_str[DEVICE_MAX_LEN] = {0};
static char server_ip[IP_MAX_LEN] = {0};
static char server_mac[MAC_MAX_LEN] = {0};
static char server_name[NAME_MAX_LEN] = {0};

static int print_usage(void) {
    printf("Usage: %s -i Ethname(eth0, eth1) -t ServerType(IHOME|IPC) [options]\n", PACKAGE);
    printf("\
Options:\n\
    -n Servername.\n\
    -d Debug mode.\n\
    -l Log file path.\n\
    -h Show usage.\n\
    -n Device name.\n\
    -v Show version and exit.\n"); 
}

static void ssdpd_response_msearch(int ssdp_sock, struct sockaddr_in* dst_addr) {
    char buf[1024] = {0};

    snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
            "ST:upnp:rootdevice\r\n"
            "CACHE-CONTROL:max-age=1800\r\n"
            "01-DeviceType:%s\r\n"
            "01-DName:%s\r\n"
            "01-IPADDR:%s\r\n"
            "01-MAC:%s\r\n"
            "\r\n"
            , server_type_str, server_name, server_ip, server_mac);

    int len = strlen(buf);
    ssize_t bytes = sendto(ssdp_sock, buf, len, 0, (struct sockaddr* )dst_addr, 
            sizeof(struct sockaddr_in));
    if (bytes != len) {
        WARNING("Send M-SEARCH response fail");
    } else {
        INFO("Send M-SEARCH response successed: %s", inet_ntoa(dst_addr->sin_addr));
    }
}

static void ssdpd_send_notify(int sock) {
    char buf[1024] =  {0};
    snprintf(buf, sizeof(buf), 
                "NOTIFY * HTTP/1.1\r\n"
                "HOST:%s:%d\r\n"
                "NT:upnp:rootdevice\r\n"
                "NTS:ssdp:alive\r\n"
                "01-DeviceType:%s\r\n"
                "01-DName:%s\r\n"
                "01-IPADDR:%s\r\n"
                "01-MAC:%s\r\n"
                "\r\n", SSDP_MULTICAST_ADDR, SSDP_PORT, 
                server_type_str, server_name, server_ip, server_mac);
    

    int len = strlen(buf);
    struct sockaddr_in dst_addr;
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(SSDP_PORT);
    dst_addr.sin_addr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);


    ssize_t bytes = sendto(sock, buf, len, 0, (struct sockaddr* )&dst_addr, 
            sizeof(struct sockaddr_in));
    if (bytes != len) {
        WARNING("Send Notify fail\n");
    } else {
        INFO("Send Notify successed\n");
    }
}

/* ssdp server process callback */
static void ssdpd_process(ssdp_packet_t* pkt) {

    /* Only process M-SEARCH request */
    http_method_t method = pkt->parser.method;
    if (method == HTTP_MSEARCH) {
        INFO("Process M-SEARCH request from %s", inet_ntoa(pkt->client_addr->sin_addr));
        const char* st = ssdp_packet_header_find(pkt, HDR_FIELD_ST);
        if (st && !strcasecmp(st, "upnp:rootdevice")) {
            ssdpd_response_msearch(pkt->ssdp_sock, pkt->client_addr);
        }
    }
}

int main (int argc, char* argv[]) {
    int arg;
    int ssdp_sock = -1;
    const char* log_file = NULL;
    int daemon = 1;

    int show_version = 0;
    int has_eth = 0;
    int has_type = 0;

    /* args */
    while ((arg = getopt(argc, argv, "dvhi:t:n:l:")) != -1) {
        switch (arg) {
            case 'l':
                log_file = optarg;
                break;
            case 'd':
                daemon = 0;
                break;
            case 'v':
                show_version = 1;
                break;
            case 'i':
                if (net_info(optarg, server_ip, sizeof(server_ip), server_mac, sizeof(server_mac)) < 0) {
                    printf("ethname unknown\n");
                    print_usage();
                    exit(0);
                }
                has_eth = 1;
                break;
            case 'n':
                strncpy(server_name, optarg, sizeof(server_name));
                break;
            case 't': {
                strncpy(server_type_str, optarg, sizeof(server_type_str));
                server_type = convert_string_to_type(server_type_str);
                if (!device_type_valid(server_type)) {
                    printf("Device type unknown\n");
                    print_usage();
                    exit(0);
                }
                has_type = 1;
                break;
            }
            case 'h':
            default:
                print_usage();
                exit(0);
        }
    }

    if (show_version) {
        printf("%s %s\n", PACKAGE, VERSION);
        exit(0);
    }

    /* should config ethname and serverType */
    if (!has_type || !has_eth) {
        print_usage();
        exit(0);
    }

    /* Daemonize */
    if (daemon) { 
        daemonize();
        ssdp_log_init(log_file);
    } 

    /* Socket */
    ssdp_sock = ssdp_socket_init();
    if (ssdp_sock < 0) {
        perror("ssdp socket init fail\n");
        return -1;
    }

    INFO("SSDP Server start!");

    /* Send notify */
    ssdpd_send_notify(ssdp_sock);

    /* Recieve loop */
    ssdp_receive_loop(ssdp_sock, ssdpd_process, 0, NULL);

    ssdp_socket_deinit(ssdp_sock);

    INFO("SSDP Server stop!");

    ssdp_log_deinit();
}


