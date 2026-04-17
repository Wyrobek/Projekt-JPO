set title 'Pomiary: tlenki azotu'
set xlabel 'Czas'
set ylabel 'tlenki azotu (ug/m3)'
set grid
set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8
set xtics rotate by -45
set xtics font ',9'
set key off
set xrange [0:23]
set yrange [0:*]
set xtics ('12:00' 0, '16:00' 4, '20:00' 8, '00:00' 12, '05:00' 16, '09:00' 20)
plot 'Plot/tmp_plot.dat' using 1:2 with linespoints ls 1
pause mouse close
