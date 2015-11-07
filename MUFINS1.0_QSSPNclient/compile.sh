cd src/glpk
gcc -c *.c
cd ../
#g++ -c *.cpp
#g++ -o qsspn glpk/*.o *.o -lm
g++ -std=c++0x -c *.cpp -O2 -Iasio -Iasio/include
g++ -std=c++0x -o qsspn glpk/*.o *.o -lm -pthread -O2 -Iasio -Iasio/include

