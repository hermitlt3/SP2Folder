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
<<<<<<< HEAD
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
=======
		SPIN,
		SPINCAP,
		POSITION,
		GALLERY_WALL,
		BUTTON,
		BUTTONSTAND,
		GEM,
		GEM2,
		GEM3,
		GEM4,
		GEMCASE,
		END,
		PICFRAME,
>>>>>>> 68816d2da8270236f56e0672128d121ca9fcf7aa
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
<<<<<<< HEAD
=======
	float rotatespin;
	float translateButton;
	bool spin1;

	bool gem1;
	bool gem2;
	bool gem3;
	bool gem4;
	bool complete1;
	bool complete2;
	bool complete3;
	bool complete4;
	float rotategem1;
	float rotategem2;
	float rotategem3;
	float rotategem4;
	float translateEND;
	float translategem1;
	float translategem2;
	float translategem3;
	float translategem4;
	float flygem1;
	float flygem2;
	float flygem3;
	float flygem4;

	float scaleFinish;


>>>>>>> 68816d2da8270236f56e0672128d121ca9fcf7aa

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
	float firingRangeX, firingRangeZ;
	float rangeFiringRangeX, rangeFiringRangeZ;
	bool nearFiringRange;
	void SimpleVariables();
	void MethCalculations();
	void gunFunctioning();
	void renderGunOnHand();
	void renderGunUI();
	void shooting();
	float test2 = 0;
	//For Shooting
	vector<Vector3> something;
	vector<Vector3> anotherSomething;
	void moreShooting();
};


#endif

