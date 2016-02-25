#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Scene.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Vector3.h"
#include "Camera3.h"
#include <vector>

class MAINMENU : public Scene
{
	enum GEOMETRY_TYPE
	{
		TEXT,
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
	MAINMENU();
	~MAINMENU();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera3 camera;

	Mesh* meshList[NUM_GEOMETRY];

	MS modelStack, viewStack, projectionStack;

	std::vector<std::string>::reverse_iterator st;
	std::vector<std::string>menuList;

};
#endif //MAIN_MENU_H