#include "Audio.h"
#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

Audio::Audio()
{

}

Audio::~Audio()
{
	xAudio2.Reset();

	//読み込み済みサウンドの波形データを解放
	for (auto& pair : soundDatas)
	{
		delete pair.second.pBuffer;
	}
}

Audio* Audio::GetInstance()
{
	static Audio instance;
	return &instance;
}

bool Audio::Initialize()
{
	HRESULT result;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	return true;
}

void Audio::LoadWave(int soundNumber, const char* filename)
{
	SoundData soundData = {};

	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバリナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	if (file.fail())
	{
		assert(0);
	}

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format;
	file.read((char*)&format, sizeof(format));

	// Dataチャンクの読み込み
	Chunk data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	soundData.pBuffer = new char[data.size];
	file.read(soundData.pBuffer, data.size);
	//波形データのサイズ記録
	soundData.dataSize = data.size;

	// Waveファイルを閉じる
	file.close();

	// 波形フォーマットの設定
	memcpy(&soundData.wfex, &format.fmt, sizeof(format.fmt));
	soundData.wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	//連想配列に要素を追加
	soundDatas.insert(std::make_pair(soundNumber, soundData));
}

void Audio::PlayWave(int soundNumber)
{
	SoundData& soundData = soundDatas[soundNumber];

	HRESULT result;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex, 0, 2.0f, nullptr);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundData.pBuffer;
	buf.pContext = soundData.pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundData.dataSize;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

void Audio::LoopPlayWave(int soundNumber)
{
	SoundData& soundData = soundDatas[soundNumber];

	HRESULT result;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex, 0, 2.0f, nullptr);
	assert(SUCCEEDED(result));

	pSourceVoices.push_back(pSourceVoice);

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundData.pBuffer;
	buf.pContext = soundData.pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	buf.AudioBytes = soundData.dataSize;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

void Audio::LoopStopWave()
{
	HRESULT result;

	result = pSourceVoices[0]->Stop();
	result = pSourceVoices[0]->FlushSourceBuffers();
	result = pSourceVoices[0]->SubmitSourceBuffer(&buf);
	pSourceVoices.clear();
}