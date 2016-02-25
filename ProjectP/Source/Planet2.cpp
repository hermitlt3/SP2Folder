#include "PLANET2.h"
#include "GL\glew.h"
#include "LoadTGA.h"
#include "timer.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "Gamemode.h"
#include "Collision.h"

#include <sstream>
#include <iomanip>
#include <fstream>
PLANET2::PLANET2()
{
}

PLANET2::~PLANET2()
{
}

void PLANET2::Init()
{
	// Init VBO here
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Set background color
	glEnable(GL_DEPTH_TEST); // Enable depth buffer and depth testing
	glEnable(GL_CULL_FACE); // Enable back face culling
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default to fill mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights"); //in case you missed out practical 7
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	glUseProgram(m_programID);

	//variable to rotate geometry

	//Initialize camera settings
	//camera.Init(Vector3(-20, 7.2, -5), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(Vector3(20, 50, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("AXES", 1000, 1000, 1000);

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 100000.f);
	projectionStack.LoadMatrix(projection);

	light[0].type = Light::LIGHT_POINT;
	light[0].position.Set(0, 48, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1.f;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;

	light[1].type = Light::LIGHT_DIRECTIONAL;
	light[1].position.Set(0, 300, -400);
	light[1].color.Set(1, 1, 1);
	light[1].power = 1;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LIGHT", Color(1, 1, 1), 36, 36);

	meshList[TEST_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[TEST_FRONT]->textureID = LoadTGA("Image//surround.tga");

	meshList[TEST_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[TEST_RIGHT]->textureID = LoadTGA("Image//surround.tga");

	meshList[TEST_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[TEST_TOP]->textureID = LoadTGA("Image//top.tga");

	meshList[TEST_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[TEST_BOTTOM]->textureID = LoadTGA("Image//top.tga");

	meshList[TEST_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[TEST_LEFT]->textureID = LoadTGA("Image//surround.tga");

	meshList[TEST_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[TEST_BACK]->textureID = LoadTGA("Image//surround.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//Redressed.tga");

	meshList[ASTEROID] = MeshBuilder::GenerateOBJ("asteroid", "OBJ//asteroid.obj");
	meshList[ASTEROID]->textureID = LoadTGA("Image//asteroid.tga");
	meshList[ASTEROID]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kShininess = 1.5f;

	meshList[MAZE_BOUND] = MeshBuilder::GenerateOBJ("mazeBound", "OBJ//mazeBound.obj");
	meshList[MAZE_BOUND]->textureID = LoadTGA("Image//spacePanels.tga");
	meshList[MAZE_BOUND]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[MAZE_BOUND]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[MAZE_BOUND]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[MAZE_BOUND]->material.kShininess = 1.f;

	meshList[MAZE_FLOOR] = MeshBuilder::GenerateOBJ("mazeFloor", "OBJ//mazeFloor.obj");
	meshList[MAZE_FLOOR]->textureID = LoadTGA("Image//concrete.tga");
	meshList[MAZE_FLOOR]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[MAZE_FLOOR]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[MAZE_FLOOR]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[MAZE_FLOOR]->material.kShininess = 1.f;

	meshList[GUESS_FLOOR] = MeshBuilder::GenerateOBJ("guessFloor", "OBJ//guessFloor.obj");
	meshList[GUESS_FLOOR]->textureID = LoadTGA("Image//tiles.tga");
	meshList[GUESS_FLOOR]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GUESS_FLOOR]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GUESS_FLOOR]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GUESS_FLOOR]->material.kShininess = 1.f;

	meshList[GUESS_WALLS] = MeshBuilder::GenerateOBJ("guessWalls", "OBJ//guessWalls.obj");
	meshList[GUESS_WALLS]->textureID = LoadTGA("Image//spacePanels.tga");
	meshList[GUESS_WALLS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GUESS_WALLS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GUESS_WALLS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GUESS_WALLS]->material.kShininess = 1.f;

	meshList[GUESS_DOORS] = MeshBuilder::GenerateOBJ("guessDoors", "OBJ//guessDoors.obj");
	meshList[GUESS_DOORS]->textureID = LoadTGA("Image//shutter.tga");
	meshList[GUESS_DOORS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GUESS_DOORS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GUESS_DOORS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GUESS_DOORS]->material.kShininess = 1.f;

	meshList[GUESS_WRONG] = MeshBuilder::GenerateQuad("theX", Color(0, 0, 0));
	meshList[GUESS_WRONG]->textureID = LoadTGA("Image//theX.tga");
	meshList[GUESS_WRONG]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GUESS_WRONG]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GUESS_WRONG]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GUESS_WRONG]->material.kShininess = 1.f;

	meshList[BLUFF_TWALLS] = MeshBuilder::GenerateOBJ("bluffTrueWalls", "OBJ//bluffTrueWalls.obj");
	meshList[BLUFF_TWALLS]->textureID = LoadTGA("Image//lighter.tga");
	meshList[BLUFF_TWALLS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_TWALLS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_TWALLS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_TWALLS]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS1] = MeshBuilder::GenerateOBJ("bluffFakeWalls1", "OBJ//bluffFakeWalls1.obj");
	meshList[BLUFF_FWALLS1]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS1]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS1]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS1]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS2] = MeshBuilder::GenerateOBJ("bluffFakeWalls2", "OBJ//bluffFakeWalls2.obj");
	meshList[BLUFF_FWALLS2]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS2]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS2]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS2]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS2]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS3] = MeshBuilder::GenerateOBJ("bluffFakeWalls3", "OBJ//bluffFakeWalls3.obj");
	meshList[BLUFF_FWALLS3]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS3]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS3]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS3]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS3]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS4] = MeshBuilder::GenerateOBJ("bluffFakeWalls4", "OBJ//bluffFakeWalls4.obj");
	meshList[BLUFF_FWALLS4]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS4]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS4]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS4]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS4]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS5] = MeshBuilder::GenerateOBJ("bluffFakeWalls5", "OBJ//bluffFakeWalls5.obj");
	meshList[BLUFF_FWALLS5]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS5]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS5]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS5]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS5]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS6] = MeshBuilder::GenerateOBJ("bluffFakeWalls6", "OBJ//bluffFakeWalls6.obj");
	meshList[BLUFF_FWALLS6]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS6]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS6]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS6]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS6]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS7] = MeshBuilder::GenerateOBJ("bluffFakeWalls7", "OBJ//bluffFakeWalls7.obj");
	meshList[BLUFF_FWALLS7]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS7]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS7]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS7]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS7]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS8] = MeshBuilder::GenerateOBJ("bluffFakeWalls8", "OBJ//bluffFakeWalls8.obj");
	meshList[BLUFF_FWALLS8]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS8]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS8]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS8]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS8]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS9] = MeshBuilder::GenerateOBJ("bluffFakeWalls9", "OBJ//bluffFakeWalls9.obj");
	meshList[BLUFF_FWALLS9]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS9]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS9]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS9]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS9]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS10] = MeshBuilder::GenerateOBJ("bluffFakeWalls10", "OBJ//bluffFakeWalls10.obj");
	meshList[BLUFF_FWALLS10]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS10]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS10]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS10]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS10]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS11] = MeshBuilder::GenerateOBJ("bluffFakeWalls11", "OBJ//bluffFakeWalls11.obj");
	meshList[BLUFF_FWALLS11]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS11]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS11]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS11]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS11]->material.kShininess = 1.f;

	meshList[BLUFF_FWALLS12] = MeshBuilder::GenerateOBJ("bluffFakeWalls12", "OBJ//bluffFakeWalls12.obj");
	meshList[BLUFF_FWALLS12]->textureID = LoadTGA("Image//darker.tga");
	meshList[BLUFF_FWALLS12]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BLUFF_FWALLS12]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BLUFF_FWALLS12]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BLUFF_FWALLS12]->material.kShininess = 1.f;

	meshList[KEYSHARDS] = MeshBuilder::GenerateOBJ("keyShards", "OBJ//keyShards.obj");
	meshList[KEYSHARDS]->textureID = LoadTGA("Image//gold.tga");
	meshList[KEYSHARDS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[KEYSHARDS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[KEYSHARDS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[KEYSHARDS]->material.kShininess = 1.f;

	meshList[INFER_FLOOR] = MeshBuilder::GenerateOBJ("inferFloor", "OBJ//inferFloor.obj");
	meshList[INFER_FLOOR]->textureID = LoadTGA("Image//tiles.tga");
	meshList[INFER_FLOOR]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[INFER_FLOOR]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[INFER_FLOOR]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[INFER_FLOOR]->material.kShininess = 1.f;

	meshList[INFER_CEILING] = MeshBuilder::GenerateOBJ("inferFloor", "OBJ//inferCeiling.obj");
	meshList[INFER_CEILING]->textureID = LoadTGA("Image//tiles.tga");
	meshList[INFER_CEILING]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[INFER_CEILING]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[INFER_CEILING]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[INFER_CEILING]->material.kShininess = 1.f;

	meshList[GLASS_BOUND] = MeshBuilder::GenerateOBJ("glassBound", "OBJ//glassBound.obj");
	meshList[GLASS_BOUND]->textureID = LoadTGA("Image//glass.tga");
	meshList[GLASS_BOUND]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GLASS_BOUND]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GLASS_BOUND]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[GLASS_BOUND]->material.kShininess = 1.f;

	meshList[GLASS_FLOOR] = MeshBuilder::GenerateOBJ("glassFloor", "OBJ//glassFloor.obj");
	meshList[GLASS_FLOOR]->textureID = LoadTGA("Image//glass.tga");
	meshList[GLASS_FLOOR]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GLASS_FLOOR]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GLASS_FLOOR]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[GLASS_FLOOR]->material.kShininess = 1.f;

	meshList[GLASS_WALLS] = MeshBuilder::GenerateOBJ("glassWalls", "OBJ//glassWalls.obj");
	meshList[GLASS_WALLS]->textureID = LoadTGA("Image//glass.tga");
	meshList[GLASS_WALLS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GLASS_WALLS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GLASS_WALLS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GLASS_WALLS]->material.kShininess = 1.f;

	mazeColliOne.Set(200.f, 0.f, 4.f);
	mazeColliTwo.Set(4.f, 0.f, 90.f);
	mazeColliThree.Set(12.f, 0.f, 4.f);
	mazeColliFour.Set(4.f, 0.f, 120.f);
	mazeColliFive.Set(220.f, 0.f, 4.f);
	mazeColliSix.Set(4.f, 0.f, 200.f);
	mazeColliSeven.Set(200.f, 0.f, 4.f);
	mazeColliEight.Set(4.f, 0.f, 100.f);
	mazeColliNine.Set(180.f, 0.f, 4.f);
	guessColliWall.Set(4.f, 0.f, 6.f);
	guessColliWallplusDoor.Set(4.f, 0.f, 18.f);
	guessColliRow.Set(4.f, 0.f, 100.f);
	wallsColliOne.Set(23.f, 0.f, 4.f);
	wallsColliTwo.Set(4.f, 0.f, 41.f);
	wallsColliThree.Set(11.f, 0.f, 4.f);
	wallsColliFour.Set(4.f, 0.f, 11.f);
	wallsColliFive.Set(13.f, 0.f, 4.f);
	wallsColliSix.Set(4.f, 0.f, 21.f);
	wallsColliSeven.Set(4.f, 0.f, 7.8f);
	wallsColliEight.Set(31.f, 0.f, 4.f);
	wallsColliNine.Set(4.f, 0.f, 40.8f);
	wallsColliTen.Set(7.8f, 0.f, 4.f);
	wallsColliEleven.Set(4.f, 0.f, 29.f);
	wallsColliTwelve.Set(4.f, 0.f, 10.2f);
	wallsColliThirteen.Set(20.f, 0.f, 4.f);
	wallsColliFourteen.Set(4.f, 0.f, 30.8f);
	wallsColliFifteen.Set(18.f, 0.f, 4.f);
	wallsColliSixteen.Set(10.f, 0.f, 4.f);
	wallsColliSeventeen.Set(4.f, 0.f, 20.f);
	wallsColliEighteen.Set(4.f, 0.f, 41.f);
	wallsColliNineteen.Set(4.f, 0.f, 10.f);
	wallsColliTwenty.Set(30.f, 0.f, 4.f);
	wallsColliTwentyone.Set(29.2f, 0.f, 4.f);
	wallsColliTwentytwo.Set(4.f, 0.f, 9.f);
	wallsColliTwentythree.Set(4.f, 0.f, 51.f);
	wallsColliTwentyfour.Set(19.f, 0.f, 4.f);
	wallsColliTwentyfive.Set(4.f, 0.f, 19.f);
	glassColli.Set(4.f, 0.f, 43.f);

	wrongDoorA = false;
	wrongDoorB = false;
	wrongDoorC = false;
	wrongDoorD = false;
	wrongDoorE = false;
	wrongDoorF = false;
	wrongDoorG = false;
	wrongDoorH = false;
	actualDoor1 = false;
	actualDoor2 = false;
	actualDoor3 = false;
	actualDoor4 = false;
	fakeWall1 = false;
	fakeWall2 = false;
	fakeWall3 = false;
	fakeWall4 = false;
	fakeWall5 = false;
	fakeWall6 = false;
	fakeWall7 = false;
	fakeWall8 = false;
	fakeWall9 = false;
	fakeWall10 = false;
	fakeWall11 = false;
	fakeWall12 = false;
	pickUpShard1 = false;
	pickUpShard2 = false;
	pickUpShard3 = false;
	pickUpShard4 = false;
	pickUpShard5 = false;

}

static float ROT_LIMIT = 45.f;
static float SCALE_LIMIT = 5.f;
static float LSPEED = 10.F;
int guessCount = 0;
int shardCount = 0;

void PLANET2::Update(double dt)
{
	if (Application::IsKeyPressed('1')) //enable back face culling
		glEnable(GL_CULL_FACE);
	if (Application::IsKeyPressed('2')) //disable back face culling
		glDisable(GL_CULL_FACE);
	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	if (Application::IsKeyPressed('F'))
		GameMode::GetInstance()->gameState = 2;
	camera.Update(dt);

	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);

	collisionCheck(0, -200, camera, mazeColliOne);
	collisionCheck(-200, -110, camera, mazeColliTwo);
	collisionCheck(200, -110, camera, mazeColliTwo);
	collisionCheck(-210, -22, camera, mazeColliThree);
	collisionCheck(210, -22, camera, mazeColliThree);
	collisionCheck(-220, 100, camera, mazeColliFour);
	collisionCheck(220, 100, camera, mazeColliFour);
	collisionCheck(0, 220, camera, mazeColliFive);
	collisionCheck(0, 0, camera, mazeColliSix);
	collisionCheck(0, 0, camera, mazeColliSeven);
	collisionCheck(-200, 98, camera, mazeColliEight);
	collisionCheck(200, 98, camera, mazeColliEight);
	collisionCheck(0, 200, camera, mazeColliNine);

	collisionCheck(40, -195, camera, guessColliWall);
	collisionCheck(80, -5, camera, guessColliWall);
	collisionCheck(120, -5, camera, guessColliWall);
	collisionCheck(160, -195, camera, guessColliWall);
	collisionCheck(40, -15, camera, guessColliWallplusDoor);
	collisionCheck(80, -185, camera, guessColliWallplusDoor);
	collisionCheck(120, -185, camera, guessColliWallplusDoor);
	collisionCheck(160, -15, camera, guessColliWallplusDoor);

	collisionCheck(20, 20, camera, wallsColliOne);
	collisionCheck(20, 120, camera, wallsColliTwo);
	collisionCheck(90, 78, camera, wallsColliThree);
	collisionCheck(102, 70, camera, wallsColliFour);
	collisionCheck(110, 58, camera, wallsColliThree);
	collisionCheck(122, 41, camera, wallsColliSix);
	collisionCheck(178, 31.2, camera, wallsColliFour);
	collisionCheck(150, 20, camera, wallsColliEight);
	collisionCheck(140, 78.2, camera, wallsColliNine);
	collisionCheck(151, 40.4, camera, wallsColliThree);
	collisionCheck(150, 120.06, camera, wallsColliFive);
	collisionCheck(130.16, 82, camera, wallsColliFive);
	collisionCheck(120.34, 100, camera, wallsColliSix);
	collisionCheck(82, 130, camera, wallsColliFour);
	collisionCheck(90.8, 122, camera, wallsColliTen);
	collisionCheck(99.54, 148, camera, wallsColliEleven);
	collisionCheck(140, 170, camera, wallsColliFour);
	collisionCheck(122, 151.2, camera, wallsColliTwelve);
	collisionCheck(139, 140, camera, wallsColliThirteen);
	collisionCheck(162, 68.2, camera, wallsColliFourteen);
	collisionCheck(181, 62, camera, wallsColliFifteen);
	collisionCheck(169, 100, camera, wallsColliSixteen);
	collisionCheck(160, 141, camera, wallsColliSeventeen);
	collisionCheck(180, 120, camera, wallsColliEighteen);
	collisionCheck(171, 182, camera, wallsColliSixteen);
	collisionCheck(160, 189, camera, wallsColliNineteen);
	collisionCheck(109, 178, camera, wallsColliTwenty);
	collisionCheck(78, 170, camera, wallsColliFour);
	collisionCheck(71, 158, camera, wallsColliSixteen);
	collisionCheck(71, 62, camera, wallsColliSixteen);
	collisionCheck(100, 12, camera, wallsColliFour);
	collisionCheck(88, 20, camera, wallsColliThree);
	collisionCheck(90.2, 100, camera, wallsColliTwentyone);
	collisionCheck(91, 38, camera, wallsColliTwenty);
	collisionCheck(48, 120, camera, wallsColliThree);
	collisionCheck(31, 140, camera, wallsColliSixteen);
	collisionCheck(60, 190, camera, wallsColliTwentytwo);
	collisionCheck(60, 110, camera, wallsColliTwentythree);
	collisionCheck(40.1, 82, camera, wallsColliTwentyfour);
	collisionCheck(12, 180, camera, wallsColliThree);
	collisionCheck(12, 60, camera, wallsColliThree);
	collisionCheck(40, 180, camera, wallsColliTwentyfive);
	collisionCheck(40, 109, camera, wallsColliNineteen);
	collisionCheck(40, 70, camera, wallsColliFour);
	collisionCheck(40, 31.2, camera, wallsColliTwelve);
	collisionCheck(20, 48.8, camera, wallsColliTwelve);
	collisionCheck(60, 21, camera, wallsColliSeventeen);

	collisionCheck(-20, 20, camera, wallsColliOne);
	collisionCheck(-20, 120, camera, wallsColliTwo);
	collisionCheck(-90, 78, camera, wallsColliThree);
	collisionCheck(-102, 70, camera, wallsColliFour);
	collisionCheck(-110, 58, camera, wallsColliThree);
	collisionCheck(-122, 41, camera, wallsColliSix);
	collisionCheck(-178, 31.2, camera, wallsColliFour);
	collisionCheck(-150, 20, camera, wallsColliEight);
	collisionCheck(-140, 78.2, camera, wallsColliNine);
	collisionCheck(-151, 40.4, camera, wallsColliThree);
	collisionCheck(-150, 120.06, camera, wallsColliFive);
	collisionCheck(-130.16, 82, camera, wallsColliFive);
	collisionCheck(-120.34, 100, camera, wallsColliSix);
	collisionCheck(-82, 130, camera, wallsColliFour);
	collisionCheck(-90.8, 122, camera, wallsColliTen);
	collisionCheck(-99.54, 148, camera, wallsColliEleven);
	collisionCheck(-140, 170, camera, wallsColliFour);
	collisionCheck(-122, 151.2, camera, wallsColliTwelve);
	collisionCheck(-139, 140, camera, wallsColliThirteen);
	collisionCheck(-162, 68.2, camera, wallsColliFourteen);
	collisionCheck(-181, 62, camera, wallsColliFifteen);
	collisionCheck(-169, 100, camera, wallsColliSixteen);
	collisionCheck(-160, 141, camera, wallsColliSeventeen);
	collisionCheck(-180, 120, camera, wallsColliEighteen);
	collisionCheck(-171, 182, camera, wallsColliSixteen);
	collisionCheck(-160, 189, camera, wallsColliNineteen);
	collisionCheck(-109, 178, camera, wallsColliTwenty);
	collisionCheck(-78, 170, camera, wallsColliFour);
	collisionCheck(-71, 158, camera, wallsColliSixteen);
	collisionCheck(-71, 62, camera, wallsColliSixteen);
	collisionCheck(-100, 12, camera, wallsColliFour);
	collisionCheck(-88, 20, camera, wallsColliThree);
	collisionCheck(-90.2, 100, camera, wallsColliTwentyone);
	collisionCheck(-91, 38, camera, wallsColliTwenty);
	collisionCheck(-48, 120, camera, wallsColliThree);
	collisionCheck(-31, 140, camera, wallsColliSixteen);
	collisionCheck(-60, 190, camera, wallsColliTwentytwo);
	collisionCheck(-60, 110, camera, wallsColliTwentythree);
	collisionCheck(-40.1, 82, camera, wallsColliTwentyfour);
	collisionCheck(-12, 180, camera, wallsColliThree);
	collisionCheck(-12, 60, camera, wallsColliThree);
	collisionCheck(-40, 180, camera, wallsColliTwentyfive);
	collisionCheck(-40, 109, camera, wallsColliNineteen);
	collisionCheck(-40, 70, camera, wallsColliFour);
	collisionCheck(-40, 31.2, camera, wallsColliTwelve);
	collisionCheck(-20, 48.8, camera, wallsColliTwelve);
	collisionCheck(-60, 21, camera, wallsColliSeventeen);

	collisionCheck(-40, -40, camera, glassColli);
	collisionCheck(-80, -40, camera, glassColli);
	collisionCheck(-120, -40, camera, glassColli);
	collisionCheck(-160, -40, camera, glassColli);
	collisionCheck(-40, -160, camera, glassColli);
	collisionCheck(-80, -160, camera, glassColli);
	collisionCheck(-120, -160, camera, glassColli);
	collisionCheck(-160, -160, camera, glassColli);

	if (actualDoor1 == false)
	{
		collisionCheck(40, -100, camera, guessColliRow);
	}
	if (actualDoor2 == false)
	{
		collisionCheck(80, -100, camera, guessColliRow);
	}
	if (actualDoor3 == false)
	{
		collisionCheck(120, -100, camera, guessColliRow);
	}
	if (actualDoor4 == false)
	{
		collisionCheck(160, -100, camera, guessColliRow);
	}
	if (guessCount == 0)
	{
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -144.f && camera.position.z >= -176.f))
		{
			wrongDoorA = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -124.f && camera.position.z >= -156.f))
		{
			wrongDoorB = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -104.f && camera.position.z >= -136.f))
		{
			wrongDoorC = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -84.f && camera.position.z >= -116.f))
		{
			wrongDoorD = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -64.f && camera.position.z >= -96.f))
		{
			wrongDoorE = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -44.f && camera.position.z >= -76.f))
		{
			wrongDoorF = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -24.f && camera.position.z >= -56.f))
		{
			wrongDoorG = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -4.f && camera.position.z >= -36.f))
		{
			wrongDoorH = true;
		}
	}
	if (guessCount == 1)
	{
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -164.f && camera.position.z >= -196.f))
		{
			wrongDoorA = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -144.f && camera.position.z >= -176.f))
		{
			wrongDoorB = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -124.f && camera.position.z >= -156.f))
		{
			wrongDoorC = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -104.f && camera.position.z >= -136.f))
		{
			wrongDoorD = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -84.f && camera.position.z >= -116.f))
		{
			wrongDoorE = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -64.f && camera.position.z >= -96.f))
		{
			wrongDoorF = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -24.f && camera.position.z >= -56.f))
		{
			wrongDoorG = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -4.f && camera.position.z >= -36.f))
		{
			wrongDoorH = true;
		}
	}
	if (guessCount == 2)
	{
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -164.f && camera.position.z >= -196.f))
		{
			wrongDoorA = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -144.f && camera.position.z >= -176.f))
		{
			wrongDoorB = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -124.f && camera.position.z >= -156.f))
		{
			wrongDoorC = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -104.f && camera.position.z >= -136.f))
		{
			wrongDoorD = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -84.f && camera.position.z >= -116.f))
		{
			wrongDoorE = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -64.f && camera.position.z >= -96.f))
		{
			wrongDoorF = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -44.f && camera.position.z >= -76.f))
		{
			wrongDoorG = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -24.f && camera.position.z >= -56.f))
		{
			wrongDoorH = true;
		}
	}
	if (guessCount == 3)
	{
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -144.f && camera.position.z >= -176.f))
		{
			wrongDoorA = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -124.f && camera.position.z >= -156.f))
		{
			wrongDoorB = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -104.f && camera.position.z >= -136.f))
		{
			wrongDoorC = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -84.f && camera.position.z >= -116.f))
		{
			wrongDoorD = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -64.f && camera.position.z >= -96.f))
		{
			wrongDoorE = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -44.f && camera.position.z >= -76.f))
		{
			wrongDoorF = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 60.f) && (camera.position.z <= -24.f && camera.position.z >= -56.f))
		{
			wrongDoorG = true;
		}
		if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 60.f) && (camera.position.z <= -4.f && camera.position.z >= -36.f))
		{
			wrongDoorH = true;
		}
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 52.f && camera.position.x >= 20.f) && (camera.position.z <= -164.f && camera.position.z >= -196.f))
	{
		actualDoor1 = true;
		wrongDoorA = false;
		wrongDoorB = false;
		wrongDoorC = false;
		wrongDoorD = false;
		wrongDoorE = false;
		wrongDoorF = false;
		wrongDoorG = false;
		wrongDoorH = false;
		guessCount = 1;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 92.f && camera.position.x >= 60.f) && (camera.position.z <= -44.f && camera.position.z >= -76.f))
	{
		actualDoor2 = true;
		wrongDoorA = false;
		wrongDoorB = false;
		wrongDoorC = false;
		wrongDoorD = false;
		wrongDoorE = false;
		wrongDoorF = false;
		wrongDoorG = false;
		wrongDoorH = false;
		guessCount = 2;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 132.f && camera.position.x >= 100.f) && (camera.position.z <= -4.f && camera.position.z >= -36.f))
	{
		actualDoor3 = true;
		wrongDoorA = false;
		wrongDoorB = false;
		wrongDoorC = false;
		wrongDoorD = false;
		wrongDoorE = false;
		wrongDoorF = false;
		wrongDoorG = false;
		wrongDoorH = false;
		guessCount = 3;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 172.f && camera.position.x >= 140.f) && (camera.position.z <= -164.f && camera.position.z >= -196.f))
	{
		actualDoor4 = true;
		wrongDoorA = false;
		wrongDoorB = false;
		wrongDoorC = false;
		wrongDoorD = false;
		wrongDoorE = false;
		wrongDoorF = false;
		wrongDoorG = false;
		wrongDoorH = false;
		guessCount = 0;
	}

	if ((camera.position.x <= 20.f && camera.position.x >= 0.f) && (camera.position.z <= 45.f && camera.position.z >= 40.f))
	{
		fakeWall1 = true;
	}
	if ((camera.position.x <= 83.f && camera.position.x >= 78.f) && (camera.position.z <= 20.f && camera.position.z >= 0.f))
	{
		fakeWall2 = true;
	}
	if ((camera.position.x <= 181.f && camera.position.x >= 176.f) && (camera.position.z <= 60.f && camera.position.z >= 40.f))
	{
		fakeWall3 = true;
	}
	if ((camera.position.x <= 60.f && camera.position.x >= 40.f) && (camera.position.z <= 65.f && camera.position.z >= 60.f))
	{
		fakeWall4 = true;
	}
	if ((camera.position.x <= 80.f && camera.position.x >= 75.f) && (camera.position.z <= 80.f && camera.position.z >= 60.f))
	{
		fakeWall5 = true;
	}
	if ((camera.position.x <= 60.f && camera.position.x >= 40.f) && (camera.position.z <= 105.f && camera.position.z >= 100.f))
	{
		fakeWall6 = true;
	}
	if ((camera.position.x <= 80.f && camera.position.x >= 60.f) && (camera.position.z <= 140.f && camera.position.z >= 135.f))
	{
		fakeWall7 = true;
	}
	if ((camera.position.x <= 144.f && camera.position.x >= 139.f) && (camera.position.z <= 140.f && camera.position.z >= 120.f))
	{
		fakeWall8 = true;
	}
	if ((camera.position.x <= 180.f && camera.position.x >= 175.f) && (camera.position.z <= 200.f && camera.position.z >= 180.f))
	{
		fakeWall9 = true;
	}
	if ((camera.position.x <= 60.f && camera.position.x >= 40.f) && (camera.position.z <= 184.f && camera.position.z >= 179.f))
	{
		fakeWall10 = true;
	}
	if ((camera.position.x <= 24.f && camera.position.x >= 19.f) && (camera.position.z <= 175.f && camera.position.z >= 160.f))
	{
		fakeWall11 = true;
	}
	if ((camera.position.x <= 25.f && camera.position.x >= 20.f) && (camera.position.z <= 200.f && camera.position.z >= 180.f))
	{
		fakeWall12 = true;
	}

	if (Application::IsKeyPressed('E') && (camera.position.x <= -180.f && camera.position.x >= -200.f) && (camera.position.z <= 20.f && camera.position.z >= 0.f))
	{
		pickUpShard1 = true;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 100.f && camera.position.x >= 80.f) && (camera.position.z <= 20.f && camera.position.z >= 0.f))
	{
		pickUpShard2 = true;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 100.f && camera.position.x >= 80.f) && (camera.position.z <= 140.f && camera.position.z >= 120.f))
	{
		pickUpShard3 = true;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 20.f && camera.position.x >= 0.f) && (camera.position.z <= 60.f && camera.position.z >= 40.f))
	{
		pickUpShard4 = true;
	}
	if (Application::IsKeyPressed('E') && (camera.position.x <= 180.f && camera.position.x >= 160.f) && (camera.position.z <= 100.f && camera.position.z >= 80.f))
	{
		pickUpShard5 = true;
	}

}

void PLANET2::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//These will be replaced by matrix stack soon
	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;

	model.SetToIdentity();
	//Set view matrix using camera settings
	view.SetToLookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);

	//Set projection matrix to perspective mode
	projection.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f); //FOV, Aspect Ratio, Near plane, Far plane

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();

	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
	Vector3 lightDirection_cameraspace2 = viewStack.Top() * lightDir;
	glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace2.x);

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();		//START OF 1ST HIERARCHY
	modelStack.Translate(0, -12, 0);
	modelStack.Scale(20.f, 40.f, 20.f);

	//**************** OBJS IN SKILL AREA ****************//
	//MAG RENDER//
	//**************** OBJS IN SKILL AREA ****************//

	//**************** OBJS IN INFER AREA ****************//
	if (pickUpShard1 == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-9.5, -1.3, 0.5);
		RenderMesh(meshList[KEYSHARDS], false);
		modelStack.PopMatrix();
	}
	//**************** OBJS IN INFER AREA ****************//

	//**************** OBJS IN BLUFF AREA ****************//
	if (pickUpShard2 == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(4.5, -1.3, 0.5);
		RenderMesh(meshList[KEYSHARDS], false);
		modelStack.PopMatrix();
	}
	if (pickUpShard3 == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(4.5, -1.3, 6.5);
		modelStack.Rotate(90, 0, 1, 0);
		RenderMesh(meshList[KEYSHARDS], false);
		modelStack.PopMatrix();
	}
	if (pickUpShard4 == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.5, -1.3, 2.5);
		modelStack.Rotate(90, 0, 1, 0);
		RenderMesh(meshList[KEYSHARDS], false);
		modelStack.PopMatrix();
	}
	if (pickUpShard5 == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(8.5, -1.3, 4.5);
		modelStack.Rotate(90, 0, 1, 0);
		RenderMesh(meshList[KEYSHARDS], false);
		modelStack.PopMatrix();
	}
	//**************** OBJS IN BLUFF AREA ****************//

	//**************** OBJS IN GUESS AREA ****************//
	if (guessCount == 0)
	{
		if (wrongDoorA == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -8);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorB == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -7);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorC == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -6);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorD == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -5);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorE == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -4);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorF == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -3);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorG == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -2);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorH == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(1.85, 0.5, -1);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
	}
	if (guessCount == 1)
	{
		if (wrongDoorA == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -9);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorB == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -8);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorC == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -7);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorD == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -6);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorE == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -5);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorF == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -4);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorG == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -2);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorH == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(3.85, 0.5, -1);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
	}
	if (guessCount == 2)
	{
		if (wrongDoorA == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -9);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorB == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -8);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorC == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -7);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorD == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -6);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorE == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -5);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorF == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -4);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorG == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -3);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorH == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(5.85, 0.5, -2);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
	}
	if (guessCount == 3)
	{
		if (wrongDoorA == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -8);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorB == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -7);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorC == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -6);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorD == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -5);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorE == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -4);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorF == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -3);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorG == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -2);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
		if (wrongDoorH == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(7.85, 0.5, -1);
			modelStack.Rotate(90, 0, 0, 1);
			modelStack.Scale(0.05, 0.05, 0.05);
			RenderMesh(meshList[GUESS_WRONG], false);
			modelStack.PopMatrix();
		}
	}

	modelStack.PushMatrix();
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -1);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -2);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -3);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -4);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -5);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -6);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -7);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -8);				//ACTUAL DOOR
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, 0);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -1);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -2);				//ACTUAL DOOR
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -3);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -4);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -5);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -6);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -7);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2, 0, -8);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, 0);				//ACTUAL DOOR
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -1);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -2);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -3);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -4);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -5);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -6);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -7);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -8);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, 0);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -1);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -2);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -3);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -4);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -5);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -6);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -7);
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 0, -8);				//ACTUAL DOOR
	RenderMesh(meshList[GUESS_DOORS], false);
	modelStack.PopMatrix();
	//**************** OBJS IN GUESS AREA ****************//

	//************** BOUNDARY, FLOOR, WALLS **************//
	if (fakeWall1 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS1], false);
		modelStack.PopMatrix();
	}
	if (fakeWall2 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS2], false);
		modelStack.PopMatrix();
	}
	if (fakeWall3 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS3], false);
		modelStack.PopMatrix();
	}
	if (fakeWall4 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS4], false);
		modelStack.PopMatrix();
	}
	if (fakeWall5 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS5], false);
		modelStack.PopMatrix();
	}
	if (fakeWall6 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS6], false);
		modelStack.PopMatrix();
	}
	if (fakeWall7 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS7], false);
		modelStack.PopMatrix();
	}
	if (fakeWall8 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS8], false);
		modelStack.PopMatrix();
	}
	if (fakeWall9 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS9], false);
		modelStack.PopMatrix();
	}
	if (fakeWall10 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS10], false);
		modelStack.PopMatrix();
	}
	if (fakeWall11 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS11], false);
		modelStack.PopMatrix();
	}
	if (fakeWall12 == false)
	{
		modelStack.PushMatrix();
		RenderMesh(meshList[BLUFF_FWALLS12], false);
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	RenderMesh(meshList[BLUFF_TWALLS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GUESS_WALLS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[INFER_CEILING], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[INFER_FLOOR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GUESS_FLOOR], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[MAZE_FLOOR], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[MAZE_BOUND], false);
	modelStack.PopMatrix();
	//************** BOUNDARY, FLOOR, WALLS **************//

	modelStack.PopMatrix();			//END OF 1ST HIERARCHY

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 0.f);
	RenderMesh(meshList[ASTEROID], false);
	modelStack.PopMatrix();

	RenderSkyBox();

	modelStack.PushMatrix();		//START OF 2ND HIERARCHY
	modelStack.Translate(0, -12, 0);
	modelStack.Scale(20.f, 40.f, 20.f);

	//**************** GLASS IN SKILL AREA ****************//
	modelStack.PushMatrix();
	glBlendFunc(1, 1);
	RenderMesh(meshList[GLASS_WALLS], true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	glBlendFunc(1.9, 1);
	RenderMesh(meshList[GLASS_FLOOR], true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	glBlendFunc(1.9, 1);
	RenderMesh(meshList[GLASS_BOUND], true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	modelStack.PopMatrix();
	//**************** GLASS IN SKILL AREA ****************//

	modelStack.PopMatrix();			//END OF 2ND HIERARCHY

}

void PLANET2::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once 
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void PLANET2::RenderSkyBox()
{
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 2500.f);
	modelStack.Rotate(180.f, 0, 1, 0);
	modelStack.Rotate(90.f, 1, 0, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TEST_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, -2500.f);
	modelStack.Rotate(90.f, 1, 0, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TEST_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 2500.f, 0.f);
	modelStack.Rotate(90.f, 0, 1, 0);
	modelStack.Rotate(180.f, 0, 0, 1);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TEST_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -2500.f, 0.f);
	modelStack.Rotate(-90.f, 0, 1, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TEST_BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-2500.f, 0, 0);
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TEST_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2500.f, 0.f, 0.f);
	modelStack.Rotate(-90.f, 0, 1, 0);
	modelStack.Rotate(90.f, 1, 0, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TEST_LEFT], false);
	modelStack.PopMatrix();
}

void PLANET2::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 0.5f, 0, 0); //1.0f is the spacing of each character, you may change this value

		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void PLANET2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 0.6f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void PLANET2::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
