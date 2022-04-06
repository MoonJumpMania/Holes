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
        p->clock = cpu->counter++;
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

int getFirstBlock(CPU *cpu, int size)
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

int getBestBlock(CPU *cpu, int size)
{
    int start, min = 0x7FFFFFFF;
    int i, j;
    for (i = 0; i < CPU_MEM; ++i)
    {
        if (cpu->memory[i] == -1)
        {
            for (j = i+1; cpu->memory[j] == -1 && j < CPU_MEM; ++j);
            if (min > j-i && j-i >= size)
            {
                min = j-i;
                start = i;
                i = j;
            }
        }
    }
    if (min == 0x7FFFFFFF)
    {
        return -1;
    }
    else
    {
        return start;
    }
}

int getWorstBlock(CPU *cpu, int size)
{
    int start, max = 0;
    int i, j;
    for (i = 0; i < CPU_MEM; ++i)
    {
        if (cpu->memory[i] == -1)
        {
            for (j = i+1; cpu->memory[j] == -1 && j < CPU_MEM; ++j);
            if (max < j - i && j-i >= size)
            {
                max = j - i;
                start = i;
                i = j;
            }
        }
    }
    if (max == 0)
    {
        return -1;
    }
    else
    {
        return start;
    }
}

int getNextBlock(CPU *cpu, int size)
{
    int i, j;
    int passed = 0;
    for (i = cpu->next + 1; !passed; ++i) // Find valid spot
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

        if (i >= CPU_MEM-1)
        {
            i = -1;
        }
        else if (i == cpu->next)
        {
            passed = 1;
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
    for (int i = r->addr; i <= r->last; ++i)
    {
        cpu->memory[i] = -1;
    }
    return r;
}

void requeueItem(CPU *cpu, Process *p)
{
    p->rCount++;
    if (p->rCount >= REM_MAX)
    {
        //printf("Process %d freed at load %d\n", p->pNum, cpu->loads);
        free(p);
        return;
    }
    p->clock = cpu->counter++;
    insertItem(cpu->queue, (int)p->clock, p);
}

void freeCPU(CPU *cpu)
{
    freeHeap(cpu->current);
    freeHeap(cpu->queue);
    free(cpu->mode);
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
    cpu->tMemUsed += memUsage;
    cpu->tHoles += holes;
    cpu->tProcesses += processes;

    printf("pid loaded, #processes = %d, #holes = %d, "
           "%%memusage = %d, cumulative %%mem = %d\n",
           processes, holes, memUsage, cpu->tMemUsed / cpu->loads);
}

void executeProcesses(CPU *cpu)
{
    while (!isEmpty(cpu->queue))
    {
        Process *p = removeMin(cpu->queue); // Take out first process in queue
        int isAdded = 0, addr;

        cpu->loads++;
        while (!isAdded)
        {
            if (strcmp(cpu->mode, "first") == 0)
            {
                addr = getFirstBlock(cpu, p->memSize);
            }
            else if (strcmp(cpu->mode, "best") == 0)
            {
                addr = getBestBlock(cpu, p->memSize);
            }
            else if (strcmp(cpu->mode, "worst") == 0)
            {
                addr = getWorstBlock(cpu, p->memSize);
            }
            else if (strcmp(cpu->mode, "next") == 0)
            {
                addr = getNextBlock(cpu, p->memSize);
            }

            if (addr != -1)
            {
                p->addr = addr;
                p->last = addr + p->memSize-1;
                cpu->next = p->last;
                p->clock = cpu->counter++;
                insertItem(cpu->current, (int)p->clock, p);
                addToMem(cpu, p);
                isAdded = 1;
            }
            else
            {
                Process *r = removeProcess(cpu); // Remove one process if there is no space
                requeueItem(cpu, r);
            }
        }

        printMemoryInfo(cpu);
    }

    printf("Total loads = %d, average #processes = %.1f, "
           "average #holes = %.1f, cumulative %%mem = %d\n",
           cpu->loads, (float)cpu->tProcesses/(float)cpu->loads,
           (float)cpu->tHoles/(float)cpu->loads, cpu->tMemUsed / cpu->loads);
}
