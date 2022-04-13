#include "ObjFactory.h"

// Ã“Iƒƒ“ƒo•Ï”‚ÌÀ‘Ì
const std::string ObjFactory::baseDirectory = "Resources/";

void ObjFactory::Initialize()
{
}

void ObjFactory::Finalize()
{
	for (auto& a : modelData)
	{
		a.model.reset();
		a.fName.clear();
	}
}

void ObjFactory::LoadModel(std::string fName, bool smoothing)
{
	ModelData tempData;
	tempData.fName = baseDirectory + fName + ".obj";
	tempData.model = Model::CreateFromObject(fName, smoothing);

	modelData.push_back(std::move(tempData));
}

Model* ObjFactory::GetModel(std::string fName)
{
	for (int i = 0; i < modelData.size(); i++)
	{
		if (modelData[i].fName == baseDirectory + fName + ".obj")
		{
			return modelData[i].model.get();
		}
	}

	return nullptr;
}

ObjFactory* ObjFactory::GetInstance()
{
	static ObjFactory instance;
	return &instance;
}