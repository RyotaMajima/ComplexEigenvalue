unset multiplot; reset
set ter pdfcairo size 21cm,29.7cm font ",15"
set output "./graph/output.pdf"
set multiplot layout 4,1

set xran [-2.0:0.0]
set yran [-0.02:0.02]
z_max = 1.8
set zran [0:z_max]; set cbra [0:z_max]
set xla "Re \\{E\\}"
set yla "Im \\{E\\}"

set label 1 \
 "----- calculation parameters -----\n \
  system size : [-5.0, 25.0]\n \
  partition number : N = 512\n \
  time step : dt = 0.5 (fixed)\n \
  energy part num (real) : N = 200\n \
  energy part num (imag) : N = 400\n \
  init func : 1st excitedstate\n \
   (S = 0.9, X = -0.5)\n \
  V_{opt}(x) = x*x" \
 front at first -2.0, 0.015

set label 2 "-1.014 - 0.0112i" front at first -1.15, 0.0 tc rgb "white"
set label 3 "-0.150 - 0.009i" front at first -0.4, 0.0 tc rgb "white"
set label 4 "-0.162 - 0.0126i" front at first -0.4, -0.017 tc rgb "white"

set title "peak of complex energy eigenvalue (T = 50)"
pl "./output/energy_complex_T_50.txt" ti "" wi image

uns label
set label 1 "-1.014 - 0.0111i" front at first -1.15, 0.0 tc rgb "white"
set label 2 "-0.150 - 0.0089i" front at first -0.4, 0.0 tc rgb "white"
set label 3 "-0.156 - 0.0126i" front at first -0.4, -0.017 tc rgb "white"

set title "peak of complex energy eigenvalue (T = 100)"
pl "./output/energy_complex_T_100.txt" ti "" wi image

uns label
set label 1 "-1.014 - 0.011i" front at first -1.15, 0.0 tc rgb "white"
set label 2 "-0.156 - 0.0088" front at first -0.4, 0.0 tc rgb "white"
#set label 3 "-0.155 - 0.0127i" front at first -0.4, -0.017 tc rgb "white"

set title "peak of complex energy eigenvalue (T = 200)"
pl "./output/energy_complex_T_200.txt" ti "" wi image

unset label
set label 1 "-1.014 - 0.011i" front at first -1.15, 0.0 tc rgb "white"
#set label 2 "-0.156 - 0.086i" front at first -0.4, 0.0 tc rgb "white"

set title "peak of complex energy eigenvalue (T = 400)"
pl "./output/energy_complex_T_400.txt" ti "" wi image

unset multiplot
set output
set ter pop
