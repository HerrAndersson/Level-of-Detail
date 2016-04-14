#pragma once
#include "stdafx.h"

using namespace DirectX;
using namespace Math;

class Camera
{
public:
	enum CameraMode { MOUSE, ARROWS, SCRIPTED };

private:

	struct KeysPressed
	{
		bool w;
		bool a;
		bool s;
		bool d;

		bool left;
		bool right;
		bool up;
		bool down;

		bool q;
		bool e;
	};

	CameraMode cameraMode;
	KeysPressed keysPressed;

	float3 initialPosition;
	float3 position;
	float3 rotation;
	float3 lookAt;

	float3 lookDirection;
	float3 upDirection;

	float moveSpeed;			// Speed at which the camera moves, in units per second.
	float turnSpeed;			// Speed at which the camera turns, in radians per second.

	void UpdateArrowsMode(float elapsedSeconds);
	void UpdateMouseMode(float elapsedSeconds, bool mouseMoved, float2 difference);
	void UpdateScriptedMode(float totalSeconds);

	float3 MoveForward(float elapsedSeconds);
	float3 MoveRight(float elapsedSeconds);

	void Reset();

public:

	Camera();
	~Camera();

	void Init(XMFLOAT3 position, CameraMode mode);
	void Update(float elapsedSeconds, float totalSeconds, bool mouseMoved, float2 difference);

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix(float fov, float aspectRatio, float nearPlane = 1.0f, float farPlane = 1000.0f);
	float3 GetPosition() const;
	float3 GetRotation() const;
	CameraMode GetCameraMode() const;

	void SetMoveSpeed(float unitsPerSecond);
	void SetTurnSpeed(float radiansPerSecond);
	void SetPosition(float3 newPos);
	void SetRotation(float3 newRot);
	void SetLookAt(float3 lookAt);
	void SetCameraMode(CameraMode mode);

	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);
};
