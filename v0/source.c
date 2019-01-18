#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
/************************RLE***************************/
#define ESCAPE  0xFF //탈출문자
#define ID  0xAA  // Run-Length 압축법에 사용될 식별 코드(고민중)
/*******************************************************/
/************************HUFF***************************/
#define ASCII 128
#define TRUE 1
#define FALSE 0
#define HEAP_LEN 1000

typedef struct {
	char ch;         // 한 문자를 저장하는 변수
	int freq;	     // 출현 빈도수를 저장하는 변수
} chOfFreq;

typedef struct hufTreeNode {
	chOfFreq weight;
	struct hufTreeNode *left;   // 허프만트리 왼쪽 노드
	struct hufTreeNode *right;  // 허프만트리 오른쪽 노드
} hufTreeNode;

typedef struct {
	hufTreeNode *pTree;
	int key;
}Element;  //최소힙에 들어가는 구조체

typedef struct {
	Element heap[HEAP_LEN];  //최소힙으로 정렬된 Element 구조체
	int heap_size;
} Heap;  //힙

char *hufCode[ASCII];  // 각 문자마다 배정된 허프만코드를 저장하는 포인터배열
char *deHufCode[ASCII]; //decoding용 허프만코드를 저장할 포인터배열

void HInsert(Heap *, Element);  //최소힙 삽입
Element HDelete(Heap *);  //최소힙 삭제
int HIsEmpty(Heap *);  //힙이 비어있는지 확인
void huffmanTree(chOfFreq *, int);  //허프만 트리를 만드는 함수
void checkFrequency(char *, int *);  //문자의 빈도수를 체크하는 함수
int freeTree(hufTreeNode *);  //동적해제하는 함수
void makeHufCode(hufTreeNode *, int, char *);  //허프만코드를 만드는 함수
void encoding(char *, char*);  //인코딩을 실행하는 함수
void printMenu(void);  //메뉴프린드
void hufEncodingMain();  //허프만 인코딩
void hufDecodingMain();  //허프만 디코딩
void decoding(FILE *, char *, int);  //디코딩을 실행하는 함수
/********************************************************/
//공통
int main(void);
/********************************************************/
long file_length(char *filename)  // 파일의 길이 구하기
{
	FILE *fp;
	long l;

	if ((fp = fopen(filename, "rb")) == NULL)  //파일을 읽기모드로 실행
		return 0L;

	fseek(fp, 0, SEEK_END);  //파일포인트를 끝으로 보냄
	l = ftell(fp);  //파일포인트의 값을 대입
	fclose(fp);

	return l;
}

void file_print(char *code, int len, FILE *fp) // 배열code의 내용을 출력
{
	int i;
	for (i = 0; i < len; i++)  // len만큼 반복
		putc(code[i], fp);
}

void run_length_Encoding(char *filename)  // Run-Length 압축 
{
	FILE *read;
	int cur;
	int code_len;  //반복도
	int length;  //길이
	char code[10];  //문자 저장
	char savename[13];  //저장파일 이름
	FILE *save;
	if ((read = fopen(filename, "rb")) == NULL)  //읽기모드로 파일 열기
	{
		printf("file open fail \n");
		system("pause");
		return;
	}
	printf("save name : ");
	scanf("%s", savename);
	if ((save = fopen(savename, "wb")) == NULL)  //쓰기모드로 파일열기 
	{
		printf("file write fail \n");
		system("pause");
		return;
	}
	cur = getc(read);  //read파일에서 한글자씩 읽어옴
	code_len = length = 0;

	while (!feof(read))  //eof까지 반복
	{
		if (length == 0)
		{
			if (cur != ESCAPE)  //탈출문자인지
			{
				code[code_len++] = cur;  //현재의 문자를 code배열에 저장
				length++;
			}
			else
			{
				code[code_len++] = ESCAPE;  //code배열에서 code_len번째에 탈출문자 입력
				file_print(code, code_len, save);  //출력
				length = code_len = 0;  //초기화
			}

			cur = getc(read);  //문자 읽기
		}
		else if (length == 1)
		{
			if (cur != code[0])  //읽어온 문자가 0번째 문자랑 같은지
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
				code_len = 0;  //code_len 초기화
				code[code_len++] = ESCAPE;  //code[0]부분에 탈출문자로 변경
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

void run_length_Decoding(char *filename)  // Run-Length 압축을 복원
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
void display()  //메뉴 출력
{
	printf("******************************\n");
	printf("[Run Length Coding] Ver 0.1\n");
	printf("           - Menu -           \n");
	printf("   1. Encoding             \n");
	printf("   2. Decoding                    \n");
	printf("   0. Back                   \n");
	printf("   => Input your choice: ");

}
void file_printStdin(void)// 버퍼비우기
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
	char readname[20]; //인코딩할 텍스트 파일이름 배열


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
	char fileName[200];  //허프만코딩할 텍스트파일 이름
	char fileName1[200];  //인코딩된 파일을 저장할 텍스트파일 이름
	FILE *fp, *fp1;
	long fileLen;  //텍스트 파일 전체크기를 저장할 변수

	printf("open file name : ");
	scanf_s("%s", fileName, sizeof(fileName));

	if ((fp = fopen(fileName, "rb")) == NULL)
	{
		printf("file open fail \n");
		system("pause");
		return;

	}
	fseek(fp, 1L, SEEK_END);  //+1해야 끝까지 읽음
	fileLen = ftell(fp);
	char *textContent = (char*)malloc(fileLen);  //텍스트크기만큼 텍스트내용을 저장할 배열 동적할당

	fseek(fp, 0L, SEEK_SET); //다시 텍스트 포인트를 처음위치로 옮김
	fgets(textContent, fileLen, fp);  //텍스트내용 저장
	fclose(fp);
	//printf("%s \n", textContent);

	chOfFreq cof[ASCII];
	int chFreq[ASCII] = { 0 };  //문자마다 빈도수를 저장할 배열
	int j = 0;
	checkFrequency(textContent, chFreq);  //빈도수 계산 함수

	for (int i = 0; i < ASCII; i++) {  //빈도수를 저장한 배열을 이용하여 구조체에 저장
		if (chFreq[i] != 0) {
			cof[j].ch = (char)i;
			cof[j].freq = chFreq[i];
			j++;
		}
	}
	huffmanTree(cof, j);  //허프만트리를 만드는 함수

	printf("result file name : ");
	scanf_s("%s", fileName1, sizeof(fileName1));

	if ((fp1 = fopen(fileName1, "wb")) == NULL)
	{
		printf("file open fail \n");
		system("pause");
		return;
	}
	int len = strlen(textContent);  //허프만 테이블 저장
	fprintf(fp1, "%d|", len);
	for (int i = 0; i < ASCII; i++) {
		if (hufCode[i] != NULL) {
			fprintf(fp1, "%c%s|", (char)i, hufCode[i]);  //'|'는 텍스트 파일에 없다고 가정
		}
	}
	fputs("|", fp1);
	fclose(fp1);
	encoding(textContent, fileName1);  //허프만테이블을 이용한 인코딩 함수
	free(textContent);  //동적해제
}

void hufDecodingMain()
{
	system("cls");
	char fileName[200];  //디코딩할 텍스트파일 이름
	char fileName1[200];  //디코딩 완료된 텍스트를 저장할 파일 이름
	FILE *fp;
	int n = 0;
	int totChNum = 0;  //총문자수를 저장할 변수

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
	char saveTotNum[100];  //총 문자수의 자리수를 모르기 때문에 임시로 저장할 배열
	char bCode[100];  //임시로 허프만코드를 저장할 변수

	while (!feof(fp)) {
		curCh = fgetc(fp);
		saveTotNum[totChNum] = curCh;
		totChNum++;
		if (curCh == '|')
			break;
	}
	totChNum = atoi(saveTotNum);  //저장된 수만큼 int형 변수로 바꾸어 주어 총 문자수를 저장한다.

	while (!feof(fp)) {  //디코딩된 파일에서 허프만테이블을 읽는다.
		i = 0;
		curCh1 = fgetc(fp);
		if (curCh1 == '|')  //'||'가 되면 허프만 테이블을 모두 읽은 것
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
				char *saveCode = (char*)malloc(i);  //포인터배열에 저장할 배열을 생성한다.
													////deHufCode포인터배열의 원소로 바뀌므로 free해줄필요가 없다고 생각한다.
				strncpy(saveCode, bCode, i);
				deHufCode[(int)curCh1] = saveCode;
				break;
			}
		}
	}

	decoding(fp, fileName1, totChNum);  //디코딩 함수
}

void checkFrequency(char *dest, int *freq)
{
	int len = strlen(dest);
	for (int i = 0; i < len; i++)
		freq[(int)dest[i]]++;
}


hufTreeNode* makeNode(hufTreeNode *left, hufTreeNode *right) // 노드 생성 
{
	hufTreeNode *node = (hufTreeNode*)malloc(sizeof(hufTreeNode));

	if (node == NULL)
	{
		printf("메모리 할당 에러\n");
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

	for (int i = 0; i < n; i++) {  //각문자를 트리로 만들어 최소힙에 삽입
		node = makeNode(NULL, NULL);
		node->weight.ch = pArr[i].ch;
		e.key = node->weight.freq = pArr[i].freq;
		e.pTree = node;
		HInsert(&heap, e);
	}
	for (int i = 0; i<n - 1; i++) {   // n-1까지 반복, 마지막 남은 노드가 루트 
		e1 = HDelete(&heap);  // 최소값을 가지는 노드 2개를 빼낸다 
		e2 = HDelete(&heap);

		temp = makeNode(e1.pTree, e2.pTree); // 2개의 노드를 가지는 노드 생성 

		e.key = temp->weight.freq = e1.key + e2.key; // 2개의 노드 값을 더한다 
		e.pTree = temp; // 위에서 만든 노드를 대입 

		HInsert(&heap, e); // 트리로 구성된 노드를 힙에 삽입  
	}

	while (!HIsEmpty(&heap)) {
		e = HDelete(&heap);  //완성된 트리
							 //printf("%d \t %d \t %d\n", e.key, e.pTree->left->weight.freq, e.pTree->right->weight.freq);
	}

	makeHufCode(e.pTree, -1, binaryCode);  //허프만 코드 생성

	if ((num = freeTree(e.pTree)) == FALSE)
		printf("메모리 반환 오류\n");
}

void makeHufCode(hufTreeNode *p, int n, char *bCode)
{
	if (p) {
		n++;
		bCode[n] = '0';
		makeHufCode(p->left, n, bCode);  //인오더 방식
		bCode[n] = '1';
		makeHufCode(p->right, n, bCode);
		bCode[n] = '\0';

		if (p->left == NULL && p->right == NULL) {
			char *saveHufCode = (char*)malloc(strlen(bCode));  //hufCode포인터배열이 가르키는 공간을 만들어준다.
															   //(hufCode포인터배열의 원소로 바뀌므로 free해줄필요가 없다고 생각한다.)
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
	i = ++(ph->heap_size);  //배열에 1부터 넣기 위해 전위연산자사용(트리사용편의를 위해)

	while (i != 1 && item.key < ph->heap[i / 2].key) { // 부모노드와 비교 
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
	temp = ph->heap[(ph->heap_size)--]; // 삭제에 의한 size 감소 

	while (child <= ph->heap_size) {  //자식 노드간 작은 수 비교  
		if ((child < ph->heap_size) && (ph->heap[child].key)
		> ph->heap[child + 1].key)
			child++;

		if (temp.key <= ph->heap[child].key)
			break;

		ph->heap[parent] = ph->heap[child];
		parent = child;
		child *= 2; // 다음 자식 노드와 비교 
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

	char bitOper = 0;  //비트 계산할 변수
	char operIng;  //읽어오는 비트를 저장할 변수
	int count = 7;  //shift할 수
	int len = strlen(input);
	for (int i = 0; i < len; i++) {  //총 문자수만큼 반복
		int len2 = strlen(hufCode[(int)input[i]]);
		for (int j = 0; j < len2; j++) {  //해당 허프만코드 수만큼 반복
										  //printf("%c ", hufCode[(int)input[i]][j]);
			if (hufCode[(int)input[i]][j] == '0')
				operIng = 0;
			else if (hufCode[(int)input[i]][j] == '1')
				operIng = 1;
			operIng = operIng << count;  //shift
			bitOper = bitOper | operIng;  //합연산
			count--;  //shift 수 감소
			if (count == -1) {  //8비트가 넘어 갈 때
				count = 7;
				fputc(bitOper, fp); //파일에 저장
				bitOper = 0;
			}
		}
	}
	fputc(bitOper, fp);  //8비트가 넘어가기 전에 끝날 때 저장

	fclose(fp);
}

void decoding(FILE *fp, char *filename, int totChar)
{
	hufTreeNode *root;
	root = makeNode(NULL, NULL);
	hufTreeNode *deHufTree = root;  //루트 노드 생성
	int tot = 0;  //전체 문자수 저장

	deHufTree = root;
	for (int i = 0; i < ASCII; i++) {  //허프만트리 복구
		if (deHufCode[i] != NULL) {  //허프만코드가 있는 문자들만
			for (int j = 0; j < 100; j++) {
				if (deHufCode[i][j] == '0') {
					if (deHufTree->left == NULL) {  //왼쪽 노드가 NULL
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
			deHufTree->weight.ch = (char)i;  //허프만 코드에 맞는 문자 삽입
			deHufTree = root;  //루트로 되돌아감
		}
	}
	FILE *fp1;
	if ((fp1 = fopen(filename, "wt")) == NULL)
		printf("file open fail \n");

	char bitOper[100];  //비트 저장할 배열

	deHufTree = root;  //루트
	while (1) {
		int size = fread(bitOper, 1, 100, fp);  //비트를 배열에 저장
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < 8; j++) {
				if ((char)(bitOper[i] & 0x80) == 0)  //코드가 0이면 왼쪽
					deHufTree = deHufTree->left;
				else   //아니면 오른쪽
					deHufTree = deHufTree->right;

				bitOper[i] = bitOper[i] << 1;  //왼쪽으로 1 shift

				if (deHufTree->left == NULL && deHufTree->right == NULL) {  //양쪽노드가 NULL이면
																			//printf("%c", deHufTree->weight.ch);
					fputc(deHufTree->weight.ch, fp1);
					deHufTree = root;  //루트로 되돌아간다
					tot++;
					if (tot == totChar)  //총문자수가 나오면 모든 반복문을 나가야 한다.
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