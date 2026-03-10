#include <stdint.h>
#include "net.h"
#include "io.h"

#define RTL8139_IO 0xC000
#define MAX_PKT 1536

static uint8_t loopback_packet[MAX_PKT];
static uint32_t loopback_len;

void net_init() {
    // Software reset
    outb(RTL8139_IO + 0x37, 0x10);
    while (inb(RTL8139_IO + 0x37) & 0x10);
    // Enable RX/TX
    outb(RTL8139_IO + 0x37, 0x0C);
}

void net_send_packet(uint8_t *data, uint32_t len) {
    if (!data || len == 0) {
        loopback_len = 0;
        return;
    }
    if (len > MAX_PKT) {
        len = MAX_PKT;
    }
    for (uint32_t i = 0; i < len; i++) {
        loopback_packet[i] = data[i];
    }
    loopback_len = len;
}

uint32_t net_recv_packet(uint8_t *buf) {
    if (!buf || loopback_len == 0) {
        return 0;
    }
    for (uint32_t i = 0; i < loopback_len; i++) {
        buf[i] = loopback_packet[i];
    }
    uint32_t out_len = loopback_len;
    loopback_len = 0;
    return out_len;
}
