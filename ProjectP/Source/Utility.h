#ifndef UTILITY_H_
#define	UTILITY_H_

#include "Vertex.h"
#include "Mtx44.h"

Position operator*(const Mtx44& lhs, const Position& rhs);

#endif // !UTILITY_H_
