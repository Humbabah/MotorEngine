#include "stdafx.h"
#include "InputMapper.h"
#include "Input.h"

CInputMapper* CInputMapper::ourInstance = nullptr;

CInputMapper* CInputMapper::GetInstance()
{
	return ourInstance;
}

CInputMapper::CInputMapper() : myInput(Input::GetInstance())
{
	ourInstance = this;

}

CInputMapper::~CInputMapper()
{
}

bool CInputMapper::Init()
{
	MapEvent(IInputObserver::EInputAction::MouseLeftPressed, IInputObserver::EInputEvent::MoveClick);
	MapEvent(IInputObserver::EInputAction::MouseLeftDown, IInputObserver::EInputEvent::MoveDown);
	MapEvent(IInputObserver::EInputAction::MouseRight, IInputObserver::EInputEvent::AttackClick);
	MapEvent(IInputObserver::EInputAction::MouseMiddle, IInputObserver::EInputEvent::MiddleMouseMove);
	MapEvent(IInputObserver::EInputAction::KeyShiftDown, IInputObserver::EInputEvent::StandStill);
	MapEvent(IInputObserver::EInputAction::KeyShiftRelease, IInputObserver::EInputEvent::Moving);
	MapEvent(IInputObserver::EInputAction::Key1, IInputObserver::EInputEvent::Ability1);
	MapEvent(IInputObserver::EInputAction::Key2, IInputObserver::EInputEvent::Ability2);
	MapEvent(IInputObserver::EInputAction::Key3, IInputObserver::EInputEvent::Ability3);
	MapEvent(IInputObserver::EInputAction::KeyA, IInputObserver::EInputEvent::Ability1);
	MapEvent(IInputObserver::EInputAction::KeyS, IInputObserver::EInputEvent::Ability2);
	MapEvent(IInputObserver::EInputAction::KeyD, IInputObserver::EInputEvent::Ability3);
	MapEvent(IInputObserver::EInputAction::KeyEscape, IInputObserver::EInputEvent::PauseGame);

	if (this == nullptr)
		return false;
	else
		return true;
}

void CInputMapper::RunEvent(const IInputObserver::EInputEvent aOutputEvent)
{
	for (int i = 0; i < myObservers[aOutputEvent].size(); ++i)
	{
		myObservers[aOutputEvent][i]->ReceiveEvent(aOutputEvent);
	}
}

void CInputMapper::TranslateActionToEvent(const IInputObserver::EInputAction aAction)
{
	const auto eventIterator = myEvents.find(aAction);
	if (eventIterator != myEvents.end())
	{
		RunEvent(myEvents[aAction]);
	}
}

void CInputMapper::UpdateKeyboardInput()
{
	if (myInput->IsKeyPressed(VK_ESCAPE))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyEscape);
	}
	
	if (myInput->IsKeyDown(VK_SHIFT))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyShiftDown);
	}
	
	if (myInput->IsKeyReleased(VK_SHIFT))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyShiftRelease);
	}

	if (myInput->IsKeyPressed('1'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::Key1);
	}
	if (myInput->IsKeyPressed('2'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::Key2);
	}
	if (myInput->IsKeyPressed('3'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::Key3);
	}

	if (myInput->IsKeyPressed('A'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyA);
	}
	if (myInput->IsKeyPressed('S'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyS);
	}
	if (myInput->IsKeyPressed('D'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyD);
	}
}

void CInputMapper::UpdateMouseInput()
{

	if (myInput->IsMouseDown(Input::MouseButton::Middle))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseMiddle);
	}
	if (myInput->IsMousePressed(Input::MouseButton::Left))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseLeftPressed);
	}
	if (myInput->IsMouseDown(Input::MouseButton::Left))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseLeftDown);
	}
	//else if (myInput->IsMouseDown(Input::MouseButton::Left))
	//{
	//	TranslateActionToEvent(IInputObserver::EInputAction::MouseLeft);
	//}
	//else if (myInput->IsMouseReleased(Input::MouseButton::Left))
	//{
	//	TranslateActionToEvent(IInputObserver::EInputAction::MouseLeft);
	//}
	if (myInput->IsMousePressed(Input::MouseButton::Right))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseRight);
	}
}

void CInputMapper::Update()
{
	UpdateKeyboardInput();
	UpdateMouseInput();
	myInput->update();
}

void CInputMapper::MapEvent(const IInputObserver::EInputAction aInputEvent, const IInputObserver::EInputEvent aOutputEvent)
{
	myEvents[aInputEvent] = aOutputEvent;
}

bool CInputMapper::AddObserver(const IInputObserver::EInputEvent aEventToListenFor, IInputObserver* aObserver)
{
	ENGINE_ERROR_BOOL_MESSAGE(aObserver, "InputObsever is nullptr!");
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	if(it == myObservers[aEventToListenFor].end())
		myObservers[aEventToListenFor].emplace_back(aObserver);
	return true;
}

bool CInputMapper::RemoveObserver(const IInputObserver::EInputEvent aEventToListenFor, IInputObserver* aObserver)
{
	ENGINE_ERROR_BOOL_MESSAGE(aObserver, "InputObsever is nullptr!");
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	if(it != myObservers[aEventToListenFor].end())
		myObservers[aEventToListenFor].erase(it);
	return true;
}

bool CInputMapper::HasObserver(const IInputObserver::EInputEvent aEventToListenFor, IInputObserver* aObserver)
{
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	return it == myObservers[aEventToListenFor].end();
}

void CInputMapper::ClearObserverList(const IInputObserver::EInputEvent aEventToListenFor)
{
	myObservers[aEventToListenFor].clear();
}
