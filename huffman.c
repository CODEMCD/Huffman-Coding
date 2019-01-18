#include "huffman.h"

//허프만 트리를 만들기 위한 최소 힙 비교 함수(문자 빈도수 오름차순)
int DataPriorityComp(HData data1, HData data2)
{
	return data2->data.freq - data1->data.freq;
}

//문자 빈도수 계산
void calculateCharacterFrequency(char *content, int size)
{
	for (int i = 0; i < size; ++i)
		frequency[content[i]]++;
}

//허프만 트리를 이용하여 허프만 코드 만들기(왼쪽 - 0, 오른쪽 - 1)
void makeHuffmanCode(Node* huffmanTree, short code, int codeSize, int totalCharSize)
{
	// 리프 노드인 경우는 문자이다.
	if (huffmanTree->left == NULL && huffmanTree->right == NULL)
	{
		for (int i = 0; i < totalCharSize; ++i)
		{
			if (hTable[i].ch == huffmanTree->data.ch)
			{
				if (codeSize == 0) {
					hTable[i].code = (code | 1) << 1;
					hTable[i].codeSize = 2;
				}
				else {
					hTable[i].code = code >> 1;
					hTable[i].codeSize = codeSize;
				}
				break;
			}
		}
		return;
	}

	if (huffmanTree->left != NULL)
		makeHuffmanCode(huffmanTree->left, code << 1, codeSize + 1, totalCharSize);
	if (huffmanTree->right != NULL)
		makeHuffmanCode(huffmanTree->right, (code | 1) << 1, codeSize + 1, totalCharSize);
 }

//허프만 테이블을 만든다.
int makeHuffmanTable()
{
	int numOfCharacter = 0;
	for (int i = 0; i < ASCII_SIZE; ++i)
		if (frequency[i] != 0)
		{
			hTable[numOfCharacter].ch = i;
			hTable[numOfCharacter].code = 0;
			hTable[numOfCharacter].codeSize = 0;
			numOfCharacter++;
		}
	return numOfCharacter;
}

//허프만 트리를 만든다.
Node* makeHuffmanTree(int numOfCharacter)
{
	// 힙
	Heap heap;
	//힙 초기화
	HeapInit(&heap, numOfCharacter, DataPriorityComp);

	//각 문자 노드를 만든다.(트리 노드)
	for (int i = 0; i < numOfCharacter; ++i)
	{
		int idx = hTable[i].ch;
		TData newData;
		newData.ch = idx;
		newData.freq = frequency[idx];
		newData.realCharFlag = 1;
		Node* newNode = makeNewNode(&newData);
		HInsert(&heap, newNode);
	}

	//허프만 트리 만들기
	while (heap.numOfData > 1) {
		//최소 힙에서 노드 2개를 꺼낸다.(빈도수가 가장 작은 2개)
		Node* minNode1 = HDelete(&heap);
		Node* minNode2 = HDelete(&heap);
		//두 노드를 합칠 임의의 노드를 만든다.
		TData tmp;
		tmp.ch = 'F';
		tmp.freq = minNode1->data.freq + minNode2->data.freq;
		tmp.realCharFlag = 0;
		Node* tmpNode = makeNewNode(&tmp);
		//임의의 노드 오른쪽과 왼쪽에 최소 힙에서 꺼낸 두 노드를 붙힌다.
		MakeLeftSubTree(tmpNode, minNode1);
		MakeRightSubTree(tmpNode, minNode2);
		//완성된 임의의 노드를 다시 최소 힙에 삽입한다.
		HInsert(&heap, tmpNode);
	}

	//허프만 트리 루트를 반환한다.
	Node* main = HDelete(&heap);
	return main;
}

//허프만 인코딩
int encoding(char* content, char* result, int numOfCharacter, int fileSize)
{
	/* 헤더 만들기 */
	//현재 배열의 위치
	int curSize = 0;
	// 전체 문자 수(8bit)
	for (int i = 7; i >= 0; --i) {
		result[curSize] <<= 1;
		result[curSize] += (numOfCharacter >> i) & 1;
	}
	curSize++;

	//문자 정보를 저장한다.
	int curBitPos = 0;
	for (int i = 0; i < numOfCharacter; ++i) {
		// 현재 문자(8bit, 아스키코드 번호)
		for (int j = 7; j >= 0; --j) {
			result[curSize] <<= 1;
			result[curSize] += (hTable[i].ch >> j) & 1;

			curBitPos++;
			if (curBitPos == 8) {
				curBitPos = 0;
				curSize++;
				result[curSize] = 0;
			}
		}

		// 현재 문자 허프만 코드 길이(5bit)
		for (int j = 4; j >= 0; --j) {
			result[curSize] <<= 1;
			result[curSize] += (hTable[i].codeSize >> j) & 1;

			curBitPos++;
			if (curBitPos == 8) {
				curBitPos = 0;
				curSize++;
				result[curSize] = 0;
			}
		}

		// 현재 문자 허프만 코드(최대 길이 32)
		for (int j = hTable[i].codeSize - 1; j >= 0; --j) {
			result[curSize] <<= 1;
			result[curSize] += (hTable[i].code >> j) & 1;

			curBitPos++;
			if (curBitPos == 8) {
				curBitPos = 0;
				curSize++;
				result[curSize] = 0;
			}
		}
	}

	/* 본문 내용 */
	for (int i = 0; i < fileSize; ++i) {
		short curChCode;
		int curChCodeSize;
		for (int j = 0; j < numOfCharacter; ++j)
			if (hTable[j].ch == content[i]) {
				curChCode = hTable[j].code;
				curChCodeSize = hTable[j].codeSize;
				break;
			}

		for (int j = curChCodeSize - 1; j >= 0; --j) {
			result[curSize] <<= 1;
			result[curSize] += (curChCode >> j) & 1;

			curBitPos++;
			if (curBitPos == 8) {
				curBitPos = 0;
				curSize++;
				result[curSize] = 0;
			}
		}
	}


	/*
	마지막 잉여 비트를 처리하지 못해 최대 7bit 만큼 쓰레기값이 디코딩될 때
	들어갈 수 있다.(이부분은 아직 처리하지 못함, 인코딩 부분에서만 처리하면
	디코딩에서는 무관하게 수행된다.)
	*/
	// 잉여 비트가 있다면 정확한 비트 계산을 위해 왼쪽으로 모두 옮겨준다.
	if (curBitPos > 0) {
		//printf("%d\n", curBitPos);
		for (int i = 0; i < 8 - curBitPos; ++i)
			result[curSize] <<= 1;
	}

	return curSize;
	/*
	//비트 출력해보기
	for (int i = 0; i < curSize + 1; ++i) {
		for (int j = 7; j >= 0; j--)
		{
			if (result[i] & (1 << j))
			{
				putchar('1');
			}
			else
			{
				putchar('0');
			}
		}
	}
	printf("\n");*/
}

//허프만 인코딩 과정
int huffmanEncodingProcess(char* fileName, char* fileContents, int fileSize, char* infoFlag)
{
	calculateCharacterFrequency(fileContents, fileSize);

	int numOfCharacter = makeHuffmanTable();
	Node* huffmanTree = makeHuffmanTree(numOfCharacter);
	makeHuffmanCode(huffmanTree, 0, 0, numOfCharacter);

	// 여유비트 설정: 320은 헤더의 최대 바이트 크기(최대 헤더 크기는 대략 2560 bit)
	char* result = (char*)malloc(sizeof(char) * (fileSize + 320));
	memset(result, 0, fileSize + 320);
	int realSize = encoding(fileContents, result, numOfCharacter, fileSize);

	//허프만 인코딩 정보를 출력한다.
	fileName[strlen(fileName) - 4] = 0;
	if (strncmp(infoFlag, "y", 1) == 0 || strncmp(infoFlag, "Y", 1) == 0 ||
		strncmp(infoFlag, "yes", 3) == 0 || strncmp(infoFlag, "YES", 3) == 0)
		writeHuffmanEncodingInfo(fileName, numOfCharacter, fileSize, realSize);

	//허프만 인코딩된 내용을 파일에 저장한다.
	FILE* fp;
	errno_t err;
	char enFileName[FILE_NAME_SIZE];
	memset(enFileName, 0, FILE_NAME_SIZE);
	//인코딩 결과 파일 이름 설정
	sprintf_s(enFileName, FILE_NAME_SIZE, "%s-%s.txt", fileName, "he");

	//바이너리 파일로 열어야 한다.
	if ((err = fopen_s(&fp, enFileName, "wb")) != 0) {
		printf("Encoding File Open Error!!\n");
		return -1;
	}

	//파일 쓰기
	fwrite(result, realSize, 1, fp);

	free(result);
	DeleteTree(huffmanTree);
	fclose(fp);

	//인코딩에 성공하면 1 반환
	return 1;
}

//현재 코드가 허프만 테이블에 있는지 검사한다.(있다면 1, 없다면 0 반환)
int matched(short code, int codeSize, short chCode, int chCodeSize)
{
	if (codeSize != chCodeSize) return 0;

	for (int i = codeSize - 1; i >= 0; --i) {
		if ((code >> i) != (chCode >> i))
			return 0;
	}
	return 1;
}

//허프만 디코딩
int decoding(char* content, char* result, int fileSize)
{
	/* 헤더 내용 디코딩 */
	int numOfCharacter = 0, curSize = 0;

	//전체 문자수
	for (int i = 7; i >= 0; --i) {
		numOfCharacter <<= 1;
		numOfCharacter += (content[curSize] >> i) & 1;
	}
	curSize++;

	int curBitPos = 7;
	for (int i = 0; i < numOfCharacter; ++i) {
		// 현재 문자(8bit, 아스키코드 번호)
		dhTable[i].ch = 0;
		for (int j = 7; j >= 0; --j) {
			dhTable[i].ch <<= 1;
			dhTable[i].ch += (content[curSize] >> curBitPos) & 1;

			curBitPos--;
			if (curBitPos == -1) {
				curBitPos = 7;
				curSize++;
			}
		}

		dhTable[i].codeSize = 0;
		// 현재 문자 허프만 코드 길이(5bit)
		for (int j = 4; j >= 0; --j) {
			dhTable[i].codeSize <<= 1;
			dhTable[i].codeSize += (content[curSize] >> curBitPos) & 1;

			curBitPos--;
			if (curBitPos == -1) {
				curBitPos = 7;
				curSize++;
			}
		}

		// 현재 문자 허프만 코드(최대 32bit)
		dhTable[i].code = 0;
		for (int j = dhTable[i].codeSize - 1; j >= 0; --j) {
			dhTable[i].code <<= 1;
			dhTable[i].code += (content[curSize] >> curBitPos) & 1;

			curBitPos--;
			if (curBitPos == -1) {
				curBitPos = 7;
				curSize++;
			}
		}
	}

	/* 본문 내용 디코딩 */
	int rSize = 0;
	//유효 비트 길이
	int validBit = 0;
	//현재 코드
	short curChCode = 0;
	while (curSize < fileSize) {
		for (int j = 7; j >= 0; --j) {
			curChCode <<= 1;
			curChCode += (content[curSize] >> curBitPos) & 1;
			curBitPos--;
			if (curBitPos == -1) {
				curBitPos = 7;
				curSize++;
			}

			//현재 코드가 허프만 테이블에 있는지 검사한다.
			validBit++;
			int matchFlag = 0;
			for (int i = 0; i < numOfCharacter; ++i)
				//있다면 결과 배열에 저장하고 초기화한다.
				if (matched(dhTable[i].code, dhTable[i].codeSize, curChCode, validBit)) {
					result[rSize++] = dhTable[i].ch;
					matchFlag = 1;
					validBit = 0;
					curChCode = 0;
					break;
				}
			if (matchFlag) break;
		}
	}

	//전체 문자 수를 반환한다.
	return numOfCharacter;
}

//허프만 디코딩 과정
int huffmanDecodingProcess(char* fileName, char* fileContents, int fileSize, char* infoFlag)
{
	//본문 내용 길이를 알지 못하므로 매우 큰 크기의 배열을 생성한다.
	char* result = (char*)malloc(sizeof(char) * (fileSize + VERY_BIG_SIZE));
	memset(result, 0, fileSize + VERY_BIG_SIZE);
	//디코딩 실행
	int numOfCharacter = decoding(fileContents, result, fileSize);

	//허프만 디코딩 정보를 파일에 저장한다.
	fileName[strlen(fileName) - 4] = 0;
	if (strncmp(infoFlag, "y", 1) == 0 || strncmp(infoFlag, "Y", 1) == 0 ||
		strncmp(infoFlag, "yes", 3) == 0 || strncmp(infoFlag, "YES", 3) == 0)
		writeHuffmanDecodingInfo(fileName, numOfCharacter);

	//디코딩된 내용을 파일에 저장한다.
	FILE* fp;
	errno_t err;
	char deFileName[FILE_NAME_SIZE];
	memset(deFileName, 0, FILE_NAME_SIZE);
	sprintf_s(deFileName, FILE_NAME_SIZE, "%s-%s.txt", fileName, "hd");

	if ((err = fopen_s(&fp, deFileName, "wb")) != 0) {
		printf("Encoding File Open Error!!\n");
		return -1;
	}
	fwrite(result, strlen(result), 1, fp);

	free(result);
	fclose(fp);

	//디코딩에 성공하면 1 반환
	return 1;
}