cmd_usr/include/asm/.install := /bin/bash scripts/headers_install.sh ./usr/include/asm ./arch/x86/include/uapi/asm a.out.h auxvec.h bitsperlong.h boot.h bootparam.h byteorder.h debugreg.h e820.h errno.h fcntl.h hw_breakpoint.h hyperv.h ioctl.h ioctls.h ipcbuf.h ist.h kvm.h kvm_para.h kvm_perf.h ldt.h mce.h mman.h msgbuf.h msr.h mtrr.h param.h perf_regs.h poll.h posix_types.h posix_types_32.h posix_types_64.h posix_types_x32.h prctl.h processor-flags.h ptrace-abi.h ptrace.h resource.h sembuf.h setup.h shmbuf.h sigcontext.h sigcontext32.h siginfo.h signal.h socket.h sockios.h stat.h statfs.h svm.h swab.h termbits.h termios.h types.h ucontext.h unistd.h vm86.h vmx.h vsyscall.h; /bin/bash scripts/headers_install.sh ./usr/include/asm ./arch/x86/include/asm ; /bin/bash scripts/headers_install.sh ./usr/include/asm ./arch/x86/include/generated/uapi/asm unistd_32.h unistd_64.h unistd_x32.h unistd_32.h unistd_64.h unistd_x32.h; for F in ; do echo "$(pound)include <asm-generic/$$F>" > ./usr/include/asm/$$F; done; touch usr/include/asm/.install
