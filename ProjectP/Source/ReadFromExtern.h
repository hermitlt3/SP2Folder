#ifndef READ_FROM_EXTERN_H_
#define READ_FROM_EXTERN_H_
#include <vector>

bool LoadCollision(
	const char *file_path,
	std::vector<float> & collisionX,
	std::vector<float> & collisionZ
);

#endif //READ_FROM_EXTERN_H_