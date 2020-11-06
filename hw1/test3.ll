; ModuleID = 'test3.c'
source_filename = "test3.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %A = alloca [20 x i32], align 16
  %C = alloca [20 x i32], align 16
  %D = alloca [20 x i32], align 16
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %0, 20
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32, i32* %i, align 4
  %idxprom = sext i32 %1 to i64
  %arrayidx = getelementptr inbounds [20 x i32], [20 x i32]* %C, i64 0, i64 %idxprom
  %2 = load i32, i32* %arrayidx, align 4
  %3 = load i32, i32* %i, align 4
  %idxprom1 = sext i32 %3 to i64
  %arrayidx2 = getelementptr inbounds [20 x i32], [20 x i32]* %A, i64 0, i64 %idxprom1
  store i32 %2, i32* %arrayidx2, align 4
  %4 = load i32, i32* %i, align 4
  %mul = mul nsw i32 3, %4
  %sub = sub nsw i32 %mul, 4
  %idxprom3 = sext i32 %sub to i64
  %arrayidx4 = getelementptr inbounds [20 x i32], [20 x i32]* %A, i64 0, i64 %idxprom3
  %5 = load i32, i32* %arrayidx4, align 4
  %6 = load i32, i32* %i, align 4
  %idxprom5 = sext i32 %6 to i64
  %arrayidx6 = getelementptr inbounds [20 x i32], [20 x i32]* %D, i64 0, i64 %idxprom5
  store i32 %5, i32* %arrayidx6, align 4
  %7 = load i32, i32* %i, align 4
  %mul7 = mul nsw i32 2, %7
  %idxprom8 = sext i32 %mul7 to i64
  %arrayidx9 = getelementptr inbounds [20 x i32], [20 x i32]* %C, i64 0, i64 %idxprom8
  %8 = load i32, i32* %arrayidx9, align 4
  %9 = load i32, i32* %i, align 4
  %sub10 = sub nsw i32 %9, 1
  %idxprom11 = sext i32 %sub10 to i64
  %arrayidx12 = getelementptr inbounds [20 x i32], [20 x i32]* %D, i64 0, i64 %idxprom11
  store i32 %8, i32* %arrayidx12, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %10 = load i32, i32* %i, align 4
  %inc = add nsw i32 %10, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (tags/RELEASE_900/final)"}
