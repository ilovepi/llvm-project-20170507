; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+sse4.1 | FileCheck %s --check-prefix=CHECK --check-prefix=SSE
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx  | FileCheck %s --check-prefix=CHECK --check-prefix=AVX --check-prefix=AVX1
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx2 | FileCheck %s --check-prefix=CHECK --check-prefix=AVX --check-prefix=AVX2

; fold (udiv x, 1) -> x
define i32 @combine_udiv_by_one(i32 %x) {
; CHECK-LABEL: combine_udiv_by_one:
; CHECK:       # %bb.0:
; CHECK-NEXT:    movl %edi, %eax
; CHECK-NEXT:    retq
  %1 = udiv i32 %x, 1
  ret i32 %1
}

define <4 x i32> @combine_vec_udiv_by_one(<4 x i32> %x) {
; CHECK-LABEL: combine_vec_udiv_by_one:
; CHECK:       # %bb.0:
; CHECK-NEXT:    retq
  %1 = udiv <4 x i32> %x, <i32 1, i32 1, i32 1, i32 1>
  ret <4 x i32> %1
}

; fold (udiv x, -1) -> select((icmp eq x, -1), 1, 0)
define i32 @combine_udiv_by_negone(i32 %x) {
; CHECK-LABEL: combine_udiv_by_negone:
; CHECK:       # %bb.0:
; CHECK-NEXT:    xorl %eax, %eax
; CHECK-NEXT:    cmpl $-1, %edi
; CHECK-NEXT:    sete %al
; CHECK-NEXT:    retq
  %1 = udiv i32 %x, -1
  ret i32 %1
}

define <4 x i32> @combine_vec_udiv_by_negone(<4 x i32> %x) {
; SSE-LABEL: combine_vec_udiv_by_negone:
; SSE:       # %bb.0:
; SSE-NEXT:    pcmpeqd %xmm1, %xmm1
; SSE-NEXT:    pcmpeqd %xmm1, %xmm0
; SSE-NEXT:    psrld $31, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_by_negone:
; AVX:       # %bb.0:
; AVX-NEXT:    vpcmpeqd %xmm1, %xmm1, %xmm1
; AVX-NEXT:    vpcmpeqd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vpsrld $31, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <4 x i32> %x, <i32 -1, i32 -1, i32 -1, i32 -1>
  ret <4 x i32> %1
}

; fold (udiv x, INT_MIN) -> (srl x, 31)
define i32 @combine_udiv_by_minsigned(i32 %x) {
; CHECK-LABEL: combine_udiv_by_minsigned:
; CHECK:       # %bb.0:
; CHECK-NEXT:    shrl $31, %edi
; CHECK-NEXT:    movl %edi, %eax
; CHECK-NEXT:    retq
  %1 = udiv i32 %x, -2147483648
  ret i32 %1
}

define <4 x i32> @combine_vec_udiv_by_minsigned(<4 x i32> %x) {
; SSE-LABEL: combine_vec_udiv_by_minsigned:
; SSE:       # %bb.0:
; SSE-NEXT:    psrld $31, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_by_minsigned:
; AVX:       # %bb.0:
; AVX-NEXT:    vpsrld $31, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <4 x i32> %x, <i32 -2147483648, i32 -2147483648, i32 -2147483648, i32 -2147483648>
  ret <4 x i32> %1
}

; TODO fold (udiv x, x) -> 1
define i32 @combine_udiv_dupe(i32 %x) {
; CHECK-LABEL: combine_udiv_dupe:
; CHECK:       # %bb.0:
; CHECK-NEXT:    xorl %edx, %edx
; CHECK-NEXT:    movl %edi, %eax
; CHECK-NEXT:    divl %edi
; CHECK-NEXT:    retq
  %1 = udiv i32 %x, %x
  ret i32 %1
}

define <4 x i32> @combine_vec_udiv_dupe(<4 x i32> %x) {
; SSE-LABEL: combine_vec_udiv_dupe:
; SSE:       # %bb.0:
; SSE-NEXT:    pextrd $1, %xmm0, %eax
; SSE-NEXT:    xorl %edx, %edx
; SSE-NEXT:    divl %eax
; SSE-NEXT:    movl %eax, %ecx
; SSE-NEXT:    movd %xmm0, %eax
; SSE-NEXT:    xorl %edx, %edx
; SSE-NEXT:    divl %eax
; SSE-NEXT:    movd %eax, %xmm1
; SSE-NEXT:    pinsrd $1, %ecx, %xmm1
; SSE-NEXT:    pextrd $2, %xmm0, %eax
; SSE-NEXT:    xorl %edx, %edx
; SSE-NEXT:    divl %eax
; SSE-NEXT:    pinsrd $2, %eax, %xmm1
; SSE-NEXT:    pextrd $3, %xmm0, %eax
; SSE-NEXT:    xorl %edx, %edx
; SSE-NEXT:    divl %eax
; SSE-NEXT:    pinsrd $3, %eax, %xmm1
; SSE-NEXT:    movdqa %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_dupe:
; AVX:       # %bb.0:
; AVX-NEXT:    vpextrd $1, %xmm0, %eax
; AVX-NEXT:    xorl %edx, %edx
; AVX-NEXT:    divl %eax
; AVX-NEXT:    movl %eax, %ecx
; AVX-NEXT:    vmovd %xmm0, %eax
; AVX-NEXT:    xorl %edx, %edx
; AVX-NEXT:    divl %eax
; AVX-NEXT:    vmovd %eax, %xmm1
; AVX-NEXT:    vpinsrd $1, %ecx, %xmm1, %xmm1
; AVX-NEXT:    vpextrd $2, %xmm0, %eax
; AVX-NEXT:    xorl %edx, %edx
; AVX-NEXT:    divl %eax
; AVX-NEXT:    vpinsrd $2, %eax, %xmm1, %xmm1
; AVX-NEXT:    vpextrd $3, %xmm0, %eax
; AVX-NEXT:    xorl %edx, %edx
; AVX-NEXT:    divl %eax
; AVX-NEXT:    vpinsrd $3, %eax, %xmm1, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <4 x i32> %x, %x
  ret <4 x i32> %1
}

; fold (udiv x, (1 << c)) -> x >>u c
define <4 x i32> @combine_vec_udiv_by_pow2a(<4 x i32> %x) {
; SSE-LABEL: combine_vec_udiv_by_pow2a:
; SSE:       # %bb.0:
; SSE-NEXT:    psrld $2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_by_pow2a:
; AVX:       # %bb.0:
; AVX-NEXT:    vpsrld $2, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <4 x i32> %x, <i32 4, i32 4, i32 4, i32 4>
  ret <4 x i32> %1
}

define <4 x i32> @combine_vec_udiv_by_pow2b(<4 x i32> %x) {
; SSE-LABEL: combine_vec_udiv_by_pow2b:
; SSE:       # %bb.0:
; SSE-NEXT:    movdqa %xmm0, %xmm2
; SSE-NEXT:    movdqa %xmm0, %xmm1
; SSE-NEXT:    psrld $3, %xmm1
; SSE-NEXT:    pblendw {{.*#+}} xmm1 = xmm0[0,1,2,3],xmm1[4,5,6,7]
; SSE-NEXT:    psrld $4, %xmm0
; SSE-NEXT:    psrld $2, %xmm2
; SSE-NEXT:    pblendw {{.*#+}} xmm2 = xmm2[0,1,2,3],xmm0[4,5,6,7]
; SSE-NEXT:    pblendw {{.*#+}} xmm1 = xmm1[0,1],xmm2[2,3],xmm1[4,5],xmm2[6,7]
; SSE-NEXT:    movdqa %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX1-LABEL: combine_vec_udiv_by_pow2b:
; AVX1:       # %bb.0:
; AVX1-NEXT:    vpsrld $4, %xmm0, %xmm1
; AVX1-NEXT:    vpsrld $2, %xmm0, %xmm2
; AVX1-NEXT:    vpblendw {{.*#+}} xmm1 = xmm2[0,1,2,3],xmm1[4,5,6,7]
; AVX1-NEXT:    vpsrld $3, %xmm0, %xmm2
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1,2,3],xmm2[4,5,6,7]
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1],xmm1[2,3],xmm0[4,5],xmm1[6,7]
; AVX1-NEXT:    retq
;
; AVX2-LABEL: combine_vec_udiv_by_pow2b:
; AVX2:       # %bb.0:
; AVX2-NEXT:    vpsrlvd {{.*}}(%rip), %xmm0, %xmm0
; AVX2-NEXT:    retq
  %1 = udiv <4 x i32> %x, <i32 1, i32 4, i32 8, i32 16>
  ret <4 x i32> %1
}

define <4 x i32> @combine_vec_udiv_by_pow2c(<4 x i32> %x, <4 x i32> %y) {
; SSE-LABEL: combine_vec_udiv_by_pow2c:
; SSE:       # %bb.0:
; SSE-NEXT:    pshuflw {{.*#+}} xmm2 = xmm1[2,3,3,3,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm3
; SSE-NEXT:    psrld %xmm2, %xmm3
; SSE-NEXT:    pshufd {{.*#+}} xmm2 = xmm1[2,3,0,1]
; SSE-NEXT:    pshuflw {{.*#+}} xmm4 = xmm2[2,3,3,3,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm5
; SSE-NEXT:    psrld %xmm4, %xmm5
; SSE-NEXT:    pblendw {{.*#+}} xmm5 = xmm3[0,1,2,3],xmm5[4,5,6,7]
; SSE-NEXT:    pshuflw {{.*#+}} xmm1 = xmm1[0,1,1,1,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm3
; SSE-NEXT:    psrld %xmm1, %xmm3
; SSE-NEXT:    pshuflw {{.*#+}} xmm1 = xmm2[0,1,1,1,4,5,6,7]
; SSE-NEXT:    psrld %xmm1, %xmm0
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm3[0,1,2,3],xmm0[4,5,6,7]
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm0[0,1],xmm5[2,3],xmm0[4,5],xmm5[6,7]
; SSE-NEXT:    retq
;
; AVX1-LABEL: combine_vec_udiv_by_pow2c:
; AVX1:       # %bb.0:
; AVX1-NEXT:    vpsrldq {{.*#+}} xmm2 = xmm1[12,13,14,15],zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero
; AVX1-NEXT:    vpsrld %xmm2, %xmm0, %xmm2
; AVX1-NEXT:    vpsrlq $32, %xmm1, %xmm3
; AVX1-NEXT:    vpsrld %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpblendw {{.*#+}} xmm2 = xmm3[0,1,2,3],xmm2[4,5,6,7]
; AVX1-NEXT:    vpxor %xmm3, %xmm3, %xmm3
; AVX1-NEXT:    vpunpckhdq {{.*#+}} xmm3 = xmm1[2],xmm3[2],xmm1[3],xmm3[3]
; AVX1-NEXT:    vpsrld %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpmovzxdq {{.*#+}} xmm1 = xmm1[0],zero,xmm1[1],zero
; AVX1-NEXT:    vpsrld %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1,2,3],xmm3[4,5,6,7]
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1],xmm2[2,3],xmm0[4,5],xmm2[6,7]
; AVX1-NEXT:    retq
;
; AVX2-LABEL: combine_vec_udiv_by_pow2c:
; AVX2:       # %bb.0:
; AVX2-NEXT:    vpsrlvd %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    retq
  %1 = shl <4 x i32> <i32 1, i32 1, i32 1, i32 1>, %y
  %2 = udiv <4 x i32> %x, %1
  ret <4 x i32> %2
}

; fold (udiv x, (shl c, y)) -> x >>u (log2(c)+y) iff c is power of 2
define <4 x i32> @combine_vec_udiv_by_shl_pow2a(<4 x i32> %x, <4 x i32> %y) {
; SSE-LABEL: combine_vec_udiv_by_shl_pow2a:
; SSE:       # %bb.0:
; SSE-NEXT:    paddd {{.*}}(%rip), %xmm1
; SSE-NEXT:    pshuflw {{.*#+}} xmm2 = xmm1[2,3,3,3,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm3
; SSE-NEXT:    psrld %xmm2, %xmm3
; SSE-NEXT:    pshufd {{.*#+}} xmm2 = xmm1[2,3,0,1]
; SSE-NEXT:    pshuflw {{.*#+}} xmm4 = xmm2[2,3,3,3,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm5
; SSE-NEXT:    psrld %xmm4, %xmm5
; SSE-NEXT:    pblendw {{.*#+}} xmm5 = xmm3[0,1,2,3],xmm5[4,5,6,7]
; SSE-NEXT:    pshuflw {{.*#+}} xmm1 = xmm1[0,1,1,1,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm3
; SSE-NEXT:    psrld %xmm1, %xmm3
; SSE-NEXT:    pshuflw {{.*#+}} xmm1 = xmm2[0,1,1,1,4,5,6,7]
; SSE-NEXT:    psrld %xmm1, %xmm0
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm3[0,1,2,3],xmm0[4,5,6,7]
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm0[0,1],xmm5[2,3],xmm0[4,5],xmm5[6,7]
; SSE-NEXT:    retq
;
; AVX1-LABEL: combine_vec_udiv_by_shl_pow2a:
; AVX1:       # %bb.0:
; AVX1-NEXT:    vpaddd {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vpsrldq {{.*#+}} xmm2 = xmm1[12,13,14,15],zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero
; AVX1-NEXT:    vpsrld %xmm2, %xmm0, %xmm2
; AVX1-NEXT:    vpsrlq $32, %xmm1, %xmm3
; AVX1-NEXT:    vpsrld %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpblendw {{.*#+}} xmm2 = xmm3[0,1,2,3],xmm2[4,5,6,7]
; AVX1-NEXT:    vpxor %xmm3, %xmm3, %xmm3
; AVX1-NEXT:    vpunpckhdq {{.*#+}} xmm3 = xmm1[2],xmm3[2],xmm1[3],xmm3[3]
; AVX1-NEXT:    vpsrld %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpmovzxdq {{.*#+}} xmm1 = xmm1[0],zero,xmm1[1],zero
; AVX1-NEXT:    vpsrld %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1,2,3],xmm3[4,5,6,7]
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1],xmm2[2,3],xmm0[4,5],xmm2[6,7]
; AVX1-NEXT:    retq
;
; AVX2-LABEL: combine_vec_udiv_by_shl_pow2a:
; AVX2:       # %bb.0:
; AVX2-NEXT:    vpbroadcastd {{.*#+}} xmm2 = [2,2,2,2]
; AVX2-NEXT:    vpaddd %xmm2, %xmm1, %xmm1
; AVX2-NEXT:    vpsrlvd %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    retq
  %1 = shl <4 x i32> <i32 4, i32 4, i32 4, i32 4>, %y
  %2 = udiv <4 x i32> %x, %1
  ret <4 x i32> %2
}

define <4 x i32> @combine_vec_udiv_by_shl_pow2b(<4 x i32> %x, <4 x i32> %y) {
; SSE-LABEL: combine_vec_udiv_by_shl_pow2b:
; SSE:       # %bb.0:
; SSE-NEXT:    paddd {{.*}}(%rip), %xmm1
; SSE-NEXT:    pshuflw {{.*#+}} xmm2 = xmm1[2,3,3,3,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm3
; SSE-NEXT:    psrld %xmm2, %xmm3
; SSE-NEXT:    pshufd {{.*#+}} xmm2 = xmm1[2,3,0,1]
; SSE-NEXT:    pshuflw {{.*#+}} xmm4 = xmm2[2,3,3,3,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm5
; SSE-NEXT:    psrld %xmm4, %xmm5
; SSE-NEXT:    pblendw {{.*#+}} xmm5 = xmm3[0,1,2,3],xmm5[4,5,6,7]
; SSE-NEXT:    pshuflw {{.*#+}} xmm1 = xmm1[0,1,1,1,4,5,6,7]
; SSE-NEXT:    movdqa %xmm0, %xmm3
; SSE-NEXT:    psrld %xmm1, %xmm3
; SSE-NEXT:    pshuflw {{.*#+}} xmm1 = xmm2[0,1,1,1,4,5,6,7]
; SSE-NEXT:    psrld %xmm1, %xmm0
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm3[0,1,2,3],xmm0[4,5,6,7]
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm0[0,1],xmm5[2,3],xmm0[4,5],xmm5[6,7]
; SSE-NEXT:    retq
;
; AVX1-LABEL: combine_vec_udiv_by_shl_pow2b:
; AVX1:       # %bb.0:
; AVX1-NEXT:    vpaddd {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vpsrldq {{.*#+}} xmm2 = xmm1[12,13,14,15],zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero
; AVX1-NEXT:    vpsrld %xmm2, %xmm0, %xmm2
; AVX1-NEXT:    vpsrlq $32, %xmm1, %xmm3
; AVX1-NEXT:    vpsrld %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpblendw {{.*#+}} xmm2 = xmm3[0,1,2,3],xmm2[4,5,6,7]
; AVX1-NEXT:    vpxor %xmm3, %xmm3, %xmm3
; AVX1-NEXT:    vpunpckhdq {{.*#+}} xmm3 = xmm1[2],xmm3[2],xmm1[3],xmm3[3]
; AVX1-NEXT:    vpsrld %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpmovzxdq {{.*#+}} xmm1 = xmm1[0],zero,xmm1[1],zero
; AVX1-NEXT:    vpsrld %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1,2,3],xmm3[4,5,6,7]
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm0[0,1],xmm2[2,3],xmm0[4,5],xmm2[6,7]
; AVX1-NEXT:    retq
;
; AVX2-LABEL: combine_vec_udiv_by_shl_pow2b:
; AVX2:       # %bb.0:
; AVX2-NEXT:    vpaddd {{.*}}(%rip), %xmm1, %xmm1
; AVX2-NEXT:    vpsrlvd %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    retq
  %1 = shl <4 x i32> <i32 1, i32 4, i32 8, i32 16>, %y
  %2 = udiv <4 x i32> %x, %1
  ret <4 x i32> %2
}

; fold (udiv x, c1)
define i32 @combine_udiv_uniform(i32 %x) {
; CHECK-LABEL: combine_udiv_uniform:
; CHECK:       # %bb.0:
; CHECK-NEXT:    movl %edi, %ecx
; CHECK-NEXT:    movl $2987803337, %eax # imm = 0xB21642C9
; CHECK-NEXT:    imulq %rcx, %rax
; CHECK-NEXT:    shrq $36, %rax
; CHECK-NEXT:    # kill: def $eax killed $eax killed $rax
; CHECK-NEXT:    retq
  %1 = udiv i32 %x, 23
  ret i32 %1
}

define <8 x i16> @combine_vec_udiv_uniform(<8 x i16> %x) {
; SSE-LABEL: combine_vec_udiv_uniform:
; SSE:       # %bb.0:
; SSE-NEXT:    movdqa {{.*#+}} xmm1 = [25645,25645,25645,25645,25645,25645,25645,25645]
; SSE-NEXT:    pmulhuw %xmm0, %xmm1
; SSE-NEXT:    psubw %xmm1, %xmm0
; SSE-NEXT:    psrlw $1, %xmm0
; SSE-NEXT:    paddw %xmm1, %xmm0
; SSE-NEXT:    psrlw $4, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_uniform:
; AVX:       # %bb.0:
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm0, %xmm1
; AVX-NEXT:    vpsubw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vpsrlw $1, %xmm0, %xmm0
; AVX-NEXT:    vpaddw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vpsrlw $4, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <8 x i16> %x, <i16 23, i16 23, i16 23, i16 23, i16 23, i16 23, i16 23, i16 23>
  ret <8 x i16> %1
}

define <8 x i16> @combine_vec_udiv_nonuniform(<8 x i16> %x) {
; SSE-LABEL: combine_vec_udiv_nonuniform:
; SSE:       # %bb.0:
; SSE-NEXT:    movdqa %xmm0, %xmm1
; SSE-NEXT:    psrlw $3, %xmm1
; SSE-NEXT:    pblendw {{.*#+}} xmm1 = xmm0[0,1,2],xmm1[3],xmm0[4,5,6,7]
; SSE-NEXT:    pmulhuw {{.*}}(%rip), %xmm1
; SSE-NEXT:    psubw %xmm1, %xmm0
; SSE-NEXT:    movl $32768, %eax # imm = 0x8000
; SSE-NEXT:    movd %eax, %xmm2
; SSE-NEXT:    pmulhuw %xmm0, %xmm2
; SSE-NEXT:    paddw %xmm1, %xmm2
; SSE-NEXT:    movdqa {{.*#+}} xmm0 = <4096,2048,8,u,u,2,2,u>
; SSE-NEXT:    pmulhuw %xmm2, %xmm0
; SSE-NEXT:    pblendw {{.*#+}} xmm0 = xmm0[0,1,2],xmm2[3,4],xmm0[5,6],xmm2[7]
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_nonuniform:
; AVX:       # %bb.0:
; AVX-NEXT:    vpsrlw $3, %xmm0, %xmm1
; AVX-NEXT:    vpblendw {{.*#+}} xmm1 = xmm0[0,1,2],xmm1[3],xmm0[4,5,6,7]
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm1, %xmm1
; AVX-NEXT:    vpsubw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    movl $32768, %eax # imm = 0x8000
; AVX-NEXT:    vmovd %eax, %xmm2
; AVX-NEXT:    vpmulhuw %xmm2, %xmm0, %xmm0
; AVX-NEXT:    vpaddw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm0, %xmm1
; AVX-NEXT:    vpblendw {{.*#+}} xmm0 = xmm1[0,1,2],xmm0[3,4],xmm1[5,6],xmm0[7]
; AVX-NEXT:    retq
  %1 = udiv <8 x i16> %x, <i16 23, i16 34, i16 -23, i16 56, i16 128, i16 -1, i16 -256, i16 -32768>
  ret <8 x i16> %1
}

define <8 x i16> @combine_vec_udiv_nonuniform2(<8 x i16> %x) {
; SSE-LABEL: combine_vec_udiv_nonuniform2:
; SSE:       # %bb.0:
; SSE-NEXT:    movdqa %xmm0, %xmm1
; SSE-NEXT:    psrlw $1, %xmm1
; SSE-NEXT:    pblendw {{.*#+}} xmm1 = xmm1[0],xmm0[1,2,3,4,5,6,7]
; SSE-NEXT:    pmulhuw {{.*}}(%rip), %xmm1
; SSE-NEXT:    pmulhuw {{.*}}(%rip), %xmm1
; SSE-NEXT:    movdqa %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_nonuniform2:
; AVX:       # %bb.0:
; AVX-NEXT:    vpsrlw $1, %xmm0, %xmm1
; AVX-NEXT:    vpblendw {{.*#+}} xmm0 = xmm1[0],xmm0[1,2,3,4,5,6,7]
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <8 x i16> %x, <i16 -34, i16 35, i16 36, i16 -37, i16 38, i16 -39, i16 40, i16 -41>
  ret <8 x i16> %1
}

define <8 x i16> @combine_vec_udiv_nonuniform3(<8 x i16> %x) {
; SSE-LABEL: combine_vec_udiv_nonuniform3:
; SSE:       # %bb.0:
; SSE-NEXT:    movdqa {{.*#+}} xmm1 = [9363,25645,18351,12137,2115,23705,1041,517]
; SSE-NEXT:    pmulhuw %xmm0, %xmm1
; SSE-NEXT:    psubw %xmm1, %xmm0
; SSE-NEXT:    psrlw $1, %xmm0
; SSE-NEXT:    paddw %xmm1, %xmm0
; SSE-NEXT:    pmulhuw {{.*}}(%rip), %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: combine_vec_udiv_nonuniform3:
; AVX:       # %bb.0:
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm0, %xmm1
; AVX-NEXT:    vpsubw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vpsrlw $1, %xmm0, %xmm0
; AVX-NEXT:    vpaddw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vpmulhuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = udiv <8 x i16> %x, <i16 7, i16 23, i16 25, i16 27, i16 31, i16 47, i16 63, i16 127>
  ret <8 x i16> %1
}

; TODO: Handle udiv-by-one
define <8 x i16> @pr38477(<8 x i16> %a0) {
; SSE-LABEL: pr38477:
; SSE:       # %bb.0:
; SSE-NEXT:    pextrw $7, %xmm0, %ecx
; SSE-NEXT:    imull $2115, %ecx, %edx # imm = 0x843
; SSE-NEXT:    shrl $16, %edx
; SSE-NEXT:    movl %ecx, %eax
; SSE-NEXT:    subl %edx, %eax
; SSE-NEXT:    movzwl %ax, %eax
; SSE-NEXT:    shrl %eax
; SSE-NEXT:    addl %edx, %eax
; SSE-NEXT:    shrl $4, %eax
; SSE-NEXT:    movl %eax, %edx
; SSE-NEXT:    shll $5, %edx
; SSE-NEXT:    subl %edx, %eax
; SSE-NEXT:    addl %ecx, %eax
; SSE-NEXT:    pextrw $2, %xmm0, %ecx
; SSE-NEXT:    imull $57457, %ecx, %edx # imm = 0xE071
; SSE-NEXT:    shrl $22, %edx
; SSE-NEXT:    leal (%rdx,%rdx,8), %esi
; SSE-NEXT:    leal (%rdx,%rsi,8), %edx
; SSE-NEXT:    subl %edx, %ecx
; SSE-NEXT:    pextrw $1, %xmm0, %edx
; SSE-NEXT:    imull $4957, %edx, %esi # imm = 0x135D
; SSE-NEXT:    shrl $16, %esi
; SSE-NEXT:    movl %edx, %edi
; SSE-NEXT:    subl %esi, %edi
; SSE-NEXT:    movzwl %di, %edi
; SSE-NEXT:    shrl %edi
; SSE-NEXT:    addl %esi, %edi
; SSE-NEXT:    shrl $6, %edi
; SSE-NEXT:    imull $119, %edi, %esi
; SSE-NEXT:    subl %esi, %edx
; SSE-NEXT:    pxor %xmm1, %xmm1
; SSE-NEXT:    pinsrw $1, %edx, %xmm1
; SSE-NEXT:    pinsrw $2, %ecx, %xmm1
; SSE-NEXT:    pextrw $3, %xmm0, %ecx
; SSE-NEXT:    imull $4103, %ecx, %edx # imm = 0x1007
; SSE-NEXT:    shrl $28, %edx
; SSE-NEXT:    imull $-111, %edx, %edx
; SSE-NEXT:    subl %edx, %ecx
; SSE-NEXT:    pinsrw $3, %ecx, %xmm1
; SSE-NEXT:    pextrw $4, %xmm0, %ecx
; SSE-NEXT:    movl %ecx, %edx
; SSE-NEXT:    shll $14, %edx
; SSE-NEXT:    addl %ecx, %edx
; SSE-NEXT:    shrl $30, %edx
; SSE-NEXT:    leal (%rdx,%rdx,2), %edx
; SSE-NEXT:    addl %ecx, %edx
; SSE-NEXT:    pinsrw $4, %edx, %xmm1
; SSE-NEXT:    pextrw $5, %xmm0, %ecx
; SSE-NEXT:    imull $35545, %ecx, %edx # imm = 0x8AD9
; SSE-NEXT:    shrl $22, %edx
; SSE-NEXT:    imull $118, %edx, %edx
; SSE-NEXT:    subl %edx, %ecx
; SSE-NEXT:    pinsrw $5, %ecx, %xmm1
; SSE-NEXT:    pextrw $6, %xmm0, %ecx
; SSE-NEXT:    andl $31, %ecx
; SSE-NEXT:    pinsrw $6, %ecx, %xmm1
; SSE-NEXT:    pinsrw $7, %eax, %xmm1
; SSE-NEXT:    movdqa %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: pr38477:
; AVX:       # %bb.0:
; AVX-NEXT:    vpextrw $7, %xmm0, %ecx
; AVX-NEXT:    imull $2115, %ecx, %edx # imm = 0x843
; AVX-NEXT:    shrl $16, %edx
; AVX-NEXT:    movl %ecx, %eax
; AVX-NEXT:    subl %edx, %eax
; AVX-NEXT:    movzwl %ax, %eax
; AVX-NEXT:    shrl %eax
; AVX-NEXT:    addl %edx, %eax
; AVX-NEXT:    shrl $4, %eax
; AVX-NEXT:    movl %eax, %edx
; AVX-NEXT:    shll $5, %edx
; AVX-NEXT:    subl %edx, %eax
; AVX-NEXT:    addl %ecx, %eax
; AVX-NEXT:    vpextrw $2, %xmm0, %ecx
; AVX-NEXT:    imull $57457, %ecx, %edx # imm = 0xE071
; AVX-NEXT:    shrl $22, %edx
; AVX-NEXT:    leal (%rdx,%rdx,8), %esi
; AVX-NEXT:    leal (%rdx,%rsi,8), %edx
; AVX-NEXT:    subl %edx, %ecx
; AVX-NEXT:    vpextrw $1, %xmm0, %edx
; AVX-NEXT:    imull $4957, %edx, %esi # imm = 0x135D
; AVX-NEXT:    shrl $16, %esi
; AVX-NEXT:    movl %edx, %edi
; AVX-NEXT:    subl %esi, %edi
; AVX-NEXT:    movzwl %di, %edi
; AVX-NEXT:    shrl %edi
; AVX-NEXT:    addl %esi, %edi
; AVX-NEXT:    shrl $6, %edi
; AVX-NEXT:    imull $119, %edi, %esi
; AVX-NEXT:    subl %esi, %edx
; AVX-NEXT:    vpxor %xmm1, %xmm1, %xmm1
; AVX-NEXT:    vpinsrw $1, %edx, %xmm1, %xmm1
; AVX-NEXT:    vpinsrw $2, %ecx, %xmm1, %xmm1
; AVX-NEXT:    vpextrw $3, %xmm0, %ecx
; AVX-NEXT:    imull $4103, %ecx, %edx # imm = 0x1007
; AVX-NEXT:    shrl $28, %edx
; AVX-NEXT:    imull $-111, %edx, %edx
; AVX-NEXT:    subl %edx, %ecx
; AVX-NEXT:    vpinsrw $3, %ecx, %xmm1, %xmm1
; AVX-NEXT:    vpextrw $4, %xmm0, %ecx
; AVX-NEXT:    movl %ecx, %edx
; AVX-NEXT:    shll $14, %edx
; AVX-NEXT:    addl %ecx, %edx
; AVX-NEXT:    shrl $30, %edx
; AVX-NEXT:    leal (%rdx,%rdx,2), %edx
; AVX-NEXT:    addl %ecx, %edx
; AVX-NEXT:    vpinsrw $4, %edx, %xmm1, %xmm1
; AVX-NEXT:    vpextrw $5, %xmm0, %ecx
; AVX-NEXT:    imull $35545, %ecx, %edx # imm = 0x8AD9
; AVX-NEXT:    shrl $22, %edx
; AVX-NEXT:    imull $118, %edx, %edx
; AVX-NEXT:    subl %edx, %ecx
; AVX-NEXT:    vpinsrw $5, %ecx, %xmm1, %xmm1
; AVX-NEXT:    vpextrw $6, %xmm0, %ecx
; AVX-NEXT:    andl $31, %ecx
; AVX-NEXT:    vpinsrw $6, %ecx, %xmm1, %xmm0
; AVX-NEXT:    vpinsrw $7, %eax, %xmm0, %xmm0
; AVX-NEXT:    retq
  %rem = urem <8 x i16> %a0, <i16 1, i16 119, i16 73, i16 -111, i16 -3, i16 118, i16 32, i16 31>
  ret <8 x i16> %rem
}
