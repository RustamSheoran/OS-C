CC ?= x86_64-elf-gcc
AS ?= x86_64-elf-as
LD ?= x86_64-elf-ld
OBJCOPY ?= x86_64-elf-objcopy

CFLAGS = -ffreestanding -fno-stack-protector -fno-pie -mcmodel=kernel -mno-red-zone -mgeneral-regs-only -std=c11 -Wall -Wextra -Iinclude
LDFLAGS = -T scripts/linker.ld -nostdlib
EFI_LDFLAGS = --target=efi-app-x86_64

OBJ_DIR = obj

C_SRCS = \
	src/efi_main.c \
	src/kernel.c \
	src/pmm.c \
	src/paging.c \
	src/idt.c \
	src/interrupts.c \
	src/scheduler.c \
	src/syscall.c \
	src/fs.c \
	src/smp.c \
	src/ata.c \
	src/net.c \
	src/graphics.c \
	src/elf_loader.c \
	src/gdt.c \
	src/process.c \
	src/vfs.c \
	src/nvme.c \
	tests/test_runner.c \
	tests/boot_test.c \
	tests/memory_test.c \
	tests/scheduler_test.c

ASM_SRCS = \
	src/start.S \
	src/interrupt_stubs.S \
	src/ap_trampoline.S

C_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRCS))
ASM_OBJS = $(patsubst %.S,$(OBJ_DIR)/%.o,$(ASM_SRCS))
OBJS = $(C_OBJS) $(ASM_OBJS)

all: kernel.efi

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) --64 $< -o $@

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

kernel.efi: kernel.elf
	@if $(OBJCOPY) --info 2>/dev/null | grep -q "efi-app-x86_64"; then \
		$(OBJCOPY) $(EFI_LDFLAGS) $< $@; \
	else \
		$(OBJCOPY) --output-target=pei-x86-64 $< $@; \
	fi

clean:
	rm -rf $(OBJ_DIR) kernel.elf kernel.efi esp boot.log

.PHONY: all clean
