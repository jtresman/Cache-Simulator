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

//Global Value

//Cache Globals
int dSize; //Cache Suze
int dWay; //Type of Associative
int dBSize; //Block Size
int iSize;
int iWay;
int iBSize;
int l2Size;
int l2Way;
int l2BSize;
int memnRTime; //Memory Ready Time
int memCSize;  //Memory Chunk Size
int memCTime;  //Memory Chunk Time

//Simulation Globals
unsigned long long int executionTime;
unsigned long long int flushTime;
unsigned long long int instRefs;
unsigned long long int totalReads;
unsigned long long int totalsWrites;
unsigned long int l1iMisses;
unsigned long long int l1iHits;
unsigned long int l1dMisses;
unsigned long long int l1dHits;
//TODO Add the Rest of the Variables 


//Print the Final Report
void printOutput(){

    
}

int main(int argc, char **argv){

    
   char op;
   unsigned long long int address;
   unsigned int bytesize;
    
   while(scanf("%c %Lx %d\n,&op,&address,&bytesize) == 3){

       //Do stuff Here

   }
    
   printOutput();


   return 0;

}
