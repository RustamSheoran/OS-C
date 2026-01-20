CC = x86_64-elf-gcc
AS = x86_64-elf-as
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

CFLAGS = -ffreestanding -mno-red-zone -mgeneral-regs-only -std=c11 -Wall -Wextra -Iinclude
LDFLAGS = -T scripts/linker.ld -nostdlib
EFI_LDFLAGS = --target=efi-app-x86_64

SRC_DIR = src
OBJ_DIR = obj
SCRIPT_DIR = scripts

SRCS = $(SRC_DIR)/efi_main.c $(SRC_DIR)/kernel.c $(SRC_DIR)/pmm.c $(SRC_DIR)/paging.c $(SRC_DIR)/idt.c $(SRC_DIR)/interrupts.c $(SRC_DIR)/scheduler.c $(SRC_DIR)/syscall.c $(SRC_DIR)/fs.c $(SRC_DIR)/smp.c $(SRC_DIR)/ata.c $(SRC_DIR)/net.c $(SRC_DIR)/graphics.c $(SRC_DIR)/elf_loader.c $(SRC_DIR)/gdt.c $(SRC_DIR)/process.c $(SRC_DIR)/vfs.c $(SRC_DIR)/nvme.c tests/test_runner.c tests/boot_test.c tests/memory_test.c tests/scheduler_test.c

ASMS = $(SRC_DIR)/start.S $(SRC_DIR)/interrupts.S $(SRC_DIR)/ap_trampoline.S
OBJS = $(OBJ_DIR)/efi_main.o $(OBJ_DIR)/kernel.o $(OBJ_DIR)/pmm.o $(OBJ_DIR)/paging.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/interrupts.o $(OBJ_DIR)/scheduler.o $(OBJ_DIR)/syscall.o $(OBJ_DIR)/fs.o $(OBJ_DIR)/smp.o $(OBJ_DIR)/ata.o $(OBJ_DIR)/net.o $(OBJ_DIR)/graphics.o $(OBJ_DIR)/elf_loader.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/process.o $(OBJ_DIR)/vfs.o $(OBJ_DIR)/nvme.o $(OBJ_DIR)/test_runner.o $(OBJ_DIR)/boot_test.o $(OBJ_DIR)/memory_test.o $(OBJ_DIR)/scheduler_test.o $(OBJ_DIR)/start.o $(OBJ_DIR)/interrupts.o $(OBJ_DIR)/ap_trampoline.o

$(shell mkdir -p $(OBJ_DIR))

all: kernel.efi

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	$(AS) --64 $< -o $@

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

kernel.efi: kernel.elf
	$(OBJCOPY) $(EFI_LDFLAGS) $< $@

clean:
	rm -rf $(OBJ_DIR) kernel.elf kernel.efi

.PHONY: all clean