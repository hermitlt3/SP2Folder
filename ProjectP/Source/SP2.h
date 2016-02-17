#ifndef SP2_H_
#define SP2_H_

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Vector3.h"

class SP2 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LIGHTBALL,
		GEO_QUAD,
		PLATFORM_LEFT,
		PLATFORM_RIGHT,
		PLATFORM_TOP,
		PLATFORM_BOTTOM,
		PLATFORM_FRONT,
		PLATFORM_BACK,
		TEST_BACK,
		TEST_LEFT,
		TEST_RIGHT,
		TEST_TOP,
		TEST_BOTTOM,
		TEST_FRONT,
		GEO_TEXT,

		ASTEROID,

		PLAYER,
		PORTALCASE,
		PORTAL,
		PORTAL2,
		PORTAL3,
		PORTAL4,
		PORTAL5,

		PLANET1,
		PLANET2,
		PLANET3,
		PLANET4,
		PLANET5,

		GLASS,
		BASE,
		WINGS,
		SWITCH,
		HOLDER,
		WHEEL,
		STAND,

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
	SP2();
	~SP2();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderMesh(Mesh *mesh, bool enableLight);
	virtual void RenderText(Mesh* mesh, std::string text, Color color);
	virtual void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float rotateCase;
	float rotatePortal;

	Camera3 camera;

	Mesh* meshList[NUM_GEOMETRY];

	MS modelStack, viewStack, projectionStack;

	Light light[2];

	//void RenderMesh(Mesh *mesh, bool enableLight);
	void RenderSkyBox();
	void collisionCheck(float colliX, float colliZ, Camera3 &camera, Vector3 radius);

	bool toggleLight;

	float rotateSwitch;

	Vector3 glassFrontColli;
	Vector3 glassRightColli;
	Vector3 glassLeftColli;
	Vector3 baseBackColli;
	Vector3 portalColli;

	};


#endif // SP2_H

