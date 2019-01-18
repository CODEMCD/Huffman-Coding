#pragma once

#include "include.h"
#include "heap.h"
#include "tree.h"
#include "file.h"

//허프만 테이블
typedef struct __huffmanTable {
	char ch;       //문자
	short code;    //허프만 코드(코드 길이가 최대 16비트이므로 shor 사용)
	int codeSize;  //허프만 코드 길이
} huffmanTable;

//인코딩 허프만 테이블
huffmanTable hTable[ASCII_SIZE];
//디코딩 허프만 테이블
huffmanTable dhTable[ASCII_SIZE];
//문자 빈도수
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