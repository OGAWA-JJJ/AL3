#pragma once
#include "../Singleton.h"

class DescriptorHeap:public Singleton<DescriptorHeap>
{
public:
	friend class Singleton<DescriptorHeap>;

private:
	DescriptorHeap();

public:
	void CreateDescriptorHeap();
};

