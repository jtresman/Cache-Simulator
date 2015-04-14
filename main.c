/*****************************************************
 * Cache Simulator
 *
 * This program takes program traces that include a 
 *  Operation, Memory Address and Byte Size. It 
 *  keeps track of L1 data and instruction cache, L2
 *  cache and main memory.
 *
 *  20 March 2015
 *
 *  Author: Jacob Resman and Healy Fuess
 ****************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "cache.h"

int main(int argc, char **argv){

    char op;
    unsigned long long int address;
    unsigned int bytesize;
    int ch;


    if (argc != 2){
        l1_cache_size = L1_DEFAULT_CACHE_SIZE;
        l2_cache_size = L2_DEFAULT_CACHE_SIZE; 
        l1_cache_block_size = L1_DEFAULT_CACHE_BLOCK_SIZE;
        l2_cache_block_size = L2_DEFAULT_CACHE_BLOCK_SIZE;
        l1_cache_assoc = L1_DEFAULT_CACHE_ASSOC;
        l2_cache_assoc = L2_DEFAULT_CACHE_ASSOC;

    } else {

        FILE *fp;
        fp = fopen(argv[1], "r");

        if(fp == 0){
            printf("Could not open file\n");
        }


        do{
            fscanf(fp,"%d %d %d %d\n",&l1_cache_size,&l1_cache_assoc,&l2_cache_size,&l2_cache_assoc);
        }
        while((ch = fgetc(fp))!= EOF);

        printf("%d %d %d %d\n",l1_cache_size,l1_cache_assoc,l2_cache_size,l2_cache_assoc);

        fclose(fp);
    }

    init_cache();


    while(scanf("%c %Lx %d\n",&op,&address,&bytesize) == 3){

        // printf("OP      : %c  ", op);
        // printf("Address : %Lx ", address);
        // printf("ByteSize: %d  \n", bytesize); 

        perform_access(address,bytesize,op);

    }

    print_stats();

    return 0;

}