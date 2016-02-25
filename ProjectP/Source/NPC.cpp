#include "NPC.h"
#include "Application.h"
#include "MyMath.h"
#include "ReadFromExtern.h"

StopNPC::StopNPC()
{

}

void StopNPC::attriSet(Vector3 translate, Vector3 rotate, const char *file_path)
{
	this->translate = translate;
	this->rotate = rotate;
	textfile = file_path;
	LoadSentences();
	talk = sentences.begin();
}

void StopNPC::Interaction(Camera3 camera, Vector3 interactCoord, Vector3 radius, bool bool_)
{
	if (camera.position.x <= interactCoord.x + radius.x && camera.position.x >= interactCoord.x - radius.x && camera.position.z <= interactCoord.z + radius.z && camera.position.z >= interactCoord.z - radius.z)
	{
		float tempx = camera.position.x - translate.x;
		float tempz = camera.position.z - translate.z;
		if (bool_)
			rotateNPC = atan(tempx / tempz) * 180 / Math::PI;
		else
			rotateNPC = atan(tempz / tempx) * 180 / Math::PI;

	}
	else
		rotateNPC = 0.f;
}


void StopNPC::LoadSentences()
{
	LoadText(textfile, sentences);
}

void StopNPC::Interaction(Camera3 camera, bool b_talk)
{
	Vector3 temp_view = (camera.target - camera.position);
	Vector3 view_to = (translate - camera.position);
	radiusDiff = (view_to - temp_view).Length();

	if ((b_talk) && radiusDiff < 20.0)
	{
		b_Chat = true;
	}
	else 
		b_Chat = false;
}

void StopNPC::Communication()
{
	if (b_Chat && Application::IsKeyPressed(VK_LBUTTON) && timer > 0.5)
	{
		if (talk != sentences.end() - 1)
		{
			talk ++;
		}
		else
		{
			b_Quest = true;
			b_Chat = false;
		}

		timer = 0.0;
	}
}

void StopNPC::delayTime(double dt)
{
	timer += dt;
}