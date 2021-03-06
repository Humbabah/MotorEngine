#pragma once
#include "GameObject.h"
#include "EnemyFactory.h"

class CGameObject;
class CEnemyFactory;

class CEnemy {
public:
	CEnemy() : myEnemy(new CGameObject(0)) { }
	CEnemy(const int aInstanceID) : myEnemy(new CGameObject(aInstanceID)) {}
	~CEnemy() { delete myEnemy; myEnemy = nullptr; }

	void init(const int aInstanceID, DirectX::SimpleMath::Vector3 aPosition, float aHealth, float aDamage, float aMoveSpeed, float aCooldown) 
	{ 
		myEnemy = &CEnemyFactory::GetInstance()->CreateEnemy(aInstanceID, aPosition, aHealth, aDamage, aMoveSpeed, aCooldown);
	}

	CEnemy* GetNext() const { return state.next; }
	void SetNext(CEnemy* aNext) { state.next = aNext; }

	CGameObject* GetEnemy() const { return myEnemy; }

private:
	union {
		CEnemy* next;
	} state;
	CGameObject* myEnemy;
};

class CObjectPool
{

	friend class CShowCase;

public:
	static CObjectPool* GetInstance();

	CGameObject* Create(const int aInstanceID, DirectX::SimpleMath::Vector3 aPosition = {0.0f, 0.0f, 0.0f}, float aHealth = 0, float aDamage = 0, float aMoveSpeed = 0, float aCooldown = 0);

	void Remove(CGameObject* aEnemy);
private:
	CObjectPool();
	~CObjectPool();
private:
	static const int poolSize = 100;
	CEnemy enemies[poolSize];
	CEnemy* firstAvailable;
	static CObjectPool* ourInstance;
};

