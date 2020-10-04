obj-m += cpuctl_drv.o
cpuctl_drv-objs := src/cpuctl_drv.o src/offset.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

