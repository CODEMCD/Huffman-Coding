#pragma once

#include "include.h"
#include "heap.h"
#include "tree.h"
#include "file.h"

//������ ���̺�
typedef struct __huffmanTable {
	char ch;       //����
	short code;    //������ �ڵ�(�ڵ� ���̰� �ִ� 16��Ʈ�̹Ƿ� shor ���)
	int codeSize;  //������ �ڵ� ����
} huffmanTable;

//���ڵ� ������ ���̺�
huffmanTable hTable[ASCII_SIZE];
//���ڵ� ������ ���̺�
huffmanTable dhTable[ASCII_SIZE];
//���� �󵵼�
int frequency[ASCII_SIZE];

int DataPriorityComp(HData data1, HData data2);
void calculateCharacterFrequency(char *content, int size);
void makeHuffmanCode(Node* huffmanTree, short code, int codeSize, int totalCharSize);
int makeHuffmanTable();
Node* makeHuffmanTree(int numOfCharacter);
int encoding(char* content, char* result, int numOfCharacter, int fileSize);
int matched(short code, int codeSize, short chCode, int chCodeSize);
int decoding(char* content, char* result, int fileSize);
int huffmanEncodingProcess(char* fileName, char* fileContents, int fileSize, char* infoFlag);
int huffmanDecodingProcess(char* fileName, char* fileContents, int fileSize, char* infoFlag);