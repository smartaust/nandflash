CC := arm-linux-gcc 
LD := arm-linux-ld
OBJCOPY:= arm-linux-objcopy
.PHONY:all
all:test.bin
test.bin:test
	$(OBJCOPY) -I elf32-littlearm -O binary test test.bin 

test:test.o
	$(LD) $< -Ttext=30000000 -o $@ 

test.o:test.c 
	$(CC) -c $< -o $@

.PHONY:clean

clean:
	rm -rf *.o
