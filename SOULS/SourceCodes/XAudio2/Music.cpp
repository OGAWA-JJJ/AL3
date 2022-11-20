#include "Music.h"
#include <fstream>
#include <cassert>

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

void Music::PlayWave(const char* filename, float volume, bool isLoop)
{
	HRESULT result = S_OK;

	/*�t�@�C�����̓X�g���[���̃C���X�^���X*/
	std::ifstream file;

	/*.wav�t�@�C�����o�C�i�����[�h�ŊJ��*/
	file.open(filename, std::ios_base::binary);

	/*�t�@�C���I�[�v�����s�����o����*/
	if (file.fail()) { assert(0); }

	/*RIFF�w�b�_�[�̓ǂݍ���*/
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	/*�t�@�C����RIFF���`�F�b�N*/
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) { assert(0); }

	/*Format�`�����N�̓ǂݍ���*/
	FormatChunk format;
	file.read((char*)&format, sizeof(format));

	/*Data�`�����N�̓ǂݍ���*/
	Chunk data;
	file.read((char*)&data, sizeof(data));

	/*Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���*/
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	/*Wave�t�@�C�������*/
	file.close();

	WAVEFORMATEX wfex{};

	/*�g�`�t�H�[�}�b�g�̐ݒ�*/
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	/*�g�`�t�H�[�}�b�g������SourceVoice�̐���*/
	//IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	//result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if FAILED(result) {
		delete[] pBuffer;
		return;
	}

	/*�Đ�����g�`�f�[�^�̐ݒ�*/
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)pBuffer;
	buf.pContext = pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;

	/*�g�`�f�[�^�̍Đ�*/
	pSourceVoice->SetVolume(volume);
	if (isLoop) { buf.LoopCount = XAUDIO2_MAX_LOOP_COUNT; }
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
