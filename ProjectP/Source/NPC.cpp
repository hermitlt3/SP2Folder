#include "NPC.h"
#include "Application.h"
#include "MyMath.h"

//void NPC::Interaction(Camera3& camera, float radius)
//{
//	if (Application::IsKeyPressed(VK_LBUTTON) && 
//		camera.target.x >= tx - radius && 
//		camera.target.x <= tx + radius && 
//		camera.target.y >= ty - radius && 
//		camera.target.y <= ty + radius && 
//		camera.target.z >= tz - radius && 
//		camera.target.z <= tz + radius 
//		)
//	{
//		state = 1;
//	}
//	if (Application::IsKeyPressed(VK_RBUTTON) && camera.target.x >= tx - radius && camera.target.x <= tx + radius && camera.target.y >= ty - radius && camera.target.y <= ty + radius && camera.target.z >= tz - radius && camera.target.z <= tz + radius)
//		state = 0;
//}
//
//3
//void NPC::Update()
//{
//	if (state == 0)
//	{
//		isMoving = false;
//		r_angle += 180.f;
//	}
//	if (state == 1)
//	{
//		isMoving = true;
//	}
//}
//
////void NPC::Position(MS &temp, float x, float y, float z)
////{
////	temp.Translate(tx, ty, tz);
////	temp.Rotate(r_angle, x, y, z);
////}
//
//void NPC::Movement(double dt, bool x, bool z)
//{
//	if (isMoving == true)
//	{
//		if (rotateArm < 30.f && armReverse == false)
//		{
//			rotateArm += (float)(80.0 * dt);
//		}
//		else
//			armReverse = true;
//		if (rotateArm > -30.f && armReverse == true)
//		{
//			rotateArm -= (float)(80.0 * dt);
//		}
//		else
//			armReverse = false;
//
//
//		if (rotateLeg < 30.f && legReverse == false)
//		{
//			rotateLeg += (float)(80.0 * dt);
//		}
//		else
//			legReverse = true;
//		if (rotateLeg > -30.f && legReverse == true)
//		{
//			rotateLeg -= (float)(80.0 * dt);
//		}
//		else
//			legReverse = false;
//		if (x)
//		{
//			if (tx <= dist && !moveReverse)
//				tx += (float)(10.0 * dt);
//			else
//				moveReverse = true;
//			if (tx >= -dist && moveReverse)
//				tx += (float)(10.0 * dt);
//			else
//				moveReverse = false;
//
//		}
//		else if (z)
//		{
//			if (tz <= -dist && !moveReverse)
//				tz += (float)(10.0 * dt);
//			else
//				moveReverse = true;
//			if (tz >= -dist && moveReverse)
//				tz += (float)(10.0 * dt);
//			else
//				moveReverse = false;
//		}
//	}
//}

float StopNPC::Interaction(Camera3& camera, float radius, float tx, float tz)
{
	float returnangle;

	float tempx;
	float tempz;
	returnangle = 0.f;

	if ((camera.position.x >= tx - radius  && camera.position.x <= tx + radius) && (camera.position.z >= tz - radius && camera.position.z <= tz + radius))
	{
		if (Application::IsKeyPressed('E'))
			buttonPress = true;
		if (buttonPress)
		{
			tempx = (camera.position.x - tx);
			tempz = (camera.position.z - tz);
			returnangle = atan(tempx / tempz) * 180 / Math::PI;
		}
	}
	else
		buttonPress = false;
	return returnangle;
}

