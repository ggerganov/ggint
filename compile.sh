#!/bin/bash

cur="ggint"
echo "Compiling ${cur} ... "
g++ -std=c++11 -O3 -I. examples/${cur}.cpp -o ${cur}

cur="find_prime"
echo "Compiling ${cur} ... "
g++ -std=c++11 -O3 -I. examples/${cur}.cpp -o ${cur}

cur="find_safe_prime"
echo "Compiling ${cur} ... "
g++ -std=c++11 -O3 -I. examples/${cur}.cpp -o ${cur}
