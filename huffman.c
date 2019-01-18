#include "huffman.h"

//������ Ʈ���� ����� ���� �ּ� �� �� �Լ�(���� �󵵼� ��������)
int DataPriorityComp(HData data1, HData data2)
{
	return data2->data.freq - data1->data.freq;
}

//���� �󵵼� ���
void calculateCharacterFrequency(char *content, int size)
{
	for (int i = 0; i < size; ++i)
		frequency[content[i]]++;
}

//������ Ʈ���� �̿��Ͽ� ������ �ڵ� �����(���� - 0, ������ - 1)
void makeHuffmanCode(Node* huffmanTree, short code, int codeSize, int totalCharSize)
{
	// ���� ����� ���� �����̴�.
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

//������ ���̺��� �����.
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

//������ Ʈ���� �����.
Node* makeHuffmanTree(int numOfCharacter)
{
	// ��
	Heap heap;
	//�� �ʱ�ȭ
	HeapInit(&heap, numOfCharacter, DataPriorityComp);

	//�� ���� ��带 �����.(Ʈ�� ���)
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

	//������ Ʈ�� �����
	while (heap.numOfData > 1) {
		//�ּ� ������ ��� 2���� ������.(�󵵼��� ���� ���� 2��)
		Node* minNode1 = HDelete(&heap);
		Node* minNode2 = HDelete(&heap);
		//�� ��带 ��ĥ ������ ��带 �����.
		TData tmp;
		tmp.ch = 'F';
		tmp.freq = minNode1->data.freq + minNode2->data.freq;
		tmp.realCharFlag = 0;
		Node* tmpNode = makeNewNode(&tmp);
		//������ ��� �����ʰ� ���ʿ� �ּ� ������ ���� �� ��带 ������.
		MakeLeftSubTree(tmpNode, minNode1);
		MakeRightSubTree(tmpNode, minNode2);
		//�ϼ��� ������ ��带 �ٽ� �ּ� ���� �����Ѵ�.
		HInsert(&heap, tmpNode);
	}

	//������ Ʈ�� ��Ʈ�� ��ȯ�Ѵ�.
	Node* main = HDelete(&heap);
	return main;
}

//������ ���ڵ�
int encoding(char* content, char* result, int numOfCharacter, int fileSize)
{
	/* ��� ����� */
	//���� �迭�� ��ġ
	int curSize = 0;
	// ��ü ���� ��(8bit)
	for (int i = 7; i >= 0; --i) {
		result[curSize] <<= 1;
		result[curSize] += (numOfCharacter >> i) & 1;
	}
	curSize++;

	//���� ������ �����Ѵ�.
	int curBitPos = 0;
	for (int i = 0; i < numOfCharacter; ++i) {
		// ���� ����(8bit, �ƽ�Ű�ڵ� ��ȣ)
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

		// ���� ���� ������ �ڵ� ����(5bit)
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

		// ���� ���� ������ �ڵ�(�ִ� ���� 32)
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

	/* ���� ���� */
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
	������ �׿� ��Ʈ�� ó������ ���� �ִ� 7bit ��ŭ �����Ⱚ�� ���ڵ��� ��
	�� �� �ִ�.(�̺κ��� ���� ó������ ����, ���ڵ� �κп����� ó���ϸ�
	���ڵ������� �����ϰ� ����ȴ�.)
	*/
	// �׿� ��Ʈ�� �ִٸ� ��Ȯ�� ��Ʈ ����� ���� �������� ��� �Ű��ش�.
	if (curBitPos > 0) {
		//printf("%d\n", curBitPos);
		for (int i = 0; i < 8 - curBitPos; ++i)
			result[curSize] <<= 1;
	}

	return curSize;
	/*
	//��Ʈ ����غ���
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

//������ ���ڵ� ����
int huffmanEncodingProcess(char* fileName, char* fileContents, int fileSize, char* infoFlag)
{
	calculateCharacterFrequency(fileContents, fileSize);

	int numOfCharacter = makeHuffmanTable();
	Node* huffmanTree = makeHuffmanTree(numOfCharacter);
	makeHuffmanCode(huffmanTree, 0, 0, numOfCharacter);

	// ������Ʈ ����: 320�� ����� �ִ� ����Ʈ ũ��(�ִ� ��� ũ��� �뷫 2560 bit)
	char* result = (char*)malloc(sizeof(char) * (fileSize + 320));
	memset(result, 0, fileSize + 320);
	int realSize = encoding(fileContents, result, numOfCharacter, fileSize);

	//������ ���ڵ� ������ ����Ѵ�.
	fileName[strlen(fileName) - 4] = 0;
	if (strncmp(infoFlag, "y", 1) == 0 || strncmp(infoFlag, "Y", 1) == 0 ||
		strncmp(infoFlag, "yes", 3) == 0 || strncmp(infoFlag, "YES", 3) == 0)
		writeHuffmanEncodingInfo(fileName, numOfCharacter, fileSize, realSize);

	//������ ���ڵ��� ������ ���Ͽ� �����Ѵ�.
	FILE* fp;
	errno_t err;
	char enFileName[FILE_NAME_SIZE];
	memset(enFileName, 0, FILE_NAME_SIZE);
	//���ڵ� ��� ���� �̸� ����
	sprintf_s(enFileName, FILE_NAME_SIZE, "%s-%s.txt", fileName, "he");

	//���̳ʸ� ���Ϸ� ����� �Ѵ�.
	if ((err = fopen_s(&fp, enFileName, "wb")) != 0) {
		printf("Encoding File Open Error!!\n");
		return -1;
	}

	//���� ����
	fwrite(result, realSize, 1, fp);

	free(result);
	DeleteTree(huffmanTree);
	fclose(fp);

	//���ڵ��� �����ϸ� 1 ��ȯ
	return 1;
}

//���� �ڵ尡 ������ ���̺� �ִ��� �˻��Ѵ�.(�ִٸ� 1, ���ٸ� 0 ��ȯ)
int matched(short code, int codeSize, short chCode, int chCodeSize)
{
	if (codeSize != chCodeSize) return 0;

	for (int i = codeSize - 1; i >= 0; --i) {
		if ((code >> i) != (chCode >> i))
			return 0;
	}
	return 1;
}

//������ ���ڵ�
int decoding(char* content, char* result, int fileSize)
{
	/* ��� ���� ���ڵ� */
	int numOfCharacter = 0, curSize = 0;

	//��ü ���ڼ�
	for (int i = 7; i >= 0; --i) {
		numOfCharacter <<= 1;
		numOfCharacter += (content[curSize] >> i) & 1;
	}
	curSize++;

	int curBitPos = 7;
	for (int i = 0; i < numOfCharacter; ++i) {
		// ���� ����(8bit, �ƽ�Ű�ڵ� ��ȣ)
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
		// ���� ���� ������ �ڵ� ����(5bit)
		for (int j = 4; j >= 0; --j) {
			dhTable[i].codeSize <<= 1;
			dhTable[i].codeSize += (content[curSize] >> curBitPos) & 1;

			curBitPos--;
			if (curBitPos == -1) {
				curBitPos = 7;
				curSize++;
			}
		}

		// ���� ���� ������ �ڵ�(�ִ� 32bit)
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

	/* ���� ���� ���ڵ� */
	int rSize = 0;
	//��ȿ ��Ʈ ����
	int validBit = 0;
	//���� �ڵ�
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

			//���� �ڵ尡 ������ ���̺� �ִ��� �˻��Ѵ�.
			validBit++;
			int matchFlag = 0;
			for (int i = 0; i < numOfCharacter; ++i)
				//�ִٸ� ��� �迭�� �����ϰ� �ʱ�ȭ�Ѵ�.
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

	//��ü ���� ���� ��ȯ�Ѵ�.
	return numOfCharacter;
}

//������ ���ڵ� ����
int huffmanDecodingProcess(char* fileName, char* fileContents, int fileSize, char* infoFlag)
{
	//���� ���� ���̸� ���� ���ϹǷ� �ſ� ū ũ���� �迭�� �����Ѵ�.
	char* result = (char*)malloc(sizeof(char) * (fileSize + VERY_BIG_SIZE));
	memset(result, 0, fileSize + VERY_BIG_SIZE);
	//���ڵ� ����
	int numOfCharacter = decoding(fileContents, result, fileSize);

	//������ ���ڵ� ������ ���Ͽ� �����Ѵ�.
	fileName[strlen(fileName) - 4] = 0;
	if (strncmp(infoFlag, "y", 1) == 0 || strncmp(infoFlag, "Y", 1) == 0 ||
		strncmp(infoFlag, "yes", 3) == 0 || strncmp(infoFlag, "YES", 3) == 0)
		writeHuffmanDecodingInfo(fileName, numOfCharacter);

	//���ڵ��� ������ ���Ͽ� �����Ѵ�.
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

	//���ڵ��� �����ϸ� 1 ��ȯ
	return 1;
}