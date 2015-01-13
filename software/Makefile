MAIN = privEscTest privEscDirectAccess privEscException privAntiDeEsc regTargetRedir regSourceRedirException interruptsDisableReg eearContamination epcrContaminationEntry epcrContaminationExit interruptsDisableMicro regSourceRedirGeneral jumpBypass ropEarlyExit
CC = or32-elf-gcc
OBJDUMP = or32-elf-objdump
CFLAGS = -O0 -g -mnewlib -mboard=ml505
LFLAGS =  
#LFLAGS = -Ttext=0x08000000
#OBJS = $(MAIN)

help:
	@echo "    all"

all: utils_make $(MAIN)

.c:	
	$(CC) $(CFLAGS) $(LFLAGS) -o $@.or32 $@.c
	$(OBJDUMP) -d $@.or32 > $@.asm	
#	or32-linux-gcc -g -I/opt/or32-linux/include -L/opt/or32-linux/lib -o $@.linux $@.c
#	or32-linux-objdump -d $@.linux > $@Linux.asm
	or32-elf-objcopy -O binary $@.or32  $@.bin
	utils/bin2vmem $@.bin > $@.vmem
	rm -rf *.bin

utils_make: utils/*.c utils/Makefile
	cd utils; make

clean:
	rm -f *.d *.o *.asm *~ *.or32 *.bin *.vmem *.linux
	cd utils; make clean