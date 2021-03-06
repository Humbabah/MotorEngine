#pragma once
#include "Behaviour.h"
#include "InputObserver.h"
#include "PostMaster.h"
#include "ObjectPool.h"

class CMouseSelection;

#define ON_DEATH_TIMER 1.77f

class CPlayerControllerComponent : public CBehaviour, public IInputObserver, public IObserver
{
public:
	CPlayerControllerComponent(CGameObject& aParent);
	~CPlayerControllerComponent() override;
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
	void ReceiveEvent(const EInputEvent aEvent) override;
	void Receive(const SMessage& aMessage) override;
	void ResetPlayer();
	void MessagePostmaster(EMessageType aMessageType, float aValue);
	bool PlayerIsAlive();
	void TakeDamage(float aDamageMultiplier, CGameObject* aGameObject);
	void RegenerateMana();
	void UpdateExperience(const SMessage& aMessage);
	bool AuraActive(){ return myAuraActive; }

public:
	void RegenerationPercentage(float aRegenerationPercentage) {
		myRegenerationPercentage = aRegenerationPercentage;
	}
	
	float RegenerationPercentage() {
		return myRegenerationPercentage;
	}

private:
	float myOnDeathTimer = ON_DEATH_TIMER;

private:
	void SetLevel(const int aLevel);

	float myLastHP;
	float mySecourceRegenerationSpeed;
	float myRegenerationPercentage = 0.0f;
	CMouseSelection* mySelection;
	CGameObject* myTargetEnemy;
	CGameObject* myTargetDestructible;
	CGameObject* myPathMarker;
	float myMarkerDuration;
	bool myIsMoving;
	bool myMiddleMousePressed;
	bool myAuraActive;
	bool myHasAttacked;
	DirectX::SimpleMath::Vector3 myLastPosition;
	bool firstTime;
};