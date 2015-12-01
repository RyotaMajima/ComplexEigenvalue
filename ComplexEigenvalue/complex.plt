unset multiplot; reset

se xlab "Re \\{E\\}"
se ylab "Im \\{E\\}"

se pm3d

spl "./output/energy_complex.txt" us 1:2:3 ti "" wi pm3d
