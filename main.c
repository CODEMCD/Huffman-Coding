#include "include.h"
#include "file.h"

void MenuDisplay()
{
	printf("******************************\n");
	printf("[Huffman Coding] Ver 2.0\n");
	printf("           - Menu -           \n");
	printf("   1. Encoding             \n");
	printf("   2. Decoding             \n");
	printf("   0. Exit                    \n");
	printf("   => Input your choice number: ");
}

int main()
{
	MenuDisplay();
	int chooseNumber;
	scanf_s("%d", &chooseNumber);

	switch (chooseNumber) {
	case 1:
		huffmanEncodingMain();
		break;
	case 2:
		huffmanDecodingMain();
		break;
	default:
		break;
	}
	return 0;
}