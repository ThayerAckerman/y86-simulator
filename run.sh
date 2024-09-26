#!/bin/bash

rm -f -r Outputs
mkdir Outputs
chmod g+rwx Outputs
dir="Tests"
tests=( addq addqfor1 addqfor2 addqfor3 addqfor4 addqfor5 addqfor6
        andq cmove cmovg cmovge cmovl cmovle cmovne
        error1 error2 error3 error4 error5 error6 error7 error8
        error9 error10 error11 error12 error13 error14 error15 error16
        except1 except2 except3 except4 except5 except6 except7
        irmovq loaduse1 loaduse2 memfor1 memfor2 mrmovq1 mrmovq2
        nop popq1 popq2 pushq1 pushq2 pushqpopq rmmovq rrmovq subq xorq
        absVal)

numTests=0
numPasses=0

for atest in ${tests[@]}
do
   infile="$dir/$atest.yo"
   studoutfile="$atest.dump"
   instoutfile="$dir/$atest.cdump"
   rm -f $studoutfile
   ./yess $infile > $studoutfile
   rm -f diffs
   diff $instoutfile $studoutfile > diffs
   if [ ! -e $studoutfile ] ||  [ ! -s $studoutfile ] || [ -s diffs ]; then
      cp $instoutfile Outputs/
      cp $infile Outputs/
      mv $studoutfile Outputs/
      echo "Testing $infile ... failed"
      cat diffs
      rm -f diffs
   else
      rm -f diffs
      rm -f $studoutfile
      echo "Testing $infile ... passed"
      numPasses=$(($numPasses+1))
   fi
   numTests=$(($numTests+1))
done
echo " "
echo "$numPasses passed out of $numTests tests."

if [ $numPasses -ne $numTests ]; then
   echo "Files saved in your Outputs directory."
   echo "Input files end with a .yo."
   echo "Your output files end with a .dump."
   echo "Correct output files end with a .cdump."
   chmod g+rw Outputs/*
else
   rm -f -r Outputs
fi 

