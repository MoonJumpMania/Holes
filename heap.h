//
// Created by Nasif on 2022-02-24.
//

#ifndef A2_HEAP_H
#define A2_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//
// Structs for the Heap ADT
//

#define HEAP_SIZE 50

typedef struct node
{
    int k;
    void* e;
} Node;

typedef struct heap
{
    Node** array;
    int size;
} Heap;

//
// Heap ADT Functions
//

int isEmpty(Heap* h);
void insertItem(Heap* h, int k, void* e);
void* minElement(Heap* h);
int minKey(Heap* h);
void* removeMin(Heap* h);
int size(Heap* h);

//
// Helper functions
//

Heap* createHeap();
Node** createArray();
Node* createNode(int k, void* e);
void freeHeap(Heap* h);
void freePtr(void* ptr);
void sortNode(Heap* h, int i);
void moveDownNode(Heap* h, int i);
void swapNodes(Heap* h, int i, int j);
void printArray(Heap* h);
void printHeap(Heap* h);

//
// Navigation functions
//

int parent(int i);
int leftChild(int i);
int rightChild(int i);

#endif //A2_HEAP_H
