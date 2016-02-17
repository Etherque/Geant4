#!/bin/tcsh
#
#$ -S /bin/tcsh -cwd
#$ -o Master.out -j y
#$ -l highprio

#cd /home/dailabs/zh27/nsect.g4

source /home/dailabs/hariwood/Geant4.9.5/bin/geant4.csh
./nsect -p -n 1e5 -v

wait;
