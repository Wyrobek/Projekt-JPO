set title 'Pomiary: dwutlenek azotu'
set xlabel 'Czas'
set ylabel 'dwutlenek azotu (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:47]
set yrange [0:*]
set xtics ('08.04' 0, '08.04' 6, '09.04' 12, '09.04' 18, '09.04' 24, '10.04' 30, '10.04' 36, '10.04' 42)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
