OBJECT_FILES_NON_STANDARD := y

obj-m = probing-nop-module.o

probing-nop-module-objs := probing-idt.o probing-function.o probing-session.o probing-device.o probing-main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean