#include "stdafx.h"
#include "PlayerControllerComponent.h"
#include "InputMapper.h"
#include "NavMeshComponent.h"
#include "MouseTracker.h"
#include "StatsComponent.h"
#include "AnimationComponent.h"
#include "MainSingleton.h"
#include "TransformComponent.h"
#include "PopupTextService.h"
#include "MouseSelection.h"
#include <DamageUtility.h>
#include <AbilityBehaviorComponent.h>
#include <AbilityBehavior.h>
#include "CircleColliderComponent.h"
#include "TriangleColliderComponent.h"
#include "TransformComponent.h"
#include "DestructibleComponent.h"
#include <PlayerGlobalState.h>
#include "VFXComponent.h"
#include "VFXFactory.h"
#include "DialogueSystem.h"

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& aParent):
	CBehaviour(aParent),
	myLastHP(0.0f),
	mySecourceRegenerationSpeed(4.0f), //TODO: read from unity
	mySelection(new CMouseSelection()),
	myIsMoving(true),
	myTargetEnemy(nullptr),
	myTargetDestructible(nullptr),
	myMiddleMousePressed(false),
	myAuraActive(false),
	myHasAttacked(false),
	firstTime(false)
{
	myLastPosition = {0.0f,0.0f,0.0f};

	myPathMarker = new CGameObject(-1337);
	myPathMarker->AddComponent<CVFXComponent>(*myPathMarker);
	std::vector<std::string> VFXPaths;
	VFXPaths.emplace_back("Json/VFXData_PathMarker.json");
	myPathMarker->GetComponent<CVFXComponent>()->Init(CVFXFactory::GetInstance()->GetVFXBaseSet(VFXPaths));
	myPathMarker->Active(true);
	myMarkerDuration = myPathMarker->GetComponent<CVFXComponent>()->GetVFXBases().back()->GetVFXBaseData().myDuration;
	myPathMarker->myTransform->Position({GameObject().myTransform->Position().x, GameObject().myTransform->Position().y , GameObject().myTransform->Position().z});
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	delete mySelection;
	mySelection = nullptr;
	CInputMapper::GetInstance()->RemoveObserver(IInputObserver::EInputEvent::MoveClick, this);
	CInputMapper::GetInstance()->RemoveObserver(IInputObserver::EInputEvent::MoveDown, this);
	CInputMapper::GetInstance()->RemoveObserver(IInputObserver::EInputEvent::StandStill, this);
	CInputMapper::GetInstance()->RemoveObserver(IInputObserver::EInputEvent::MiddleMouseMove, this);
	CInputMapper::GetInstance()->RemoveObserver(IInputObserver::EInputEvent::Moving, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyDied, this);
	firstTime = false;
}

void CPlayerControllerComponent::Awake()
{
	myLastHP = GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth;
	CInputMapper::GetInstance()->AddObserver(IInputObserver::EInputEvent::MoveClick, this);
	CInputMapper::GetInstance()->AddObserver(IInputObserver::EInputEvent::MoveDown, this);
	CInputMapper::GetInstance()->AddObserver(IInputObserver::EInputEvent::StandStill, this);
	CInputMapper::GetInstance()->AddObserver(IInputObserver::EInputEvent::MiddleMouseMove, this);
	CInputMapper::GetInstance()->AddObserver(IInputObserver::EInputEvent::Moving, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyDied, this);

	CEngine::GetInstance()->GetActiveScene().AddInstance(myPathMarker);
}

void CPlayerControllerComponent::Start()
{
	GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience = CMainSingleton::PlayerGlobalState().GetSavedExperience();

	SetLevel(CMainSingleton::PlayerGlobalState().GetSavedPlayerLevel());

	if (this->GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myMaxLevel
		== this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel)
	{
		MessagePostmaster(EMessageType::PlayerExperienceChanged, 1.0f);
	} else {
		float maxValue = this->GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myExperienceToLevelUp;
		float difference = this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience;
		this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience = difference;

		difference = difference / maxValue;
		MessagePostmaster(EMessageType::PlayerExperienceChanged, difference);
	}
}

void CPlayerControllerComponent::Update()
{
	//bs fix aswell... fixes aura bug
	if (this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel >= 2 && firstTime == false) {
		this->GameObject().GetComponent<CAbilityComponent>()->UseAbility(EAbilityType::PlayerAbility2, GameObject().myTransform->Position());
		myAuraActive = true;
		firstTime = true;
	}
	if (!CMainSingleton::DialogueSystem().Active()) {

		if (!myHasAttacked) {
			if (myTargetEnemy) {
				if (myTargetEnemy->GetComponent<CStatsComponent>()->GetStats().myHealth > 0) {
					float abilityLength = GameObject().GetComponent<CAbilityComponent>()->MeleeAttackRange();
					this->GameObject().GetComponent<CNavMeshComponent>()->CalculatePath(myTargetEnemy->myTransform->Position());
					if (DirectX::SimpleMath::Vector3::Distance(myTargetEnemy->myTransform->Position(), GameObject().myTransform->Position())
						< (myTargetEnemy->GetComponent<CCircleColliderComponent>()->GetRadius() + abilityLength)) {
						this->GameObject().GetComponent<CTransformComponent>()->ClearPath();
						this->GameObject().GetComponent<CAbilityComponent>()->UseAbility(EAbilityType::PlayerMelee, GameObject().myTransform->Position());
						myHasAttacked = true;
					}
				}
			}
		}

		if (myTargetDestructible) {
			if (myTargetDestructible->GetComponent<CDestructibleComponent>()->IsDead() == false) {
				if (DirectX::SimpleMath::Vector3::Distance(myTargetDestructible->myTransform->Position(), GameObject().myTransform->Position()) < myTargetDestructible->GetComponent<CCircleColliderComponent>()->GetRadius()) {
					myTargetDestructible->GetComponent<CDestructibleComponent>()->IsDead(true);
					this->GameObject().GetComponent<CTransformComponent>()->ClearPath();

					auto animComp = this->GameObject().GetComponent<CAnimationComponent>();
					animComp->PlayAttack01ID();
					float delay = (animComp->GetCurrentAnimationDuration() / animComp->GetCurrentAnimationTicksPerSecond()) / 6.0f;
					CMainSingleton::PostMaster().Send({ EMessageType::LightAttack, &delay });
				}
			}
		}

		if (!PlayerIsAlive()) {
			myOnDeathTimer -= CTimer::Dt();
			if (myOnDeathTimer <= 0.0f)
			{
				ResetPlayer();
			}
		} else {
			RegenerateMana();
		}
		if (myPathMarker->Active()) {
			if (myMarkerDuration >= 0.0f) {
				myMarkerDuration -= CTimer::Dt();
			}
			if (myMarkerDuration <= 0.0f) {
				myPathMarker->Active(false);
			}
		}
		if (myIsMoving) {
			this->GameObject().myTransform->MoveAlongPath();
		}
	} else {
		myTargetEnemy = nullptr;

		this->GameObject().GetComponent<CTransformComponent>()->ClearPath();
	}
}

void CPlayerControllerComponent::OnEnable() {}

void CPlayerControllerComponent::OnDisable() {}

void CPlayerControllerComponent::ReceiveEvent(const IInputObserver::EInputEvent aEvent)
{
	if (CEngine::GetInstance()->GetActiveScene().GetNavMesh() && !CMainSingleton::DialogueSystem().Active() && GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth > 0.0f) {
		switch (aEvent)
		{
		case IInputObserver::EInputEvent::MoveClick:
			myHasAttacked = false;

			/*if (myPathMarker->Active()) {
				myPathMarker->GetComponent<CVFXComponent>()->();
			}*/
			myPathMarker->Active(true);
			myMarkerDuration = myPathMarker->GetComponent<CVFXComponent>()->GetVFXBases().back()->GetVFXBaseData().myDuration;
			myPathMarker->myTransform->Position(mySelection->GetPositionAtNavmesh());

			break;
		case  IInputObserver::EInputEvent::StandStill:
			myMiddleMousePressed = false;

			myIsMoving = false;
			this->GameObject().GetComponent<CTransformComponent>()->ClearPath();
			break;
		case  IInputObserver::EInputEvent::Moving:
			myMiddleMousePressed = false;
			myIsMoving = true;
			break;
		case IInputObserver::EInputEvent::MoveDown:
			myMiddleMousePressed = false;
			myHasAttacked = false;
			if (myIsMoving) {
				if (mySelection)
				{
					if (CEngine::GetInstance()->GetActiveScene().GetBoss()) {
						myTargetEnemy = mySelection->FindSelectedBoss();
					} else {
						myTargetEnemy = mySelection->FindSelectedEnemy();
					}
					if (myTargetEnemy && myTargetEnemy->GetComponent<CStatsComponent>()->GetStats().myHealth > 0.f) {
						this->GameObject().GetComponent<CNavMeshComponent>()->CalculatePath(myTargetEnemy->myTransform->Position());
					} else {
						this->GameObject().GetComponent<CNavMeshComponent>()->CalculatePath();
					}

					myTargetDestructible = mySelection->FindSelectedDestructible();
					if (myTargetDestructible && myTargetDestructible->GetComponent<CDestructibleComponent>()->IsDead() == false) {

						this->GameObject().GetComponent<CNavMeshComponent>()->CalculatePath(myTargetDestructible->myTransform->Position());
					} else {
						this->GameObject().GetComponent<CNavMeshComponent>()->CalculatePath();
					}
				}
			} else {
					this->GameObject().GetComponent<CAbilityComponent>()->UseAbility(EAbilityType::PlayerMelee, GameObject().myTransform->Position());
			}
			break;
		case IInputObserver::EInputEvent::MiddleMouseMove:
			if (myIsMoving) {
				this->GameObject().GetComponent<CNavMeshComponent>()->CalculatePath();
			}
			break;
		default:
			break;
		}
	} /*else if (CMainSingleton::DialogueSystem().Active()){
		myIsMoving = false;
	}*/
}

void CPlayerControllerComponent::Receive(const SMessage& aMessage)
{

	switch (aMessage.myMessageType)
	{
	case EMessageType::EnemyDied:
		UpdateExperience(aMessage);
		break;
	default:
		break;
	}
}

void CPlayerControllerComponent::ResetPlayer()
{
	GameObject().GetComponent<CTransformComponent>()->Position(GameObject().GetComponent<CTransformComponent>()->StartPosition());
	GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth = GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myBaseHealth;
	GameObject().GetComponent<CStatsComponent>()->GetStats().myResource = GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myBaseResource;
	GameObject().GetComponent<CTransformComponent>()->ClearPath();
	MessagePostmaster(EMessageType::PlayerHealthChanged, 1.0f);
	MessagePostmaster(EMessageType::PlayerResourceChanged, 1.0f);
	GameObject().GetComponent<CAnimationComponent>()->Start();
	myOnDeathTimer = ON_DEATH_TIMER;
}

void CPlayerControllerComponent::MessagePostmaster(EMessageType aMessageType, float aValue)
{
	SMessage message;
	message.myMessageType = aMessageType;
	message.data = &aValue;
	CMainSingleton::PostMaster().Send(message);
}

bool CPlayerControllerComponent::PlayerIsAlive()
{
	if (GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth >= 0.0f && GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth < GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myBaseHealth)
		GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth += CTimer::Dt();
	/*if (myLastHP != GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth)
	{*/
	float baseHealth = GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myBaseHealth;
	float difference = baseHealth - GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth;
	difference = (baseHealth - difference) / baseHealth;
	MessagePostmaster(EMessageType::PlayerHealthChanged, difference);

	myLastHP = GameObject().GetComponent<CStatsComponent>()->GetStats().myHealth;
	
	if (myLastHP < 0.0f)
		GameObject().GetComponent<CAnimationComponent>()->DeadState();

	return myLastHP > 0.0f;
}

void CPlayerControllerComponent::TakeDamage(float aDamageMultiplier, CGameObject* aGameObject)
{
	EHitType hitType = EHitType::Enemy;
	float damage = CDamageUtility::CalculateDamage(hitType, aGameObject->GetComponent<CStatsComponent>()->GetBaseStats().myDamage, aDamageMultiplier);

	if (GameObject().GetComponent<CStatsComponent>()->AddDamage(damage)) {
		CMainSingleton::PostMaster().Send({EMessageType::AttackHits, nullptr});
		SDamagePopupData data = {damage, static_cast<int>(hitType), &GameObject()};
		CMainSingleton::PopupTextService().SpawnPopup(EPopupType::Damage, &data);
	}
}


void CPlayerControllerComponent::RegenerateMana()
{
	if (GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myBaseResource > GameObject().GetComponent<CStatsComponent>()->GetStats().myResource) {
		GameObject().GetComponent<CStatsComponent>()->GetStats().myResource += mySecourceRegenerationSpeed * CTimer::Dt();
		float difference = GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myBaseResource - GameObject().GetComponent<CStatsComponent>()->GetStats().myResource;
		difference = (100.0f - difference) / 100.0f;
		MessagePostmaster(EMessageType::PlayerResourceChanged, difference);
	}
}

void CPlayerControllerComponent::UpdateExperience(const SMessage& aMessage)
{
	if (this->GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myMaxLevel
		> this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel)
	{
		float difference;
		float maxValue = this->GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myExperienceToLevelUp;
		float experience = *static_cast<float*>(aMessage.data);
		float experienceModifier = static_cast<float>(this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel) + 1.0f;
		this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience += experience / experienceModifier;

		if (maxValue <= this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience)
		{
			CMainSingleton::PostMaster().Send({EMessageType::PlayLevelUpSFX, nullptr});

			this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel += 1;
			int level = this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel;
			std::string abilityInfo = "Skill ";
			abilityInfo += std::to_string(level);

			CMainSingleton::PopupTextService().SpawnPopup(EPopupType::Info, abilityInfo);

			////This is for group 4
			////Comment this in before last build
			if (level == 2) {
				this->GameObject().GetComponent<CAbilityComponent>()->UseAbility(EAbilityType::PlayerAbility2, GameObject().myTransform->Position());
				CMainSingleton::PostMaster().Send({EMessageType::ShieldSpell, nullptr});
				myAuraActive = true;
			}

			this->GameObject().GetComponent<CAbilityComponent>()->ResetCooldown(this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel);
			if (this->GameObject().GetComponent<CStatsComponent>()->GetBaseStats().myMaxLevel
				== this->GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel) {
				MessagePostmaster(EMessageType::PlayerExperienceChanged, 1.0f);
			} else {
				difference = this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience - maxValue;
				this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience = difference;

				difference = difference / maxValue;
				MessagePostmaster(EMessageType::PlayerExperienceChanged, difference);
			}
		} else
		{
			difference = this->GameObject().GetComponent<CStatsComponent>()->GetStats().myExperience / maxValue;
			MessagePostmaster(EMessageType::PlayerExperienceChanged, difference);
		}
	}

	CMainSingleton::PlayerGlobalState().SetStatsToSave(GameObject().GetComponent<CStatsComponent>()->GetStats());
}

void CPlayerControllerComponent::SetLevel(const int aLevel)
{
	const int level = aLevel > 3 ? 3 : aLevel;

	GameObject().GetComponent<CStatsComponent>()->GetStats().myLevel = level;
	switch (level)
	{
	case 3: // Activate ability 3
		this->GameObject().GetComponent<CAbilityComponent>()->ResetCooldown(3);
	case 2: // Activate ability 2
		this->GameObject().GetComponent<CAbilityComponent>()->UseAbility(EAbilityType::PlayerAbility2, GameObject().myTransform->Position());
		this->GameObject().GetComponent<CAbilityComponent>()->ResetCooldown(2);
		CMainSingleton::PostMaster().Send({EMessageType::ShieldSpell, nullptr});
		myAuraActive = true;
	case 1: // Activate ability 1
		this->GameObject().GetComponent<CAbilityComponent>()->ResetCooldown(1);
	case 0:
		break;
	}
}
