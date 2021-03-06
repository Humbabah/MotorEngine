#pragma once
#include <Behaviour.h>

class CGameObject;
class IAIBehavior;

class CAIBehaviorComponent : public CBehaviour
{
public:
	CAIBehaviorComponent(CGameObject& aParent, IAIBehavior* aBehavior);
	~CAIBehaviorComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Collided(CGameObject* aCollidedGameObject) override;

	void OnEnable() override;
	void OnDisable() override;
	IAIBehavior* AIBehavior();
private:
	IAIBehavior* myBehavior;
};

