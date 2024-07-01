#!/bin/bash

./lab4 > student.output
diff instructor.output student.output > diffs

if [ -s diffs ]; then
   echo ' '
   echo 'Some tests did not pass.'
   echo 'It should produce identical output to the file instructor.output.'
   echo 'To see your output, run: lab5'
   echo 'To see the diffs, run: diff instructor.output student.output'
   echo 'The lines from the instructor.output file begin with <'
   echo 'The lines from the student.output file begin with >'
   echo 'If there are no lines that begin with > then your program produced no output.'
   echo ' '
else
   echo ' '
   echo 'All tests passed.'
fi
rm diffs
