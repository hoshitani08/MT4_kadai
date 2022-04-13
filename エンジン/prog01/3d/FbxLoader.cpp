#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

// 静的メンバ変数の実体
const std::string FbxLoader::baseDirectory = "Resources/";
const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader instance;
	return &instance;
}

void FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
{
	//行
	for (int i = 0; i < 4; i++)
	{
		//列
		for (int j = 0; j < 4; j++)
		{
			//1要素コピー
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}

void FbxLoader::Initialize(ID3D12Device* device)
{
	//再初期化チェック
	assert(fbxManager == nullptr);

	//引数からメンバ変数に代入
	this->device = device;

	//FBXマネージャの生成
	fbxManager = FbxManager::Create();

	//FBXマネージャの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	//FBXインポータの生成
	fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
	//各種FBXインスタンスの破棄
	fbxImporter->Destroy();
	fbxManager->Destroy();
}

std::unique_ptr<FbxModel> FbxLoader::LoadModelFromFile(const string& modelName)
{
	// モデルと同じ名前のフォルダから読み込む
	const string directoryPath = baseDirectory + modelName + "/";
	// 拡張子.fbxを付加
	const string fileName = modelName + ".fbx";
	// 連結してフルパスを得る
	const string fullpath = directoryPath + fileName;

	// ファイル名を指定してFBXファイルを読み込む
	if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings()))
	{
		assert(0);
	}

	// シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

	// ファイルからロードしたFBXの情報をシーンにインポート
	fbxImporter->Import(fbxScene);

	FbxGeometryConverter fbx_converter(fbxManager);
	fbx_converter.Triangulate(fbxScene, true);
	fbx_converter.RemoveBadPolygonsFromMeshes(fbxScene);

	// モデル生成
	FbxModel* model = new FbxModel();
	model->name = modelName;
	// FBXノードの数を取得
	int nodeCount = fbxScene->GetNodeCount();
	// あらかじめ必要数分のメモリを確保することで、アドレスがずれるのを予防
	model->nodes.reserve(nodeCount);
	// ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(model, fbxScene->GetRootNode());
	// FBXシーン解放
	//fbxScene->Destroy();
	model->fbxScene = fbxScene;
	// バッファ生成
	model->CreateBuffers(device);

	return std::unique_ptr<FbxModel>(model);
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent)
{
	//モデルにノードを追加
	model->nodes.emplace_back();
	Node& node = model->nodes.back();
	// ノード名を取得
	node.name = fbxNode->GetName();

	// FBXノードのローカル移動情報
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	// 形式変換して代入
	node.rotation = { (float)rotation[0] * (XM_PI / 180.0f), (float)rotation[1] * (XM_PI / 180.0f), (float)rotation[2] * (XM_PI / 180.0f), 0.0f };
	node.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.0f };
	node.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 1.0f };

	// スケール、回転、平行移動行列の計算
	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScalingFromVector(node.scaling);
	matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTranslation = XMMatrixTranslationFromVector(node.translation);

	// ローカル変形行列の計算
	node.transform = XMMatrixIdentity();
	node.transform *= matScaling; // ワールド行列にスケーリングを反映
	node.transform *= matRotation; // ワールド行列に回転を反映
	node.transform *= matTranslation; // ワールド行列に平行移動を反映

	//グローバル変形行列の計算
	node.globalTransform = node.transform;
	if (parent)
	{
		node.parent = parent;
		// 親の変形を乗算
		node.globalTransform *= parent->globalTransform;
	}

	// FBXノードのメッシュ情報を解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			model->meshNode = &node;
			ParseMesh(model, fbxNode);
		}
	}

	// 子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
	}
}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode)
{
	// ノードのメッシュを取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	// 頂点座標読み取り、面を構成するデータの読み取り
	ParseMeshFaces(model, fbxMesh);
	// マテリアルの読み取り
	ParseMaterial(model, fbxNode);
	//3頂点からローカル座標とUV座標からU軸(angent)とV軸(Binormal)を求める
	BuildTangentAndBiNormalImp(model);
	//スキニング情報の読み取り
	ParseSkin(model, fbxMesh);
}

void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh)
{
	auto& vertices = model->vertices;
	auto& indices = model->indices;

	// 1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);

	// 面の数
	const int polygonCount = fbxMesh->GetPolygonCount();

	// UVデータの数
	const int textureUVCount = fbxMesh->GetTextureUVCount();
	// UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//必要数だけ頂点データ配列を確保
	FbxModel::VertexPosNormalUvSkin vert{};
	model->vertices.resize(polygonCount * 3, vert);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4* pCoord = fbxMesh->GetControlPoints();

	int indexCount = 0;
	controlPointsData.resize(fbxMesh->GetControlPointsCount());

	// 面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++)
	{
		// 面を構成する頂点の数を取得（3なら三角形ポリゴン)
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 3);

		// 1頂点ずつ処理
		for (int j = 0; j < polygonSize; j++)
		{
			// FBX頂点配列のインデックス(コントロールポイントの座標データのインデックス)
			int controlPointIndex = fbxMesh->GetPolygonVertex(i, j);
			assert(controlPointIndex >= 0);

			controlPointsData[controlPointIndex].push_back(indexCount);

			FbxModel::VertexPosNormalUvSkin& vertex = vertices[indexCount];
			// 座標のコピー
			vertex.pos.x = (float)pCoord[controlPointIndex][0];
			vertex.pos.y = (float)pCoord[controlPointIndex][1];
			vertex.pos.z = (float)pCoord[controlPointIndex][2];

			// 頂点法線読込
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			// テクスチャUV読込
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;
				// 0番決め打ちで読込
				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1] * -1.0f;
				}
			}

			// インデックス配列に頂点インデックス追加
			// 3頂点目までなら
			if (j < 3)
			{
				// 1点追加し、他の2点と三角形を構築する
				indices.push_back(indexCount);
			}
			// 4頂点目
			else
			{
				// 3点追加し、四角形の0,1,2,3の内 2,3,0で三角形を構築する
				int index2 = indices[indices.size() - 1];
				int index3 = controlPointIndex;
				int index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}

			indexCount++;
		}
	}
}

void FbxLoader::ParseMaterial(FbxModel* model, FbxNode* fbxNode)
{
	const int materialCount = fbxNode->GetMaterialCount();
	if (materialCount > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		//テクスチャを読み込んだかどうかを表すフラグ
		bool textureLoaded = false;

		if (material)
		{
			// FbxSurfaceLambertクラスかどうかを調べる
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				model->ambient.x = (float)ambient.Get()[0];
				model->ambient.y = (float)ambient.Get()[1];
				model->ambient.z = (float)ambient.Get()[2];

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				model->diffuse.x = (float)diffuse.Get()[0];
				model->diffuse.y = (float)diffuse.Get()[1];
				model->diffuse.z = (float)diffuse.Get()[2];
			}

			//マテリアル名(デバック用)
			string name = material->GetName();
			// ディフューズテクスチャを取り出す
			const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			//ベースカラー
			const FbxProperty propBaseColor = FbxSurfaceMaterialUtils::GetProperty("baseColor", material);
			if (propBaseColor.IsValid())
			{
				const FbxFileTexture* texture = propBaseColor.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&model->baseTexture, baseDirectory + model->name + "/" + name);
					textureLoaded = true;
				}
			}
			else if (diffuseProperty.IsValid())
			{
				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&model->baseTexture, baseDirectory + model->name + "/" + name);
					textureLoaded = true;
				}
			}

			//金属度
			const FbxProperty propMetalness = FbxSurfaceMaterialUtils::GetProperty("metalness", material);
			if (propMetalness.IsValid())
			{
				const FbxFileTexture* texture = propMetalness.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&model->metalnessTexture, baseDirectory + model->name + "/" + name);
					textureLoaded = true;
				}
			}
			else if (diffuseProperty.IsValid())
			{
				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&model->metalnessTexture, baseDirectory + model->name + "/" + name);
					textureLoaded = true;
				}
			}

			//法線マップ
			const FbxProperty propNormalCamera = FbxSurfaceMaterialUtils::GetProperty("normalCamera", material);
			if (propNormalCamera.IsValid())
			{
				const FbxFileTexture* texture = propNormalCamera.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&model->normalTexture, baseDirectory + model->name + "/" + name);
				}
			}
			else if (diffuseProperty.IsValid())
			{
				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&model->normalTexture, baseDirectory + model->name + "/" + name);
					textureLoaded = true;
				}
			}
		}

		// テクスチャがない場合は白テクスチャを貼る
		if (!textureLoaded)
		{
			LoadTexture(&model->baseTexture, baseDirectory + defaultTextureFileName);
		}
	}
}

void FbxLoader::LoadTexture(TextureData* texData, const std::string& fullpath)
{
	HRESULT result = S_FALSE;

	// WICテクスチャのロード
	TexMetadata& metadata = texData->metaData;
	ScratchImage& scratchImg = texData->scratchImg;

	// ユニコード文字列に変換
	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));

	result = LoadFromWICFile
	(
		wfilepath, WIC_FLAGS_NONE,
		&metadata, scratchImg
	);
	if (FAILED(result))
	{
		assert(0);
	}
}

void FbxLoader::ParseSkin(FbxModel* model, FbxMesh* fbxMesh)
{
	//スキニング情報
	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	//スキニング情報がなければ終了
	if (fbxSkin == nullptr)
	{
		//各頂点について処理
		for (int i = 0; i < model->vertices.size(); i++)
		{
			//最初のボーン(単位行列)の影響100%にする
			model->vertices[i].boneIndex[0] = 0;
			model->vertices[i].boneWeight[0] = 1.0f;
		}
		return;
	}

	//ボーン配列の参照
	std::vector<FbxModel::Bone>& bones = model->bones;

	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーン自体のノードの名前を取得
		const char* boneName = fbxCluster->GetLink()->GetName();

		//新しくボーンを追加し、追加したボーンの参照を得る
		bones.emplace_back(FbxModel::Bone(boneName));
		FbxModel::Bone& bone = bones.back();
		//自作ボーンとFBXのボーンを紐づける
		bone.fbxCluster = fbxCluster;

		//FBXから初期姿勢行列を取得する
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//XMMatrix型に変換する
		XMMATRIX initialPose;
		ConvertMatrixFromFbx(&initialPose, fbxMat);

		//初期姿勢行列の逆行列を得る
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
	}

	//ボーン番号とスキンウェイトのペア
	struct WeightSet
	{
		UINT index;
		float weight;
	};

	//二次元配列(ジャグ配列)
	//list:頂点が影響を受けるボーンの全リスト
	//vector:それを全頂点分
	std::vector<std::list<WeightSet>> weightLists(controlPointsData.size());
	//頂点配列書き換え用参照
	auto& vertices = model->vertices;

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//このボーンに影響を受ける頂点の数
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
		//このボーンに影響を受ける頂点の配列
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controPointWeights = fbxCluster->GetControlPointWeights();

		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点番号
			int controlPointIndex = controlPointIndices[j];
			//スキンウェイト
			float weight = (float)controPointWeights[j];
			//その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加
			weightLists[controlPointIndex].emplace_back(WeightSet{ (UINT)i, weight });
		}
	}

	//各頂点について処理
	for (int i = 0; i < controlPointsData.size(); i++)
	{
		//頂点のウェイトから最も大きい4つを選択
		auto& weightList = weightLists[i];
		//大小比較用のラムダ式を指定して降順にソート
		weightList.sort
		(
			[](auto const& lhs, auto const& rhs)
			{
				//左の要素の方が大きければtrue、それでなければfalseを返す
				return lhs.weight > rhs.weight;
			}
		);

		if (weightList.size() > 4)
		{
			weightList.resize(4);
		}


		//降順ソート済みのウェイトリストから
		for (int j = 0; j < controlPointsData[i].size(); j++)
		{
			int weightArrayIndex = 0;

			for (auto& weightSet : weightList)
			{
				std::vector<int>& controlPoint = controlPointsData[i];
				int indexCount = controlPoint[j];

				vertices[indexCount].boneIndex[weightArrayIndex] = weightSet.index;
				vertices[indexCount].boneWeight[weightArrayIndex] = weightSet.weight;

				//4つに達したら終了
				if (++weightArrayIndex >= FbxModel::MAX_BONE_INDICES)
				{
					float weight = 0.0f;
					//2つ番以降のウェイトを合計
					for (int j = 1; j < FbxModel::MAX_BONE_INDICES; j++)
					{
						weight += vertices[indexCount].boneWeight[j];
					}

					//合計で1.0f(100%)になるように調整
					vertices[indexCount].boneWeight[0] = 1.0f - weight;
					break;
				}
			}
		}
	}
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
	size_t pos1;
	// 区切り文字 '\\' が出てくる一番最後の部分を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	// 区切り文字 '/' が出てくる一番最後の部分を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

void FbxLoader::BuildTangentAndBiNormalImp(FbxModel* model)
{
	auto& vertices = model->vertices;
	auto& indices = model->indices;

	//頂点スムースは気にしない。
	auto numPolygon = indices.size() / 3;
	for (auto polyNo = 0; polyNo < numPolygon; polyNo++)
	{
		auto no = polyNo * 3;
		auto vertNo_0 = indices[no];
		auto vertNo_1 = indices[no + 1];
		auto vertNo_2 = indices[no + 2];

		auto& vert_0 = vertices[vertNo_0];
		auto& vert_1 = vertices[vertNo_1];
		auto& vert_2 = vertices[vertNo_2];

		XMFLOAT3 cp0[] =
		{
			{ vert_0.pos.x, vert_0.uv.x, vert_0.uv.y},
			{ vert_0.pos.y, vert_0.uv.x, vert_0.uv.y},
			{ vert_0.pos.z, vert_0.uv.x, vert_0.uv.y}
		};

		XMFLOAT3 cp1[] =
		{
			{ vert_1.pos.x, vert_1.uv.x, vert_1.uv.y},
			{ vert_1.pos.y, vert_1.uv.x, vert_1.uv.y},
			{ vert_1.pos.z, vert_1.uv.x, vert_1.uv.y}
		};

		XMFLOAT3 cp2[] =
		{
			{ vert_2.pos.x, vert_2.uv.x, vert_2.uv.y},
			{ vert_2.pos.y, vert_2.uv.x, vert_2.uv.y},
			{ vert_2.pos.z, vert_2.uv.x, vert_2.uv.y}
		};

		// 平面パラメータからUV軸座標算出する。
		float U[3], V[3];
		for (int i = 0; i < 3; ++i)
		{
			XMFLOAT3 V1 =
			{
				cp1[i].x - cp0[i].x,
				cp1[i].y - cp0[i].y,
				cp1[i].z - cp0[i].z
			};

			XMFLOAT3 V2 =
			{
				cp2[i].x - cp1[i].x,
				cp2[i].y - cp1[i].y,
				cp2[i].z - cp1[i].z
			};

			// 外積を計算
			XMFLOAT3 ABC = Cross(V1, V2);
			if (ABC.x == 0.0f)
			{
				U[i] = 0.0f;
				V[i] = 0.0f;
			}
			else
			{
				U[i] = -ABC.y / ABC.x;
				V[i] = -ABC.z / ABC.x;
			}
		}

		XMFLOAT3 tangent = { U[0], U[1], U[2] };
		XMFLOAT3 binormal = { V[0], V[1], V[2] };

		// ベクトルを正規化
		tangent = Normalize(tangent);
		binormal = Normalize(binormal);

		// ベクトルの加算(tangent)
		vert_0.tangent = Add(vert_0.tangent, tangent);
		vert_1.tangent = Add(vert_1.tangent, tangent);
		vert_2.tangent = Add(vert_2.tangent, tangent);
		// ベクトルの加算(binormal)
		vert_0.binormal = Add(vert_0.binormal, binormal);
		vert_1.binormal = Add(vert_1.binormal, binormal);
		vert_2.binormal = Add(vert_2.binormal, binormal);
	}

	//法線、接ベクトル、従ベクトルを平均化する。
	for (auto& vert : vertices)
	{
		vert.tangent = Normalize(vert.tangent);
		vert.binormal = Normalize(vert.binormal);
	}
}

const XMFLOAT3& FbxLoader::Add(XMFLOAT3 m, XMFLOAT3 k)
{
	XMVECTOR xmv0 = XMLoadFloat3(&m);
	XMVECTOR xmv1 = XMLoadFloat3(&k);
	XMVECTOR xmvr = XMVectorAdd(xmv0, xmv1);

	return XMFLOAT3(xmvr.m128_f32[0], xmvr.m128_f32[1], xmvr.m128_f32[2]);
}

const XMFLOAT3& FbxLoader::Normalize(XMFLOAT3 m)
{
	XMVECTOR xmv = XMLoadFloat3(&m);
	xmv = XMVector3Normalize(xmv);

	return XMFLOAT3(xmv.m128_f32[0], xmv.m128_f32[1], xmv.m128_f32[2]);
}

const XMFLOAT3& FbxLoader::Cross(XMFLOAT3 m, XMFLOAT3 k)
{
	XMVECTOR xmv0 = DirectX::XMLoadFloat3(&m);
	XMVECTOR xmv1 = DirectX::XMLoadFloat3(&k);
	XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);

	return XMFLOAT3(xmvr.m128_f32[0], xmvr.m128_f32[1], xmvr.m128_f32[2]);
}