; ModuleID = 'llvm_ir_crc.ll'
source_filename = "llvm_ir_crc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [11 x i8] c"inputs.txt\00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"r\00", align 1
@.str.2 = private unnamed_addr constant [12 x i8] c"output3.txt\00", align 1
@.str.3 = private unnamed_addr constant [2 x i8] c"w\00", align 1
@.str.4 = private unnamed_addr constant [6 x i8] c"%d %d\00", align 1
@.str.5 = private unnamed_addr constant [13 x i8] c"report = %u\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local zeroext i16 @crcu8(i8 noundef zeroext %0, i16 noundef zeroext %1) #0 {
entry:
  %data.addr = alloca i8, align 1
  %_crc.addr = alloca i16, align 2
  %i = alloca i8, align 1
  %x16 = alloca i8, align 1
  %carry = alloca i8, align 1
  %crc = alloca i64, align 8
  store i8 %0, ptr %data.addr, align 1
  store i16 %1, ptr %_crc.addr, align 2
  store i8 0, ptr %i, align 1
  store i8 0, ptr %x16, align 1
  store i8 0, ptr %carry, align 1
  %2 = load i16, ptr %_crc.addr, align 2
  %conv = zext i16 %2 to i64
  store i64 %conv, ptr %crc, align 8
  %3 = load i8, ptr %data.addr, align 1
  %conv1 = zext i8 %3 to i64
  %4 = load i64, ptr %crc, align 8
  %xor = xor i64 %4, %conv1
  store i64 %xor, ptr %crc, align 8
  store i8 0, ptr %i, align 1
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %5 = load i8, ptr %i, align 1
  %conv2 = zext i8 %5 to i32
  %cmp = icmp slt i32 %conv2, 8
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %6 = load i64, ptr %crc, align 8
  %conv4 = trunc i64 %6 to i8
  %conv5 = zext i8 %conv4 to i32
  %and = and i32 %conv5, 1
  %conv6 = trunc i32 %and to i8
  store i8 %conv6, ptr %x16, align 1
  %7 = load i8, ptr %data.addr, align 1
  %conv7 = zext i8 %7 to i32
  %shr = ashr i32 %conv7, 1
  %conv8 = trunc i32 %shr to i8
  store i8 %conv8, ptr %data.addr, align 1
  %8 = load i64, ptr %crc, align 8
  %shr9 = ashr i64 %8, 1
  store i64 %shr9, ptr %crc, align 8
  %9 = load i8, ptr %x16, align 1
  %conv10 = zext i8 %9 to i32
  %and11 = and i32 %conv10, 1
  %tobool = icmp ne i32 %and11, 0
  %10 = zext i1 %tobool to i64
  %11 = select i1 %tobool, i32 40961, i32 0
  %conv12 = sext i32 %11 to i64
  %12 = load i64, ptr %crc, align 8
  %xor13 = xor i64 %12, %conv12
  store i64 %xor13, ptr %crc, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %13 = load i8, ptr %i, align 1
  %inc = add i8 %13, 1
  store i8 %inc, ptr %i, align 1
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %14 = load i64, ptr %crc, align 8
  %conv14 = trunc i64 %14 to i16
  ret i16 %conv14
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i16, align 2
  store i32 0, ptr %1, align 4
  %7 = call noalias ptr @fopen(ptr noundef @.str, ptr noundef @.str.1)
  store ptr %7, ptr %4, align 8
  %8 = load ptr, ptr %4, align 8
  %9 = icmp eq ptr %8, null
  br i1 %9, label %10, label %11

10:                                               ; preds = %0
  store i32 -1, ptr %1, align 4
  br label %32

11:                                               ; preds = %0
  %12 = call noalias ptr @fopen(ptr noundef @.str.2, ptr noundef @.str.3)
  store ptr %12, ptr %5, align 8
  br label %13

13:                                               ; preds = %17, %11
  %14 = load ptr, ptr %4, align 8
  %15 = call i32 (ptr, ptr, ...) @__isoc99_fscanf(ptr noundef %14, ptr noundef @.str.4, ptr noundef %2, ptr noundef %3)
  %16 = icmp ne i32 %15, -1
  br i1 %16, label %17, label %27

17:                                               ; preds = %13
  %18 = load i32, ptr %2, align 4
  %19 = trunc i32 %18 to i8
  %20 = load i32, ptr %3, align 4
  %21 = trunc i32 %20 to i16
  %22 = call zeroext i16 @crcu8(i8 noundef zeroext %19, i16 noundef zeroext %21)
  store i16 %22, ptr %6, align 2
  %23 = load ptr, ptr %5, align 8
  %24 = load i16, ptr %6, align 2
  %25 = zext i16 %24 to i32
  %26 = call i32 (ptr, ptr, ...) @fprintf(ptr noundef %23, ptr noundef @.str.5, i32 noundef %25)
  br label %13, !llvm.loop !6

27:                                               ; preds = %13
  %28 = load ptr, ptr %4, align 8
  %29 = call i32 @fclose(ptr noundef %28)
  %30 = load ptr, ptr %5, align 8
  %31 = call i32 @fclose(ptr noundef %30)
  store i32 0, ptr %1, align 4
  br label %32

32:                                               ; preds = %27, %10
  %33 = load i32, ptr %1, align 4
  ret i32 %33
}

declare noalias ptr @fopen(ptr noundef, ptr noundef) #1

declare i32 @__isoc99_fscanf(ptr noundef, ptr noundef, ...) #1

declare i32 @fprintf(ptr noundef, ptr noundef, ...) #1

declare i32 @fclose(ptr noundef) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
