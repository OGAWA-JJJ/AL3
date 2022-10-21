#pragma once
#include "../3D/FbxObject3D.h"
#include "../3D/Object.h"
#include "../3D/FbxObjects.h"

class PipelineManager
{
public:	//パイプライン
	static ObjPipelineSet obj_normal;
	static ObjPipelineSet obj_shadow;
	static ObjPipelineSet obj_receiveShadow;

	static FbxObjects::FbxPipelineSet fbx_normal;
	static FbxObjects::FbxPipelineSet fbx_shadow;

public:
	static void Init();
};

