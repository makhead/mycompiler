define dso_local i32 @main() #0 {
  %x = alloca i32, align 4
  store i32 1, i32* %x, align 4
  %y = alloca i32, align 4
  store i32 2, i32* %y, align 4
  %a = alloca i32, align 4
  store i32 3, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 4, i32* %b, align 4
  %c = alloca i32, align 4
  store i32 5, i32* %c, align 4
  %1 = load i32, i32* %y, align 4
  %2 = mul nsw i32 4, %1
  %3 = load i32, i32* %b, align 4
  %4 = add nsw i32 666, %3
  %5 = load i32, i32* %c, align 4
  %6 = add nsw i32 %4, %5
  %7 = sub nsw i32 0, %2
  %8 = sub nsw i32 %7, 5
  %9 = mul nsw i32 10, %8
  %10 = mul nsw i32 %9, %6
  %11 = load i32, i32* %x, align 4
  %12 = sdiv i32 %10, %11
  store i32 %12, i32* %x, align 4
  store i32 %12, i32* %b, align 4
  store i32 %12, i32* %a, align 4
  %13 = load i32, i32* %x, align 4
  %14 = load i32, i32* %y, align 4
  %15 = add nsw i32 %13, %14
  store i32 %15, i32* %x, align 4
  ret i32 0
}


attributes #0 = { noinline nounwind optnone uwtable"correctly-rounded-divide-sqrt-fp-math"="false""disable-tail-calls"="false" "frame-pointer"="all""less-precise-fpmad"="false" "min-legal-vector-width"="0""no-infs-fp-math"="false" "no-jump-tables"="false""no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false""no-trapping-math"="false" "stack-protector-buffer-size"="8""target-cpu"="x86-64""target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87""unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = {"correctly-rounded-divide-sqrt-fp-math"="false""disable-tail-calls"="false" "frame-pointer"="all""less-precise-fpmad"="false" "no-infs-fp-math"="false""no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false""no-trapping-math"="false" "stack-protector-buffer-size"="8""target-cpu"="x86-64""target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87""unsafe-fp-math"="false" "use-soft-float"="false" }
