#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

class MapChip final
{
private: // エイリアス
	// std::を省略
	using string = std::string;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;

public: // サブクラス
	// アニメーション用データ構造体
	struct MapChipData
	{
		string mapName;
		std::vector<int> mapCsvNumber;
		int mapChipMaxX;
		int mapChipMaxY;
	};

private:
	MapChip();
	~MapChip();

public:
	MapChip(const MapChip& mapChip) = delete;
	MapChip& operator=(const MapChip& mapChip) = delete;

	static MapChip* GetInstance();

public:
	// ルートパス
	static const string baseDirectory;

public:
	//CSVファイル読み込み
	void CsvLoad(int mapChipMaxX, int mapChipMaxY, std::string fName);
	//マップナンバー指定
	int GetChipNum(int x, int y, std::string fName, int mapChipSize = 1);
	//大きさの最大を取得
	const XMFLOAT2& GetMapChipMaxXY(std::string fName);

private:
	std::vector<MapChipData> mapData;
};