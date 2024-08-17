; RUN: ../build/bin/llc syrmia_crc_unoptimized.ll -print-after-all -debug %s 2>&1 | FileCheck %s

; CHECK: crcu8: 
crcu8:  
; CHECK-NEXT: li      a4, quotient
   li      a4, quotient
; CHECK-NEXT: li      a5, polynomial
   li      a5, polynomial
; CHECK-NEXT: xor     a0, a1, a0
   xor     a0, a1, a0
; CHECK-NEXT: clmul   a0, a0, a4
   clmul   a0, a0, a4
; CHECK-NEXT: srli    a0, a0, crc_size
   srli    a0, a0, crc_size
; CHECK-NEXT: clmul   a0, a0, a5
   clmul   a0, a0, a5
; CHECK-NEXT: slli    a0, a0, GET_MODE_BITSIZE (word_mode) - crc_size
   slli    a0, a0, GET_MODE_BITSIZE (word_mode) - crc_size
; CHECK-NEXT: srli    a0, a0, GET_MODE_BITSIZE (word_mode) - crc_size
   srli    a0, a0, GET_MODE_BITSIZE (word_mode) - crc_size
; CHECK-NEXT: ret
   ret