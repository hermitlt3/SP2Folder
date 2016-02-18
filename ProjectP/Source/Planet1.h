#ifndef PLANET1_H_
#define PLANET1_H_

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Vector3.h"

class PLANET1 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LIGHTBALL,
		GEO_QUAD,
		TEST_BACK,
		TEST_LEFT,
		TEST_RIGHT,
		TEST_TOP,
		TEST_BOTTOM,
		TEST_FRONT,
		GEO_TEXT,
		ASTEROID,
		SPIN,
		SPINCAP,
		POSITION,
		GALLERY_WALL,
<<<<<<< HEAD
=======
<<<<<<< HEAD
		BUTTON,
		BUTTONSTAND,
=======
<<<<<<< HEAD
		PICFRAME,
=======
>>>>>>> 30ae59f77bcd68082db73f34f56a0276bdbb666e
>>>>>>> f41d26dce52e7ecebe8398a54657aaa121c23257
>>>>>>> 8d4eaa54a5e77f39f3b2af8adcfee8f8f73b7335
>>>>>>> 13ee25131f09d2b52497d3ac7bfcdc9957e75dbc
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
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float rotatespin;
	float translateButton;
	bool spin1;

	Camera3 camera;

	Mesh* meshList[NUM_GEOMETRY];

	MS modelStack, viewStack, projectionStack;

	Light light[2];

	void RenerMesh(Mesh *mesh, bool enableLight);
	void RenderSkyBox();

};


#endif // PLANET1_H

