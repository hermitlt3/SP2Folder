#include "Bullet.h"

Bullet::Bullet()
{

}

Bullet::~Bullet()
{

}

void Bullet::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	/*this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();*/
}

void Bullet::Update()
{

}

void Bullet::veriGudShooting()
{
	//Vector3 view = (target - position).Normalized();
}