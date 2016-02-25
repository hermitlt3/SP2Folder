#ifndef PLANET2_H_
#define PLANET2_H_

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Vector3.h"

class PLANET2 : public Scene
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

		MAZE_BOUND,
		MAZE_FLOOR,
		GUESS_FLOOR,
		GUESS_WALLS,
		GUESS_DOORS,
		GUESS_WRONG,
		BLUFF_TWALLS,
		BLUFF_FWALLS1,
		BLUFF_FWALLS2,
		BLUFF_FWALLS3,
		BLUFF_FWALLS4,
		BLUFF_FWALLS5,
		BLUFF_FWALLS6,
		BLUFF_FWALLS7,
		BLUFF_FWALLS8,
		BLUFF_FWALLS9,
		BLUFF_FWALLS10,
		BLUFF_FWALLS11,
		BLUFF_FWALLS12,
		INFER_FLOOR,
		INFER_CEILING,
		INFER_WALLS,
		GLASS_BOUND,
		GLASS_FLOOR,
		GLASS_WALLS,

		KEYSHARDS,

		GEO_TEXT,
		ASTEROID,
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
	PLANET2();
	~PLANET2();

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

	Camera3 camera;

	Mesh* meshList[NUM_GEOMETRY];

	MS modelStack, viewStack, projectionStack;

	Light light[2];

	//void RenerMesh(Mesh *mesh, bool enableLight);
	void RenderSkyBox();

	Vector3 mazeColliOne;
	Vector3 mazeColliTwo;
	Vector3 mazeColliThree;
	Vector3 mazeColliFour;
	Vector3 mazeColliFive;
	Vector3 mazeColliSix;
	Vector3 mazeColliSeven;
	Vector3 mazeColliEight;
	Vector3 mazeColliNine;			//VECTORS FOR MAZE BOUNDARIES
	Vector3 guessColliWall;
	Vector3 guessColliWallplusDoor;	//VECTORS FOR GUESS AREA
	Vector3 guessColliRow;			//VECTOR FOR BEFORE FINDING ACTUAL DOOR
	Vector3 wallsColliOne;
	Vector3 wallsColliTwo;
	Vector3 wallsColliThree;
	Vector3 wallsColliFour;
	Vector3 wallsColliFive;
	Vector3 wallsColliSix;
	Vector3 wallsColliSeven;
	Vector3 wallsColliEight;
	Vector3 wallsColliNine;
	Vector3 wallsColliTen;
	Vector3 wallsColliEleven;
	Vector3 wallsColliTwelve;
	Vector3 wallsColliThirteen;
	Vector3 wallsColliFourteen;
	Vector3 wallsColliFifteen;
	Vector3 wallsColliSixteen;
	Vector3 wallsColliSeventeen;
	Vector3 wallsColliEighteen;
	Vector3 wallsColliNineteen;
	Vector3 wallsColliTwenty;
	Vector3 wallsColliTwentyone;
	Vector3 wallsColliTwentytwo;
	Vector3 wallsColliTwentythree;
	Vector3 wallsColliTwentyfour;
	Vector3 wallsColliTwentyfive;	//VECTORS FOR BLUFF AND INFER AREA
	Vector3 glassColliOne;
	Vector3 glassColli;				//VECTORS FOR SKILL AREA
	//Vector3 shardColli;

	bool wrongDoorA;
	bool wrongDoorB;
	bool wrongDoorC;
	bool wrongDoorD;
	bool wrongDoorE;
	bool wrongDoorF;
	bool wrongDoorG;
	bool wrongDoorH;
	bool actualDoor1;
	bool actualDoor2;
	bool actualDoor3;
	bool actualDoor4;
	bool fakeWall1;
	bool fakeWall2;
	bool fakeWall3;
	bool fakeWall4;
	bool fakeWall5;
	bool fakeWall6;
	bool fakeWall7;
	bool fakeWall8;
	bool fakeWall9;
	bool fakeWall10;
	bool fakeWall11;
	bool fakeWall12;
	bool pickUpShard1;
	bool pickUpShard2;
	bool pickUpShard3;
	bool pickUpShard4;
	bool pickUpShard5;

};


#endif // PLANET2_H