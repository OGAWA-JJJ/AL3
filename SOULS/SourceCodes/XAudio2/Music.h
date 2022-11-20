#pragma once
//#include "../DirectX/DirectXImportant.h"
#include <wrl.h>
#include <xaudio2.h>
#include <cstdint>

#pragma comment(lib,"xaudio2.lib")

class Music
{
private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	IXAudio2SourceVoice* pSourceVoice = nullptr;

private:
	/*�`�����N�w�b�_*/
	struct  Chunk {
		char id[4];   //�`�����N����ID
		int32_t size; //�`�����N�T�C�Y
	};

	/*RIFF�w�b�_�`�����N*/
	struct  RiffHeader {
		Chunk chunk;  //RIFF
		char type[4]; //WAVE
	};

	struct FormatChunk {
		Chunk chunk;    //FMT
		WAVEFORMAT fmt; //�g�`�t�H�[�}�b�g
	};

public:
	Music();
	void PlayWave(const char* filename, float volume, bool isLoop = false);
	void StopWave() { pSourceVoice->Stop(); }

};

class XAudio2VoiceCallback :public IXAudio2VoiceCallback
{
public:
	//�{�C�X�����p�X�̊J�n��
	STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
	//�{�C�X�����p�X�̏I����
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//�o�b�t�@�X�g���[���̍Đ����I��������
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//�o�b�t�@�̎g�p�J�n��
	STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};
	//�o�b�t�@�̖����ɒB������
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext) { delete[] pBufferContext; };
	//�Đ������[�v�ʒu�ɒB������
	STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBufferContext) {};
	//�{�C�X�̎��s�G���[
	STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};
};

