#ifndef PLANET1_H_
#define PLANET1_H_

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Vector3.h"
#include "NPC.h"

#include <vector>
#include <iterator>

using std::vector;

class PLANET1 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LIGHTBALL,
		GEO_QUAD,
		BACK,
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		FRONT,
		WALL,
		GUN,
		GROUND,

		LEG,
		LEG2,
		ARM,
		ARM2,
		HEAD,
		CHEST,

		GEO_TEXT,
		ASTEROID,

		BULLET,
		QUAD,

		POSITION,
		NUM_GEOMETRY,
	};
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		U_LIGHT1ENABLED,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,
		U_NUMLIGHTS,
		U_TOTAL,
	};

public:
	PLANET1();
	~PLANET1();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderMesh(Mesh *mesh, bool enableLight);
	virtual void RenderText(Mesh* mesh, std::string text, Color color);
	virtual void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	virtual void RenderNPC(bool enableLight);
	virtual void RenderHandOnScreen();
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera3 camera;

	Mesh* meshList[NUM_GEOMETRY];

	MS modelStack, viewStack, projectionStack;

	Light light[2];

	void RenerMesh(Mesh *mesh, bool enableLight);
	void RenderSkyBox();

	StopNPC pla2npc;
	float MS_rotate;
	bool MS_reverse;
	float GunX, GunZ, rangeGunX, rangeGunZ;
	bool inRange, holdingGun;
	float rotateA, rotateB, rotateC;
	float rotateA2, rotateB2, rotateC2;
	float firingRangeX, firingRangeZ;
	float rangeFiringRangeX, rangeFiringRangeZ;
	bool nearFiringRange;
	int firingNow, rateOfFire, typeOfFire;
	void SimpleVariables();
	void MethCalculations();
	void gunFunctioning();
	void renderGunOnHand();
	void renderGunUI();
	void shooting();
	float test2 = 0;
<<<<<<< HEAD
	bool bulletCollision(Vector3 bulletPosition, Vector3 targetPosition);
	bool  astralHit1, astralHit2, astralHit3, astralHit4, astralHit5, astralHit6, astralHit7, astralHit8, astralHit9, astralHit10;
	bool  AstralHit1, AstralHit2, AstralHit3, AstralHit4, AstralHit5, AstralHit6, AstralHit7, AstralHit8, AstralHit9, AstralHit10;
=======

	bool bulletCollision(Vector3 bulletPosition, Vector3 targetPosition);
	bool  astralHit1, astralHit2, astralHit3, astralHit4, astralHit5, astralHit6, astralHit7, astralHit8, astralHit9, astralHit10;
>>>>>>> e6984df0f69e6d11589fff09d58b4a0763af4da3
	int astralCounter;
	float astral1X, astral1Y, astral1Z;
	float astral2X, astral2Y, astral2Z;
	float astral3X, astral3Y, astral3Z;
	float astral4X, astral4Y, astral4Z;
	float astral5X, astral5Y, astral5Z;
	float astral6X, astral6Y, astral6Z;
	float astral7X, astral7Y, astral7Z;
	float astral8X, astral8Y, astral8Z;
	float astral9X, astral9Y, astral9Z;
	float astral10X, astral10Y, astral10Z;
	bool astralFinish1, astralFinish2, astralFinish3, astralFinish4, astralFinish5, astralFinish6, astralFinish7, astralFinish8, astralFinish9, astralFinish10;
	float rotateAstral1;
	void AstroidFunction();
	void CheatSystem();
<<<<<<< HEAD
	bool ironSight;
=======

>>>>>>> e6984df0f69e6d11589fff09d58b4a0763af4da3

	//For Shooting
	vector<Vector3> ShotsFired;
	vector<Vector3> DirectionFired;
	void moreShooting();
};


#endif

