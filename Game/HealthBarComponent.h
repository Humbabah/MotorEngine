#pragma once
#include "Behaviour.h"

class CGameObject;
class CCanvas;

class CHealthBarComponent : public CBehaviour
{
public:
	CHealthBarComponent(CGameObject& aParant, std::string aPath);
	~CHealthBarComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	CCanvas* GetCanvas() { return myCanvas; }
private:
	CCanvas* myCanvas;
};
