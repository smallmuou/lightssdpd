/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file device.h 
 * @brief define device information
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#ifndef __SSDP_DEVICE_H_
#define __SSDP_DEVICE_H_

enum device_type {
    DEVICE_TYPE_UNKNOWN = 0,
    DEVICE_TYPE_IPC,
    DEVICE_TYPE_IHOME,

    DEVICE_TYPE_ALL, /* for search */
};
typedef char device_type_t;

#define DEVICE_MAX_LEN  (32)
#define IP_MAX_LEN      (16)
#define MAC_MAX_LEN     (32)
#define NAME_MAX_LEN    (128)

/* Base device */
struct device {
    device_type_t   type; /* enum device_type */
    char            name[NAME_MAX_LEN];
    char            ip[IP_MAX_LEN]; /* like 192.168.1.100 */
    char            mac[MAC_MAX_LEN]; /* like B8:8D:12:42:D9:94 */
};
typedef struct device device_t;

/* IPC device */
struct ipc_device {
    struct device base;

    short rtsp_port;
};
typedef struct ipc_device ipc_device_t;

/* IHOME device */
struct ihome_device {
    struct device base;
};
typedef struct ihome_device ihome_device_t;

/**
 * @function device_type_valid
 * @brief check device_type if valid
 * @param type
 * @return 
 *      return 0 if invalid, otherwise return 1;
 */
int device_type_valid(device_type_t type);


/**
 * @function convert_type_to_string
 * @brief convert type to string
 * 
 * @param type device type
 * @return 
 *      return NULL if error, otherwise return device type string
 */
const char* convert_type_to_string(device_type_t type);

/**
 * @function convert_string_to_type
 * @brief convert string to type
 *
 * @param str device type string
 * @return 
 *      return device type
 *
 */
device_type_t convert_string_to_type(const char* str);

#endif /* #ifndef __SSDP_DEVICE_H_ */

