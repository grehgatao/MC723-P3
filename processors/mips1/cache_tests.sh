#!/bin/bash

CACHE_SIZES=(
"256" "1K" "256K" "4M"
)

TESTS=(
"basicmath_small" "jpeg_small" "qsort_small" "sha_large"
)

rm -R results
mkdir results
for j in "${TESTS[@]}"
do
	echo "" > results/$j.txt
done

for i in ${CACHE_SIZES[@]}
do
	for ub_size in 4 16 32 64 128
	do
		for u_assoc in 1 2 4 8 16
		do
			for j in "${TESTS[@]}"
			do
				echo -e "Cache Size: $i\nBlock Size: $ub_size\nAssociativity: $u_assoc\n\n" >> results/$j.txt
				./dineroIV -l1-usize $i -l1-ubsize $ub_size -l1-uassoc $u_assoc -informat d < $j.din >> results/$j.txt
				echo -e "\n\n" >> results/$j.txt
			done
		done
	done
done
