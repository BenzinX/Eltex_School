obj-m += sysleds.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ins:
	sudo insmod sysleds.ko

rm:
	sudo rmmod sysleds

log:
	sudo dmesg | tail -n 20