#include "stdafx.h"
#include "AnimatedUIElement.h"
#include "SpriteFactory.h"
#include "SpriteInstance.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "Scene.h"

#include "rapidjson\document.h"
#include "rapidjson\istreamwrapper.h"

CAnimatedUIElement::CAnimatedUIElement(std::string aFilePath) : mySpriteInstance(nullptr), myLevel(1.0f)
{
    using namespace rapidjson;

    std::ifstream input_stream(aFilePath);
    IStreamWrapper input_wrapper(input_stream);
    Document document;
    document.ParseStream(input_wrapper);

    mySpriteInstance = new CSpriteInstance();
    mySpriteInstance->Init(CSpriteFactory::GetInstance()->GetSprite(document["Texture Overlay"].GetString()));
    myData = CSpriteFactory::GetInstance()->GetVFXSprite(aFilePath);
}

CAnimatedUIElement::~CAnimatedUIElement()
{
    delete mySpriteInstance;
    mySpriteInstance = nullptr;
}

void CAnimatedUIElement::Level(float aLevel)
{
    myLevel = aLevel;
}

float CAnimatedUIElement::Level() const
{
    return myLevel;
}

void CAnimatedUIElement::SetPosition(DirectX::SimpleMath::Vector2 aPosition)
{
    mySpriteInstance->SetPosition(aPosition);
}

CSpriteInstance* CAnimatedUIElement::GetInstance() const
{
    return mySpriteInstance;
}

SAnimatedSpriteData* CAnimatedUIElement::GetVFXBaseData()
{
    return myData;
}
