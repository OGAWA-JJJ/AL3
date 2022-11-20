#include "GaussianBlur.h"
#include "../DirectX/DirectXImportant.h"

std::vector<float> GaussianBlur::GetGaussianWeights(size_t count, float s)
{
	std::vector <float> weights(count);

	//ウェイト配列返却用
	float x = 0.0f;
	float total = 0.0f;
	for (auto& wgt : weights) {
		wgt = expf(-(x * x) / (2 * s * s));
		total += wgt;
		x += 1.0f;
	}

	total = total * 2.0f - 1;

	//足して1になるようにする
	for (auto& wgt : weights) { wgt /= total; }

	/*auto result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(
			
		)
	)*/

	return weights;
}
