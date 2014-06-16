# -- Machine type EFI2
# mark_description "Intel(R) C++ Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.0 Build 20110112";
# mark_description "-m64 -mavx -S";
	.file "dot.C"
	.text
..TXTST0:
# -- Begin  main
# mark_begin;
       .align    16,0x90
	.globl main
main:
..B1.1:                         # Preds ..B1.0
..___tag_value_main.1:                                          #47.12
        pushq     %rbp                                          #47.12
..___tag_value_main.3:                                          #
        movq      %rsp, %rbp                                    #47.12
..___tag_value_main.4:                                          #
        andq      $-128, %rsp                                   #47.12
        subq      $128, %rsp                                    #47.12
        movl      $3, %edi                                      #47.12
..___tag_value_main.6:                                          #47.12
        call      __intel_new_proc_init                         #47.12
..___tag_value_main.7:                                          #
                                # LOE rbx r12 r13 r14 r15
..B1.13:                        # Preds ..B1.1
        vstmxcsr  96(%rsp)                                      #47.12
        xorl      %eax, %eax                                    #48.14
        movl      $1073741824, %edx                             #48.14
        orl       $32832, 96(%rsp)                              #47.12
        movl      $1077936128, %ecx                             #48.14
        movl      $1082130432, %esi                             #48.14
        movl      $1084227584, %edi                             #48.14
        movl      $1086324736, %r8d                             #48.14
        movl      $1088421888, %r9d                             #48.14
        vldmxcsr  96(%rsp)                                      #47.12
        movl      %eax, (%rsp)                                  #48.14
        movl      $1065353216, 4(%rsp)                          #48.14
        movl      %edx, 8(%rsp)                                 #48.14
        movl      %ecx, 12(%rsp)                                #48.14
        movl      %esi, 16(%rsp)                                #48.14
        movl      %edi, 20(%rsp)                                #48.14
        movl      %r8d, 24(%rsp)                                #48.14
        movl      %r9d, 28(%rsp)                                #48.14
        movl      %eax, 32(%rsp)                                #49.14
        movl      $1065353216, 36(%rsp)                         #49.14
        movl      %edx, 40(%rsp)                                #49.14
        movl      %ecx, 44(%rsp)                                #49.14
        movl      %esi, 48(%rsp)                                #49.14
        movl      %edi, 52(%rsp)                                #49.14
        movl      %r8d, 56(%rsp)                                #49.14
        movl      %r9d, 60(%rsp)                                #49.14
        vpxor     %xmm0, %xmm0, %xmm0                           #50.14
        vmovups   %xmm0, 80(%rsp)                               #50.14
        vmovups   %xmm0, 64(%rsp)                               #50.14
                                # LOE rbx r12 r13 r14 r15
..B1.2:                         # Preds ..B1.13
        vmovups   (%rsp), %xmm2                                 #52.31
        vmovups   32(%rsp), %xmm3                               #53.31
        vmovss    (%rsp), %xmm8                                 #65.44
        xorl      %esi, %esi                                    #65.5
        vmulss    32(%rsp), %xmm8, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        xorl      %edx, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        vinsertf128 $1, 16(%rsp), %ymm2, %ymm4                  #52.31
        vinsertf128 $1, 48(%rsp), %ymm3, %ymm5                  #53.31
        vdpps     $255, %ymm5, %ymm4, %ymm6                     #54.15
        vperm2f128 $1, %ymm6, %ymm6, %ymm7                      #55.26
        vaddps    %ymm7, %ymm6, %ymm1                           #55.8
        vmovups   %xmm1, 64(%rsp)                               #61.20
        vextractf128 $1, %ymm1, 80(%rsp)                        #61.20
        vcvtss2sd %xmm1, %xmm1, %xmm1                           #65.5
        vzeroupper                                              #65.5
..___tag_value_main.8:                                          #65.5
        call      printf                                        #65.5
..___tag_value_main.9:                                          #
                                # LOE rbx r12 r13 r14 r15
..B1.3:                         # Preds ..B1.2
        vmovss    4(%rsp), %xmm2                                #65.44
        movl      $1, %esi                                      #65.5
        vmulss    36(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 68(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
..___tag_value_main.10:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.11:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.4:                         # Preds ..B1.3
        vmovss    8(%rsp), %xmm2                                #65.44
        movl      $2, %esi                                      #65.5
        vmulss    40(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 72(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      %esi, %eax                                    #65.5
..___tag_value_main.12:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.13:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.5:                         # Preds ..B1.4
        vmovss    12(%rsp), %xmm2                               #65.44
        movl      $3, %esi                                      #65.5
        vmulss    44(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 76(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
..___tag_value_main.14:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.15:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.6:                         # Preds ..B1.5
        vmovss    16(%rsp), %xmm2                               #65.44
        movl      $4, %esi                                      #65.5
        vmulss    48(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 80(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
..___tag_value_main.16:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.17:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.7:                         # Preds ..B1.6
        vmovss    20(%rsp), %xmm2                               #65.44
        movl      $5, %esi                                      #65.5
        vmulss    52(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 84(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
..___tag_value_main.18:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.19:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.8:                         # Preds ..B1.7
        vmovss    24(%rsp), %xmm2                               #65.44
        movl      $6, %esi                                      #65.5
        vmulss    56(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 88(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
..___tag_value_main.20:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.21:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.9:                         # Preds ..B1.8
        vmovss    28(%rsp), %xmm2                               #65.44
        movl      $7, %esi                                      #65.5
        vmulss    60(%rsp), %xmm2, %xmm0                        #65.49
        movl      $.L_2__STRING.0, %edi                         #65.5
        vcvtss2sd 92(%rsp), %xmm1, %xmm1                        #65.5
        vcvtss2sd %xmm0, %xmm0, %xmm0                           #65.5
        movl      %esi, %edx                                    #65.5
        movl      $2, %eax                                      #65.5
..___tag_value_main.22:                                         #65.5
        call      printf                                        #65.5
..___tag_value_main.23:                                         #
                                # LOE rbx r12 r13 r14 r15
..B1.10:                        # Preds ..B1.9
        xorl      %eax, %eax                                    #67.10
        movq      %rbp, %rsp                                    #67.10
        popq      %rbp                                          #67.10
..___tag_value_main.24:                                         #
        ret                                                     #67.10
        .align    16,0x90
..___tag_value_main.26:                                         #
                                # LOE
# mark_end;
	.type	main,@function
	.size	main,.-main
	.data
# -- End  main
	.text
# -- Begin  _Z7testmsmR8msmparms
# mark_begin;
       .align    16,0x90
	.globl _Z7testmsmR8msmparms
_Z7testmsmR8msmparms:
# parameter 1: %rdi
..B2.1:                         # Preds ..B2.0
..___tag_value__Z7testmsmR8msmparms.27:                         #9.30
        xorl      %eax, %eax                                    #44.10
        movq      40(%rdi), %rdx                                #15.41
        movq      32(%rdi), %rcx                                #16.38
        movq      24(%rdi), %rsi                                #20.35
        vbroadcastss (%rdx), %ymm0                              #15.20
        vbroadcastss 4(%rdx), %ymm1                             #15.20
        vbroadcastss 8(%rdx), %ymm4                             #15.20
        vbroadcastss 12(%rdx), %ymm7                            #15.20
        vbroadcastss 16(%rdx), %ymm10                           #15.20
        vbroadcastss 20(%rdx), %ymm13                           #15.20
        vmulps    (%rcx), %ymm0, %ymm2                          #17.21
        vmulps    32(%rcx), %ymm1, %ymm3                        #17.21
        vmulps    64(%rcx), %ymm4, %ymm6                        #17.21
        vmulps    96(%rcx), %ymm7, %ymm9                        #17.21
        vmulps    128(%rcx), %ymm10, %ymm12                     #17.21
        vmulps    160(%rcx), %ymm13, %ymm15                     #17.21
        vbroadcastss 24(%rdx), %ymm0                            #15.20
        vaddps    %ymm3, %ymm2, %ymm5                           #18.12
        vbroadcastss 28(%rdx), %ymm3                            #15.20
        vmulps    192(%rcx), %ymm0, %ymm2                       #17.21
        vaddps    %ymm6, %ymm5, %ymm8                           #18.12
        vmulps    224(%rcx), %ymm3, %ymm5                       #17.21
        vaddps    %ymm9, %ymm8, %ymm11                          #18.12
        vaddps    %ymm12, %ymm11, %ymm14                        #18.12
        vaddps    %ymm15, %ymm14, %ymm1                         #18.12
        vaddps    %ymm2, %ymm1, %ymm4                           #18.12
        vaddps    %ymm5, %ymm4, %ymm6                           #18.12
        vaddps    (%rsi), %ymm6, %ymm7                          #21.13
        vmovups   %ymm7, (%rsi)                                 #22.21
        vzeroupper                                              #44.10
        ret                                                     #44.10
        .align    16,0x90
..___tag_value__Z7testmsmR8msmparms.29:                         #
                                # LOE
# mark_end;
	.type	_Z7testmsmR8msmparms,@function
	.size	_Z7testmsmR8msmparms,.-_Z7testmsmR8msmparms
	.data
# -- End  _Z7testmsmR8msmparms
	.section .rodata, "a"
	.align 4
	.align 4
.L_2il0floatpacket.0:
	.long	0x40000000
	.type	.L_2il0floatpacket.0,@object
	.size	.L_2il0floatpacket.0,4
	.align 4
.L_2il0floatpacket.1:
	.long	0x40400000
	.type	.L_2il0floatpacket.1,@object
	.size	.L_2il0floatpacket.1,4
	.align 4
.L_2il0floatpacket.2:
	.long	0x40800000
	.type	.L_2il0floatpacket.2,@object
	.size	.L_2il0floatpacket.2,4
	.align 4
.L_2il0floatpacket.3:
	.long	0x40a00000
	.type	.L_2il0floatpacket.3,@object
	.size	.L_2il0floatpacket.3,4
	.align 4
.L_2il0floatpacket.4:
	.long	0x40c00000
	.type	.L_2il0floatpacket.4,@object
	.size	.L_2il0floatpacket.4,4
	.align 4
.L_2il0floatpacket.5:
	.long	0x40e00000
	.type	.L_2il0floatpacket.5,@object
	.size	.L_2il0floatpacket.5,4
	.align 4
.L_2il0floatpacket.6:
	.long	0x3f800000
	.type	.L_2il0floatpacket.6,@object
	.size	.L_2il0floatpacket.6,4
	.section .rodata.str1.4, "aMS",@progbits,1
	.align 4
	.align 4
.L_2__STRING.0:
	.byte	97
	.byte	42
	.byte	98
	.byte	91
	.byte	37
	.byte	100
	.byte	93
	.byte	58
	.byte	32
	.byte	37
	.byte	103
	.byte	32
	.byte	32
	.byte	32
	.byte	99
	.byte	91
	.byte	37
	.byte	100
	.byte	93
	.byte	58
	.byte	32
	.byte	37
	.byte	103
	.byte	10
	.byte	0
	.type	.L_2__STRING.0,@object
	.size	.L_2__STRING.0,25
	.data
	.section .note.GNU-stack, ""
// -- Begin DWARF2 SEGMENT .eh_frame
	.section .eh_frame,"a",@progbits
.eh_frame_seg:
	.align 8
	.4byte 0x0000001c
	.8byte 0x4c507a0100000000
	.4byte 0x10780100
	.2byte 0x000a
	.8byte __gxx_personality_v0
	.4byte 0x08070c00
	.2byte 0x0190
	.4byte 0x0000003c
	.4byte 0x00000024
	.8byte ..___tag_value_main.1
	.8byte ..___tag_value_main.26-..___tag_value_main.1
	.8byte 0x0000000000000008
	.2byte 0x0400
	.4byte ..___tag_value_main.3-..___tag_value_main.1
	.4byte 0x0410070c
	.4byte ..___tag_value_main.4-..___tag_value_main.3
	.4byte 0x8610060c
	.2byte 0x0402
	.4byte ..___tag_value_main.24-..___tag_value_main.4
	.8byte 0x000006060908070c
	.4byte 0x00000024
	.4byte 0x00000064
	.8byte ..___tag_value__Z7testmsmR8msmparms.27
	.8byte ..___tag_value__Z7testmsmR8msmparms.29-..___tag_value__Z7testmsmR8msmparms.27
	.8byte 0x0000000000000008
	.8byte 0x0000000000000000
# End
