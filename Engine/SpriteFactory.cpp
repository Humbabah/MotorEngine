#include "stdafx.h"
#include "SpriteFactory.h"
#include "Sprite.h"
#include "DDSTextureLoader.h"
#include "AnimatedUIElement.h"

#include "rapidjson\document.h"
#include "rapidjson\istreamwrapper.h"

CSpriteFactory* CSpriteFactory::ourInstance = nullptr;

CSpriteFactory::CSpriteFactory() : myVertexShader(nullptr), myPixelShader(nullptr), myGeometryShader(nullptr)
{
	ourInstance = this;
	myFramework = nullptr;
}

CSpriteFactory::~CSpriteFactory()
{
	ourInstance = nullptr;
	myFramework = nullptr;
}

bool CSpriteFactory::Init(CDirectXFramework* aFramework)
{
	if (!aFramework) {
		return false;
	}

	myFramework = aFramework;

	//Start Shaders
	std::ifstream vsFile;
	vsFile.open("SpriteVertexShader.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader), "Sprite Vertex Shader could not be created.");
	vsFile.close();

	std::ifstream psFile;
	psFile.open("SpritePixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader), "Sprite Pixel Shader could not be created.");
	psFile.close();

	std::ifstream gsFile;
	gsFile.open("SpriteGeometryShader.cso", std::ios::binary);
	std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, &myGeometryShader), "Sprite Geometry Shader could not be created.");
	gsFile.close();
	//End Shaders

	return true;
}

CSprite* CSpriteFactory::LoadSprite(std::string aTexturePath)
{
	//Start Sampler
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateSamplerState(&samplerDesc, &sampler), "Sampler could not be created.");
	//End Sampler

	ID3D11ShaderResourceView* shaderResourceView = GetShaderResourceView(myFramework->GetDevice(), aTexturePath);

	CSprite* sprite = new CSprite();
	if (!sprite) {
		return nullptr;
	}

	CSprite::SSpriteData spriteData;

	spriteData.myVertexShader = myVertexShader;
	spriteData.myPixelShader = myPixelShader;
	spriteData.myGeometryShader = myGeometryShader;
	spriteData.mySampler = sampler;
	spriteData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	spriteData.myTexture = shaderResourceView;

	sprite->Init(spriteData);
	return sprite;
}

SAnimatedSpriteData* CSpriteFactory::LoadVFXSprite(std::string aFilePath)
{
	using namespace rapidjson;

	std::ifstream input_stream(aFilePath);
	IStreamWrapper input_wrapper(input_stream);
	Document document;
	document.ParseStream(input_wrapper);

	SAnimatedSpriteData* spriteData = new SAnimatedSpriteData();

	spriteData->scrollSpeed1 = { document["Scroll Speed 1 X"].GetFloat(), document["Scroll Speed 1 Y"].GetFloat() };
	spriteData->scrollSpeed2 = { document["Scroll Speed 2 X"].GetFloat(), document["Scroll Speed 2 Y"].GetFloat() };
	spriteData->scrollSpeed3 = { document["Scroll Speed 3 X"].GetFloat(), document["Scroll Speed 3 Y"].GetFloat() };
	spriteData->scrollSpeed4 = { document["Scroll Speed 4 X"].GetFloat(), document["Scroll Speed 4 Y"].GetFloat() };
	spriteData->uvScale1 = document["UV Scale 1"].GetFloat();
	spriteData->uvScale2 = document["UV Scale 2"].GetFloat();
	spriteData->uvScale3 = document["UV Scale 3"].GetFloat();
	spriteData->uvScale4 = document["UV Scale 4"].GetFloat();
	spriteData->glowColor = { document["Glow Color R"].GetFloat(), document["Glow Color G"].GetFloat(), document["Glow Color B"].GetFloat() };
	spriteData->glowWidth = { document["Glow Width"].GetFloat() };
	spriteData->verticalDirectionOfChange = document["Vertical Direction Of Change"].GetBool();

	std::ifstream psFile;
	psFile.open("SpriteVFXTextureBlendingPixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	ENGINE_HR_BOOL_MESSAGE(myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader), "Pixel Shader could not be created.");
	psFile.close();

	//Start Sampler
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateSamplerState(&samplerDesc, &sampler), "Sampler could not be created.");
	//End Sampler

	ID3D11ShaderResourceView* textureOneShaderResourceView = GetShaderResourceView(myFramework->GetDevice(), document["Texture 1"].GetString());
	ID3D11ShaderResourceView* textureTwoShaderResourceView = GetShaderResourceView(myFramework->GetDevice(), document["Texture 2"].GetString());
	ID3D11ShaderResourceView* textureThreeShaderResourceView = GetShaderResourceView(myFramework->GetDevice(), document["Texture 3"].GetString());
	ID3D11ShaderResourceView* textureFourShaderResourceView = GetShaderResourceView(myFramework->GetDevice(), document["Texture Mask"].GetString());

	spriteData->myPixelShader = pixelShader;
	spriteData->myTexture[0] = textureOneShaderResourceView;
	spriteData->myTexture[1] = textureTwoShaderResourceView;
	spriteData->myTexture[2] = textureThreeShaderResourceView;
	spriteData->myTexture[3] = textureFourShaderResourceView;

	return spriteData;
}

CSprite* CSpriteFactory::GetSprite(std::string aTexturePath)
{
	if (mySpriteMap.find(aTexturePath) == mySpriteMap.end())
	{
		return LoadSprite(aTexturePath);
	}
	return mySpriteMap.at(aTexturePath);
}

SAnimatedSpriteData* CSpriteFactory::GetVFXSprite(std::string aFilePath)
{
	if (myVFXSpriteMap.find(aFilePath) == myVFXSpriteMap.end())
	{
		return LoadVFXSprite(aFilePath);
	}
	return myVFXSpriteMap.at(aFilePath);
}

ID3D11ShaderResourceView* CSpriteFactory::GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath) {
	ID3D11ShaderResourceView* shaderResourceView;

	wchar_t* widePath = new wchar_t[aTexturePath.length() + 1];
	std::copy(aTexturePath.begin(), aTexturePath.end(), widePath);
	widePath[aTexturePath.length()] = 0;

	////==ENABLE FOR TEXTURE CHECKING==
	//ENGINE_HR_MESSAGE(DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView), aTexturePath.append(" could not be found.").c_str());
	////===============================

	//==DISABLE FOR TEXTURE CHECKING==
	HRESULT result;
	result = DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView);
	if (FAILED(result))
		DirectX::CreateDDSTextureFromFile(aDevice, L"ErrorTexture.dds", nullptr, &shaderResourceView);
	//================================

	delete[] widePath;
	return shaderResourceView;
}

CSpriteFactory* CSpriteFactory::GetInstance()
{
	return ourInstance;
}

