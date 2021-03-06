#include "stdafx.h"
#include "LevelSelectState.h"
#include "Engine.h"
#include "Scene.h"
#include "SpriteInstance.h"
#include "SpriteFactory.h"
#include "Canvas.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "TransformComponent.h"
#include "EnviromentLightComponent.h"
#include "PlayerGlobalState.h"
#include "JsonReader.h"
#include "MainSingleton.h"
#include "rapidjson\document.h"
#include "rapidjson\istreamwrapper.h"
#include "StatsComponent.h"

using namespace rapidjson;

CLevelSelectState::CLevelSelectState(CStateStack& aStateStack, const CStateStack::EState aState) :
	CState(aStateStack, aState),
	myScene(nullptr),
	myBackground(nullptr),
	myCanvas(nullptr),
	myGrupp(0)
{
}

CLevelSelectState::~CLevelSelectState()
{
}

void CLevelSelectState::Awake()
{
	std::ifstream inputStream("Json/Grupp.json");
	IStreamWrapper inputWrapper(inputStream);
	Document document;
	document.ParseStream(inputWrapper);
	myGrupp = document["Grupp"].GetInt();
}

void CLevelSelectState::Start()
{
	myScene = new CScene();

	CGameObject* camera = new CGameObject(0);
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 25.0f);
	camera->myTransform->Position({ 0.0f, 0.0f, 0.0f });
	camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
	myScene->AddInstance(camera);
	myScene->SetMainCamera(camera->GetComponent<CCameraComponent>());

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	myScene->AddInstance(envLight);
	myScene->SetEnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());

	myCanvas = new CCanvas();
	myCanvas->Init("Json/UI_LevelSelect_Description.json", *myScene);

	CMainSingleton::PostMaster().Subscribe(EMessageType::MainMenu, this);

	if (myGrupp == 3)
		Group3Sub();
	else if (myGrupp == 4)
		Group4Sub();

	CEngine::GetInstance()->AddScene(myState, myScene);
	CEngine::GetInstance()->SetActiveScene(myState);
}

void CLevelSelectState::Stop()
{
	if (myScene)
	{
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::MainMenu, this);

		if (myGrupp == 3)
			Group3Unsub();
		else if (myGrupp == 4)
			Group4Unsub();

		delete myCanvas;
		myCanvas = nullptr;
		myScene = nullptr;
	}
}

void CLevelSelectState::Update()
{
	myCanvas->Update();
}

void CLevelSelectState::Receive(const SMessage& aMessage)
{
	if (myGrupp == 3)
		Group3Receive(aMessage);
	else if(myGrupp == 4)
		Group4Receive(aMessage);
}

void CLevelSelectState::Group3Sub()
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel1, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel2, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel3, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel4, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel5, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel6, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel7, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel8, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel9, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel10, this);
}

void CLevelSelectState::Group3Unsub()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel1, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel2, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel3, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel4, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel5, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel6, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel7, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel8, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel9, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel10, this);
}

int Group3PlayerLevel(const EMessageType aLevel)
{
	int playerLevel = 0;
	switch (aLevel)
	{
		case EMessageType::LoadLevel1:
			case EMessageType::LoadLevel2:
				playerLevel = 0;
		break;
		case EMessageType::LoadLevel3:
			case EMessageType::LoadLevel4:
				case EMessageType::LoadLevel5:
					playerLevel = 1;
		break;
		case EMessageType::LoadLevel6:
			case EMessageType::LoadLevel7:
				case EMessageType::LoadLevel8:
					playerLevel = 2;
		break;
		case EMessageType::LoadLevel9:
			case EMessageType::LoadLevel10:
				playerLevel = 3;
		break;
		default:break;
	}
	return playerLevel;
}

void CLevelSelectState::Group3Receive(const SMessage & aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::MainMenu:
		{
			myStateStack.PopUntil(CStateStack::EState::MainMenu);
		} break;
		case EMessageType::LoadLevel1:
		{
			SStringMessage stringMessage = { "Level_1_1", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel1));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel2:
		{
			SStringMessage stringMessage = { "Level_1_2", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel2));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel3:
		{
			SStringMessage stringMessage = { "Level_1_3", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel3));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel4:
		{
			SStringMessage stringMessage = { "Level_2_1", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel4));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel5:
		{
			SStringMessage stringMessage = { "Level_2_2", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel5));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel6:
		{
			SStringMessage stringMessage = { "Level_3_1", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel6));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel7:
		{
			SStringMessage stringMessage = { "Level_3_2", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel7));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel8:
		{
			SStringMessage stringMessage = { "Level_3_3", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel8));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel9:
		{
			SStringMessage stringMessage = { "Level_3_4", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel9));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadLevel10:
		{
			SStringMessage stringMessage = { "Level_3_5", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(Group3PlayerLevel(EMessageType::LoadLevel10));

			myStateStack.PushState(CStateStack::EState::LoadLevel);
		} break;
		default:
		break;
	}
}

void CLevelSelectState::Group4Sub()
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadDungeon, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadGardens, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadCastle, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadBossRoom, this);
}

void CLevelSelectState::Group4Unsub()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadDungeon, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadGardens, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadCastle, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadBossRoom, this);
}

void CLevelSelectState::Group4Receive(const SMessage & aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::MainMenu:
		{
			myStateStack.PopUntil(CStateStack::EState::MainMenu);
		} break;

		case EMessageType::LoadDungeon:
		{
			SStringMessage stringMessage = { "Dungeon", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			SStats* stats = new SStats();
			stats->myExperience = 0.f;
			CMainSingleton::PlayerGlobalState().SetStatsToSave(*stats);
			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(0);

			myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadGardens:
		{
			SStringMessage stringMessage = { "Gardens", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			SStats* stats = new SStats();
			stats->myExperience = 0.f;
			CMainSingleton::PlayerGlobalState().SetStatsToSave(*stats);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(1);

			myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadCastle:
		{
			SStringMessage stringMessage = { "Castle", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			SStats* stats = new SStats();
			stats->myExperience = 0.f;
			CMainSingleton::PlayerGlobalState().SetStatsToSave(*stats);

			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(2);

			myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
		} break;
		case EMessageType::LoadBossRoom:
		{
			SStringMessage stringMessage = { "BossRoom", nullptr };
			CMainSingleton::PostMaster().Send(stringMessage);

			SStats* stats = new SStats();
			stats->myExperience = 0.f;
			CMainSingleton::PlayerGlobalState().SetStatsToSave(*stats);
			CMainSingleton::PlayerGlobalState().SetSavedPlayerLevel(3);

			myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
		} break;

		default:
		break;
	}
}
