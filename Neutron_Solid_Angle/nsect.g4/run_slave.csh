#!/bin/tcsh
#
#$ -S /bin/tcsh -cwd
#$ -o Slave.out -j y
#$ -t 1-1200


#cd /home/dailabs/zh27/nsect.g4

source /home/dailabs/hariwood/Geant4.9.5/bin/geant4.csh

./nsect -H dailabs-n06;
