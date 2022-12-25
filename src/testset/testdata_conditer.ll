define dso_local i32 @main() #0 {
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %j, align 4
  %sum = alloca i32, align 4
  store i32 0, i32* %sum, align 4
  br label %L0

L0:
  %1 = load i32, i32* %i, align 4
  %2 = icmp sle i32 %1, 100
  br i1 %2, label %L1, label %L2   

L1:
  %3 = load i32, i32* %i, align 4
  %4 = add nsw i32 %3, 1
  store i32 %4, i32* %i, align 4
  %5 = load i32, i32* %sum, align 4
  %6 = load i32, i32* %i, align 4
  %7 = add nsw i32 %5, %6
  store i32 %7, i32* %sum, align 4
  br label %L0

L2:
  %8 = load i32, i32* %sum, align 4
  %9 = icmp sgt i32 %8, 100
  br i1 %9, label %L3, label %L4   

L3:
  %10 = load i32, i32* %sum, align 4
  %11 = sdiv i32 %10, 10
  store i32 %11, i32* %sum, align 4
  br label %L5   

L4:
  %12 = load i32, i32* %sum, align 4
  %13 = mul nsw i32 %12, 10
  store i32 %13, i32* %sum, align 4
  br label %L5

L5:
  ret i32 0
}


attributes #0 = { noinline nounwind optnone uwtable"correctly-rounded-divide-sqrt-fp-math"="false""disable-tail-calls"="false" "frame-pointer"="all""less-precise-fpmad"="false" "min-legal-vector-width"="0""no-infs-fp-math"="false" "no-jump-tables"="false""no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false""no-trapping-math"="false" "stack-protector-buffer-size"="8""target-cpu"="x86-64""target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87""unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = {"correctly-rounded-divide-sqrt-fp-math"="false""disable-tail-calls"="false" "frame-pointer"="all""less-precise-fpmad"="false" "no-infs-fp-math"="false""no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false""no-trapping-math"="false" "stack-protector-buffer-size"="8""target-cpu"="x86-64""target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87""unsafe-fp-math"="false" "use-soft-float"="false" }
