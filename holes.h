#ifndef A3_HOLES_H
#define A3_HOLES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "heap.h"

#define CPU_MEM 1024 // Size of CPU memory
#define MAX_PRO_MEM 512 // Max size of a process
#define REM_MAX 3 // Max amount of times a process can be removed from queue

typedef struct process
{
    int pNum; // Process number
    int memSize; // Total amount of memory
    int addr; // Starting address of process memory
    int last; // Last MB of memory
    int rCount; // Amount of times process has been removed
    clock_t clock; // Time its placed
} Process;

typedef struct cPU
{
    int tMemUsed, loads, tProcesses, tHoles, counter; // Info about cpu
    int next; // Address for next fit
    Heap *current, *queue; // Two heaps for the ready and waiting queues
    int memory[CPU_MEM]; // Where the memory is stored
    char *mode; // First, best, worst, next fits
} CPU;

CPU *readFile(char **argv);
CPU *createCPU();
Process *createProcess(int num, int size);
void requeueItem(CPU *cpu, Process *p);
void freeCPU(CPU *cpu);
void executeProcesses(CPU *cpu);
int getFirstBlock(CPU *cpu, int size);
Process *removeProcess(CPU *cpu);
void addToMem(CPU *cpu, Process *p);
int getBestBlock(CPU *cpu, int size);
int getWorstBlock(CPU *cpu, int size);
int getNextBlock(CPU *cpu, int size);

#endif //A3_HOLES_H
