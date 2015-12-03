unset multiplot; reset

set xran[-1.4:0]
set yra[-0.02:0.02]
se xlab "Re \\{E\\}"
se ylab "Im \\{E\\}"
uns colorb


spl "./output/energy_complex_T_300.txt" us 1:2:3 ti "" wi pm3d
#pl "./output/energy_complex.txt" ti "" wi image
#spl "./output/energy_complex.txt" ti "" w d
