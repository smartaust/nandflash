CC := arm-linux-gcc 
LD := arm-linux-ld
AS := arm-linux-as
OBJCOPY:= arm-linux-objcopy

.PHONY:all
all:main.bin vector.bin

main.bin:main
	$(OBJCOPY) -I elf32-littlearm -O binary $< $@ 

main:main.o
	$(LD) $< -Ttext=30000000 -o $@ 

main.o:main.c 
	$(CC) -c $< -o $@

vector.bin :vector
	$(OBJCOPY) -I elf32-littlearm -O binary $< $@

vector:vector.o
	$(LD) $< -Ttext=70000000 -o $@ 

vector.o:vector.S 
	$(AS) $< -o $@
.PHONY:clean

clean:
	rm -rf *.o
