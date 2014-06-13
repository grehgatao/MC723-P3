#!/bin/bash

TESTS=(
"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 5\n
#define FORWARD 1\n
#define BRANCH 0\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 5\n
#define FORWARD 1\n
#define BRANCH 1\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 5\n
#define FORWARD 1\n
#define BRANCH 2\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 7\n
#define FORWARD 1\n
#define BRANCH 0\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 7\n
#define FORWARD 1\n
#define BRANCH 1\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 7\n
#define FORWARD 1\n
#define BRANCH 2\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 13\n
#define FORWARD 1\n
#define BRANCH 0\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 13\n
#define FORWARD 1\n
#define BRANCH 1\n
"

"
#define GEN_DIN 0\n
#define PIPELINE_LENGTH 13\n
#define FORWARD 1\n
#define BRANCH 2\n
"
)

j=0

for i in "${TESTS[@]}"
do
	echo -e $i > macros.h
	acsim mips1.ac -abi
	make -f Makefile.archc
        ./mips1.x --load=bin/basicmath_small 1> results/basicmath_small_result_$j.txt 2> results/basicmath_small_$j.txt
        echo -e $i >> results/basicmath_small_$j.txt
	./mips1.x --load=bin/qsort_small bin/qsort_input_small.dat 1> results/qsort_small_result_$j.txt 2> results/qsort_small_$j.txt
	echo -e $i >> results/qsort_small_$j.txt
	./mips1.x --load=bin/cjpeg -dct int -progressive -opt -outfile bin/output_small_encode.jpeg bin/input_small.ppm 1> results/cjpeg_small_result_$j.txt 2> results/cjpeg_small_$j.txt
	echo -e $i >> results/cjpeg_small_$j.txt
	./mips1.x --load=bin/sha bin/input_large.asc 1> results/sha_large_result_$j.txt 2> results/sha_large_$j.txt
	echo -e $i >> results/sha_large_$j.txt
	echo $j
	((j++))
done