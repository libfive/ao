#!/bin/bash

rm studio.icns

inkscape --export-filename=icon512.png icon.svg
convert icon512.png -resize 256x256 icon256.png
convert icon512.png -resize 128x128 icon128.png
convert icon512.png -resize 32x32 icon32.png
convert icon512.png -resize 16x16 icon16.png

png2icns studio.icns icon512.png icon256.png icon128.png icon32.png icon16.png
rm icon512.png icon256.png icon128.png icon32.png icon16.png
