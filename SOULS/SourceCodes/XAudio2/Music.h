#pragma once
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
	/*チャンクヘッダ*/
	struct  Chunk {
		char id[4];   //チャンク毎のID
		int32_t size; //チャンクサイズ
	};

	/*RIFFヘッダチャンク*/
	struct  RiffHeader {
		Chunk chunk;  //RIFF
		char type[4]; //WAVE
	};

	struct FormatChunk {
		Chunk chunk;    //FMT
		WAVEFORMAT fmt; //波形フォーマット
	};

public:
	Music();
	~Music();
	void PlayWave(const char* filename, float volume, bool isLoop = false);
	void StopWave() { pSourceVoice->Stop(); }

};

class XAudio2VoiceCallback :public IXAudio2VoiceCallback
{
public:
	//ボイス処理パスの開始時
	STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//バッファの使用開始時
	STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};
	//バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext) { delete[] pBufferContext; };
	//再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBufferContext) {};
	//ボイスの実行エラー
	STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};
};

