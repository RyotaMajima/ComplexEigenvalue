unset multiplot; reset
set multiplot layout 5,1

set label 1 "-1.019 - 0.0032i" front at first -1.2, 0.015 textco rgb "white" font ",10"
#set label 2 "$E_{1} = -0.1575$" at fir E1-0.1, 0.45 font ",14"
#set label 3 "$E_{2} = 0.5325$" at fir E2-0.1, 0.45 font ",14"
#set arrow 1 from first E0,0.4 to E0,0.2 lw 2
#set arrow 2 from first E1,0.4 to E1,0.2 lw 2
#set arrow 3 from first E2,0.4 to E2,0.05 lw 2

set xran [-1.4:0]
set yran [-0.02:0.02]
set zran [0:6]
set cbra [0:6]
set xla "Re \\{E\\}"
set yla "Im \\{E\\}"

set title "peak of complex energy eigenvalue (T = 50)"
pl "./output/energy_complex_T_50.txt" ti "" wi image

set title "peak of complex energy eigenvalue (T = 75)"
pl "./output/energy_complex_T_75.txt" ti "" wi image

set title "peak of complex energy eigenvalue (T = 100)"
pl "./output/energy_complex_T_100.txt" ti "" wi image

set title "peak of complex energy eigenvalue (T = 200)"
pl "./output/energy_complex_T_200.txt" ti "" wi image

set title "peak of complex energy eigenvalue (T = 300)"
pl "./output/energy_complex_T_300.txt" ti "" wi image

unset multiplot
