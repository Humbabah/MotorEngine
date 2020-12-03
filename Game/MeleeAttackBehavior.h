#pragma once
#include "AbilityBehavior.h"

namespace DirectX
{
	namespace SimpleMath
	{
		struct Vector3;
	}
}

namespace SM = DirectX::SimpleMath;

class CMeleeAttackBehavior : public IAbilityBehavior {
public:
	CMeleeAttackBehavior(float aDuration);
	~CMeleeAttackBehavior() override;

	void Init(CGameObject* aCaster) override;
	void Update(CGameObject* aParent) override;

private:
	DirectX::SimpleMath::Vector3 myDirection;
	CGameObject* myCaster;
};