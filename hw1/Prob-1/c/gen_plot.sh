#!/bin/bash
cat agc.plt | ../clean_plt.sh > tmp01.plt
mv tmp01.plt agc.plt
gnuplot agc.plt
