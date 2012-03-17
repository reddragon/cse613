#!/bin/bash
cat agc.plt | ../clean_plt.sh > newagc.plt
gnuplot newagc.plt
