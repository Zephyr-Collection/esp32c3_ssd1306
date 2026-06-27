#ifndef TIME_H
#define TIME_H

#include <zephyr/logging/log.h>

#include <zephyr/net/socket.h>
#include <zephyr/net/socket_service.h>
#include <zephyr/net/sntp.h>
#include <arpa/inet.h>
#include <netdb.h>

extern int wifi_connected;

#define TIMEZONE_OFFSET_SEC (7 * 3600)

int do_sntp(int family);

#endif /* TIME_H */