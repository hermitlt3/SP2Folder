#ifndef _BULLET_H
#define _BULLET_H

#include "Planet2.h"

class Bullet
{
private:

public:
	Vector3 Position;
	Vector3 Target;
	Vector3 Up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Bullet();
	~Bullet();
	void veriGudShooting();
	void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	void Update();
protected:


};

#endif // !_BULLET_H
