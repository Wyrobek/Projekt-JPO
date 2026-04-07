set title 'Pomiary: tlenek azotu'
set xlabel 'Czas'
set ylabel 'tlenek azotu (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:67]
set yrange [0:*]
set xtics ('04.04' 0, '04.04' 6, '04.04' 12, '04.04' 18, '05.04' 24, '05.04' 30, '05.04' 36, '05.04' 42, '06.04' 48, '06.04' 54, '06.04' 60, '06.04' 66)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
