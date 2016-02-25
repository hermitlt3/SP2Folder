#ifndef NPC_H_
#define NPC_H_
#include "MatrixStack.h"
#include "Camera3.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

class StopNPC
{
public:
	StopNPC();
	void attriSet(Vector3 translate, Vector3 rotate, const char *file_path);
	void Interaction(Camera3 camera, Vector3 interactCoord, Vector3 radius, bool bool_);
	void Interaction(Camera3 camera, bool b_talk);
	void Communication();
	void LoadSentences();

	float rotateNPC = 0.0f;
	float radiusDiff = 0.0f;

	void delayTime(double dt);

	vector<string>sentences;
	vector<string>::iterator talk;

	int state_no = 0;

	bool b_Chat = false;
	bool b_Quest = false;

	Vector3 getTranslate() { return translate; };
	Vector3 getRotate() { return rotate; };

private:
	Vector3 translate;
	Vector3 rotate;
	const char* textfile;
	double timer = 0.0;
};

void f_getGun(bool b_getGun);

#endif //NPC_H_