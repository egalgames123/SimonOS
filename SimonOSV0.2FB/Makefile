CC = gcc
AS = nasm
LD = ld

SRC_DIR = src
BUILD_DIR = build

CFLAGS = -m32 -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -std=gnu99 -O2 -fno-pie -fno-stack-protector -I$(SRC_DIR)
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib -z max-page-size=0x1000

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
S_SOURCES := $(wildcard $(SRC_DIR)/*.S)

OBJS := $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) \
        $(S_SOURCES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

all: kernel.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Link the kernel
kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Create ISO
iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o SimonOS.iso isodir
	rm -rf isodir

# Clean up
clean:
	rm -rf $(BUILD_DIR) *.bin *.iso *.o

# Run with QEMU
run: iso
	qemu-system-i386 -cdrom SimonOS.iso -serial stdio -vga std -display sdl -usb -device usb-mouse

