obj-m = hello.o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C ${KDIR} SUBDIRS=${PWD} modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
