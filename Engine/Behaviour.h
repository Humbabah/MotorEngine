#pragma once
#include "Component.h"

class CBehaviour : public CComponent
{
public:
	CBehaviour(CGameObject& aParent) : CComponent(aParent) {}
	virtual ~CBehaviour() {}

	virtual void OnEnable() = 0;
	virtual void OnDisable() = 0;

	void Enabled(bool aEnabled) { myEnabled = aEnabled; }
	const bool Enabled() const { return myEnabled; }
private:
};
