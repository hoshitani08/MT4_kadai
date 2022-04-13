#include "MapChip.h"

using namespace DirectX;

// 静的メンバ変数の実体
const std::string MapChip::baseDirectory = "Resources/";

MapChip::MapChip()
{
}

MapChip::~MapChip()
{
}

MapChip* MapChip::GetInstance()
{
	static MapChip instance;
	return &instance;
}

void MapChip::CsvLoad(int mapChipMaxX, int mapChipMaxY, std::string fName)
{
	std::ifstream ifs(baseDirectory + fName + ".csv");
	std::string line;
	//仮データ作成
	MapChipData tempData;
	while (getline(ifs, line))
	{
		std::istringstream stream(line);
		std::string field;
		std::vector<int> result;
		while (std::getline(stream, field, ','))
		{
			result.push_back(stoi(field));
		}
		for (auto i : result)
		{
			tempData.mapCsvNumber.push_back(i);
		}
	}
	//ファイル名を保存
	tempData.mapName = baseDirectory + fName + ".csv";
	//Xの最大値を保存
	tempData.mapChipMaxX = mapChipMaxX;
	//Yの最大値を保存
	tempData.mapChipMaxY = mapChipMaxY;
	//データ構造体に保存
	mapData.push_back(tempData);
}

int MapChip::GetChipNum(int x, int y, std::string fName, int mapChipSize)
{
	const int X = x / mapChipSize;
	const int Y = y / mapChipSize;
	int count = 0;

	for (int i = 0; i < mapData.size(); i++)
	{
		if (mapData[i].mapName == baseDirectory + fName + ".csv")
		{
			break;
		}
		count++;
	}

	if (X < 0 || X >= mapData[count].mapChipMaxX || Y < 0 || Y >= mapData[count].mapChipMaxY)
	{
		assert(0);
	}

	std::vector<int> map = mapData[count].mapCsvNumber;

	return map[Y * mapData[count].mapChipMaxX + X];
}

const XMFLOAT2& MapChip::GetMapChipMaxXY(std::string fName)
{
	int count = 0;

	for (int i = 0; i < mapData.size(); i++)
	{
		if (mapData[i].mapName == baseDirectory + fName + ".csv")
		{
			return XMFLOAT2(mapData[count].mapChipMaxX, mapData[count].mapChipMaxY);
		}
		count++;
	}
	//ファイルがなかった場合
	return XMFLOAT2(0,0);
}