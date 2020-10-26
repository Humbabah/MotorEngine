#pragma once

class CStateStack;

class CState {

public:
	CState(CStateStack& aStateStack);


	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;

protected:

	CStateStack& myStateStack;
};

