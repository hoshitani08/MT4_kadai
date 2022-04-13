#pragma once
#include <string>
#include <vector>
#include <memory>
#include "FbxModel.h"
#include "FbxLoader.h"

class FbxFactory final
{
public: // サブクラス
	struct ModelData
	{
		std::string fName;
		std::unique_ptr<FbxModel> fbxModel;
	};

public:
	// ルートパス
	static const std::string baseDirectory;

private:
	FbxFactory() = default;
	~FbxFactory() = default;

public:
	FbxFactory(const FbxFactory & fbxFactory) = delete;
	FbxFactory& operator=(const FbxFactory & fbxFactory) = delete;

	// 初期化
	void Initialize();
	// 終了
	void Finalize();
	// モデルのロード
	void LoadModel(std::string fName);
	// モデルの取得
	FbxModel* GetModel(std::string fName);
	// インスタンス生成
	static FbxFactory* GetInstance();

private:
	std::vector<ModelData> modelData;
};