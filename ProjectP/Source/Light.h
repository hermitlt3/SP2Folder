#ifndef LIGHT_H_
#define LIGHT_H_

#include "Vertex.h"
#include "Vector3.h"

struct Light
{
	Position position;
	Color color;
	float power;
	float kC, kL, kQ;
	Light()
	{
		position.Set(0, 0, 0);
		color.Set(1, 1, 1);
		power = 1.f;
		kC = kL = kQ = 0.f;
	}
	enum LIGHT_TYPE
	{
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL,
		LIGHT_SPOT,
	};
	LIGHT_TYPE type;
	Vector3 spotDirection;
	float cosCutoff;
	float cosInner;
	float exponent;
};

#endif//LIGHT_H_
