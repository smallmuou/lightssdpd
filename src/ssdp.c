/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file ssdp.c
 * @brief ssdp protocol
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>

#include "ssdp.h"
#include "http_parser.h"
#include "ssdp_log.h"
#include "threadpool.h"
#include "list.h"

#define THREAD_CONCURRENCE_COUNT    (10)
#define THREAD_QUEUE_SIZE           (1024)

/* SSDP standard header field */
char* const HDR_FIELD_ST                        = "ST";
char* const HDR_FIELD_MX                        = "MX";
char* const HDR_FIELD_MAN                       = "MAN";
char* const HDR_FIELD_HOST                      = "HOST";
char* const HDR_FIELD_NTS                       = "NTS";
char* const HDR_FIELD_CACHE_CONTROL             = "CACHE-CONTROL";
char* const HDR_FIELD_LOCATION                  = "LOCATION";
char* const HDR_FIELD_MULTICAST_PORTi           = "MULTICAST-PORT";
char* const HDR_FIELD_SERVER                    = "SERVER";
char* const HDR_FIELD_USN                       = "USN";

/* Star-net IPC <详见：设备检索信息.pdf> */
char* const HDR_FIELD_MAGIC_NUMBER              = "01-MAGICNUMBER";
char* const HDR_FIELD_DEVICE_NAME               = "01-DEVICENAME";
char* const HDR_FIELD_DEVICE_TIME               = "01-DEVICETIME";
char* const HDR_FIELD_DEVICE_TYPE               = "01-DEVICETYPE";
char* const HDR_FIELD_DNAME                     = "01-DNAME";
char* const HDR_FIELD_IPADDR                    = "01-IPADDR";
char* const HDR_FIELD_NETMASK                   = "01-NETMASK";
char* const HDR_FIELD_GATEWAY                   = "01-GATEWAY";
char* const HDR_FIELD_DHCPSET                   = "01-DHCPSet";
char* const HDR_FIELD_DHCPIP                    = "01-DHCPIP";
char* const HDR_FIELD_HTTP_PORT                 = "01-HTTPPORT";
char* const HDR_FIELD_WEB_PORT                  = "01-WEBPORT";
char* const HDR_FIELD_RTSP_PORT                 = "01-RTSPPORT";
char* const HDR_FIELD_MAC                       = "01-MAC";
char* const HDR_FIELD_CHANNEL_NUM               = "01-CHANNUM";
char* const HDR_FIELD_SOFTWARE                  = "01-SOFTWARE";
char* const HDR_FIELD_POWERON_TIME              = "01-POWERONTIME";
char* const HDR_FIELD_MANU_FACTURERS            = "01-MANUFACTURES";

typedef struct thread_data thread_data_t;
struct thread_data {
    unsigned char packet[1024];
    int packet_len;
    int ssdp_sock;
    struct sockaddr_in client_addr;
    void* extra_data;
    ssdp_packet_process_cb pkt_process_cb;
};

int ssdp_socket_init(void) {
    int opt = 1, ret = -1;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        ERROR("socket create error");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SSDP_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Must be set before bind*/
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
   if (ret < 0) WARNING("setsockopt(SO_REUSEADDR) fail");

#if defined(__FreeBSD__) || defined(__OSX__) || defined(__APPLE__)
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
   if (ret < 0) WARNING("setsockopt(SO_REUSEPORT) fail");
#endif

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ERROR("socket bind fail");
        goto _err;
    }

    /* Join multicast group */
    struct ip_mreq imreq;
    imreq.imr_multiaddr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);
    imreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&imreq, 
            sizeof(struct ip_mreq)) < 0) {
        ERROR("join multicast group fail");
        goto _err;
    }

    return sock;
_err:
    if (sock >= 0) close(sock);
    return -1;
}

void ssdp_socket_deinit(int socket) {
    if (socket >= 0) close(socket);
}

int set_socket_blocking(int socket, int block) {
    int flags = fcntl(socket, F_GETFL); 
    if (flags < 0) return -1;

    if (block) {
        flags &= O_NONBLOCK;
        fcntl(socket, F_SETFL, flags & ~O_NONBLOCK);
    } else {
        flags |= O_NONBLOCK;
    }
    
    if (fcntl(socket, F_SETFL, flags) < 0) return -1;

    return 0;
}

void process_ssdp_packet(void* ptr) {
    thread_data_t* data = ptr;
    if (!data) return;

    ssdp_packet_t* ssdp_pkt = ssdp_packet_parse(data->packet, data->packet_len);
    if (!ssdp_pkt) goto _out;

    /* add client address */
    ssdp_pkt->client_addr = &data->client_addr;
    ssdp_pkt->ssdp_sock = data->ssdp_sock;
    ssdp_pkt->extra_data = data->extra_data;

    if (data->pkt_process_cb) data->pkt_process_cb(ssdp_pkt);

_out:
    ssdp_packet_free(ssdp_pkt);
    free(data);
}

/* unit is ms */
static long diff_time(struct timeval* t1, struct timeval* t2) {
    long long t1ms = t1 ? ((t1->tv_sec)*1000 + (t1->tv_usec)/1000) : 0;
    long long t2ms = t2 ? ((t2->tv_sec)*1000 + (t2->tv_usec)/1000) : 0;
    return (t2ms - t1ms);
}

void ssdp_receive_loop(int socket, ssdp_packet_process_cb cb, long idletime, void* extra_data) {
    int quit = 0;
    fd_set readfds;
    threadpool_t* pool = NULL;
    struct timeval timeout, start_time;

    gettimeofday(&start_time, NULL);

    if (socket < 0) return;

    /* Init thread pool */
    pool = threadpool_create(THREAD_CONCURRENCE_COUNT, THREAD_QUEUE_SIZE, 0);
    if (!pool) {
        perror("threadpool create fail\n");
        return;
    }

    /* Receive loop */
    while(!quit) {
        FD_ZERO(&readfds);
        FD_SET(socket, &readfds);
        struct timeval* timeout_ptr = NULL;
        if (idletime > 0) {
            struct timeval cur_time;
            gettimeofday(&cur_time, NULL);
            long dt = idletime - diff_time(&start_time, &cur_time);
            if (dt <= 0) break;

            timeout.tv_sec = dt/1000;
            timeout.tv_usec = (dt%1000)*1000;
            timeout_ptr = &timeout;
        }
        if (select(socket+1, &readfds, NULL, NULL, timeout_ptr) < 0) {
            if(errno != EINTR) break;
            
            continue;
        }

        /* Process ssdp socket */
        if (FD_ISSET(socket, &readfds)) {
            socklen_t addr_len = sizeof(struct sockaddr_in);
            thread_data_t *data = (thread_data_t* )malloc(sizeof(thread_data_t));
            data->pkt_process_cb = cb;
            data->ssdp_sock = socket;
            data->extra_data = extra_data;
            int bytes = recvfrom(socket, data->packet, sizeof(data->packet), 0, 
                    (struct sockaddr*)&(data->client_addr), &addr_len);
            data->packet_len = bytes;

            threadpool_add(pool, process_ssdp_packet, data, 0);
        }
    }
    
    threadpool_destroy(pool, 0);
}
