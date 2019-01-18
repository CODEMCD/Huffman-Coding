#pragma once

#define TRUE	1
#define FALSE	0

#include "include.h"
#include "tree.h"

typedef Node* HData;

// d1�� �켱������ ���ٸ� 0���� ū ��
// d2�� �켱������ ���ٸ� 0���� ���� ��
// d1�� d2�� �켱������ ���ٸ� 0�� ��ȯ
typedef int PriorityComp(HData d1, HData d2);

typedef struct _heap
{
	PriorityComp* comp;
	int numOfData;
	int capacity;
	HData* heapArr;
} Heap;

/*** Heap ���� ����� ****/
void HeapInit(Heap* ph, int capacity, PriorityComp pc);
int HIsEmpty(Heap* ph);

void HInsert(Heap* ph, HData data);
HData HDelete(Heap* ph);
