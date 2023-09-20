#!/bin/sh

# Rafael Corsi
# corsiferrao@gmail.com
#
# Script to convert makefile generate from atmel studio 7 (windows)
#  to linux path, assuming that you have gcc on PATH
# 
# on Arch 
#  - sudo pacman -S arm-none-eabi-gcc  
#  - sudo pacman -S arm-none-eabi-newlib          
#
# based on https://gist.github.com/theterg/6082389 
# GPL

FILEMAKE=$1

if [ "$FILEMAKE" != "" ]; then

  # remove ref to gcc windows path
  sed -i 's/C:\\.*\\bin\\//g' $FILEMAKE

  # remove ref to extension .bin
  sed -i 's/.exe//g' $FILEMAKE

  # remove SHELL
  sed -i 's/SHELL := cmd//g' $FILEMAKE

else

  echo "Usage: studio7_linux_conversion.sh MAKE FILE"

fi

