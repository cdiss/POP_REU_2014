# -- Machine type EFI2
# mark_description "Intel(R) C++ Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.0 Build 20110112";
# mark_description "-m64 -mavx -S";
	.file "foo.C"
	.text
..TXTST0:
# -- Begin  _Z7testmsmR8msmparms
# mark_begin;
       .align    16,0x90
	.globl _Z7testmsmR8msmparms
_Z7testmsmR8msmparms:
# parameter 1: %rdi
..B1.1:                         # Preds ..B1.0
..___tag_value__Z7testmsmR8msmparms.1:                          #7.30
        pushq     %rbp                                          #7.30
..___tag_value__Z7testmsmR8msmparms.3:                          #
        movq      %rsp, %rbp                                    #7.30
..___tag_value__Z7testmsmR8msmparms.4:                          #
        andq      $-32, %rsp                                    #7.30
        movq      40(%rdi), %rdx                                #8.34
        movq      32(%rdi), %rcx                                #10.37
        movq      24(%rdi), %rsi                                #12.5
        vmovups   (%rdx), %ymm0                                 #8.34
        vmovups   (%rcx), %ymm1                                 #11.19
        vdpps     $255, %ymm0, %ymm1, %ymm2                     #11.19
        vmovups   %ymm2, -32(%rsp)                              #11.17
        vmovss    -32(%rsp), %xmm3                              #12.32
        vaddss    -16(%rsp), %xmm3, %xmm4                       #12.5
        vaddss    (%rsi), %xmm4, %xmm5                          #12.55
        vmovss    %xmm5, (%rsi)                                 #12.5
        movq      32(%rdi), %r8                                 #10.37
        movq      24(%rdi), %r9                                 #12.5
        vmovups   32(%r8), %ymm6                                #11.19
        vdpps     $255, %ymm0, %ymm6, %ymm7                     #11.19
        vmovups   %ymm7, -32(%rsp)                              #11.17
        vmovss    -32(%rsp), %xmm8                              #12.32
        vaddss    -16(%rsp), %xmm8, %xmm9                       #12.5
        vaddss    4(%r9), %xmm9, %xmm10                         #12.55
        vmovss    %xmm10, 4(%r9)                                #12.5
        movq      32(%rdi), %r10                                #10.37
        movq      24(%rdi), %r11                                #12.5
        vmovups   64(%r10), %ymm11                              #11.19
        vdpps     $255, %ymm0, %ymm11, %ymm12                   #11.19
        vmovups   %ymm12, -32(%rsp)                             #11.17
        vmovss    -32(%rsp), %xmm13                             #12.32
        vaddss    -16(%rsp), %xmm13, %xmm14                     #12.5
        vaddss    8(%r11), %xmm14, %xmm15                       #12.55
        vmovss    %xmm15, 8(%r11)                               #12.5
        movq      32(%rdi), %rax                                #10.37
        movq      24(%rdi), %rdx                                #12.5
        vmovups   96(%rax), %ymm1                               #11.19
        vdpps     $255, %ymm0, %ymm1, %ymm2                     #11.19
        vmovups   %ymm2, -32(%rsp)                              #11.17
        vmovss    -32(%rsp), %xmm3                              #12.32
        vaddss    -16(%rsp), %xmm3, %xmm4                       #12.5
        vaddss    12(%rdx), %xmm4, %xmm5                        #12.55
        vmovss    %xmm5, 12(%rdx)                               #12.5
        movq      32(%rdi), %rcx                                #10.37
        movq      24(%rdi), %rsi                                #12.5
        vmovups   128(%rcx), %ymm6                              #11.19
        vdpps     $255, %ymm0, %ymm6, %ymm7                     #11.19
        vmovups   %ymm7, -32(%rsp)                              #11.17
        vmovss    -32(%rsp), %xmm8                              #12.32
        vaddss    -16(%rsp), %xmm8, %xmm9                       #12.5
        vaddss    16(%rsi), %xmm9, %xmm10                       #12.55
        vmovss    %xmm10, 16(%rsi)                              #12.5
        movq      32(%rdi), %r8                                 #10.37
        movq      24(%rdi), %r9                                 #12.5
        vmovups   160(%r8), %ymm11                              #11.19
        vdpps     $255, %ymm0, %ymm11, %ymm12                   #11.19
        vmovups   %ymm12, -32(%rsp)                             #11.17
        vmovss    -32(%rsp), %xmm13                             #12.32
        vaddss    -16(%rsp), %xmm13, %xmm14                     #12.5
        vaddss    20(%r9), %xmm14, %xmm15                       #12.55
        vmovss    %xmm15, 20(%r9)                               #12.5
        movq      32(%rdi), %r10                                #10.37
        movq      24(%rdi), %r11                                #12.5
        vmovups   192(%r10), %ymm1                              #11.19
        vdpps     $255, %ymm0, %ymm1, %ymm2                     #11.19
        vmovups   %ymm2, -32(%rsp)                              #11.17
        vmovss    -32(%rsp), %xmm3                              #12.32
        vaddss    -16(%rsp), %xmm3, %xmm4                       #12.5
        vaddss    24(%r11), %xmm4, %xmm5                        #12.55
        vmovss    %xmm5, 24(%r11)                               #12.5
        movq      32(%rdi), %rax                                #10.37
        movq      24(%rdi), %rdi                                #12.5
        vmovups   224(%rax), %ymm6                              #11.19
        xorl      %eax, %eax                                    #14.10
        vdpps     $255, %ymm0, %ymm6, %ymm0                     #11.19
        vmovups   %ymm0, -32(%rsp)                              #11.17
        vmovss    -32(%rsp), %xmm0                              #12.32
        vaddss    -16(%rsp), %xmm0, %xmm1                       #12.5
        vaddss    28(%rdi), %xmm1, %xmm2                        #12.55
        vmovss    %xmm2, 28(%rdi)                               #12.5
        vzeroupper                                              #14.10
        movq      %rbp, %rsp                                    #14.10
        popq      %rbp                                          #14.10
..___tag_value__Z7testmsmR8msmparms.6:                          #
        ret                                                     #14.10
        .align    16,0x90
..___tag_value__Z7testmsmR8msmparms.8:                          #
                                # LOE
# mark_end;
	.type	_Z7testmsmR8msmparms,@function
	.size	_Z7testmsmR8msmparms,.-_Z7testmsmR8msmparms
	.data
# -- End  _Z7testmsmR8msmparms
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
	.8byte ..___tag_value__Z7testmsmR8msmparms.1
	.8byte ..___tag_value__Z7testmsmR8msmparms.8-..___tag_value__Z7testmsmR8msmparms.1
	.8byte 0x0000000000000008
	.2byte 0x0400
	.4byte ..___tag_value__Z7testmsmR8msmparms.3-..___tag_value__Z7testmsmR8msmparms.1
	.4byte 0x0410070c
	.4byte ..___tag_value__Z7testmsmR8msmparms.4-..___tag_value__Z7testmsmR8msmparms.3
	.4byte 0x8610060c
	.2byte 0x0402
	.4byte ..___tag_value__Z7testmsmR8msmparms.6-..___tag_value__Z7testmsmR8msmparms.4
	.8byte 0x000006060908070c
# End
