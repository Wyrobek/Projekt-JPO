set title 'Pomiary: pył zawieszony PM10'
set xlabel 'Czas'
set ylabel 'pył zawieszony PM10 (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:47]
set yrange [0:*]
set xtics ('05.04' 0, '06.04' 6, '06.04' 12, '06.04' 18, '06.04' 24, '07.04' 30, '07.04' 36, '07.04' 42)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
