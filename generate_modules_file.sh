#!/bin/bash

# Generates the `modules.gen.h` file
#
# INPUT: program arguments: the list of modules in order

for m in "$@"; do
    echo "void ${m}_init();"
    echo "int ${m}_update(char * const str, int n, const usecs_t usecs);"
done

echo "Module MODULES[] = {"
    for m in "$@"; do
        echo "{ ${m}_init, ${m}_update },"
    done
echo "};"

