#pragma once

#include "include.h"
#include "huffman.h"

void huffmanEncodingMain();
void huffmanDecodingMain();
void writeHuffmanEncodingInfo(char* fileName, int numOfCharacter, int beforeSize, int afterSize);
void writeHuffmanDecodingInfo(char* fileName, int numOfCharacter);