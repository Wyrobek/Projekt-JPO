set title 'Pomiary czujnika'
set xlabel 'Godzina'
set ylabel 'Wartosc (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:46]
set xtics ('02:00' 0, '11:00' 9, '20:00' 18, '05:00' 27, '15:00' 36, '00:00' 45)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
