obj-m = hello.o

KDIR	:= /media/bartek/1ca67ce1-9665-423c-b1c7-7ae14467e2fa/programowanie/qemu/shared_files/linux-headers-4.9.0-kali4-amd64	
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
