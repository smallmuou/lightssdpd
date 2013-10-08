/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file ssdp_log.h
 * @brief ssdp log
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#ifndef __SSDP_LOG_
#define __SSDP_LOG_

/* Log level */
enum {
    SSDP_LOG_ERR = 0, 
    SSDP_LOG_WARNING,
    SSDP_LOG_INFO,
    SSDP_LOG_DEBUG,
    SSDP_LOG_MAX,
};

#ifndef DEBUG
#define DEBUG(x...) ssdp_log_message(SSDP_LOG_DEBUG, "[%s:%d] " x, __FILE__, __LINE__)
#else
#define DEBUG(x...) do { } while(0)
#endif

#undef ERROR
#define ERROR(x...) ssdp_log_message(SSDP_LOG_ERR, x)

#undef WARNING
#define WARNING(x...) ssdp_log_message(SSDP_LOG_WARNING, x)

#undef INFO
#define INFO(x...) ssdp_log_message(SSDP_LOG_INFO, x)

/**
 * log_init
 *
 * @param log_file
 *      log file path, if log_file=null, default /var/log/lightssdpd.log
 *          
 * @param fmt
 *      format args
 *
 * @return 
 *      return 0 if success, otherwise return -1
*/
int ssdp_log_init(const char* log_file);

/**
 * log_deinit
 *
 * @return null
*/
void ssdp_log_deinit(void);

/**
 * @funtion log_message
 *
 * @brief log message
 *
 * @param level
 *          log level
 * @param fmt
 *          format args
 * @return null
*/
void ssdp_log_message(int level, char* fmt, ...);

#endif

