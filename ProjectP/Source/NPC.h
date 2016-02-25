#ifndef NPC_H_
#define NPC_H_
#include "MatrixStack.h"
#include "Camera3.h"
#include <string>

using std::string;

struct StopNPC
{
	void Position(MS &temp, float x, float y, float z);
	float Interaction(Camera3& camera, float radius, float tx, float tz);

	float rotateNPC;

	bool startInteraction = true;
	bool renderDialogue = true;
	bool buttonPress = false;
	string Dialogue;

	int state_no = 0;
};

void f_getGun(bool b_getGun);

#endif //NPC_H_