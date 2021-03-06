#include "Component.h"

class CCameraComponent;
class CGameObject;
class CPlayerControllerComponent;

class CCameraControllerComponent : public CComponent
{
public:
	enum class ECameraMode
	{
		Player,
		FreeCam,
		MenuCam
	};

public:
	CCameraControllerComponent(CGameObject& gameObject,  float aCameraMoveSpeed = 25.0f, ECameraMode aCameraMode = ECameraMode::Player, char aToggleFreeCam = 'f', DirectX::SimpleMath::Vector3 aOffset = {0.f, 0.f, 0.f});
	~CCameraControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;


private:
	void UpdateFreeCam();

	const char myToggleFreeCam;
	ECameraMode myCameraMode;
	float myCameraMoveSpeed;
	CCameraComponent* myCamera;
	CPlayerControllerComponent* myPlayer;
	DirectX::SimpleMath::Vector3 myOffset;
};
