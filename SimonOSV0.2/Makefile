ISO_NAME = SimonOS.iso
ISO_DIR = iso
KERNEL_BIN = kernel.bin

SRC = kernel.c fs.c
OBJ = $(SRC:.c=.o)

all: $(ISO_NAME)

%.o: %.c
	gcc -m32 -ffreestanding -c $< -o $@

$(KERNEL_BIN): $(OBJ) linker.ld
	ld -m elf_i386 -T linker.ld -o $(KERNEL_BIN) $(OBJ)

$(ISO_NAME): $(KERNEL_BIN) grub.cfg
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	cp grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

run: $(ISO_NAME)
	qemu-system-i386 -cdrom $(ISO_NAME)

clean:
	rm -rf iso *.o *.bin $(ISO_NAME)
