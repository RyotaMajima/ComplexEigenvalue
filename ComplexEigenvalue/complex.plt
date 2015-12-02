unset multiplot; reset

se xlab "Re \\{E\\}"
se ylab "Im \\{E\\}"
uns colorb


pl "./output/energy_complex.txt" us 1:2:3 ti "" wi image
#spl "./output/energy_complex.txt" ti "" w d
