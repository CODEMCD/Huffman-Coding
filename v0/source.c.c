#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
/************************RLE***************************/
#define ESCAPE  0xFF //Ż�⹮��
#define ID  0xAA  // Run-Length ������� ���� �ĺ� �ڵ�(�����)
/*******************************************************/
/************************HUFF***************************/
#define ASCII 128
#define TRUE 1
#define FALSE 0
#define HEAP_LEN 1000

typedef struct {
	char ch;         // �� ���ڸ� �����ϴ� ����
	int freq;	     // ���� �󵵼��� �����ϴ� ����
} chOfFreq;

typedef struct hufTreeNode {
	chOfFreq weight;
	struct hufTreeNode *left;   // ������Ʈ�� ���� ���
	struct hufTreeNode *right;  // ������Ʈ�� ������ ���
} hufTreeNode;

typedef struct {
	hufTreeNode *pTree;
	int key;
}Element;  //�ּ����� ���� ����ü

typedef struct {
	Element heap[HEAP_LEN];  //�ּ������� ���ĵ� Element ����ü
	int heap_size;
} Heap;  //��

char *hufCode[ASCII];  // �� ���ڸ��� ������ �������ڵ带 �����ϴ� �����͹迭
char *deHufCode[ASCII]; //decoding�� �������ڵ带 ������ �����͹迭

void HInsert(Heap *, Element);  //�ּ��� ����
Element HDelete(Heap *);  //�ּ��� ����
int HIsEmpty(Heap *);  //���� ����ִ��� Ȯ��
void huffmanTree(chOfFreq *, int);  //������ Ʈ���� ����� �Լ�
void checkFrequency(char *, int *);  //������ �󵵼��� üũ�ϴ� �Լ�
int freeTree(hufTreeNode *);  //���������ϴ� �Լ�
void makeHufCode(hufTreeNode *, int, char *);  //�������ڵ带 ����� �Լ�
void encoding(char *, char*);  //���ڵ��� �����ϴ� �Լ�
void printMenu(void);  //�޴�������
void hufEncodingMain();  //������ ���ڵ�
void hufDecodingMain();  //������ ���ڵ�
void decoding(FILE *, char *, int);  //���ڵ��� �����ϴ� �Լ�
/********************************************************/
//����
int main(void);
/********************************************************/
long file_length(char *filename)  // ������ ���� ���ϱ�
{
	FILE *fp;
	long l;

	if ((fp = fopen(filename, "rb")) == NULL)  //������ �б���� ����
		return 0L;

	fseek(fp, 0, SEEK_END);  //��������Ʈ�� ������ ����
	l = ftell(fp);  //��������Ʈ�� ���� ����
	fclose(fp);

	return l;
}

void file_print(char *code, int len, FILE *fp) // �迭code�� ������ ���
{
	int i;
	for (i = 0; i < len; i++)  // len��ŭ �ݺ�
		putc(code[i], fp);
}

void run_length_Encoding(char *filename)  // Run-Length ���� 
{
	FILE *read;
	int cur;
	int code_len;  //�ݺ���
	int length;  //����
	char code[10];  //���� ����
	char savename[13];  //�������� �̸�
	FILE *save;
	if ((read = fopen(filename, "rb")) == NULL)  //�б���� ���� ����
	{
		printf("file open fail \n");
		system("pause");
		return;
	}
	printf("save name : ");
	scanf("%s", savename);
	if ((save = fopen(savename, "wb")) == NULL)  //������� ���Ͽ��� 
	{
		printf("file write fail \n");
		system("pause");
		return;
	}
	cur = getc(read);  //read���Ͽ��� �ѱ��ھ� �о��
	code_len = length = 0;

	while (!feof(read))  //eof���� �ݺ�
	{
		if (length == 0)
		{
			if (cur != ESCAPE)  //Ż�⹮������
			{
				code[code_len++] = cur;  //������ ���ڸ� code�迭�� ����
				length++;
			}
			else
			{
				code[code_len++] = ESCAPE;  //code�迭���� code_len��°�� Ż�⹮�� �Է�
				file_print(code, code_len, save);  //���
				length = code_len = 0;  //�ʱ�ȭ
			}

			cur = getc(read);  //���� �б�
		}
		else if (length == 1)
		{
			if (cur != code[0])  //�о�� ���ڰ� 0��° ���ڶ� ������
			{
				file_print(code, code_len, save);
				length = code_len = 0;
			}
			else
			{
				length++;
				code[code_len++] = cur;
				cur = getc(read);
			}
		}
		else if (length >= 2)
		{
			if (cur != code[1] || length > 254)
			{
				file_print(code, code_len, save);
				length = code_len = 0;
			}
			else
			{
				length++;
				code_len = 0;  //code_len �ʱ�ȭ
				code[code_len++] = ESCAPE;  //code[0]�κп� Ż�⹮�ڷ� ����
				code[code_len++] = cur;  //
				code[code_len++] = length;
				cur = getc(read);
			}
		}
	}

	file_print(code, code_len, save);
	fclose(save);
	fclose(read);
}

void run_length_Decoding(char *filename)  // Run-Length ������ ����
{
	FILE *load;
	int cur;
	char savename[13];
	FILE *newfile;
	int i = 0, j;
	int length;
	if ((load = fopen(filename, "rb")) == NULL)
	{
		printf("file open fail \n");
		system("pause");
		return;
	}
	printf("save name : ");
	scanf("%s", savename);
	if ((newfile = fopen(savename, "wb")) == NULL)
	{
		printf("file write fail\n ");
		system("pause");
		return;
	}

	cur = getc(load);
	while (!feof(load))
	{
		if (cur != ESCAPE)
			putc(cur, newfile);
		else
		{
			cur = getc(load);
			if (cur == ESCAPE)
				putc(ESCAPE, newfile);

			else
			{
				length = getc(load);
				for (j = 0; j < length; j++)
					putc(cur, newfile);
			}
		}

		cur = getc(load);

	}
	fclose(load);
	fclose(newfile);
}
void display()  //�޴� ���
{
	printf("******************************\n");
	printf("[Run Length Coding] Ver 0.1\n");
	printf("           - Menu -           \n");
	printf("   1. Encoding             \n");
	printf("   2. Decoding                    \n");
	printf("   0. Back                   \n");
	printf("   => Input your choice: ");

}
void file_printStdin(void)// ���ۺ���
{
	int ch;

	fseek(stdin, 0, SEEK_END);
	if (ftell(stdin)>0)
		do
			ch = getchar();
	while (ch != EOF && ch != '\n');
}
void RLE()
{
	char choose;
	char readname[20]; //���ڵ��� �ؽ�Ʈ �����̸� �迭


	while (1)
	{
		system("cls");
		display();
		scanf("%c", &choose);
		switch (choose - '0')
		{
		case 1:
			system("cls");
			file_printStdin();
			printf("open file name : ");
			scanf("%s", readname);
			run_length_Encoding(readname);
			break;
		case 2:
			system("cls");
			file_printStdin();
			printf("open file name : ");
			scanf("%s", readname);
			run_length_Decoding(readname);
			break;
		case 0:
			main();
			break;
		default:
			continue;
		}
		printf("\n");

	}
}
void HUFF()
{
	char choice;

	while (1)
	{
		system("cls");
		printMenu();
		scanf("%c", &choice);
		switch (choice - '0')
		{
		case 1:
			hufEncodingMain();
			break;
		case 2:
			hufDecodingMain();
			break;
		case 0:
			main();
			break;
		default:
			continue;
		}

	}
}
int main(void)
{
	system("cls");
	printf("******************************\n");
	printf("           - Menu -           \n");
	printf("   1. Huffman             \n");
	printf("   2. Run_Length                    \n");
	printf("   0. Exit                    \n");
	printf("   => Input your choice: ");
	int choose;
	scanf("%d", &choose);
	system("cls");
	while (1)
	{
		switch (choose)
		{
		case 1:
			HUFF();
			break;
		case 2:
			RLE();
			break;
		case 0:
			exit(1);
		}
		return 0;
	}
}
void printMenu(void)
{
	system("cls");
	printf("******************************\n");
	printf("[Huffman Coding] Ver 0.1\n");
	printf("           - Menu -           \n");
	printf("   1. Encoding             \n");
	printf("   2. Decoding             \n");
	printf("   0. Back                    \n");
	printf("   => Input your choice: ");
}

void hufEncodingMain()
{
	system("cls");
	char fileName[200];  //�������ڵ��� �ؽ�Ʈ���� �̸�
	char fileName1[200];  //���ڵ��� ������ ������ �ؽ�Ʈ���� �̸�
	FILE *fp, *fp1;
	long fileLen;  //�ؽ�Ʈ ���� ��üũ�⸦ ������ ����

	printf("open file name : ");
	scanf_s("%s", fileName, sizeof(fileName));

	if ((fp = fopen(fileName, "rb")) == NULL)
	{
		printf("file open fail \n");
		system("pause");
		return;

	}
	fseek(fp, 1L, SEEK_END);  //+1�ؾ� ������ ����
	fileLen = ftell(fp);
	char *textContent = (char*)malloc(fileLen);  //�ؽ�Ʈũ�⸸ŭ �ؽ�Ʈ������ ������ �迭 �����Ҵ�

	fseek(fp, 0L, SEEK_SET); //�ٽ� �ؽ�Ʈ ����Ʈ�� ó����ġ�� �ű�
	fgets(textContent, fileLen, fp);  //�ؽ�Ʈ���� ����
	fclose(fp);
	//printf("%s \n", textContent);

	chOfFreq cof[ASCII];
	int chFreq[ASCII] = { 0 };  //���ڸ��� �󵵼��� ������ �迭
	int j = 0;
	checkFrequency(textContent, chFreq);  //�󵵼� ��� �Լ�

	for (int i = 0; i < ASCII; i++) {  //�󵵼��� ������ �迭�� �̿��Ͽ� ����ü�� ����
		if (chFreq[i] != 0) {
			cof[j].ch = (char)i;
			cof[j].freq = chFreq[i];
			j++;
		}
	}
	huffmanTree(cof, j);  //������Ʈ���� ����� �Լ�

	printf("result file name : ");
	scanf_s("%s", fileName1, sizeof(fileName1));

	if ((fp1 = fopen(fileName1, "wb")) == NULL)
	{
		printf("file open fail \n");
		system("pause");
		return;
	}
	int len = strlen(textContent);  //������ ���̺� ����
	fprintf(fp1, "%d|", len);
	for (int i = 0; i < ASCII; i++) {
		if (hufCode[i] != NULL) {
			fprintf(fp1, "%c%s|", (char)i, hufCode[i]);  //'|'�� �ؽ�Ʈ ���Ͽ� ���ٰ� ����
		}
	}
	fputs("|", fp1);
	fclose(fp1);
	encoding(textContent, fileName1);  //���������̺��� �̿��� ���ڵ� �Լ�
	free(textContent);  //��������
}

void hufDecodingMain()
{
	system("cls");
	char fileName[200];  //���ڵ��� �ؽ�Ʈ���� �̸�
	char fileName1[200];  //���ڵ� �Ϸ�� �ؽ�Ʈ�� ������ ���� �̸�
	FILE *fp;
	int n = 0;
	int totChNum = 0;  //�ѹ��ڼ��� ������ ����

	printf("open name : ");
	scanf_s("%s", fileName, sizeof(fileName));

	printf("result file name : ");
	scanf_s("%s", fileName1, sizeof(fileName1));

	if ((fp = fopen(fileName, "rb")) == NULL)
	{
		printf("file open fail \n");
		system("pause");
		return;
	}
	char buf, curCh, curCh1;
	int i = 0;
	char saveTotNum[100];  //�� ���ڼ��� �ڸ����� �𸣱� ������ �ӽ÷� ������ �迭
	char bCode[100];  //�ӽ÷� �������ڵ带 ������ ����

	while (!feof(fp)) {
		curCh = fgetc(fp);
		saveTotNum[totChNum] = curCh;
		totChNum++;
		if (curCh == '|')
			break;
	}
	totChNum = atoi(saveTotNum);  //����� ����ŭ int�� ������ �ٲپ� �־� �� ���ڼ��� �����Ѵ�.

	while (!feof(fp)) {  //���ڵ��� ���Ͽ��� ���������̺��� �д´�.
		i = 0;
		curCh1 = fgetc(fp);
		if (curCh1 == '|')  //'||'�� �Ǹ� ������ ���̺��� ��� ���� ��
			break;
		while (!feof(fp)) {  
			buf = fgetc(fp);
			if (buf != '|') {
				if (buf == '0') {
					bCode[i] = '0';
					i++;
				}
				else if (buf == '1') {
					bCode[i] = '1';
					i++;
				}
			}
			else {
				//printf("%s \n", bCode);
				char *saveCode = (char*)malloc(i);  //�����͹迭�� ������ �迭�� �����Ѵ�.
													////deHufCode�����͹迭�� ���ҷ� �ٲ�Ƿ� free�����ʿ䰡 ���ٰ� �����Ѵ�.
				strncpy(saveCode, bCode, i);
				deHufCode[(int)curCh1] = saveCode;
				break;
			}
		}
	}

	decoding(fp, fileName1, totChNum);  //���ڵ� �Լ�
}

void checkFrequency(char *dest, int *freq)
{
	int len = strlen(dest);
	for (int i = 0; i < len; i++)
		freq[(int)dest[i]]++;
}


hufTreeNode* makeNode(hufTreeNode *left, hufTreeNode *right) // ��� ���� 
{
	hufTreeNode *node = (hufTreeNode*)malloc(sizeof(hufTreeNode));

	if (node == NULL)
	{
		printf("�޸� �Ҵ� ����\n");
		exit(-1);
	}

	node->left = left;
	node->right = right;

	return node;
}

void huffmanTree(chOfFreq *pArr, int n)
{
	hufTreeNode *node, *temp;
	Element e, e1, e2;
	Heap heap;
	int num;
	char binaryCode[100];

	heap.heap_size = 0;

	for (int i = 0; i < n; i++) {  //�����ڸ� Ʈ���� ����� �ּ����� ����
		node = makeNode(NULL, NULL);
		node->weight.ch = pArr[i].ch;
		e.key = node->weight.freq = pArr[i].freq;
		e.pTree = node;
		HInsert(&heap, e);
	}
	for (int i = 0; i<n - 1; i++) {   // n-1���� �ݺ�, ������ ���� ��尡 ��Ʈ 
		e1 = HDelete(&heap);  // �ּҰ��� ������ ��� 2���� ������ 
		e2 = HDelete(&heap);

		temp = makeNode(e1.pTree, e2.pTree); // 2���� ��带 ������ ��� ���� 

		e.key = temp->weight.freq = e1.key + e2.key; // 2���� ��� ���� ���Ѵ� 
		e.pTree = temp; // ������ ���� ��带 ���� 

		HInsert(&heap, e); // Ʈ���� ������ ��带 ���� ����  
	}

	while (!HIsEmpty(&heap)) {
		e = HDelete(&heap);  //�ϼ��� Ʈ��
							 //printf("%d \t %d \t %d\n", e.key, e.pTree->left->weight.freq, e.pTree->right->weight.freq);
	}

	makeHufCode(e.pTree, -1, binaryCode);  //������ �ڵ� ����

	if ((num = freeTree(e.pTree)) == FALSE)
		printf("�޸� ��ȯ ����\n");
}

void makeHufCode(hufTreeNode *p, int n, char *bCode)
{
	if (p) {
		n++;
		bCode[n] = '0';
		makeHufCode(p->left, n, bCode);  //�ο��� ���
		bCode[n] = '1';
		makeHufCode(p->right, n, bCode);
		bCode[n] = '\0';

		if (p->left == NULL && p->right == NULL) {
			char *saveHufCode = (char*)malloc(strlen(bCode));  //hufCode�����͹迭�� ����Ű�� ������ ������ش�.
															   //(hufCode�����͹迭�� ���ҷ� �ٲ�Ƿ� free�����ʿ䰡 ���ٰ� �����Ѵ�.)
			strcpy(saveHufCode, bCode);
			hufCode[(int)p->weight.ch] = saveHufCode;
			//printf("%c\t%s\n", p->weight.ch, hufCode[(int)p->weight.ch]);
		}
	}
}

int freeTree(hufTreeNode *p)
{
	if (p == NULL) return FALSE;

	freeTree(p->left);
	freeTree(p->right);

	free(p);

	return TRUE;
}

void HInsert(Heap *ph, Element item)
{
	int i;
	i = ++(ph->heap_size);  //�迭�� 1���� �ֱ� ���� ���������ڻ��(Ʈ��������Ǹ� ����)

	while (i != 1 && item.key < ph->heap[i / 2].key) { // �θ���� �� 
		ph->heap[i] = ph->heap[i / 2];
		i /= 2;
	}

	ph->heap[i] = item;
}

Element HDelete(Heap *ph)
{
	int parent = 1, child = 2;
	Element data, temp;

	data = ph->heap[parent];
	temp = ph->heap[(ph->heap_size)--]; // ������ ���� size ���� 

	while (child <= ph->heap_size) {  //�ڽ� ��尣 ���� �� ��  
		if ((child < ph->heap_size) && (ph->heap[child].key)
		> ph->heap[child + 1].key)
			child++;

		if (temp.key <= ph->heap[child].key)
			break;

		ph->heap[parent] = ph->heap[child];
		parent = child;
		child *= 2; // ���� �ڽ� ���� �� 
	}

	ph->heap[parent] = temp;
	return data;
}

int HIsEmpty(Heap *ph)
{
	if (ph->heap_size == 0)
		return TRUE;
	else
		return FALSE;
}

void encoding(char *input, char *filename)
{
	FILE *fp;
	if ((fp = fopen(filename, "ab")) == NULL)
		printf("file open fail \n");

	char bitOper = 0;  //��Ʈ ����� ����
	char operIng;  //�о���� ��Ʈ�� ������ ����
	int count = 7;  //shift�� ��
	int len = strlen(input);
	for (int i = 0; i < len; i++) {  //�� ���ڼ���ŭ �ݺ�
		int len2 = strlen(hufCode[(int)input[i]]);
		for (int j = 0; j < len2; j++) {  //�ش� �������ڵ� ����ŭ �ݺ�
										  //printf("%c ", hufCode[(int)input[i]][j]);
			if (hufCode[(int)input[i]][j] == '0')
				operIng = 0;
			else if (hufCode[(int)input[i]][j] == '1')
				operIng = 1;
			operIng = operIng << count;  //shift
			bitOper = bitOper | operIng;  //�տ���
			count--;  //shift �� ����
			if (count == -1) {  //8��Ʈ�� �Ѿ� �� ��
				count = 7;
				fputc(bitOper, fp); //���Ͽ� ����
				bitOper = 0;
			}
		}
	}
	fputc(bitOper, fp);  //8��Ʈ�� �Ѿ�� ���� ���� �� ����

	fclose(fp);
}

void decoding(FILE *fp, char *filename, int totChar)
{
	hufTreeNode *root;
	root = makeNode(NULL, NULL);
	hufTreeNode *deHufTree = root;  //��Ʈ ��� ����
	int tot = 0;  //��ü ���ڼ� ����

	deHufTree = root;
	for (int i = 0; i < ASCII; i++) {  //������Ʈ�� ����
		if (deHufCode[i] != NULL) {  //�������ڵ尡 �ִ� ���ڵ鸸
			for (int j = 0; j < 100; j++) {
				if (deHufCode[i][j] == '0') {
					if (deHufTree->left == NULL) {  //���� ��尡 NULL
						deHufTree->left = makeNode(NULL, NULL);
					}
					deHufTree = deHufTree->left;
				}
				else if (deHufCode[i][j] == '1') {
					if (deHufTree->right == NULL)
						deHufTree->right = makeNode(NULL, NULL);
					deHufTree = deHufTree->right;
				}
				else {
					break;
				}
			}
			deHufTree->weight.ch = (char)i;  //������ �ڵ忡 �´� ���� ����
			deHufTree = root;  //��Ʈ�� �ǵ��ư�
		}
	}
	FILE *fp1;
	if ((fp1 = fopen(filename, "wt")) == NULL)
		printf("file open fail \n");

	char bitOper[100];  //��Ʈ ������ �迭

	deHufTree = root;  //��Ʈ
	while (1) {
		int size = fread(bitOper, 1, 100, fp);  //��Ʈ�� �迭�� ����
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < 8; j++) {
				if ((char)(bitOper[i] & 0x80) == 0)  //�ڵ尡 0�̸� ����
					deHufTree = deHufTree->left;
				else   //�ƴϸ� ������
					deHufTree = deHufTree->right;

				bitOper[i] = bitOper[i] << 1;  //�������� 1 shift

				if (deHufTree->left == NULL && deHufTree->right == NULL) {  //���ʳ�尡 NULL�̸�
																			//printf("%c", deHufTree->weight.ch);
					fputc(deHufTree->weight.ch, fp1);
					deHufTree = root;  //��Ʈ�� �ǵ��ư���
					tot++;
					if (tot == totChar)  //�ѹ��ڼ��� ������ ��� �ݺ����� ������ �Ѵ�.
						break;
				}
			}
			if (tot == totChar)
				break;
		}
		if (tot == totChar)
			break;
	}
	fclose(fp1);
}