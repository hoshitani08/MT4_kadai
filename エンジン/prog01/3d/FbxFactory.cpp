#include "FbxFactory.h"
#include "FbxLoader.h"

// Ã“Iƒƒ“ƒo•Ï”‚ÌŽÀ‘Ì
const std::string FbxFactory::baseDirectory = "Resources/";

void FbxFactory::Initialize()
{
	LoadModel("uma");
	LoadModel("a");
}

void FbxFactory::Finalize()
{
	for (auto& a : modelData)
	{
		a.fbxModel.reset();
		a.fName.clear();
	}
}

void FbxFactory::LoadModel(std::string fName)
{
	ModelData tempData;
	tempData.fName = baseDirectory + fName + ".fbx";
	tempData.fbxModel = FbxLoader::GetInstance()->LoadModelFromFile(fName);

	modelData.push_back(std::move(tempData));
}

FbxModel* FbxFactory::GetModel(std::string fName)
{
	for (int i = 0; i < modelData.size(); i++)
	{
		if (modelData[i].fName == baseDirectory + fName + ".fbx")
		{
			return modelData[i].fbxModel.get();
		}
	}

	return nullptr;
}

FbxFactory* FbxFactory::GetInstance()
{
	static FbxFactory instance;
	return &instance;
}