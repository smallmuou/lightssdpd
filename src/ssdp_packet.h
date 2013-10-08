/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file ssdp_packet.c
 * @brief ssdp packet interface implement
 *
 * @history
 *  2013-07-29 create by xuwf
 */

#ifndef __SSDP_PACKET_H_
#define __SSDP_PACKET_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include "http_parser.h"
#include "list.h"
#define URI_MAX_LENGTH  (256)

/* define container_of */
#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
#undef container_of
#define container_of(ptr, type, member) ({\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);\
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define SSDP_PACKET_PTR(ptr) container_of(ptr, ssdp_packet_t, parser)
  
/**
 * @struct ssdp_packet
 *
 * @brief Define SSDP packet that receieve from net.
 *  
 */
typedef struct ssdp_packet ssdp_packet_t;
struct ssdp_packet {
    http_parser         parser;
    char                uri[URI_MAX_LENGTH];
    list_t*             headers;
    int                 ssdp_sock;
    struct sockaddr_in* client_addr;
    void*               extra_data;
};

#ifndef MAX
#define MAX(a, b)  ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a, b)  ((a)<(b)?(a):(b))
#endif

/**
 * @funtion ssdp_packet_parse
 *
 * @brief parse ssdp packet
 *
 * @param packet        content receieve from multicast group
 * @param packet_len    length of content
 * @param client_addr   client address for reply
 * @return ssdp_packet_t
 *      return NULL if fail, otherwise return ssdp_packet
 *
 * @Note: should call ssdp_packet_free to free return packet
 *      
 */
ssdp_packet_t* ssdp_packet_parse(unsigned char *packet, int packet_len);

/**
 * @function ssdp_packet_free
 *
 * @brief free ssdp packet
 *
 * @param pkt       ssdp packet for free
 * @return null
 *      
 */
void ssdp_packet_free(ssdp_packet_t* pkt);

/**
 * @function ssdp_packet_header_find
 *
 * @brief find header value of field
 *
 * @param pkt       ssdp packet
 * @return value if find, otherwise return NULL
 *      
 */
const char* ssdp_packet_header_find(ssdp_packet_t* pkt, const char* field);

#endif /* #ifndef __SSDP_PACKET_H_ */
