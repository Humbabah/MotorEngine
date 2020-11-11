#include "stdafx.h"
#include "TextInstance.h"

namespace SM = DirectX::SimpleMath;

CTextInstance::CTextInstance() : myTextData(nullptr)
{
}

CTextInstance::~CTextInstance()
{
    myTextData = nullptr;
}

bool CTextInstance::Init(CText* aText)
{
    myTextData = aText;
    if (!myTextData) {
        return false;
    }

    return true;
}

void CTextInstance::SetText(std::string aString)
{
    myText = aString;
}

void CTextInstance::SetPosition(DirectX::SimpleMath::Vector2 aPosition)
{
    myPosition = aPosition;
}

void CTextInstance::SetColor(DirectX::SimpleMath::Vector4 aColor)
{
    myColor = aColor;
}