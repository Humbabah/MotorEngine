#include "stdafx.h"
#include "Scene.h"
#include "EnvironmentLight.h"
#include "ModelComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "PointLight.h"
#include "ParticleInstance.h"
#include "VFXInstance.h"
#include "LineInstance.h"
#include "SpriteInstance.h"
#include "Component.h"
#include "Debug.h"


CScene* CScene::ourInstance = nullptr;

CScene* CScene::GetInstance()
{
	return ourInstance;
}

CScene::CScene()
{
	ourInstance = this;
	myMainCamera = nullptr;
	myCollisionManager = new CCollisionManager();
}

CScene::~CScene()
{
	ourInstance = nullptr;
	myMainCamera = nullptr;
	delete myCollisionManager;
	myCollisionManager = nullptr;
}


bool CScene::Init()
{
	return true;
}


void CScene::SetMainCamera(CCamera* aCamera)
{
	myMainCamera = aCamera;
}

CCamera* CScene::GetMainCamera()
{
	return myMainCamera;
}

CEnvironmentLight* CScene::GetEnvironmentLight()
{
	return myEnvironmentLights[0];
}

std::vector<CGameObject*> CScene::CullGameObjects(CCamera* aMainCamera)
{
	using namespace DirectX::SimpleMath;
	Vector3 cameraPosition = aMainCamera->GetTransform().Translation();
	std::vector<CGameObject*> culledGameObjects;
	for (auto& gameObject : myGameObjects)
	{
		float distanceToCameraSquared = Vector3::DistanceSquared(gameObject->GetComponent<CTransformComponent>()->Position(), cameraPosition);
		if (distanceToCameraSquared < 1500.0f)
		{
			culledGameObjects.emplace_back(gameObject);
		}
	}
	return culledGameObjects;
}

std::pair<unsigned int, std::array<CPointLight*, 8>> CScene::CullLights(CGameObject* aGameObject) {
	std::pair<unsigned int, std::array<CPointLight*, 8>> pointLightPair;
	UINT counter = 0;

	for (UINT i = 0; i < myPointLights.size() && counter < 8; ++i) {
		float distanceSquared = DirectX::SimpleMath::Vector3::DistanceSquared(myPointLights[i]->GetPosition(), aGameObject->GetComponent<CTransformComponent>()->Transform().Translation());
		float range = myPointLights[i]->GetRange();
		if (distanceSquared < (range * range)) {
			pointLightPair.second[counter] = myPointLights[i];
			++counter;
		}
	}

	pointLightPair.first = counter;
	return pointLightPair;
}

std::vector<CParticleInstance*> CScene::CullParticles(CCamera* aMainCamera)
{
	for (unsigned int i = 0; i < myParticles.size(); ++i)
	{
		myParticles[i]->Update(CTimer::Dt(), aMainCamera->GetTransform().Translation());
	}
	return myParticles;
}

std::vector<CVFXInstance*> CScene::CullVFX(CCamera* /*aMainCamera*/) {
	
	for (unsigned int i = 0; i < myVFXInstances.size(); ++i) {
		
		myVFXInstances[i]->Scroll({0.15f * CTimer::Dt(), 0.15f * CTimer::Dt() }, { 0.15f * CTimer::Dt() , 0.15f * CTimer::Dt() });
	}
	return myVFXInstances;
}

const std::vector<CLineInstance>& CScene::CullLines(CCamera* /*aMainCamera*/) const
{
	return CDebug::GetInstance()->GetLines();
}

std::vector<CSpriteInstance*> CScene::CullSprites(CCamera* /*aMainCamera*/)
{
	return mySprites;
}

bool CScene::AddInstance(CCamera* aCamera)
{
	myCameras.emplace_back(aCamera);
	return true;
}

bool CScene::AddInstance(CEnvironmentLight* anEnvironmentLight)
{
	myEnvironmentLights.emplace_back(anEnvironmentLight);
	return true;
}

bool CScene::AddInstance(CPointLight* aPointLight) {
	myPointLights.emplace_back(aPointLight);
	return true;
}

bool CScene::AddInstance(CGameObject* aGameObject)
{
	myGameObjects.emplace_back(aGameObject);
	return true;
}

bool CScene::AddInstance(CParticleInstance* aParticleInstance)
{
	myParticles.emplace_back(aParticleInstance);
	return true;
}

bool CScene::AddInstance(CVFXInstance* aVFXInstance) {
	myVFXInstances.emplace_back(aVFXInstance);
	return true;
}

bool CScene::AddInstance(CLineInstance* aLineInstance) {
	myLineInstances.emplace_back(aLineInstance);
	return true;
}

bool CScene::AddInstance(CSpriteInstance* aSprite) {
	if (!aSprite) {
		return false;
	}
	mySprites.emplace_back(aSprite);
	return true;
}

bool CScene::ClearScene() {

	for (auto gameObject : myGameObjects) {
		delete gameObject;
		gameObject = nullptr;
	}
	myGameObjects.clear();
	return true;
}

bool CScene::ClearSprites() {

	for (auto  sprite : mySprites) {
		delete sprite;
		sprite = nullptr;
	}
	mySprites.clear();

	return true;
}
