CC := arm-linux-gcc 
LD := arm-linux-ld
AS := arm-linux-as
OBJCOPY:= arm-linux-objcopy

.PHONY:all
all:test.bin
test.bin:test
	$(OBJCOPY) -I elf32-littlearm -O binary test test.bin 

test:test.o
	$(LD) $< -Ttext=30000000 -o $@ 

test.o:test.S 
	$(AS) $< -o $@

.PHONY:clean

clean:
	rm -rf *.o
