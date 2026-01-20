#ifndef NET_H
#define NET_H

#include <stdint.h>

void net_init();
void net_send_packet(uint8_t *data, uint32_t len);
uint32_t net_recv_packet(uint8_t *buf);

#endif