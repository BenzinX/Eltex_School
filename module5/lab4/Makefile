obj-m += chardev.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
ins:
	sudo insmod chardev.ko
rm:
	sudo rmmod chardev.ko
log:
	sudo dmesg | tail -$L