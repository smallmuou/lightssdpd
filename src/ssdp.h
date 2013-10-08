/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file ssdp.h
 * @brief ssdp protocol
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#ifndef __SSDP_H_
#define __SSDP_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include "http_parser.h"
#include "ssdp_packet.h"
#include "list.h"

/* SSDP standard header field */
extern char* const HDR_FIELD_ST;
extern char* const HDR_FIELD_MX;
extern char* const HDR_FIELD_MAN;
extern char* const HDR_FIELD_HOST;
extern char* const HDR_FIELD_NTS;
extern char* const HDR_FIELD_CACHE_CONTROL;
extern char* const HDR_FIELD_LOCATION;
extern char* const HDR_FIELD_MULTICAST_PORT;
extern char* const HDR_FIELD_SERVER;
extern char* const HDR_FIELD_USN;

/* Star-net IPC <详见：设备检索信息.pdf> */
extern char* const HDR_FIELD_MAGIC_NUMBER;
extern char* const HDR_FIELD_DEVICE_NAME;
extern char* const HDR_FIELD_DEVICE_TIME;
extern char* const HDR_FIELD_DEVICE_TYPE;
extern char* const HDR_FIELD_DNAME;
extern char* const HDR_FIELD_IPADDR;
extern char* const HDR_FIELD_NETMASK;
extern char* const HDR_FIELD_GATEWAY;
extern char* const HDR_FIELD_DHCPSET;
extern char* const HDR_FIELD_DHCPIP;
extern char* const HDR_FIELD_HTTP_PORT;
extern char* const HDR_FIELD_WEB_PORT;
extern char* const HDR_FIELD_RTSP_PORT;
extern char* const HDR_FIELD_MAC;
extern char* const HDR_FIELD_CHANNEL_NUM;
extern char* const HDR_FIELD_SOFTWARE;
extern char* const HDR_FIELD_POWERON_TIME;
extern char* const HDR_FIELD_MANU_FACTURERS;


#define SSDP_PORT                   (1900)
#define SSDP_MULTICAST_ADDR         ("239.255.255.250")

/**
 * @function ssdp_socket_init
 *
 * @brief init ssdp socket
 *
 * @return
 *  return -1 if an error occurs, otherwise the return value is a descriptor
 *  referencing the socket
*/
int ssdp_socket_init(void);


/**
 * @function ssdp_socket_deinit
 *
 * @brief deinit ssdp socket
 *
 * @param socket    socket fd
 * @return null
 *      
 */
void ssdp_socket_deinit(int socket);


/**
 * @function set_socket_blocking
 *
 * @brief   set socket block, if block =1 then set blocking, else if block = 0 then set
 *          nonblocking
 *
 * @param socket    socket fd
 * @param block     block flag
 * @return 
 *      return -1 if fail, otherwise return 0
 *      
 */
int set_socket_blocking(int socket, int block);

/**
 * @typedef ssdp_packet_process_cb
 *
 * @brief callback for process ssdp_packet
 *      
 */
typedef void (*ssdp_packet_process_cb)(ssdp_packet_t* ); 

/**
 * @function ssdp_receive_loop
 *
 * @brief ssdp receive loop
 *
 * @param socket    socket fd
 * @param idletime  idle time, if idletime=0, forever loop
 * @param extra_data extra data
 * @return null
 *      
 */
void ssdp_receive_loop(int socket, ssdp_packet_process_cb cb, long idletime, void* extra_data);

#endif
