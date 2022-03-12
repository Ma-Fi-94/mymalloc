make clean
make all


echo "merge, allocate_first, seed, nb_free_blocks, nb_all_blocks, fraction_free_blocks, sum_free_memory, sum_all_memory, occupation, avg_free_size\n"


for MERGE in 1 0
do
    for SEED in 1 2 3 4 5 6 7 8 9 10
    do
        for ALLOCATEFIRST in 0 1
        do
            ./performance_comparison.elf $MERGE $ALLOCATEFIRST $SEED
        done
    done
done

