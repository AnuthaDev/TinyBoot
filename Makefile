CC := clang -target x86_64-unknown-windows

CFLAGS = \
	-std=c17 \
	-MMD \
	-Wall \
	-Wextra \
	-Wpedantic \
	-mno-red-zone \
	-ffreestanding \
	-ggdb \
	-O3

LDFLAGS = \
	-nostdlib \
	-fuse-ld=lld-link \
	-Wl,-subsystem:efi_application \
	-Wl,-entry:efi_main


SOURCES = main.c
OBJS = $(SOURCES:.c=.o)
DEPENDS = $(OBJS:.o=.d)
TARGET = BOOTX64.EFI

QEMU_SCRIPT = qemu.sh


default: all

all: $(TARGET)
	cd scripts/; \
	./$(QEMU_SCRIPT)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	cp $(TARGET) root/efi/boot

-include $(DEPENDS)


# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@
#
# bootx64.efi: main.o
# 	$(LD) $(LDFLAGS) $< -out:$@
#
# -include $(SRCS:.c=.d)

.PHONY: clean all default


clean:
	rm -f $(TARGET) *.efi *.o *.d *.pdb
