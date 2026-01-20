#include <stdint.h>
#include "net.h"
#include "io.h"

#define RTL8139_IO 0xC000

void net_init() {
    // Software reset
    outb(RTL8139_IO + 0x37, 0x10);
    while (inb(RTL8139_IO + 0x37) & 0x10);
    // Enable RX/TX
    outb(RTL8139_IO + 0x37, 0x0C);
}

void net_send_packet(uint8_t *data, uint32_t len) {
    (void)data; (void)len; // Suppress unused parameter warnings
    // Stub
}

uint32_t net_recv_packet(uint8_t *buf) {
    (void)buf; // Suppress unused parameter warning
    // Stub
    return 0;
}