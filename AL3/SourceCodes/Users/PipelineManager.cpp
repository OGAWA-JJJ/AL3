#include "PipelineManager.h"

ObjPipelineSet PipelineManager::obj_normal;
ObjPipelineSet PipelineManager::obj_shadow;
ObjPipelineSet PipelineManager::obj_receiveShadow;

FbxPipelineSet PipelineManager::fbx_normal;
FbxPipelineSet PipelineManager::fbx_shadow;

void PipelineManager::Init()
{
	ObjectInitData normalInit;
	normalInit.m_psEntryPoint = "PSmain";
	normalInit.m_vsEntryPoint = "VSmain";
	obj_normal = Object::CreateGraphicsPipeline(normalInit);

	ObjectInitData shadowInit;
	shadowInit.m_psEntryPoint = "PSBlack";
	shadowInit.m_vsEntryPoint = "VSShadowMain";
	obj_shadow = Object::CreateGraphicsPipeline(shadowInit);

	ObjectInitData receiveInit;
	receiveInit.m_psEntryPoint = "PSShadowMain";
	receiveInit.m_vsEntryPoint = "VSShadowMain";
	obj_receiveShadow = Object::CreateGraphicsPipeline(receiveInit);

	FbxInitData fbxNormalInit;
	fbxNormalInit.m_vsEntryPoint = "PSmain";
	fbxNormalInit.m_vsEntryPoint = "VSmain";
	fbx_normal = FbxObject3D::CreateGraphicsPipeline(fbxNormalInit);

	FbxInitData fbxShadowInit;
	fbxShadowInit.m_psEntryPoint = "PSBlack";
	fbxShadowInit.m_vsEntryPoint = "VSShadowMain";
	fbx_shadow = FbxObject3D::CreateGraphicsPipeline(fbxShadowInit);
}
