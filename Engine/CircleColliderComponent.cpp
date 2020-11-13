#include "stdafx.h"
#include "CircleColliderComponent.h"
#include "TriangleColliderComponent.h"
#include "RectangleColliderComponent.h"
#include "CollisionManager.h"
#include "TransformComponent.h"
#include "Debug.h"

CCircleColliderComponent::CCircleColliderComponent(CGameObject& aParent, float aRadius, ECollisionLayer aCollisionLayer, uint64_t someCollisionFlags) :
	CCollider(aParent, aCollisionLayer, someCollisionFlags),
	myRadius(aRadius)
{
	CCollisionManager::GetInstance()->RegisterCollider(this);
	SetCollisionLayer(aCollisionLayer);
}

CCircleColliderComponent::~CCircleColliderComponent() {
}

void CCircleColliderComponent::Awake() {
	SetPosition(GameObject().GetComponent<CTransformComponent>()->Position());
}

void CCircleColliderComponent::Start() {
}

void CCircleColliderComponent::Update() {
	SetPosition(GameObject().GetComponent<CTransformComponent>()->Position());

	if (GetAsyncKeyState('C')) {
		CDebug::GetInstance()->DrawLine({ GetPosition().x - (myRadius / 2.0f), GetPosition().y, GetPosition().z }, { GetPosition().x + (myRadius / 2.0f), GetPosition().y, GetPosition().z });
		CDebug::GetInstance()->DrawLine({ GetPosition().x, GetPosition().y, GetPosition().z - (myRadius / 2.0f) }
		

		, { GetPosition().x, GetPosition().y, GetPosition().z + (myRadius / 2.0f) });
	}
}

bool CCircleColliderComponent::Collided(CCircleColliderComponent* aCollidedGameObject)
{
	if (this->myRadius + aCollidedGameObject->GetRadius() < 
		DirectX::SimpleMath::Vector3::Distance(this->GetPosition(), aCollidedGameObject->GetPosition()))
		return false;
	else
		return true;
}

bool CCircleColliderComponent::Collided(CRectangleColliderComponent* aCollidedGameObject)
{
	float squaredDistance = 0.0f;

	if (this->GetPosition().x < aCollidedGameObject->GetMin().x) {
		squaredDistance += (aCollidedGameObject->GetMin().x - this->GetPosition().x) * (aCollidedGameObject->GetMin().x - this->GetPosition().x);
	}
	if (this->GetPosition().x > aCollidedGameObject->GetMax().x) {
		squaredDistance += (this->GetPosition().x - aCollidedGameObject->GetMax().x) * (this->GetPosition().x - aCollidedGameObject->GetMax().x);
	}

	if (this->GetPosition().z < aCollidedGameObject->GetMin().z) {
		squaredDistance += (aCollidedGameObject->GetMin().z - this->GetPosition().z) * (aCollidedGameObject->GetMin().z - this->GetPosition().z);
	}
	if (this->GetPosition().z > aCollidedGameObject->GetMax().z) {
		squaredDistance += (this->GetPosition().z - aCollidedGameObject->GetMax().z) * (this->GetPosition().z - aCollidedGameObject->GetMax().z);
	}

	if (squaredDistance <= this->myRadius * this->myRadius) {
		return true; //Has collided
	}
	return false; //Has not collided
}

bool CCircleColliderComponent::Collided(CTriangleColliderComponent* aCollidedGameObject)
{
	aCollidedGameObject;
	//Test 1, Vertex within circle
	float c1x = this->GetPosition().x - aCollidedGameObject->GetPosition().x;
	float c1z = this->GetPosition().z - aCollidedGameObject->GetPosition().z;

	float radiusSqr = this->GetRadius() * this->GetRadius();
	float c1sqr = c1x * c1x + c1z * c1z - radiusSqr;

	if (c1sqr <= 0) {
		return true;
	}

	float c2x = this->GetPosition().x - aCollidedGameObject->GetLeftVertex().x;
	float c2z = this->GetPosition().z - aCollidedGameObject->GetLeftVertex().z;
	float c2sqr = c2x * c2x + c2z * c2z - radiusSqr;

	if (c2sqr <= 0) {
		return true;
	}

	float c3x = this->GetPosition().x - aCollidedGameObject->GetRightVertex().x;
	float c3z = this->GetPosition().z - aCollidedGameObject->GetRightVertex().z;

	float c3sqr = c3x * c3x + c3z * c3z - radiusSqr;

	if (c3sqr <= 0) {
		return true;
	}

	//Test 2, Circle centre within triangle
	float e1x = aCollidedGameObject->GetLeftVertex().x - aCollidedGameObject->GetPosition().x;
	float e1z = aCollidedGameObject->GetLeftVertex().z - aCollidedGameObject->GetPosition().z;
	float e2x = aCollidedGameObject->GetRightVertex().x - aCollidedGameObject->GetLeftVertex().x;
	float e2z = aCollidedGameObject->GetRightVertex().z - aCollidedGameObject->GetLeftVertex().z;
	float e3x = aCollidedGameObject->GetPosition().x - aCollidedGameObject->GetRightVertex().x;
	float e3z = aCollidedGameObject->GetPosition().z - aCollidedGameObject->GetRightVertex().z;

	if (((e1z * c1x) - (e1x * c1z)) >= 0 || ((e2z * c2x) - (e2x * c2z)) >= 0 || ((e3z * c3x) - (e3x * c3z)) >= 0) {

	}
	else {
		return true;
	}
	//Test 3, Circle intersects edge
	float k = c1x * e1x + c1z * e1z;

	//First edge
	if (k > 0) {
		float len = e1x * e1x + e1z * e1z;

		if (k < len) {
			if (c1sqr * len <= k * k) {
				return true;
			}
		}
	}

	//Second edge
	k = c2x * e2x + c2z * e2z;

	if (k > 0) {
		float len = e2x * e2x + e2z * e2z;

		if (k < len) {
			if (c2sqr * len <= k * k) {
				return true;
			}
		}
	}

	//Third edge
	k = c3x * e3x + c3z * e3z;

	if (k > 0) {
		float len = e3x * e3x + e3z * e3z;

		if (k < len) {
			if (c3sqr * len <= k * k) {
				return true;
			}
		}
	}

	//No haqvinsection #PHATCODE
	return false;
}

bool CCircleColliderComponent::Collided(CCollider* aCollidedGameObject) {
	return aCollidedGameObject->Collided(this);
}

void CCircleColliderComponent::OnEnable()
{

}
void CCircleColliderComponent::OnDisable()
{

}
