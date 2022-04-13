#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>
#include <map>
#include <vector>

class Audio final
{// オーディオ
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T1, class T2> using map = std::map<T1, T2>;

public: // サブクラス

	class XAudio2VoiceCallback : public IXAudio2VoiceCallback
	{// オーディオコールバック
	public:
		// ボイス処理パスの開始時
		//STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
		void OnVoiceProcessingPassStart(UINT32 BytesRequired) {};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd) (THIS) {};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext)
		{
			// バッファを解放する
			delete[] pBufferContext;
		};
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
		// ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
	};

	// チャンクヘッダ
	struct Chunk
	{
		char id[4]; // チャンク毎のID
		int	size; // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader
	{
		Chunk chunk;   // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk
	{
		Chunk chunk; // "fmt "
		WAVEFORMAT fmt; // 波形フォーマット
	};

	//音声データ
	struct SoundData
	{
		WAVEFORMATEX wfex; // 波形フォーマット
		char* pBuffer; // バッファの先頭アドレス
		unsigned int dataSize; // バッファのサイズ
	};

public: // メンバ関数
	// 初期化
	bool Initialize();
	// サウンドファイルの読み込み
	void LoadWave(int soundNumber, const char* filename);
	// サウンドファイルの再生
	void PlayWave(int soundNumber);
	void LoopPlayWave(int soundNumber);
	void LoopStopWave();

private:
	Audio();
	~Audio();

public:
	Audio(const Audio& audio) = delete;
	Audio& operator=(const Audio& audio) = delete;

	static Audio* GetInstance();

private: // メンバ変数
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;

	map<int, SoundData> soundDatas;
	std::vector<IXAudio2SourceVoice*> pSourceVoices;
	XAUDIO2_BUFFER buf{};
};