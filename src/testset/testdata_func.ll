define dso_local i32 @f(i32 %1, i32 %2, i32 %3) #0 {
  %a = alloca i32, align 4
  store i32 %1, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 %2, i32* %b, align 4
  %c = alloca i32, align 4
  store i32 %3, i32* %c, align 4
  %4 = load i32, i32* %a, align 4
  %5 = load i32, i32* %b, align 4
  %6 = add nsw i32 %4, %5
  %7 = load i32, i32* %c, align 4
  %8 = add nsw i32 %6, %7
  ret i32 %8
}

define dso_local i32 @main() #0 {
  %a = alloca i32, align 4
  store i32 1, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 2, i32* %b, align 4
  %1 = load i32, i32* %b, align 4
  %2 = mul nsw i32 3, %1
  %3 = load i32, i32* %a, align 4
  %4 = add nsw i32 %3, %2
  %5 = load i32, i32* %a, align 4
  %6 = load i32, i32* %b, align 4
  %7 = sub nsw i32 %5, %6
  %8 = load i32, i32* %a, align 4
  %9 = load i32, i32* %b, align 4
  %10 = add nsw i32 %8, %9
  %11 = call i32 @f(i32 %10, i32 %7, i32 %4)
  store i32 %11, i32* %a, align 4
  ret i32 0
}


attributes #0 = { noinline nounwind optnone uwtable"correctly-rounded-divide-sqrt-fp-math"="false""disable-tail-calls"="false" "frame-pointer"="all""less-precise-fpmad"="false" "min-legal-vector-width"="0""no-infs-fp-math"="false" "no-jump-tables"="false""no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false""no-trapping-math"="false" "stack-protector-buffer-size"="8""target-cpu"="x86-64""target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87""unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = {"correctly-rounded-divide-sqrt-fp-math"="false""disable-tail-calls"="false" "frame-pointer"="all""less-precise-fpmad"="false" "no-infs-fp-math"="false""no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false""no-trapping-math"="false" "stack-protector-buffer-size"="8""target-cpu"="x86-64""target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87""unsafe-fp-math"="false" "use-soft-float"="false" }
