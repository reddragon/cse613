#!/bin/bash
while read line
do
    echo $line | sed s/0.0,0.0//g | sed s/#set/set/g
done
