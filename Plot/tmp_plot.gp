set title 'Pomiary czujnika'
set xlabel 'Godzina'
set ylabel 'Wartosc (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set style line 2 lc rgb '#FF0000' lw 2
set xtics rotate by -45
set xtics font ',9'
set key box top left
set xrange [0:66]
set xtics ('02:00' 0, '15:00' 13, '04:00' 26, '17:00' 39, '06:00' 52, '20:00' 65)
f(x) = a * x + b
set fit quiet
fit f(x) 'Plot/tmp_plot.dat' using 1:2 via a, b
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1 title 'Dane z czujnika', \
     f(x) with lines ls 2 title 'Regresja liniowa'
pause mouse close
