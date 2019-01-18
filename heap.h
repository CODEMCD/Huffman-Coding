#pragma once

#define TRUE	1
#define FALSE	0

#include "include.h"
#include "tree.h"

typedef Node* HData;

// d1의 우선순위가 높다면 0보다 큰 값
// d2의 우선순위가 높다면 0보다 작은 값
// d1과 d2의 우선순위가 같다면 0을 반환
typedef int PriorityComp(HData d1, HData d2);

typedef struct _heap
{
	PriorityComp* comp;
	int numOfData;
	int capacity;
	HData* heapArr;
} Heap;

/*** Heap 관련 연산들 ****/
void HeapInit(Heap* ph, int capacity, PriorityComp pc);
int HIsEmpty(Heap* ph);

void HInsert(Heap* ph, HData data);
HData HDelete(Heap* ph);
