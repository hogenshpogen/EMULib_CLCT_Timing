cmd_/home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.o := gcc -Wp,-MD,/home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/.eth_hook_2.o.d -nostdinc -iwithprefix include -D__KERNEL__ -Iinclude  -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Os -fomit-frame-pointer -g -Wdeclaration-after-statement  -mno-red-zone -mcmodel=kernel -pipe -fno-reorder-blocks	 -Wno-sign-compare -funit-at-a-time   -DMODULE -DKBUILD_BASENAME=eth_hook_2 -DKBUILD_MODNAME=eth_hook_2 -c -o /home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/.tmp_eth_hook_2.o /home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.c

deps_/home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.o := \
  /home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.c \
    $(wildcard include/config/smp.h) \
  include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/kallsyms.h) \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/linux/sched.h \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/preempt.h) \
  include/asm/param.h \
  include/linux/capability.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
  include/linux/compiler-gcc3.h \
  include/linux/compiler-gcc.h \
  include/asm/posix_types.h \
  include/asm/types.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/lockmeter.h) \
  include/linux/preempt.h \
  include/linux/linkage.h \
  include/asm/linkage.h \
  include/linux/thread_info.h \
  include/linux/bitops.h \
  include/asm/bitops.h \
  include/asm/thread_info.h \
  include/asm/page.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/arch/gate/area.h) \
  include/asm/bug.h \
  include/linux/stringify.h \
  include/asm-generic/bug.h \
  include/asm/pda.h \
  include/linux/cache.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
  /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stdarg.h \
  include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/byteorder/swab.h \
  include/linux/byteorder/generic.h \
  include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
  include/asm/mmsegment.h \
  include/asm/processor.h \
  include/asm/segment.h \
  include/asm/sigcontext.h \
  include/asm/cpufeature.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
  include/asm/msr.h \
  include/asm/current.h \
  include/asm/system.h \
    $(wildcard include/config/unordered/io.h) \
  include/asm/percpu.h \
  include/linux/personality.h \
  include/asm/spinlock.h \
    $(wildcard include/config/x86/oostore.h) \
    $(wildcard include/config/x86/ppro/fence.h) \
  include/asm/atomic.h \
  include/asm/rwlock.h \
  include/linux/timex.h \
    $(wildcard include/config/time/interpolation.h) \
  include/asm/timex.h \
  include/asm/8253pit.h \
  include/asm/vsyscall.h \
  include/linux/seqlock.h \
  include/asm/hpet.h \
    $(wildcard include/config/hpet/emulate/rtc.h) \
  include/linux/time.h \
  include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/jiffies.h \
  include/linux/rbtree.h \
  include/linux/cpumask.h \
    $(wildcard include/config/hotplug/cpu.h) \
  include/linux/bitmap.h \
  include/linux/string.h \
  include/asm/string.h \
  include/asm/semaphore.h \
  include/linux/wait.h \
  include/linux/list.h \
  include/linux/prefetch.h \
    $(wildcard include/config/xen.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/asm/ptrace.h \
  include/asm/mmu.h \
  include/linux/smp.h \
  include/asm/smp.h \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/x86/io/apic.h) \
  include/asm/fixmap.h \
  include/asm/apicdef.h \
  include/asm/vsyscall32.h \
  include/asm/mpspec.h \
    $(wildcard include/config/acpi/boot.h) \
  include/asm/io_apic.h \
    $(wildcard include/config/pci/msi.h) \
  include/asm/apic.h \
    $(wildcard include/config/x86/good/apic.h) \
  include/linux/pm.h \
    $(wildcard include/config/pm.h) \
  include/linux/sem.h \
    $(wildcard include/config/sysvipc.h) \
  include/linux/ipc.h \
  include/asm/ipcbuf.h \
  include/asm/sembuf.h \
  include/linux/signal.h \
  include/asm/signal.h \
  include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/resource.h \
  include/asm/resource.h \
  include/linux/securebits.h \
  include/linux/fs_struct.h \
  include/linux/completion.h \
  include/linux/pid.h \
  include/linux/percpu.h \
  include/linux/slab.h \
    $(wildcard include/config/.h) \
  include/linux/gfp.h \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
  include/linux/numa.h \
  include/asm/numnodes.h \
  include/linux/topology.h \
  include/asm/topology.h \
  include/asm-generic/topology.h \
  include/asm/mmzone.h \
  include/linux/init.h \
    $(wildcard include/config/hotplug.h) \
  include/linux/kmalloc_sizes.h \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/large/allocs.h) \
  include/linux/param.h \
  include/linux/timer.h \
  include/linux/aio.h \
  include/linux/workqueue.h \
  include/linux/aio_abi.h \
  include/linux/stat.h \
  include/asm/stat.h \
  include/linux/kmod.h \
    $(wildcard include/config/kmod.h) \
  include/linux/errno.h \
  include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/elf.h \
  include/asm/elf.h \
  include/asm/user.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
    $(wildcard include/config/sysfs.h) \
  include/linux/kref.h \
  include/linux/moduleparam.h \
  include/asm/local.h \
  include/asm/module.h \
  include/linux/fs.h \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/auditsyscall.h) \
  include/linux/limits.h \
  include/linux/kdev_t.h \
  include/linux/ioctl.h \
  include/asm/ioctl.h \
  include/linux/dcache.h \
  include/linux/rcupdate.h \
  include/linux/prio_tree.h \
  include/linux/radix-tree.h \
  include/linux/quota.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/fcntl.h \
  include/asm/fcntl.h \
  include/linux/err.h \
  include/linux/poll.h \
  include/asm/poll.h \
  include/linux/mm.h \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/debug/pagealloc.h) \
  include/asm/pgtable.h \
    $(wildcard include/config/mem/mirror.h) \
  include/asm/mm_track.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/swap.h) \
  include/asm/uaccess.h \
  include/net/irda/wrapper.h \
  include/linux/skbuff.h \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/vlan/8021q/module.h) \
    $(wildcard include/config/netfilter/debug.h) \
    $(wildcard include/config/hippi.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/have/arch/alloc/skb.h) \
  include/linux/highmem.h \
  include/asm/cacheflush.h \
  include/linux/net.h \
  include/asm/socket.h \
  include/asm/sockios.h \
  include/net/checksum.h \
  include/asm/checksum.h \
  include/linux/netdevice.h \
    $(wildcard include/config/ax25.h) \
    $(wildcard include/config/ax25/module.h) \
    $(wildcard include/config/tr.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/ipv6/module.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/net/divert.h) \
    $(wildcard include/config/netpoll/trap.h) \
  include/linux/if.h \
  include/linux/socket.h \
    $(wildcard include/config/compat.h) \
  include/linux/sockios.h \
  include/linux/uio.h \
  include/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_packet.h \
  include/linux/device.h \
  include/linux/ioport.h \
  include/linux/interrupt.h \
  include/linux/hardirq.h \
  include/linux/smp_lock.h \
  include/asm/hardirq.h \
  include/linux/irq.h \
    $(wildcard include/config/arch/s390.h) \
  include/asm/irq.h \
  include/asm/hw_irq.h \
  include/linux/profile.h \
    $(wildcard include/config/profiling.h) \
  include/linux/irq_cpustat.h \
  include/linux/notifier.h \
  include/net/irda/irda_device.h \
    $(wildcard include/config/isa.h) \
  include/linux/tty.h \
    $(wildcard include/config/legacy/pty/count.h) \
  include/linux/major.h \
  include/linux/termios.h \
  include/asm/termios.h \
  include/asm/termbits.h \
  include/asm/ioctls.h \
  include/linux/tty_driver.h \
  include/linux/cdev.h \
  include/linux/tty_ldisc.h \
  include/linux/irda.h \
  include/net/pkt_sched.h \
    $(wildcard include/config/net/sch/clk/gettimeofday.h) \
    $(wildcard include/config/net/sch/clk/jiffies.h) \
    $(wildcard include/config/net/sch/clk/cpu.h) \
  include/linux/pkt_sched.h \
  include/net/pkt_cls.h \
  include/linux/pkt_cls.h \
    $(wildcard include/config/cls/u32/perf.h) \
    $(wildcard include/config/net/cls/ind.h) \
  include/linux/rtnetlink.h \
  include/linux/netlink.h \
  include/net/irda/irda.h \
    $(wildcard include/config/irda/debug.h) \
  include/net/irda/qos.h \
  include/net/irda/parameters.h \
  include/net/irda/irqueue.h \
  include/linux/proc_fs.h \
    $(wildcard include/config/proc/devicetree.h) \
  include/linux/sysctl.h \
    $(wildcard include/config/tux/debug/blocking.h) \
  include/linux/vmalloc.h \
  include/linux/etherdevice.h \
  include/linux/random.h \
  include/asm/io.h \
  include/asm-generic/iomap.h \
  include/net/irda//wrapper.h \
  /home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/schar.h \
  /home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.h \

/home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.o: $(deps_/home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.o)

$(deps_/home/durkin/TriDAS/emu/emuDCS/DMBTest/drivers/inteldual_newkernel/eth_hook_2_nobigphysxxx/eth_hook_2.o):
