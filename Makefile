# (!) using paths with spaces may not work with Kbuild

#SHELL='sh -x '

# this is what is used by Kbuild
obj-m := cdevmod.o


V?= 2

# directory containing Makefile for kernel build
KBUILDDIR:= ../linux${suffix}
ifeq ($(KBUILDDIR)k,k)
    $(error set KBUILDDIR above)
endif

# Do not print "Entering directory" on recursive make call if not verbose
MAKEFLAGS+= $(if $(value V),,--no-print-directory)

.PHONY: modules clean tidy sim

# recur to the original kernel Makefile with some additions
modules:
	$(MAKE) -C "$(KBUILDDIR)" M="$$PWD" modules

tidy:
	$(MAKE) -C "$(KBUILDDIR)" M="$$PWD" clean

# we link original clean to target named tidy
clean:
	-rm -rf .tmp_versions
	-rm -f modules.order .modules.order.cmd Module.symvers .Module.symvers.cmd
	-rm -f $(obj-m) $(obj-m:.o=.mod) $(obj-m:.o=.mod.o) $(obj-m:.o=.mod.c) .$(obj-m:.o=.mod.cmd) $(obj-m:.o=.lst)
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.ko)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.lst)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.mod.o)))

sim: modules
	qemu-system-x86_64 -m 400M -smp 4 -kernel "${KBUILDDIR}/arch/x86_64/boot/bzImage" \
	-initrd ${KBUILDDIR}/../initramfs.cpio.gz -drive file=fat:rw:.,format=raw,media=disk
