#pragma once
#include "../3D/Object.h"
#include "../3D/FbxObjects.h"

class PipelineManager
{
public:	//パイプライン
	static ObjPipelineSet obj_normal;
	static ObjPipelineSet obj_shadow;
	static ObjPipelineSet obj_receiveShadow;
	static ObjPipelineSet obj_texColorReceice;

	static FbxObjects::FbxPipelineSet fbx_normal;
	static FbxObjects::FbxPipelineSet fbx_shadow;
	static FbxObjects::FbxPipelineSet fbx_receiveShadow;

public:
	static void Init();
};

