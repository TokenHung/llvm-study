; ModuleID = 'test2.c'
source_filename = "test2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %A = alloca [310 x i32], align 16
  %C = alloca [60 x i32], align 16
  %D = alloca [60 x i32], align 16
  store i32 0, i32* %retval, align 4
  store i32 2, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %0, 50
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32, i32* %i, align 4
  %idxprom = sext i32 %1 to i64
  %arrayidx = getelementptr inbounds [60 x i32], [60 x i32]* %C, i64 0, i64 %idxprom
  %2 = load i32, i32* %arrayidx, align 4
  %3 = load i32, i32* %i, align 4
  %mul = mul nsw i32 3, %3
  %sub = sub nsw i32 %mul, 5
  %idxprom1 = sext i32 %sub to i64
  %arrayidx2 = getelementptr inbounds [310 x i32], [310 x i32]* %A, i64 0, i64 %idxprom1
  store i32 %2, i32* %arrayidx2, align 4
  %4 = load i32, i32* %i, align 4
  %mul3 = mul nsw i32 6, %4
  %add = add nsw i32 %mul3, 1
  %idxprom4 = sext i32 %add to i64
  %arrayidx5 = getelementptr inbounds [310 x i32], [310 x i32]* %A, i64 0, i64 %idxprom4
  %5 = load i32, i32* %arrayidx5, align 4
  %6 = load i32, i32* %i, align 4
  %idxprom6 = sext i32 %6 to i64
  %arrayidx7 = getelementptr inbounds [60 x i32], [60 x i32]* %D, i64 0, i64 %idxprom6
  store i32 %5, i32* %arrayidx7, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %7 = load i32, i32* %i, align 4
  %inc = add nsw i32 %7, 1
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
