#pragma once

#include "fbxsdk.h"
#include "FbxModel.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

using namespace DirectX;

class FbxLoader
{
private: // エイリアス
	// std::を省略
	using string = std::string;
public: // 定数
	// モデル格納ルートパス
	static const string baseDirectory;
	// テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;
public:
	// シングルトンインスタンスの取得
	static FbxLoader* GetInstance();
	//FBXの行列をXMMatrixに変換
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

public:
	// 初期化
	void Initialize(ID3D12Device* device);
	// 後始末
	void Finalize();
	// ファイルからFBXモデル読込
	std::unique_ptr<FbxModel> LoadModelFromFile(const string& modelName);

private:
	// D3D12デバイス
	ID3D12Device* device = nullptr;
	// FBXマネージャ
	FbxManager* fbxManager = nullptr;
	// FBXインポータ
	FbxImporter* fbxImporter = nullptr;
	// コントロールポイントのデータ
	std::vector<std::vector<int>> controlPointsData;

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;
	// 再帰的にノード構成を解析
	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);
	// メッシュ読み取り
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	// 頂点座標読み取り、面情報読み取り
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	// マテリアル読み取り
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	// テクスチャ読み込み
	void LoadTexture(TextureData* texData, const std::string& fullpath);
	//スキニング情報の読み取り
	void ParseSkin(FbxModel* model, FbxMesh* fbxMesh);
	// ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);
	//3頂点からローカル座標とUV座標からU軸(angent)とV軸(Binormal)を求める
	void BuildTangentAndBiNormalImp(FbxModel* model);

	const XMFLOAT3& Add(XMFLOAT3 m, XMFLOAT3 k);
	const XMFLOAT3& Normalize(XMFLOAT3 m);
	const XMFLOAT3& Cross(XMFLOAT3 m, XMFLOAT3 k);
};