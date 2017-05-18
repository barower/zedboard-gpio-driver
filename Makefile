obj-m = hello.o

KDIR	:= /../Linux-headers-4.9.0-kali4-amd64	
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
