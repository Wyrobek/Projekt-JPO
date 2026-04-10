set title 'Pomiary: tlenek azotu'
set xlabel 'Czas'
set ylabel 'tlenek azotu (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:23]
set yrange [0:*]
set xtics ('18:00' 0, '22:00' 4, '02:00' 8, '06:00' 12, '10:00' 16, '14:00' 20)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
