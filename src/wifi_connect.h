#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>

void wifi_event_handler(struct net_mgmt_event_callback *cb, uint64_t mgmt_event,
			       struct net_if *iface);
void init_wifi(void);
int connect_to_wifi(void);

#endif /* WIFI_CONNECT_H */