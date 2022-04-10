#pragma once
#include <vector>

class GaussianBlur
{
public:
	static std::vector<float> GetGaussianWeights(size_t count, float s);


};

