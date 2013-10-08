/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file device_private.h 
 * @brief device  private interface
 *
 * @history
 *  2013-08-02 create by xuwf
 */

#ifndef __SSDP_DEVICE_PRIVATE_H_
#define __SSDP_DEVICE_PRIVATE_H_

#include "device.h"
/**
 * @function device_factory
 * @brief product device according type
 * @param type  device type
 * @param name device name
 * @param ip    ip address
 * @param mac   mac address
 *
 * @return 
 *      return NULL if error, otherwise return device
 */
device_t* ssdp_device_factory(device_type_t type, const char* name,  const char* ip, const char* mac);

/**
 * @function device_free
 * @brief free device
 * @param dev device for free
 * @return null
 */
void ssdp_device_free(device_t* dev);


#endif

