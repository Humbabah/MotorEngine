#include "stdafx.h"
#include "Animation.h"
#include "AnimationController.h"


CAnimation::CAnimation()
	: myController(nullptr)
	, myTotalAnimationTime(0)
{}
CAnimation::~CAnimation()
{
	delete myController;
	myController = nullptr;
}

void CAnimation::Init(const char* aRig, std::vector<std::string>& somePathsToAnimations)
{
	myController = new AnimationController(aRig);
	myController->Import3DFromFile(aRig);
	for (std::string s : somePathsToAnimations)
	{
		myController->Add3DAnimFromFile(s);
	}

	//myController->Release();
	//myController->SetAnimIndex(1, true, 0.0f);
	//myController->SetAnimIndex(2, true, 5.0f);
}
void CAnimation::BoneTransformsWithBlend(SlimMatrix44* Transforms, float aBlendFactor)
{
	std::vector<aiMatrix4x4> trans;
	myController->BoneTransformWithBlend(trans);
	myController->SetBlendTime(aBlendFactor);

	memcpy(&Transforms[0], &trans[0], (sizeof(float) * 16) * trans.size());
}
void CAnimation::BoneTransforms(SlimMatrix44* Transforms, const float anAnimSpeedMultiplier)
{
	std::vector<aiMatrix4x4> trans;
	myController->BoneTransform(trans, anAnimSpeedMultiplier);
	if (trans.size() == 0)
	{
		return;
	}
	memcpy(&Transforms[0], &trans[0], (sizeof(float) * 16) * trans.size());
}

void CAnimation::BlendStep(float aDelta)
{
	myTotalAnimationTime += aDelta;
	myController->UpdateBlendFrame();
}

void CAnimation::Step()
{
	//THIS DOES NOTHING WTF
	/*myTotalAnimationTime += aDelta;*/
	myController->UpdateFrame();
}

const size_t CAnimation::GetNrOfAnimations() const
{
	return myController->GetNrOfAnimations(); 
}

void CAnimation::SetCurAnimationScene(const int aCurAnimScene)
{
	myController->SetCurSceneIndex(aCurAnimScene);
}

// Old: before 2020 11 13
/*
	void CAnimation::Step(float aDelta)
	{
		// commented 2020 11 12 - The if check is used for nothing. The commented contents were already not in use.
		//if (myController->IsDoneBlending())
		//{
		//	//myController->SetAnimIndex();
		//	//myController->SetAnimIndex(2, true, 5);
		//}
		
		myTotalAnimationTime += aDelta;
		myController->Update();
	}

*/