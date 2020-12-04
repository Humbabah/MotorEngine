#include "stdafx.h"
#include "InGameState.h"

#include "StateStack.h"
#include "Scene.h"
#include "GameObject.h"
#include "SpriteInstance.h"
#include "SpriteFactory.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"
#include "Model.h"
#include "Animation.h"
#include "TransformComponent.h"
#include "DialogueSystem.h"
#include "Timer.h"
#include "CameraComponent.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "AnimatedUIElement.h"
#include "InputMapper.h"
#include "PostMaster.h"
#include "Canvas.h"
#include "AbilityComponent.h"
//collider test
#include "CircleColliderComponent.h"
#include "RectangleColliderComponent.h"
#include "TriangleColliderComponent.h"
//collider test

#include "TokenPool.h"
#include "StatsComponent.h"
#include "EnemyBehavior.h"
#include "PlayerControllerComponent.h"
#include "AIBehaviorComponent.h"
#include "TransformComponent.h"
#include "PauseState.h"
#include "PostMaster.h"
#include "MainSingleton.h"
#include "AIBehaviorComponent.h"
#include <iostream>

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState) 
	: CState(aStateStack, aState) 
	, myCanvas(nullptr)
	, myTokenPool(nullptr)
{
}

CInGameState::~CInGameState()
{
}


void CInGameState::Awake()
{
}

void CInGameState::Start()
{
	CInputMapper::GetInstance()->AddObserver(IInputObserver::EInputEvent::PauseGame, this);
	
	CEngine::GetInstance()->SetActiveScene(myState);

	myCanvas = new CCanvas();
	myCanvas->Init("Json/UI_InGame_Description.json", CEngine::GetInstance()->GetActiveScene());

	myTokenPool = new CTokenPool(4, 4.0f);

	std::vector<CGameObject*>& gameObjects = CEngine::GetInstance()->GetActiveScene().myGameObjects;
	size_t currentSize = gameObjects.size();
	for (size_t i = 0; i < currentSize; ++i)
	{
		if (gameObjects[i])
		{
			gameObjects[i]->Awake();
		}
	}

	////Late awake
	size_t newSize = gameObjects.size();
	for (size_t j = currentSize; j < newSize; ++j) 
	{
		if (gameObjects[j])
		{
			gameObjects[j]->Awake();
		}
	}

	for (auto& gameObject : gameObjects)
	{
		gameObject->Start();
	}
}

void CInGameState::Stop()
{
	CInputMapper::GetInstance()->RemoveObserver(IInputObserver::EInputEvent::PauseGame, this);

	delete myTokenPool;
	myTokenPool = nullptr;

	delete myCanvas;
	myCanvas = nullptr;
}

void CInGameState::Update()
{
	CCollisionManager::GetInstance()->Update();

	myCanvas->Update();
	myTokenPool->GetInstance()->Update();

	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Update();

	}
}

void CInGameState::ReceiveEvent(const EInputEvent aEvent)
{
	if (this == myStateStack.GetTop()) {
		switch (aEvent) {
		case IInputObserver::EInputEvent::PauseGame:
			myStateStack.PushState(CStateStack::EState::PauseMenu);
			break;
		default:
			break;
		}
	}
}

