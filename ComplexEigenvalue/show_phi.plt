unset multiplot; reset
set ter pdfcairo size 21cm,29.7cm font ",15"
set output "./graph/output_phi.pdf"
set multiplot layout 4,1

set xran [-2.5:0.7]
set yran [-0.02:0.02]
z_max = 2.5
set zran [0:z_max]; set cbra [0:z_max]
set xla "Re \\{E\\}"
set yla "Im \\{E\\}"

set label 1 \
 "----- calculation parameters -----\n \
  system size : [-5.0, 25.0]\n \
  partition number : N = 256\n \
  time step : dt = 0.1 (fixed)\n \
  energy part num (real) : N = 400\n \
  energy part num (imag) : N = 100\n \
  init func : 1st excitedstate\n \
   (S = 0.9, X = -0.5)\n \
  V_{opt}(x) = x*x" \
 front at first -2.5, 0.015 #tc rgb "white"

set label 2 "-1.02 - 0.0028i" front at first -1.2, 0.01 tc rgb "white"
set label 3 "-0.155 + 0.0008i" front at first -0.4, 0.01 tc rgb "white"
set label 4 "-0.165 - 0.0056i" front at first -0.4, -0.015 tc rgb "white"

set title "peak of complex energy eigenvalue (T = 50)"
pl "./output/energy_complex_T_50.txt" ti "" wi image

unset label

set xran [-5:5]; set yran [-2:2]
set grid
set xla "x"; set yla "|{/Symbol F}(x)|^{2}"
se sty fill trans solid 0.5 nobor

set title "{/Symbol e} = -1.02 - 0.0028i"
pl "./output/output_phi.txt" us 1:2 w l ti "V(x)" lc rgb "black" lw 2, \
 "" us 1:3 w filledcur ti "" lc rgb "red"

set title "{/Symbol e} = -0.155 + 0.0008i"
pl "./output/output_phi.txt" us 1:2 w l ti "V(x)" lc rgb "black" lw 2, \
 "" us 1:4 w filledcur ti "" lc rgb "blue"

set title "{/Symbol e} = -0.165 - 0.0056i"
pl "./output/output_phi.txt" us 1:2 w l ti "V(x)" lc rgb "black" lw 2, \
 "" us 1:5 w filledcur ti "" lc rgb "blue"

unset multiplot
set output
set ter pop
