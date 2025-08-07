# Tools
ASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

# Flags
ASMFLAGS_BOOT = -f bin
ASMFLAGS_OBJ = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -Wall -Wextra -fno-pic

# Output files
BOOT_BIN = boot.bin
KERNEL_ELF = kernel.elf
KERNEL_BIN = kernel.bin
DISK_IMG = disk.img

# Source files
BOOT_SRC = boot.s        # switch32bit.s is included in boot.s
KERNEL_SRC_ASM = kernel.s
KERNEL_SRC_C = main.c

.PHONY: all clean run

all: $(BOOT_BIN) $(KERNEL_BIN)

# Assemble bootloader (boot.s includes switch32bit.s via %include)
$(BOOT_BIN): $(BOOT_SRC)
	$(ASM) $(ASMFLAGS_BOOT) -o $@ $<

# Assemble kernel.s to object
kernel.o: $(KERNEL_SRC_ASM)
	$(ASM) $(ASMFLAGS_OBJ) -o $@ $<

# Compile main.c
main.o: $(KERNEL_SRC_C)
	$(CC) $(CFLAGS) -c -o $@ $<

$(KERNEL_ELF): kernel.o main.o
	$(LD) -m elf_i386 -T linker.ld -o $@ $^

# Convert ELF to flat binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# Create floppy disk image
$(DISK_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOT_BIN) of=$@ conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

# Run in QEMU
run: $(DISK_IMG)
	qemu-system-x86_64 -fda $(DISK_IMG)

# Clean all build files
clean:
	rm -f *.o *.bin *.elf *.img
