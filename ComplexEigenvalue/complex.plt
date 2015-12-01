unset multiplot; reset

se xlab "Re \\{E\\}"
se ylab "Im \\{E\\}"

set pm3d
uns colorb

spl "./output/energy_complex.txt" us 1:2:3 ti "" wi pm3d
