unset multiplot; reset
T_END = 100
TN = T_END * 10
set xran [0:10]
set yran [-2:3]
set grid
set xla "x"
set yla "|{/Symbol y}(x,t)|^2"
plot for[i=0:TN:50] sprintf("./output/timeEvo/output%03d.txt", i) us 1:2 w l \
 ti sprintf("t = %.2f", T_END*i/TN),\
  "" us 1:3 tit "V(x)" w l lc rgb "blue" lw 2
