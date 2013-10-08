/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file lightssdpc.h
 * @brief ssdp client interface
 *
 * @history
 *  2013-07-29 create by xuwf
 */

#ifndef __LIGHTSSDPC_H_
#define __LIGHTSSDPC_H_

#include "device.h"

/**
 * @struct ssdp_result
 *
 * @brief ssdp search result
 */
struct ssdp_result {
    int count;
    device_t* devices[];
};
typedef struct ssdp_result ssdp_result_t;

/**
 * @function ssdp_search_device
 *
 * @brief search device according type
 *
 * @param socket    socket fd
 * @param timeout   time to wait for search. unit is ms
 * @return list_t   return NULL if error, otherwise return list_t
 *
 * @Note should call ssdp_result_free to free result what no longer use
 *      
 */
ssdp_result_t* ssdp_search_device(device_type_t type, long timeout);

/**
 * @function ssdp_search_free
 *
 * @brief free the result that call by ssdp_search_device 
 *
 * @param result search result
 * 
 * @return null
 *      
 */
void ssdp_result_free(ssdp_result_t* result);

#endif /* #ifndef __LIGHTSSDPC_H_ */
