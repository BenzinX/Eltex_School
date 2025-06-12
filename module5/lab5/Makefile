obj-m += netlink.o  # Указываем объектный файл для модуля ядра

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
	gcc -o netlink_test netlink_test.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f netlink_test

ins:
	sudo insmod netlink.ko

rm:
	sudo rmmod netlink

log:
	sudo dmesg | tail -n 5