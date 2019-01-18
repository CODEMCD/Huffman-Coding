#include "file.h"

void huffmanEncodingMain()
{
	FILE* fp;
	errno_t err;
	char fileName[FILE_NAME_SIZE];
	char infoFlag[5];
	memset(fileName, 0, FILE_NAME_SIZE);
	memset(infoFlag, 0, sizeof(infoFlag));

	// C:/Users/SUNGBUM/Desktop/SB/TestCase/huffmanTest.txt
	// ���ڵ��� ������ �Է��Ѵ�.
	while (1) {
		system("cls");
		printf("- Huffman Compress Encoding -\n");
		printf("���ڵ� ���� �Է�: ");
		scanf_s("%s", fileName, 100);

		if ((err = fopen_s(&fp, fileName, "rb")) != 0) {
			printf("�߸��� �����̰ų� ���� �����Դϴ�. �ٽ� �Է����ּ���.\n");
			memset(fileName, 0, sizeof(fileName));
			printf("�ٽ� ���ư����� �ƹ�Ű�� ��������.\n");
			_getch();
			continue;
		}
		else {
			printf(">> ������ ������ ����ϸ� ������ �� �ֽ��ϴ�!\n");
			printf(">> ������ �ڵ� ������ ���Ϸ� �����Ͻðڽ��ϱ�? (y/n): ");
			scanf_s("%s", infoFlag, sizeof(infoFlag));
			break;
		}
	}

	//���� ũ�� ���
	int fileSize = 0;
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);

	/*
	���� �ִ� 7bit ��ŭ �߸��� ���ڰ� �� �� �ִ�.
	���� �׿���Ʈ�� ó������ �ʾҴ�.
	*/
	char* content = (char*)malloc(sizeof(char) * (fileSize + 10));
	memset(content, 0, sizeof(content));

	//���� �б�
	fseek(fp, 0, SEEK_SET);
	fread(content, fileSize, 1, fp);

	//������ ���ڵ� ����
	if ((huffmanEncodingProcess(fileName, content, fileSize, infoFlag)) == -1)
		printf("Encoding Error!!\n");

	//�޸� ����
	free(content);
	fclose(fp);
}


void huffmanDecodingMain()
{
	FILE* fp;
	errno_t err;
	char fileName[FILE_NAME_SIZE];
	char infoFlag[5];
	memset(fileName, 0, FILE_NAME_SIZE);
	memset(infoFlag, 0, sizeof(infoFlag));

	// C:/Users/SUNGBUM/Desktop/SB/TestCase/huffmanTest-he.txt
	//������ �Է¹޴´�.
	while (1) {
		system("cls");
		printf("- Huffman Compress Decoding -\n");
		printf("���ڵ� ���� �Է�: ");
		scanf_s("%s", fileName, 100);

		//���� ���Ϸ� �Է¹޾ƾ� �Ѵ�.(������ ���ڵ� ����� �������Ϸ� ����Ǿ� �ִ�.)
		if ((err = fopen_s(&fp, fileName, "rb")) != 0) {
			printf("�߸��� �����̰ų� ���� �����Դϴ�. �ٽ� �Է����ּ���.\n");
			memset(fileName, 0, sizeof(fileName));
			printf("�ٽ� ���ư����� �ƹ�Ű�� ��������.\n");
			_getch();
			continue;
		}
		else {
			printf(">> ������ ������ ����ϸ� ������ �� �ֽ��ϴ�!\n");
			printf(">> ������ �ڵ� ������ ���Ϸ� �����Ͻðڽ��ϱ�? (y/n): ");
			scanf_s("%s", infoFlag, sizeof(infoFlag));
			break;
		}
	}

	//���� ũ�� ���
	int fileSize = 0;
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);

	/*
	���� �ִ� 7bit ��ŭ �߸��� ���ڰ� �� �� �ִ�.
	���� �׿���Ʈ�� ó������ �ʾҴ�.
	*/
	char* content = (char*)malloc(sizeof(char) * (fileSize + 1));
	memset(content, 0, fileSize + 1);

	//���� �б�
	fseek(fp, 0, SEEK_SET);
	fread(content, fileSize, 1, fp);

	//������ ���ڵ� ����
	if ((huffmanDecodingProcess(fileName, content, fileSize, infoFlag)) == -1)
		printf("Decoding Error!!\n");

	//�޸� ����
	free(content);
	fclose(fp);
}

void writeHuffmanEncodingInfo(char* fileName, int numOfCharacter, int beforeSize, int afterSize)
{
	FILE* fp;
	errno_t err;
	char infoFileName[FILE_NAME_SIZE];
	memset(infoFileName, 0, FILE_NAME_SIZE);

	if (fileName != NULL)
		sprintf_s(infoFileName, FILE_NAME_SIZE, "%s-%s.txt", fileName, "hInfo");


	if ((err = fopen_s(&fp, infoFileName, "w")) != 0)
		printf("Encoding File Open Error!!\n");

	char* huffmanInfo = (char*)malloc(VERY_BIG_SIZE);
	memset(huffmanInfo, 0, VERY_BIG_SIZE);

	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s\n", "Huffman Encoding Information");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%sFile: %s.txt\n", huffmanInfo, fileName);
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%sNumber of characters: %d\n", huffmanInfo, numOfCharacter);
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%sCompression Ratio: %.3f\n", huffmanInfo, 100 - ((float)afterSize / beforeSize * 100));
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10s ", huffmanInfo, "Character");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10s ", huffmanInfo, "Frequency");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%12s ", huffmanInfo, "Code Length");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%20s\n", huffmanInfo, "Huffman Code");

	char code[20];
	for (int i = 0; i < numOfCharacter; ++i) {
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10c ", huffmanInfo, hTable[i].ch);
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10d ", huffmanInfo, frequency[hTable[i].ch]);
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10d ", huffmanInfo, hTable[i].codeSize);
		memset(code, 0, sizeof(code));
		int cnt = 0;
		for (int j = hTable[i].codeSize - 1; j >= 0; j--)
			if (hTable[i].code & (1 << j))
				code[cnt++] = '1';
			else
				code[cnt++] = '0';
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%20s\n", huffmanInfo, code);
	}

	fwrite(huffmanInfo, strlen(huffmanInfo), 1, fp);

	free(huffmanInfo);
	fclose(fp);
}

void writeHuffmanDecodingInfo(char* fileName, int numOfCharacter)
{
	FILE* fp;
	errno_t err;
	char infoFileName[FILE_NAME_SIZE];
	memset(infoFileName, 0, FILE_NAME_SIZE);

	if (fileName != NULL)
		sprintf_s(infoFileName, FILE_NAME_SIZE, "%s-%s.txt", fileName, "hInfo");


	if ((err = fopen_s(&fp, infoFileName, "w")) != 0)
		printf("Encoding File Open Error!!\n");

	char* huffmanInfo = (char*)malloc(VERY_BIG_SIZE);
	memset(huffmanInfo, 0, VERY_BIG_SIZE);

	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s\n", "Huffman Decoding Information");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s[%s.txt] Number of characters: %d\n", huffmanInfo, fileName, numOfCharacter);
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10s ", huffmanInfo, "Character");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%12s ", huffmanInfo, "Code Length");
	sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%20s\n", huffmanInfo, "Huffman Code");

	char code[20];
	for (int i = 0; i < numOfCharacter; ++i) {
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10c ", huffmanInfo, dhTable[i].ch);
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%10d ", huffmanInfo, dhTable[i].codeSize);
		memset(code, 0, sizeof(code));
		int cnt = 0;
		for (int j = dhTable[i].codeSize - 1; j >= 0; j--)
			if (dhTable[i].code & (1 << j))
				code[cnt++] = '1';
			else
				code[cnt++] = '0';
		sprintf_s(huffmanInfo, VERY_BIG_SIZE, "%s%20s\n", huffmanInfo, code);
	}

	fwrite(huffmanInfo, strlen(huffmanInfo), 1, fp);

	free(huffmanInfo);
	fclose(fp);
}