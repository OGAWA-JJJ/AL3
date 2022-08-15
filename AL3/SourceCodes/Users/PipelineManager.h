#pragma once
#include "../3D/FbxObject3D.h"
#include "../3D/Object.h"

class PipelineManager
{
private:	//パイプライン
	static ObjPipelineSet obj_normal;
	static ObjPipelineSet obj_shadow;
	static ObjPipelineSet obj_receiveShadow;

	static FbxPipelineSet fbx_normal;
	static FbxPipelineSet fbx_shadow;

public:
	static void Init();

public:
	static ObjPipelineSet& GetObjNormalPipeline() { return obj_normal; }
	static ObjPipelineSet& GetObjShadowPipeline() { return obj_shadow; }
	static ObjPipelineSet& GetObjReceiveShadowPipeline() { return obj_receiveShadow; }

	static FbxPipelineSet& GetFbxNormalPipeline() { return fbx_normal; }
	static FbxPipelineSet& GetFbxShadowPipeline() { return fbx_shadow; }
};

