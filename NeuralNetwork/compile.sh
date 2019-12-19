# Standard compilation command but with -lm to link the math.h library
if [ $# -eq 0 ] 
then
    gcc -Wall -Wextra -Werror -std=c99 -O3 -o a.out *.[co] -lm -flto
else
    gcc -Wall -Wextra -Werror -std=c99 -O3 -o a.out *.[co] -lm -flto $1
fi
