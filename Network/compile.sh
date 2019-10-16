# Standard compilation command but with -lm to link the math.h library
if [ $# -eq 0 ] 
then
    gcc -Wall -Wextra -Werror -std=c99 -O1 -o a.out *.[co] -lm
else
    gcc -Wall -Wextra -Werror -std=c99 -O1 -o a.out *.[co] -lm $1
fi
