
#include "Camera.h"

Camera::Camera():
	initialPosition(0, 0, 0),
	position(initialPosition),
	rotation(0,0,0),
	lookAt(0,1,0),
	lookDirection(0, 0, 1),
	upDirection(0, 1, 0),
	moveSpeed(20.0f),
	turnSpeed(XM_PIDIV2),
	cameraMode(ARROWS)
{
	ZeroMemory(&keysPressed, sizeof(keysPressed));
}

Camera::~Camera()
{

}

void Camera::Init(XMFLOAT3 position, CameraMode mode)
{
	initialPosition = position;
	cameraMode = mode;
	Reset();
}

void Camera::Reset()
{
	position = initialPosition;
	rotation = float3(0, 0, 0);
	lookDirection = { 0, 0, 1 };
}

void Camera::Update(float elapsedSeconds, float totalSeconds, bool mouseMoved, float2 difference)
{
	if (cameraMode == ARROWS)
		UpdateArrowsMode(elapsedSeconds);
	else if (cameraMode == MOUSE)
		UpdateMouseMode(elapsedSeconds, mouseMoved, difference);
	else if (cameraMode == SCRIPTED)
		UpdateScriptedMode(totalSeconds);

	if (abs(difference.x) > F_EPSILON + 1.0f && abs(difference.y) > F_EPSILON + 1.0f)
	{
		printf((std::to_string(difference.x) + " " + std::to_string(difference.y)).c_str());
		printf("\n");
	}
}

void Camera::UpdateScriptedMode(float totalSeconds)
{
	//position.x = sin(totalSeconds) * 10;
	//position.y = sin(totalSeconds) * 10;
	//position.z = cos(totalSeconds) * 10;

	position.z = (sin(totalSeconds) * 25) - 28;
}

void Camera::UpdateArrowsMode(float elapsedSeconds)
{
	// Calculate the move vector in camera space.
	float3 move(0, 0, 0);

	if (keysPressed.a)
		move.x -= 1.0f;
	if (keysPressed.d)
		move.x += 1.0f;
	if (keysPressed.s)
		move.z -= 1.0f;
	if (keysPressed.w)
		move.z += 1.0f;

	if (fabs(move.x) > 0.1f && fabs(move.z) > 0.1f)
		move = move.Normalize(move);

	float moveInterval = moveSpeed * elapsedSeconds;
	float rotateInterval = turnSpeed * elapsedSeconds;

	if (keysPressed.left)
		rotation.y -= rotateInterval;
	if (keysPressed.right)
		rotation.y += rotateInterval;
	if (keysPressed.up)
		rotation.x += rotateInterval;
	if (keysPressed.down)
		rotation.x -= rotateInterval;

	// Prevent looking too far up or down.
	rotation.x = min(rotation.x, XM_PIDIV4);
	rotation.x = max(-XM_PIDIV4, rotation.x);

	// Move the camera in model space.
	float x = move.x * -cosf(rotation.y) - move.z * sinf(rotation.y);
	float z = move.x * sinf(rotation.y) - move.z * cosf(rotation.y);
	position.x += x * moveInterval;
	position.z += z * moveInterval;

	// Determine the look direction.
	float r = -cosf(rotation.x);
	lookDirection.x = r * sinf(rotation.y);
	lookDirection.y = sinf(rotation.x);
	lookDirection.z = r * cosf(rotation.y);

	if (keysPressed.q)
		position.y -= 1.0f * moveInterval;
	else if (keysPressed.e)
		position.y += 1.0f*moveInterval;
}

void Camera::UpdateMouseMode(float elapsedSeconds, bool mouseMoved, float2 difference)
{
	float rotateInterval = turnSpeed * elapsedSeconds;

	if (mouseMoved)
	{
		rotation.y += difference.x * rotateInterval;
		rotation.x += difference.y * rotateInterval;

		//Keep y-rotation within 2PI-bounds
		if (rotation.y < 0.0f)
			rotation.y += XM_2PI;
		else if (rotation.y > XM_2PI)
			rotation.y -= XM_2PI;

		//Prevent looking too far up or down
		rotation.x = min(rotation.x, XM_PIDIV4);
		rotation.x = max(-XM_PIDIV4, rotation.x);

		float r = cosf(rotation.x);
		lookDirection.x = r * sinf(rotation.y);
		lookDirection.y = sinf(-rotation.x);
		lookDirection.z = r * cosf(rotation.y);
	}

	if (keysPressed.w)
		position += MoveForward(elapsedSeconds);
	else if (keysPressed.s)
		position -= MoveForward(elapsedSeconds);
	if (keysPressed.a)
		position -= MoveRight(elapsedSeconds);
	else if(keysPressed.d)
		position += MoveRight(elapsedSeconds);
}

float3 Camera::MoveForward(float elapsedSeconds)
{
	float3 move;
	float moveInterval = moveSpeed * elapsedSeconds;

	move.x = sinf(rotation.y) * moveInterval;
	move.y = -sinf(rotation.x) * moveInterval;
	move.z = cosf(rotation.y) * moveInterval;

	return move;
}

float3 Camera::MoveRight(float elapsedSeconds)
{
	float3 move;
	float moveInterval = moveSpeed * elapsedSeconds;

	float rotY = rotation.y + XM_PIDIV2;
	move.x = sinf(rotY) * moveInterval;
	move.z = cosf(rotY) * moveInterval;
	
	return move;
}

XMMATRIX Camera::GetViewMatrix()
{
	if (cameraMode == SCRIPTED)
		return XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&lookAt), XMLoadFloat3(&upDirection));
	else
		return XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&lookDirection), XMLoadFloat3(&upDirection));
}

XMMATRIX Camera::GetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	return XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}

float3 Camera::GetPosition() const
{
	return position;
}

float3 Camera::GetRotation() const
{
	return rotation;
}

Camera::CameraMode Camera::GetCameraMode() const
{
	return cameraMode;
}

void Camera::SetMoveSpeed(float unitsPerSecond)
{
	moveSpeed = unitsPerSecond;
}

void Camera::SetTurnSpeed(float radiansPerSecond)
{
	turnSpeed = radiansPerSecond;
}

void Camera::SetPosition(float3 newPos)
{
	position = newPos;
}

void Camera::SetRotation(float3 newRot)
{
	rotation = newRot;
}

void Camera::SetLookAt(float3 lookAt)
{
	this->lookAt = lookAt;
}

void Camera::SetCameraMode(Camera::CameraMode mode)
{
	cameraMode = mode;
}

void Camera::OnKeyDown(WPARAM key)
{
	switch (key)
	{
		case 'W':
			keysPressed.w = true;
			break;
		case 'A':
			keysPressed.a = true;
			break;
		case 'S':
			keysPressed.s = true;
			break;
		case 'D':
			keysPressed.d = true;
			break;
		case VK_LEFT:
			keysPressed.left = true;
			break;
		case VK_RIGHT:
			keysPressed.right = true;
			break;
		case VK_UP:
			keysPressed.up = true;
			break;
		case VK_DOWN:
			keysPressed.down = true;
			break;
		case 'Q':
			keysPressed.q = true;
			break;
		case 'E':
			keysPressed.e = true;
			break;
		case VK_ESCAPE:
			Reset();
			break;
		default:
			break;
	}
}

void Camera::OnKeyUp(WPARAM key)
{
	switch (key)
	{
		case 'W':
			keysPressed.w = false;
			break;
		case 'A':
			keysPressed.a = false;
			break;
		case 'S':
			keysPressed.s = false;
			break;
		case 'D':
			keysPressed.d = false;
			break;
		case VK_LEFT:
			keysPressed.left = false;
			break;
		case VK_RIGHT:
			keysPressed.right = false;
			break;
		case VK_UP:
			keysPressed.up = false;
			break;
		case VK_DOWN:
			keysPressed.down = false;
			break;
		case 'Q':
			keysPressed.q = false;
			break;
		case 'E':
			keysPressed.e = false;
			break;
		default:
			break;
	}
}
