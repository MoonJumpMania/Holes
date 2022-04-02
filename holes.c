#include "holes.h"


int getAvailableSpace(CPU *cpu, int size);

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Invalid arguments\n");
        return -1;
    }

    CPU *cpu = readFile(argv); // O(nlogn)

    executeProcesses(cpu);

    printf("Total loads = %d, average #processes = %.1f, "
           "average #holes = %.1f, cumulative %%mem = %d\n",
           cpu->loads, (float)cpu->tProcesses/(float)cpu->loads,
           (float)cpu->tHoles/(float)cpu->loads, cpu->memUsed);

    freeCPU(cpu);
    return 0;
}

CPU *readFile(char **argv)
{
    FILE *f = fopen(argv[1], "r");
    int pNum, memSize;
    CPU *cpu = createCPU();

    strcpy(cpu->mode, argv[2]);

    while (fscanf(f, "%d %d\n", &pNum, &memSize) != EOF)
    {
        Process *p = createProcess(pNum, memSize);
        p->clock = clock();
        insertItem(cpu->queue, p->clock, p);
    }

    fclose(f);
    return cpu;
}

CPU *createCPU()
{
    CPU *cpu = calloc(1, sizeof(CPU));
    cpu->current = createHeap();
    cpu->queue = createHeap();
    cpu->mode = calloc(20, sizeof(char));
    for (int i = 0; i < CPU_MEM; ++i)
    {
        cpu->memory[i] = -1;
    }
    return cpu;
}

Process *createProcess(int pNum, int memSize)
{
    Process *p = calloc(1, sizeof(Process));
    p->pNum = pNum;
    p->memSize = memSize;
    return p;
}

void firstFit(CPU *cpu, Process *p)
{
    int set = 0; // Indicator to see if process can be added;
    int isFilled;

    p->clock = clock();
    insertItem(cpu->current, (int)p->clock, p);
    
    set = getAvailableSpace(cpu, p->memSize);

    while (!isFilled)
    {
        Process *r = getFirst(cpu); // Firstly added process
        reinsertNode(cpu, r); // Reinsert into priority queue
        int i;
        for (i = r->addr; i <= r->last; ++i)
        {
            cpu->memory[i] = -1; // Reset allocation in memory
        }
        for (i; cpu->memory[i] == -1 && i >= 0; --i); // Get first empty space in block
        printf("N%d\n", cpu->memory[i]);
    }
}

int getAvailableSpace(CPU *cpu, int size)
{
    int start, end;
    for (int i = 0; i < CPU_MEM; ++i)
    {
        if (cpu->memory[i] == -1)
        {
            start
        }
    }
}

void reinsertNode(CPU *cpu, Process *p)
{
    p->rCount++;
    if (p->rCount >= REM_MAX)
    {
        free(p);
        return;
    }
    p->clock = clock();
    insertItem(cpu->current, (int)p->clock, p);
}

Process *getFirst(CPU *cpu) // Gets the first added node in heap
{
    Node *n = removeMin(cpu->current);
    Process *p = n->e;
    free(n);
    return p;
}

void freeCPU(CPU *cpu)
{
    freeHeap(cpu->current);
    freeHeap(cpu->queue);
    free(cpu);
}

int printMemoryInfo(CPU *cpu) // Memory usage, processes, holes, etc...
{
    int holes = 0, processes = cpu->current->size, memUsage = 0;

    for (int i = 0; i < CPU_MEM; ++i)
    {
        if (cpu->memory[i] != -1) // Occupied MB
        {
            memUsage++;
        }
    }
    memUsage *= 100/CPU_MEM;
    cpu->memUsed += memUsage;
    cpu->tHoles += holes;
    cpu->tProcesses += processes;

    printf("pid loaded, #processes = %d, #holes = %d, "
           "%%memusage = %d, cumulative %%mem = %d\n",
           processes, holes, memUsage, 100 * cpu->memUsed/cpu->loads);
}

void executeProcesses(CPU *cpu)
{
    while (!isEmpty(cpu->queue))
    {
        Process *p = removeMin(cpu->queue);
        cpu->loads++;
        if (strcmp(cpu->mode, "first") == 0)
        {
            firstFit(cpu, p);
        }

        printMemoryInfo(cpu);
    }
}
