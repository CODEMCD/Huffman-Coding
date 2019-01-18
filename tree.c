#include "tree.h"

Node* makeNewNode(TData* data)
{
	Node* newNode = (Node*)malloc(sizeof(Node));

	newNode->data.ch = data->ch;
	newNode->data.freq = data->freq;
	newNode->data.realCharFlag = data->realCharFlag;
	newNode->left = NULL;
	newNode->right = NULL;

	return newNode;
}

void MakeLeftSubTree(Node* main, Node* sub)
{
	if (main->left != NULL)
		free(main->left);
	main->left = sub;
}

void MakeRightSubTree(Node * main, Node * sub)
{
	if (main->right != NULL)
		free(main->right);
	main->right = sub;
}

void PostorderTraverse(Node * bt, VisitFuncPtr action)
{
	if (bt == NULL)
		return;

	PostorderTraverse(bt->left, action);
	PostorderTraverse(bt->right, action);
	action(bt->data);
}

void DeleteTree(Node * main)
{
	if (main == NULL)
		return;

	DeleteTree(main->left);
	DeleteTree(main->right);

	//printf("del tree data: %d \n", main->data.freq);
	free(main);
}