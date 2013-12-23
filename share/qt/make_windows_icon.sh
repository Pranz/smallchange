#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/toakrona.png
ICON_DST=../../src/qt/res/icons/toakrona.ico
convert ${ICON_SRC} -resize 16x16 toakrona-16.png
convert ${ICON_SRC} -resize 32x32 toakrona-32.png
convert ${ICON_SRC} -resize 48x48 toakrona-48.png
convert toakrona-16.png toakrona-32.png toakrona-48.png ${ICON_DST}

