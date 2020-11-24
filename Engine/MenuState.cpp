#include "stdafx.h"
#include "MenuState.h"
#include "SpriteInstance.h"
#include "SpriteFactory.h"
#include "Scene.h"
#include "CameraFactory.h"
#include "Canvas.h"

#include "Engine.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "TransformComponent.h"

CMenuState::CMenuState(CStateStack& aStateStack) :
	CState(aStateStack)
{
	CScene* scene = new CScene();
	CEngine::GetInstance()->AddScene(scene);

	CGameObject* camera = new CGameObject();
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 25.0f);
	camera->myTransform->Position({ 0.0f, 0.0f, 0.0f });
	camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
	scene->AddInstance(camera);
	scene->SetMainCamera(camera->GetComponent<CCameraComponent>());

	CGameObject* envLight = new CGameObject();
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	scene->AddInstance(envLight);
	scene->SetEnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());

	myCanvas = new CCanvas();
	myCanvas->Init("Json/UI_MainMenu_Description.json");
}

CMenuState::~CMenuState() {
}

void CMenuState::Awake() {
}

void CMenuState::Start() {
}

void CMenuState::Update() {
	myCanvas->Update();
}
