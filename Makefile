asm_source_files := $(shell find myos/src/ -name *.asm)
asm_object_files := $(patsubst myos/src/%.asm, build/%.o, $(asm_source_files))

$(asm_object_files): build/%.o : myos/src/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/%.o, myos/src/%.asm, $@) -o $@

.PHONY: build-iso
build-iso: $(asm_object_files)
	mkdir -p dist/ && \
	mkdir -p build/iso/boot/grub/
	cp myos/src/boot/grub/grub.cfg build/iso/boot/grub/grub.cfg && \
	x86_64-elf-ld -n -o dist/kernel.bin -T myos/src/linker.ld $(asm_object_files) && \
	cp dist/kernel.bin build/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/myos.iso build/iso

.PHONY: clean
clean:
	rm -rf build/