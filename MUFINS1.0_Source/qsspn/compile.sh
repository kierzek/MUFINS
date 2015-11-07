cd src/glpk
gcc -c *.c
cd ../
g++ -c *.cpp
g++ -o qsspn glpk/*.o *.o -lm
