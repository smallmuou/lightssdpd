/**
 * @copyright (C) 2013 eVideo. All rights reserved.
 *
 * @file ssdp_log.c
 * @brief ssdp log
 *
 * @history
 *  2013-07-28 create by xuwf
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "ssdp_log.h"

#define DEFAULT_LOG_FILE    ("/var/log/lightssdpd.log")

static char* log_leves[] = {
   "ERROR",
   "WARNING",
   "INFO",
   "DEBUG"
};

int log_file_fd = -1;

int ssdp_log_init(const char* log_file) {
    ssdp_log_deinit();

    if (!log_file) log_file = DEFAULT_LOG_FILE;

    log_file_fd = open(log_file, O_CREAT|O_WRONLY);
    if (log_file_fd < 0) {
        perror("log_init: open log file fail");
        return -1;
    }

    return 0;
}

void ssdp_log_deinit(void) {
    if (log_file_fd >= 0) {
        close(log_file_fd);
        log_file_fd = -1;
    }
}

void ssdp_log_message(int level, char* fmt, ...) {
    va_list args;
    time_t nowtime;
    char time_string[16] = {0};
    char buf[1024] = {0};

    assert(level >= 0 && level < SSDP_LOG_MAX);

    va_start(args, fmt);

    nowtime = time(NULL);
    strftime(time_string, sizeof(time_string), "%b %d %H:%M:%S", localtime(&nowtime));
    snprintf(buf, sizeof(buf), "%-9s %s [%ld]: ", log_leves[level], 
            time_string, (long int) getpid());

    if (log_file_fd < 0) {
        fprintf(stderr, "%s", buf);
        vsnprintf(buf, sizeof(buf), fmt, args);
        fprintf(stderr, "%s\n", buf);
    } else {
        write(log_file_fd, buf, strlen(buf));
        vsnprintf(buf, sizeof(buf), fmt, args);
        write(log_file_fd, buf, strlen(buf));
        write(log_file_fd, "\n", 1);
    }

    va_end(args);
}

