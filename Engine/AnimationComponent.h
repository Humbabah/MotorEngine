#pragma once
#include "Component.h"

#include "SimpleMath.h"
#include "ModelMath.h"

struct SAnimationBlend
{
	int myFirst = -1;
	int mySecond = -1;
	float myBlendLerp = 0;
};

class CGameObject;
class CAnimation;
class CAnimationComponent : public CComponent
{
public:
	CAnimationComponent(CGameObject& aParent);
	~CAnimationComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

public:
	CAnimation* GetMyAnimation() { return myAnimation; }

public:
	std::array<SlimMatrix44, 64> GetBones() { return myBones; }
	void GetAnimatedTransforms(float dt, SlimMatrix44* transforms);
	void SetBlend(int anAnimationIndex, int anAnimationIndexTwo, float aBlend);

private:
	CAnimation* myAnimation;
	std::array<SlimMatrix44, 64> myBones { };
	SAnimationBlend myBlend;

};