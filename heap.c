//
// Created by Nasif Mauthoor on 2022-02-24.
// Student ID: 1083611
// Source: https://d-michail.github.io/assets/teaching/data-structures/033_BinaryHeapImplementation.en.pdf
// Data Structures Binary Heap Implementation in C
// Author: Dimitrios Michail
// Modifications: Adapted the code to support a heap of nodes instead of a heap of integers.
//


#include "heap.h"

Heap* createHeap() // Allocate memory for a heap
{
    Heap* h = calloc(1, sizeof(Heap));
    h->array = createArray();
    h->size = 0;
    return h;
}


void insertItem(Heap* h, int k, void* e) // Insert new item sorted into heap
{
    Node* n = createNode(k, e);
    h->size++;
    h->array[h->size] = n;
    sortNode(h, size(h));
}


int size(Heap* h) // Returns size of heap
{
    return h->size;
}


void* minElement(Heap* h) // Returns the element of the smallest key node
{
    if (h->array[1] == NULL)
        return NULL;
    return h->array[1]->e;
}


int minKey(Heap* h) // Returns the smallest key
{
    if (h->array[1] == NULL)
        return -1;
    return h->array[1]->k;
}


void* removeMin(Heap* h)
{
    if (isEmpty(h))
    {
        return NULL;
    }
    void* ret = h->array[1]->e;
    swapNodes(h, 1, size(h));
    free(h->array[size(h)]);
    h->array[size(h)] = NULL;
    h->size--;
    moveDownNode(h, 1);
    return ret;
}


int isEmpty(Heap* h)
{
    return h->size == 0;
}


// Helper functions


Node** createArray()
{
    Node** array = calloc(HEAP_SIZE, sizeof(Node*));
    return array;
}


Node* createNode(int k, void* e)
{
    Node* n = malloc(sizeof(Node));
    n->k = k;
    n->e = e;
    return n;
}


void swapNodes(Heap* h, int i, int j) // Swap the values of two nodes
{
    int k = h->array[i]->k;
    void* e = h->array[i]->e;
    h->array[i]->k = h->array[j]->k;
    h->array[i]->e = h->array[j]->e;
    h->array[j]->k = k;
    h->array[j]->e = e;
}


void sortNode(Heap* h, int i)
{
    while (i > 1 && h->array[i]->k < h->array[parent(i)]->k)
    {
        swapNodes(h, parent(i), i);
        i = parent(i);
    }
}


void moveDownNode(Heap* h, int i)
{
    while(leftChild(i) <= size(h))
    {
        int lc = leftChild(i);
        if (lc < size(h) && h->array[rightChild(i)]->k < h->array[lc]->k)
        {
            lc++;
        }
        if (h->array[i]->k <= h->array[lc]->k)
        {
            break;
        }
        swapNodes(h, i, lc);
        i = lc;
    }
}


void freePtr(void* ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}


void freeHeap(Heap* h)
{
    Node** array = h->array;
    for (int i = 1; i < size(h); ++i)
    {
        freePtr(array[i]->e);
        freePtr(array[i]);
    }
    free(h->array);
    free(h);
}


void printHeap(Heap* h)
{
    int line = 1;
    for (int i = 1; i <= size(h); ++i)
    {
        if (line <= i)
        {
            printf("\n");
            line *= 2;
        }

        for (int j = 0; j < size(h) - line; ++j)
        {
            printf(" ");
        }
        printf("%d ", h->array[i]->k);
    }
    printf("\n");
}


// Navigation through heap
int parent(int i) { return i / 2; }
int leftChild(int i) { return 2 * i; }
int rightChild(int i) { return 2 * i + 1; }
