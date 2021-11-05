#include "Music.h"

XAudio2VoiceCallback voiceCallback;

Music::Music()
{
	HRESULT result = S_OK;
	result = XAudio2Create(
		&xAudio2,
		0,
		XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

void Music::PlayWave(const char* filename,float volume)
{
	HRESULT result = S_OK;

	/*ファイル入力ストリームのインスタンス*/
	std::ifstream file;

	/*.wavファイルをバイナリモードで開く*/
	file.open(filename, std::ios_base::binary);

	/*ファイルオープン失敗を検出する*/
	if (file.fail()) { assert(0); }

	/*RIFFヘッダーの読み込み*/
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	/*ファイルがRIFFかチェック*/
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) { assert(0); }

	/*Formatチャンクの読み込み*/
	FormatChunk format;
	file.read((char*)&format, sizeof(format));

	/*Dataチャンクの読み込み*/
	Chunk data;
	file.read((char*)&data, sizeof(data));

	/*Dataチャンクのデータ部（波形データ）の読み込み*/
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	/*Waveファイルを閉じる*/
	file.close();

	WAVEFORMATEX wfex{};

	/*波形フォーマットの設定*/
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	/*波形フォーマットを元にSourceVoiceの生成*/
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	//result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if FAILED(result) {
		delete[] pBuffer;
		return;
	}

	/*再生する波形データの設定*/
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)pBuffer;
	buf.pContext = pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;

	/*波形データの再生*/
	pSourceVoice->SetVolume(volume);
	//buf.LoopCount = XAUDIO2_MAX_LOOP_COUNT;
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
