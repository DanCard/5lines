# 5lines
Log output to a file and limit output of a command to 5 lines<br>
Options:<br>
 -n \<tail number of lines to output\>
 (optional: log file name)<br>
If log file name is not specified then the first word in output is used for log file name.

Example usage:
echo -e "bob\nmike\ndave\nc\n a \n b\n c \n 1\n 2 \n oompa\n loompa" | 5lines -n 23

To compile:
gcc -o 5lines 5lines.c -lncurses
