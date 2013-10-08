/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file net.h
 * @brief net util interfaces
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#ifndef __SSDP_NET_H_
#define __SSDP_NET_H_


/**
 * @function net_info
 *
 * @brief get ip and mac
 *
 * @param ethname   like "eth0"、“en0”
 * @param ip        for fill ip 
 * @param ip_len    ip max len
 * @param mac       for fill mac
 * @param mac_len   mac max len
 *
 * @return
 *  return -1 if an error occurs, otherwise return 00
 *
 * @Note only support ipv4
*/
int net_info(const char* ethname, char* ip, int ip_len, char* mac, int mac_len);

#endif /* #ifndef __SSDP_NET_H_ */

