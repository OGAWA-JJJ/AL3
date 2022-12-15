#include "PipelineManager.h"

ObjPipelineSet PipelineManager::obj_normal;
ObjPipelineSet PipelineManager::obj_shadow;
ObjPipelineSet PipelineManager::obj_receiveShadow;
ObjPipelineSet PipelineManager::obj_texColorReceice;

FbxObjects::FbxPipelineSet PipelineManager::fbx_normal;
FbxObjects::FbxPipelineSet PipelineManager::fbx_shadow;
FbxObjects::FbxPipelineSet PipelineManager::fbx_receiveShadow;

void PipelineManager::Init()
{
	ObjectInitData normalInit;
	normalInit.m_psEntryPoint = "PSmain";
	normalInit.m_vsEntryPoint = "VSmain";
	obj_normal = Object::CreateGraphicsPipeline(normalInit);

	ObjectInitData shadowInit;
	shadowInit.m_psEntryPoint = "PSBlack";
	shadowInit.m_vsEntryPoint = "VSShadowTex";
	obj_shadow = Object::CreateGraphicsPipeline(shadowInit);

	ObjectInitData receiveInit;
	receiveInit.m_psEntryPoint = "PSShadowMain";
	receiveInit.m_vsEntryPoint = "VSShadowReceive";
	obj_receiveShadow = Object::CreateGraphicsPipeline(receiveInit);

	ObjectInitData texInit;
	texInit.m_psEntryPoint = "PSTexture";
	texInit.m_vsEntryPoint = "VSmain";
	obj_texColorReceice = Object::CreateGraphicsPipeline(texInit);

	FbxObjects::FbxInitData fbxNormalInit;
	fbxNormalInit.m_vsEntryPoint = "PSmain";
	fbxNormalInit.m_vsEntryPoint = "VSmain";
	fbx_normal = FbxObjects::CreateGraphicsPipeline(fbxNormalInit);

	FbxObjects::FbxInitData fbxShadowInit;
	fbxShadowInit.m_psEntryPoint = "PSBlack";
	fbxShadowInit.m_vsEntryPoint = "VSShadowTex";
	fbx_shadow = FbxObjects::CreateGraphicsPipeline(fbxShadowInit);

	FbxObjects::FbxInitData fbxReceiveInit;
	fbxReceiveInit.m_psEntryPoint = "PSShadowMain";
	fbxReceiveInit.m_vsEntryPoint = "VSShadowReceive";
	fbx_receiveShadow = FbxObjects::CreateGraphicsPipeline(fbxReceiveInit);
}
