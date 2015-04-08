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

    init_cache();

    while(scanf("%c %Lx %d\n",&op,&address,&bytesize) == 3){

        // printf("OP      : %c  ", op);
        // printf("Address : %Lx ", address);
        // printf("ByteSize: %d  \n", bytesize); 

        perform_access(address,bytesize,op);

    }

    return 0;

}