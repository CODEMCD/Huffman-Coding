#pragma once

#include "include.h"

typedef struct __treeDate {
	char ch;
	int freq;
	int realCharFlag;
} TData;

typedef struct __treeNode {
	TData data;
	struct __treeNode *left;
	struct __treeNode *right;
} Node;

Node* makeNewNode(TData *data);

void MakeLeftSubTree(Node * main, Node * sub);
void MakeRightSubTree(Node * main, Node * sub);

typedef void VisitFuncPtr(TData data);

void PostorderTraverse(Node * bt, VisitFuncPtr action);

void DeleteTree(Node * main);
