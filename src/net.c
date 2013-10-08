/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file net.c
 * @brief net util interfaces
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#if defined(__FreeBSD__) || defined(__OSX__) || defined(__APPLE__)
#include <net/if_dl.h>
#include <ifaddrs.h>
#endif

#include "net.h"

static int strupper(char* str) {
    if (!str) return -1;

    while (*str) {
        *str = (char)toupper(*str);
        str++;
    }
    return 0;
}

int net_info(const char* ethname, char* ip, int ip_len, char* mac, int mac_len) {

    /* MAC */
#if defined(__FreeBSD__) || defined(__OSX__) || defined(__APPLE__)
    struct ifaddrs *ifap, *ifaptr;

    if (getifaddrs(&ifap) == 0) {
        for(ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
            if (!strcasecmp(ifaptr->ifa_name, ethname)) {
                int af = ifaptr->ifa_addr->sa_family;
                switch (af) {
                case AF_LINK:{
                    unsigned char *ptr = (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
                    snprintf(mac, mac_len, "%02x:%02x:%02x:%02x:%02x:%02x",
                        (unsigned char)ptr[0],
                        (unsigned char)ptr[1],
                        (unsigned char)ptr[2],
                        (unsigned char)ptr[3],
                        (unsigned char)ptr[4],
                        (unsigned char)ptr[5]);
                    strupper(mac);
                    break;
                }
                case AF_INET:
                    snprintf(ip, ip_len, "%s", inet_ntoa(((struct sockaddr_in*)(ifaptr)->ifa_addr)->sin_addr));
                    break;
                case AF_INET6:
                    /* IPV6 Address */
                    break;
                }
                


            }
        }
        freeifaddrs(ifap);
    }
#else
    int fd, intrface;
    struct ifreq buf[16];
    struct ifreq ifr;
    struct ifconf ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return -1;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;

    /* IP */
    if (ioctl(fd, SIOCGIFCONF, (char* )&ifc) < 0) {
        perror("ioctl(SIOCGIFCONF)");
        goto _err;
    }

    intrface  = ifc.ifc_len/sizeof(struct ifreq);
    while (intrface-- > 0) {
        if (strstr(buf[intrface].ifr_name, ethname)) {
            if ((ioctl(fd, SIOCGIFADDR, (char* )&buf[intrface])) < 0) {
                perror("ioctl(SIOCGIFADDR)");
            }
            snprintf(ip, ip_len, "%s", inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
            break;
        }
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, ethname, sizeof(ifr.ifr_name));
    if (ioctl(fd,SIOCGIFHWADDR, &ifr) == 0) {
        snprintf(mac, mac_len, "%02x:%02x:%02x:%02x:%02x:%02x",
                (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        strupper(mac);
    }

    return 0;
_err:
    if (fd >= 0) close(fd);
    return -1;

#endif

    return 0;
}

