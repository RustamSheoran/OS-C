#include <stdint.h>
#include "nvme.h"
#include "paging.h"
#include "pmm.h"

#define NVME_BAR 0xFE000000ULL

#define NVME_CAP 0x00

#define NVME_CC 0x14

#define NVME_CSTS 0x1C

#define NVME_AQA 0x24

#define NVME_ASQ 0x28

#define NVME_ACQ 0x30

#define NVME_SQ0TDBL 0x1000

#define NVME_CQ0HDBL 0x1004

struct nvme_cmd {
    uint32_t opcode : 8;
    uint32_t fuse : 2;
    uint32_t rsvd : 4;
    uint32_t psdt : 2;
    uint32_t cid : 16;
    uint32_t nsid;
    uint64_t rsvd2;
    uint64_t mptr;
    uint64_t prp1;
    uint64_t prp2;
    uint32_t cdw10;
    uint32_t cdw11;
    uint32_t cdw12;
    uint32_t cdw13;
    uint32_t cdw14;
    uint32_t cdw15;
};

volatile uint32_t *nvme_regs;
uint64_t admin_sq_phys;
uint64_t admin_cq_phys;
struct nvme_cmd *admin_sq;
struct nvme_cmd *admin_cq;

void nvme_init() {
    // Map NVMe registers
    for (uint64_t addr = NVME_BAR; addr < NVME_BAR + 0x2000; addr += 0x1000) {
        map_page(addr, addr, 3);
    }
    nvme_regs = (volatile uint32_t *)NVME_BAR;

    // Disable controller
    nvme_regs[NVME_CC / 4] = 0;
    while (nvme_regs[NVME_CSTS / 4] & 1);

    // Allocate admin queues
    admin_sq_phys = pmm_alloc_page();
    admin_cq_phys = pmm_alloc_page();
    admin_sq = (struct nvme_cmd *)admin_sq_phys;
    admin_cq = (struct nvme_cmd *)admin_cq_phys;

    // Set admin queue addresses
    nvme_regs[NVME_ASQ / 4] = admin_sq_phys;
    nvme_regs[NVME_ACQ / 4] = admin_cq_phys;
    nvme_regs[NVME_AQA / 4] = (64 << 16) | 64;

    // Enable controller
    nvme_regs[NVME_CC / 4] = 0x00460001;
    while (!(nvme_regs[NVME_CSTS / 4] & 1));
}

void nvme_submit_cmd(struct nvme_cmd *cmd) {
    // Copy to SQ
    memcpy(admin_sq, cmd, sizeof(struct nvme_cmd));
    // Ring doorbell
    nvme_regs[NVME_SQ0TDBL / 4] = 1;
}

int nvme_read_sector(uint64_t lba, uint8_t *buf) {
    struct nvme_cmd cmd = {0};
    cmd.opcode = 0x02;
    cmd.nsid = 1;
    cmd.cdw10 = lba & 0xFFFFFFFF;
    cmd.cdw11 = lba >> 32;
    cmd.cdw12 = 0; // 1 sector
    cmd.prp1 = (uint64_t)buf;
    nvme_submit_cmd(&cmd);
    // Wait completion
    while (!(nvme_regs[NVME_CQ0HDBL / 4] & 1));
    return 0;
}

int nvme_write_sector(uint64_t lba, uint8_t *buf) {
    struct nvme_cmd cmd = {0};
    cmd.opcode = 0x01;
    cmd.nsid = 1;
    cmd.cdw10 = lba & 0xFFFFFFFF;
    cmd.cdw11 = lba >> 32;
    cmd.cdw12 = 0;
    cmd.prp1 = (uint64_t)buf;
    nvme_submit_cmd(&cmd);
    while (!(nvme_regs[NVME_CQ0HDBL / 4] & 1));
    return 0;
}

#include "block.h"

block_dev_t nvme_dev = {
    .read = nvme_read_sector,
    .write = nvme_write_sector
};