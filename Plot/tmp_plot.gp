set title 'Pomiary czujnika'
set xlabel 'Godzina'
set ylabel 'Wartosc (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:69]
set xtics ('00:00' 0, '14:00' 14, '04:00' 28, '19:00' 42, '09:00' 56)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
