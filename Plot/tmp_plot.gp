set title 'Pomiary: pył zawieszony PM2.5'
set xlabel 'Czas'
set ylabel 'pył zawieszony PM2.5 (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:23]
set yrange [0:*]
set xtics ('14:00' 0, '18:00' 4, '22:00' 8, '02:00' 12, '06:00' 16, '10:00' 20)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
