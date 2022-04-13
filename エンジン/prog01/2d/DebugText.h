#pragma once

#include "Sprite.h"
#include <Windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>

class DebugText final
{
private: // エイリアス
	using XMVECTOR = DirectX::XMVECTOR;

public:
	// デバッグテキスト用のテクスチャ番号を指定
	static const int maxCharCount = 256; // 最大文字数
	static const int fontWidth = 9; // フォント画像内1文字分の横幅
	static const int fontHeight = 18; // フォント画像内1文字分の縦幅
	static const int fontLineCount = 14; // フォント画像内1行分の文字数

public:// 静的メンバ関数
	static DebugText* GetInstance();

public:// メンバ関数

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float size);

	void VariablePrint(float x, float y, const std::string& text, float i, float size);

	void DrawAll(ID3D12GraphicsCommandList* cmdList);

	DebugText(const DebugText&) = delete;
	DebugText& operator=(const DebugText&) = delete;

private:
	DebugText();
	~DebugText();

private:
	// スプライトデータの配列
	std::unique_ptr<Sprite> spriteDatas[maxCharCount];
	// スプライトデータ配列の添え字番号
	int spriteIndex = 0;

	XMVECTOR variable;
};