# vim:noet sw=4 ts=4
INC = /usr/include/efi
CFLAGS = -I$(INC) -I$(INC)/x86_64 -I$(INC)/protocol \
		 -Wall -std=c11 \
		 -fpic -fshort-wchar -fno-stack-protector -mno-red-zone
LDFLAGS =  -znocombreloc -shared -Bsymbolic \
		  -L/usr/lib -lgnuefi -lefi \
		  -T/usr/lib/elf_x86_64_efi.lds

main.efi: main.so
	objcopy -j .text \
			-j .sdata \
			-j .data \
			-j .dynamic \
			-j .dynsym \
			-j .rel \
			-j .rela \
			-j .reloc \
			--target=efi-app-x86_64 \
			$< \
			$@
	$(RM) $<

main.so: main.o
	$(LD) -o $@ $< /usr/lib/crt0-efi-x86_64.o $(LDFLAGS)
	$(RM) $<

main.o: main.c

uefi.img: main.efi
	cp gpt.img $@
	dd if=/dev/zero of=/tmp/part.img bs=512 count=91669
	mformat -i /tmp/part.img -h 32 -t 32 -n 64 -c 1
	mcopy -i /tmp/part.img $< ::
	dd if=/tmp/part.img of=$@ bs=512 count=91669 seek=2048 conv=notrunc
	$(RM) /tmp/part.img

clean:
	$(RM) main.efi
	$(RM) main.so
	$(RM) main.o

qemu: uefi.img
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,file=./ovmf_code_x64.bin,readonly=on \
		-drive if=pflash,format=raw,file=./ovmf_vars_x64.bin \
		-drive file=uefi.img,if=ide

.PHONY: clean qemu
