#pragma once
#include "AbilityBehavior.h"

class CGameObject;

class CAuraBehavior : public IAbilityBehavior
{
public:
	CAuraBehavior(CGameObject* anObjectToFollow, float aRotationalSpeed, float aRegenerationPercentage);
	~CAuraBehavior() override;

	void Update(CGameObject* aParent) override;
	void Init(CGameObject* aCaster) override;

private:
	CGameObject* myObjectToFollow;
	float myRotationalSpeed;
	float myRegenerationPercentage;
};

