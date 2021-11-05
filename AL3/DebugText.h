#pragma once
#include "DirectX2D.h"

class DebugText
{
public:
	static const int maxCharCount = 256;
	static const int fontWidth = 9;
	static const int fontHeight = 18;
	static const int fontLineCount = 14;

private:
	DirectX2D directX2D[maxCharCount];
	int spriteIndex = 0;

public:
	DebugText();
	void Initialize(UINT texnumber);
};

