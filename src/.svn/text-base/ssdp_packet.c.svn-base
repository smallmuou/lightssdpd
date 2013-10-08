/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file ssdp_packet.h
 * @brief ssdp packet interface
 *
 * @history
 *  2013-07-29 create by xuwf
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ssdp_packet.h"
#include "ssdp_log.h"

/**
 * struct struct map
 *
 * store headers
 *  
 */
typedef struct header header_t;
struct header {
    char field[64];
    char value[256];
};

static header_t* header_alloc(void) {
    header_t* hdr = calloc(1, sizeof(header_t));
    return hdr;
}

static void header_free(void* val) {
    if (val) free(val);
}

/* return 1 if match, otherwise return 0 */
static int header_match(void* a, void* b) {
    header_t* hdr = (header_t* )b;
    char* field = (char* )a;

    if (!a || !b) return 0;

    return !strcasecmp(hdr->field, field);
}

ssdp_packet_t* ssdp_packet_alloc(void) {
    ssdp_packet_t* pkt = malloc(sizeof(ssdp_packet_t));
    if (!pkt) {
        ERROR("ssdp packet malloc error");
        goto _err;
    }
    
    memset(pkt, 0, sizeof(ssdp_packet_t));

    /* headers */
    pkt->headers = list_new();
    if (!pkt->headers) {
        ERROR("ssdp packet malloc error");
        goto _err;
    }

    pkt->headers->free = header_free;
    pkt->headers->match = header_match;

    return pkt;

_err:
    ssdp_packet_free(pkt);
    return NULL;
}

void ssdp_packet_free(ssdp_packet_t* pkt) {
    if (pkt) {
        if (pkt->headers) {
            list_destroy(pkt->headers);
            pkt->headers = NULL;
        }

        free(pkt);
    }
}

const char* ssdp_packet_header_find(ssdp_packet_t* pkt, const char* field) {
    if (!pkt||!field||!pkt->headers) return NULL;

    list_node_t* node = list_find(pkt->headers, (void*)field);
    if (!node) return NULL;

    header_t* hdr = node->val;

    return hdr->value;
}

static int request_url_cb(http_parser *p, const char *buf, size_t len) {
    ssdp_packet_t* pkt = SSDP_PACKET_PTR(p);
    strncpy(pkt->uri, buf, MIN(sizeof(pkt->uri), len));
    return 0;
}

static int header_field_cb(http_parser *p, const char *buf, size_t len) {
    ssdp_packet_t* pkt = SSDP_PACKET_PTR(p);
    header_t* hdr = header_alloc();
    if (hdr) {
        strncpy(hdr->field, buf, MIN(sizeof(hdr->field), len));
        list_node_t* node = list_node_new(hdr);
        list_rpush(pkt->headers, node);
    }
    return 0;
}

static int header_value_cb(http_parser *p, const char *buf, size_t len) {
    ssdp_packet_t* pkt = SSDP_PACKET_PTR(p);
    list_node_t* node = pkt->headers->tail;
    if (node) {
        header_t* hdr = node->val;
        strncpy(hdr->value, buf, MIN(sizeof(hdr->value), len));
    }
    return 0;
}

static http_parser_settings settings = {
    .on_url = request_url_cb,
    .on_header_field = header_field_cb,
    .on_header_value = header_value_cb,
};

ssdp_packet_t* ssdp_packet_parse(unsigned char *packet, int packet_len) {
    ssdp_packet_t* pkt = ssdp_packet_alloc();

    http_parser_init(&pkt->parser, HTTP_BOTH);
    http_parser_execute(&pkt->parser, &settings, packet, packet_len);

    /* detech */

    return pkt;
}


