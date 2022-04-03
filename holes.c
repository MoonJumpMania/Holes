#include "holes.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Invalid arguments\n");
        return -1;
    }

    CPU *cpu = readFile(argv); // O(nlogn)

    executeProcesses(cpu);

    freeCPU(cpu);
    return 0;
}

CPU *readFile(char **argv)
{
    FILE *f = fopen(argv[1], "r");
    int pNum, memSize, order = 0;
    CPU *cpu = createCPU();

    strcpy(cpu->mode, argv[2]);

    while (fscanf(f, "%d %d\n", &pNum, &memSize) != EOF)
    {
        Process *p = createProcess(pNum, memSize);
        p->clock = pNum;
        insertItem(cpu->queue, (int)p->clock, p);
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
    int isAdded = 0, addr;
    while (!isAdded)
    {
        addr = getBlock(cpu, p->memSize);

        if (addr != -1)
        {
            p->addr = addr;
            p->last = addr + p->memSize-1;
            p->clock = clock();
            insertItem(cpu->current, (int)p->clock, p);
            addToMem(cpu, p);
            isAdded = 1;
        }
        else
        {
            Process *r = removeProcess(cpu); // Remove one process if there is no space
            reinsertItem(cpu, r);
        }
    }
}

int getBlock(CPU *cpu, int size)
{
    int i, j;
    for (i = 0; i+size-1 < CPU_MEM; ++i) // Find valid spot
    {
        if (cpu->memory[i] == -1)
        {
            for (j = i + 1; j < i+size; ++j)
            {
                if (cpu->memory[j] != -1)
                {
                    i = j;
                    break;
                }
            }
            if (j-i == size)
            {
                return i;
            }
        }
    }
    return -1;
}

void addToMem(CPU *cpu, Process *p)
{
    for (int i = p->addr; i < p->last+1; ++i)
    {
        cpu->memory[i] = p->pNum;
    }
}

Process *removeProcess(CPU *cpu)
{
    Process *r = removeMin(cpu->current);
    for (int i = r->addr; i < r->last+1; ++i)
    {
        cpu->memory[i] = -1;
    }
    return r;
}

void reinsertItem(CPU *cpu, Process *p)
{
    p->rCount++;
    if (p->rCount >= REM_MAX)
    {
        free(p);
        return;
    }
    p->clock = clock();
    insertItem(cpu->queue, (int)p->clock, p);
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
    int i, j;
    for (i = 0; i < CPU_MEM; ++i)
    {
        if (cpu->memory[i] != -1) // Occupied MB
        {
            memUsage++;
        }
        else
        {
            holes++;
            for (j = i+1; cpu->memory[j] == -1; ++j);
            i = j;
        }
    }
    memUsage = memUsage * 100 / CPU_MEM;
    cpu->memUsed += memUsage;
    cpu->tHoles += holes;
    cpu->tProcesses += processes;

    printf("pid loaded, #processes = %d, #holes = %d, "
           "%%memusage = %d, cumulative %%mem = %d\n",
           processes, holes, memUsage, cpu->memUsed/cpu->loads);
}

void executeProcesses(CPU *cpu)
{
    while (!isEmpty(cpu->queue))
    {
        Process *p = removeMin(cpu->queue); // Take out first process in queue
        cpu->loads++;

        if (strcmp(cpu->mode, "first") == 0)
        {
            firstFit(cpu, p);
        }
        else if (strcmp(cpu->mode, "best") == 0)
        {

        }
        else if (strcmp(cpu->mode, "worst") == 0)
        {

        }
        else if (strcmp(cpu->mode, "next") == 0)
        {

        }

        printMemoryInfo(cpu);
    }

    printf("Total loads = %d, average #processes = %.1f, "
           "average #holes = %.1f, cumulative %%mem = %d\n",
           cpu->loads, (float)cpu->tProcesses/(float)cpu->loads,
           (float)cpu->tHoles/(float)cpu->loads, cpu->memUsed/cpu->loads);
}
