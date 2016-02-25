#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"
#include <GLFW\glfw3.h>

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void Camera3::Update(double dt)
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	static const float CAMERA_SPEED = 50.f;
	static const float BULLET_SPEED = 300.f;
	POINT mousepos;
	int midX = mode->width / 2;
	int midY = mode->height / 2;

	GetCursorPos(&mousepos);
	if (mousepos.y != midY)
	{
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		float rotateZ = (float)(midY - mousepos.y) / 10.f;
		ShowCursor(false);

		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();

		Mtx44 rotation;
		rotation.SetToRotation(rotateZ, right.x, right.y, right.z);
		view = rotation * view;
		if (view.y < 0.9f && view.y > -0.9f)
		{
			target = position + view;
			up = rotation * up;
		}
		SetCursorPos(midX, midY);
	}
	if (mousepos.x != midX)
	{
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		float rotateX = (float)(midX - mousepos.x) / 10.f;
		Mtx44 rotation;
		rotation.SetToRotation(rotateX, 0, 1, 0);

		view = rotation * view;
		target = position + view;
		up = rotation * up;
		SetCursorPos(midX, midY);
	}
		if (Application::IsKeyPressed('W'))
		{
			Vector3 view = (target - position).Normalized();
			Vector3 right = view.Cross(up);
			up = right.Cross(view).Normalized();
			target.x += view.x * (float)(CAMERA_SPEED * dt);
			target.z += view.z * (float)(CAMERA_SPEED * dt);
			position.x += view.x * (float)(CAMERA_SPEED * dt);
			position.z += view.z * (float)(CAMERA_SPEED * dt);
		}

		if (Application::IsKeyPressed('S'))
		{
			Vector3 view = (target - position).Normalized();
			Vector3 right = view.Cross(up);
			up = right.Cross(view).Normalized();
			target.x += view.x * (float)(-CAMERA_SPEED * dt);
			target.z += view.z * (float)(-CAMERA_SPEED * dt);
			position.x += view.x * (float)(-CAMERA_SPEED * dt);
			position.z += view.z * (float)(-CAMERA_SPEED * dt);
		}
		if (Application::IsKeyPressed('A'))
		{
			Vector3 view = (target - position).Normalized();
			Vector3 right = view.Cross(up);
			up = right.Cross(view).Normalized();
			target.x += right.x * (float)(-CAMERA_SPEED * dt);
			target.z += right.z * (float)(-CAMERA_SPEED * dt);
			position.x += right.x * (float)(-CAMERA_SPEED * dt);
			position.z += right.z * (float)(-CAMERA_SPEED * dt);
		}
		if (Application::IsKeyPressed('D'))
		{
			Vector3 view = (target - position).Normalized();
			Vector3 right = view.Cross(up);
			up = right.Cross(view).Normalized();
			target.x += right.x * (float)(CAMERA_SPEED * dt);
			target.z += right.z * (float)(CAMERA_SPEED * dt);
			position.x += right.x * (float)(CAMERA_SPEED * dt);
			position.z += right.z * (float)(CAMERA_SPEED * dt);
		}
		//Part of shooting
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		bullets = view * (float)(BULLET_SPEED * dt);
		positions = position + view * (float)(BULLET_SPEED * dt);

	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
	
}

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

