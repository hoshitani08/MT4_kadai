#pragma once
#include <string>
#include <vector>
#include <memory>

#include "Model.h"

class ObjFactory final
{
public: // サブクラス
	struct ModelData
	{
		std::string fName;
		std::unique_ptr<Model> model;
	};

public:
	// ルートパス
	static const std::string baseDirectory;

private:
	ObjFactory() = default;
	~ObjFactory() = default;

public:
	ObjFactory(const ObjFactory& objFactory) = delete;
	ObjFactory& operator=(const ObjFactory& objFactory) = delete;

	// 初期化
	void Initialize();
	// 終了
	void Finalize();
	// モデルのロード
	void LoadModel(std::string fName, bool smoothing = false);
	// モデルの取得
	Model* GetModel(std::string fName);
	// インスタンス生成
	static ObjFactory* GetInstance();

private:
	std::vector<ModelData> modelData;
};