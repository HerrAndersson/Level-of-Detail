#pragma once
#include "stdafx.h"

using namespace DirectX;
using namespace Math;

class SimpleCamera
{
public:
	enum CameraMode { MOUSE, ARROWS };

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
	};

	CameraMode cameraMode;

	float3 initialPosition;
	float3 position;

	float yaw;					// Relative to the +z axis.
	float pitch;				// Relative to the xz plane.

	float3 lookDirection;
	float3 upDirection;

	float moveSpeed;			// Speed at which the camera moves, in units per second.
	float turnSpeed;			// Speed at which the camera turns, in radians per second.

	KeysPressed keysPressed;

	void Reset();

public:

	SimpleCamera();
	~SimpleCamera();

	void Init(XMFLOAT3 position, CameraMode mode);
	void Update(float elapsedSeconds, bool mouseMoved, float2 difference);

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix(float fov, float aspectRatio, float nearPlane = 1.0f, float farPlane = 1000.0f);

	void SetMoveSpeed(float unitsPerSecond);
	void SetTurnSpeed(float radiansPerSecond);

	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);
};
