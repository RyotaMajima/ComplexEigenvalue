unset multiplot; reset
set ter pdfcairo size 21cm,29.7cm font ",14"
set output "./graph/output.pdf"
set multiplot layout 5,1

set xran [-1.4:0]
set yran [-0.02:0.02]
#set zran [0:6]; set cbra [0:6]
set xla "Re \\{E\\}"
set yla "Im \\{E\\}"

set label 1 "-1.019 - 0.0032i" front at first -1.2, 0.01 textco rgb "white"
set label 2 "-0.1505 + 0.0016i" front at first -0.4, 0.01 textco rgb "white"
set label 3 "-0.1645 - 0.006i" front at first -0.4, -0.015 textc rgb "white"

set title "peak of complex energy eigenvalue (T = 50)"
pl "./output/energy_complex_T_50.txt" ti "" wi image

set label 1 "-1.019 - 0.0032i" front at first -1.2, 0.01 textco rgb "white"
set label 2 "-0.154 + 0.0016i" front at first -0.4, 0.01 textco rgb "white"
set label 3 "-0.161 - 0.006i" front at first -0.4, -0.015 textc rgb "white"

set title "peak of complex energy eigenvalue (T = 75)"
pl "./output/energy_complex_T_75.txt" ti "" wi image

set label 1 "-1.019 - 0.0036i" front at first -1.2, 0.01 textco rgb "white"
set label 2 "-0.1575 + 0.0016i" front at first -0.4, 0.01 textco rgb "white"
set label 3 "-0.1575 - 0.006i" front at first -0.4, -0.015 textc rgb "white"

set title "peak of complex energy eigenvalue (T = 100)"
pl "./output/energy_complex_T_100.txt" ti "" wi image

unset label
set label 1 "-1.019 - 0.004i" front at first -1.2, 0.01 textco rgb "white"
set label 3 "-0.154 - 0.0068i" front at first -0.4, -0.015 textc rgb "white"

set title "peak of complex energy eigenvalue (T = 200)"
pl "./output/energy_complex_T_200.txt" ti "" wi image

unset label

set title "peak of complex energy eigenvalue (T = 300)"
pl "./output/energy_complex_T_300.txt" ti "" wi image

unset multiplot
set output
set ter pop
