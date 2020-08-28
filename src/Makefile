KERNELSRCDIR := /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(shell pwd)
VERBOSE = 0

obj-m := ztxmap.o
ccflags-y := -I$(src)/


all:
	make -C $(KERNELSRCDIR) M=$(BUILD_DIR) V=$(VERBOSE) modules

install:
	install -m 644 ztxmap.ko /lib/modules/`uname -r`/kernel/net/
	install -m 644 ztxmap.h /usr/local/include/
	depmod -A

clean:
	make -C $(KERNELSRCDIR) M=$(BUILD_DIR) clean



