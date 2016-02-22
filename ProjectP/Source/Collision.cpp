#include "Collision.h"

void collisionCheck(float colliX, float colliZ, Camera3 &camera, Vector3 radius)
{
	Vector3 view = (camera.target - camera.position).Normalized();
	if (camera.position.x >= colliX - radius.x  && camera.position.x - 2.f <= colliX - radius.x && camera.position.z <= colliZ + radius.z && camera.position.z >= colliZ - radius.z)
	{
		camera.position.x = colliX - radius.x;
	}
	if (camera.position.x <= colliX + radius.x  && camera.position.x + 2.f >= colliX + radius.x && camera.position.z <= colliZ + radius.z && camera.position.z >= colliZ - radius.z)
	{
		camera.position.x = colliX + radius.x;
	}
	if (camera.position.z >= colliZ - radius.z  && camera.position.z - 2.f <= colliZ - radius.z && camera.position.x <= colliX + radius.x && camera.position.x >= colliX - radius.x)
	{
		camera.position.z = colliZ - radius.z;
	}
	if (camera.position.z <= colliZ + radius.z  && camera.position.z + 2.f >= colliZ + radius.z && camera.position.x <= colliX + radius.x && camera.position.x >= colliX - radius.x)
	{
		camera.position.z = colliZ + radius.z;
	}
	camera.target = camera.position + view;
}
