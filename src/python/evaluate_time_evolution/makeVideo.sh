#!/bin/bash

j=0
for i in 'leaf'*.pdf; do
j=$((j + 1))
out='resized-'$(printf "%06d" $j)'.png'
pdftoppm -png -r 300 "$i" tmpframe
convert tmpframe-1.png -density 300 -trim -resize 1024x1024 \
-background White -gravity center \
-quality 40 -strip -interlace JPEG \
-extent 1024x1024 $out
done
ffmpeg -framerate 10 -start_number 1 -i resized-%06d.png \
-c:v libx265 -r 10 -pix_fmt yuv420p S1_Video.mp4
rm resized-*.jpg
