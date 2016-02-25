#include "SP2.h"
#include "GL\glew.h"
#include "LoadTGA.h"
#include "timer.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "GameMode.h"
#include "Movement.h"
#include "Collision.h"


#include <sstream>
#include <iomanip>
#include <fstream>
SP2::SP2()
{
}

SP2::~SP2()
{
}

void SP2::Init()
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
	camera.Init(Vector3(0, 7.2f, -20), Vector3(0, 0, 0), Vector3(0, 1, 0));

	//camera.Init(Vector3(-125, 7.2f, -5), Vector3(0, 0, 0), Vector3(0, 1, 0));

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

	meshList[TEXTBOX] = MeshBuilder::GenerateCube("TEXTBOX", Color(1, 1, 1));
	meshList[TEXTBOX]->textureID = LoadTGA("Image//asteroid.tga");

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LIGHT", Color(1, 1, 1), 36, 36);
	meshList[PLATFORM_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[PLATFORM_FRONT]->textureID = LoadTGA("Image//surround.tga");

	meshList[PLATFORM_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[PLATFORM_RIGHT]->textureID = LoadTGA("Image//surround.tga");

	meshList[PLATFORM_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[PLATFORM_TOP]->textureID = LoadTGA("Image//top.tga");


	meshList[PLATFORM_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[PLATFORM_BOTTOM]->textureID = LoadTGA("Image//top.tga");

	meshList[PLATFORM_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[PLATFORM_LEFT]->textureID = LoadTGA("Image//surround.tga");

	meshList[PLATFORM_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[PLATFORM_BACK]->textureID = LoadTGA("Image//surround.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//Redressed.tga");

	meshList[ASTEROID] = MeshBuilder::GenerateOBJ("asteroid", "OBJ//asteroid.obj");
	meshList[ASTEROID]->textureID = LoadTGA("Image//asteroid.tga");
	meshList[ASTEROID]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kShininess = 1.5f;

	meshList[ARM] = MeshBuilder::GenerateOBJ("arm", "OBJ//arm1.obj");
	meshList[ARM]->textureID = LoadTGA("Image//Human.tga");
	meshList[ARM]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM]->material.kShininess = 1.5f;

	meshList[ARM2] = MeshBuilder::GenerateOBJ("arm2", "OBJ//arm2.obj");
	meshList[ARM2]->textureID = LoadTGA("Image//Human.tga");
	meshList[ARM2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kShininess = 1.5f;

	meshList[LEG] = MeshBuilder::GenerateOBJ("leg", "OBJ//leg1.obj");
	meshList[LEG]->textureID = LoadTGA("Image//Human.tga");
	meshList[LEG]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG]->material.kShininess = 1.5f;

	meshList[LEG2] = MeshBuilder::GenerateOBJ("leg2", "OBJ//leg2.obj");
	meshList[LEG2]->textureID = LoadTGA("Image//Human.tga");
	meshList[LEG2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG2]->material.kShininess = 1.5f;


	meshList[HEAD] = MeshBuilder::GenerateOBJ("head", "OBJ//head.obj");
	meshList[HEAD]->textureID = LoadTGA("Image//Human.tga");
	meshList[HEAD]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[HEAD]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[HEAD]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[HEAD]->material.kShininess = 1.5f;

	meshList[CHEST] = MeshBuilder::GenerateOBJ("chest", "OBJ//chest.obj");
	meshList[CHEST]->textureID = LoadTGA("Image//Human.tga");
	meshList[CHEST]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[CHEST]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[CHEST]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[CHEST]->material.kShininess = 1.5f;

	meshList[PLAYER] = MeshBuilder::GenerateOBJ("asteroid", "OBJ//Model.obj");
	meshList[PLAYER]->textureID = LoadTGA("Image//asteroid.tga");
	meshList[PLAYER]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PLAYER]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PLAYER]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PLAYER]->material.kShininess = 1.5f;
	
	meshList[PORTALCASE] = MeshBuilder::GenerateOBJ("portalcase", "OBJ//portalcase.obj");
	meshList[PORTALCASE]->textureID = LoadTGA("Image//portalcase.tga");
	meshList[PORTALCASE]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTALCASE]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTALCASE]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTALCASE]->material.kShininess = 1.5f;

	meshList[PORTAL] = MeshBuilder::GenerateOBJ("portal", "OBJ//portal.obj");
	meshList[PORTAL]->textureID = LoadTGA("Image//portal.tga");
	meshList[PORTAL]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL]->material.kShininess = 1.5f;

	meshList[PORTAL2] = MeshBuilder::GenerateOBJ("portal", "OBJ//portal.obj");
	meshList[PORTAL2]->textureID = LoadTGA("Image//portal2.tga");
	meshList[PORTAL2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL2]->material.kShininess = 1.5f;

	meshList[PORTAL3] = MeshBuilder::GenerateOBJ("portal", "OBJ//portal.obj");
	meshList[PORTAL3]->textureID = LoadTGA("Image//portal3.tga");
	meshList[PORTAL3]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL3]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL3]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL3]->material.kShininess = 1.5f;

	meshList[PORTAL4] = MeshBuilder::GenerateOBJ("portal", "OBJ//portal.obj");
	meshList[PORTAL4]->textureID = LoadTGA("Image//portal4.tga");
	meshList[PORTAL4]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL4]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL4]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL4]->material.kShininess = 1.5f;	

	meshList[PORTAL5] = MeshBuilder::GenerateOBJ("portal", "OBJ//portal.obj");
	meshList[PORTAL5]->textureID = LoadTGA("Image//portal5.tga");
	meshList[PORTAL5]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL5]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL5]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[PORTAL5]->material.kShininess = 1.5f;

	rotateCase = 0;
	rotatePortal = 0;

	meshList[GLASS] = MeshBuilder::GenerateOBJ("glass", "OBJ//glass.obj");
	meshList[GLASS]->textureID = LoadTGA("Image//glass.tga");
	meshList[GLASS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GLASS]->material.kDiffuse.Set(0.1f, 0.1f, 0.1f);
	meshList[GLASS]->material.kSpecular.Set(0.0f, 0.0f, 0.0f);
	meshList[GLASS]->material.kShininess = 1.f;

	meshList[GLASSDESIGN] = MeshBuilder::GenerateOBJ("glassDesign", "OBJ//glassDesign.obj");
	meshList[GLASSDESIGN]->textureID = LoadTGA("Image//glassDesign.tga");
	meshList[GLASSDESIGN]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[GLASSDESIGN]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[GLASSDESIGN]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GLASSDESIGN]->material.kShininess = 1.f;

	meshList[BASE] = MeshBuilder::GenerateOBJ("base", "OBJ//base.obj");
	meshList[BASE]->textureID = LoadTGA("Image//base.tga");
	meshList[BASE]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[BASE]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[BASE]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[BASE]->material.kShininess = 1.f;

	glassFrontColli.Set(120.f, 0.f, 5.f);
	glassSideColli.Set(5.f, 0.f, 5.f);
	baseBackColli.Set(240.f, 0.f, 5.f);
	wheelLightColli.Set(30.f, 0.f, 20.f);
	portalColli.Set(190.f, 0.f, 20.f);

	meshList[WINGS] = MeshBuilder::GenerateOBJ("wings", "OBJ//wings.obj");
	meshList[WINGS]->textureID = LoadTGA("Image//wings.tga");
	meshList[WINGS]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[WINGS]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[WINGS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[WINGS]->material.kShininess = 1.f;

	meshList[SWITCH] = MeshBuilder::GenerateOBJ("switch", "OBJ//switch.obj");
	meshList[SWITCH]->textureID = LoadTGA("Image//switch.tga");
	meshList[SWITCH]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[SWITCH]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[SWITCH]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[SWITCH]->material.kShininess = 1.f;

	meshList[HOLDER] = MeshBuilder::GenerateOBJ("holder", "OBJ//holder.obj");
	meshList[HOLDER]->textureID = LoadTGA("Image//holder.tga");
	meshList[HOLDER]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[HOLDER]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[HOLDER]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[HOLDER]->material.kShininess = 1.f;

	rotateSwitch = 20.0f;

	meshList[WHEEL] = MeshBuilder::GenerateOBJ("wheel", "OBJ//Wheel.obj");
	meshList[WHEEL]->textureID = LoadTGA("Image//WheelTexture.tga");
	meshList[WHEEL]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[WHEEL]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[WHEEL]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[WHEEL]->material.kShininess = 1.f;

	meshList[STAND] = MeshBuilder::GenerateOBJ("wheelstand", "OBJ//stand.obj");
	meshList[STAND]->textureID = LoadTGA("Image//WheelTexture.tga");
	meshList[STAND]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
	meshList[STAND]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[STAND]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[STAND]->material.kShininess = 1.f;

	meshList[PLANET1] = MeshBuilder::GenerateOBJ("Planet1", "OBJ//Planet_1.obj");
	meshList[PLANET1]->textureID = LoadTGA("Image//Planet_1Test.tga");
	meshList[PLANET1]->material.kAmbient.Set(0.4f,0.4f, 0.4f);
	meshList[PLANET1]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[PLANET1]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[PLANET1]->material.kShininess = 1.f;

	meshList[PLANET2] = MeshBuilder::GenerateOBJ("Planet2", "OBJ//PLanet.obj");
	meshList[PLANET2]->textureID = LoadTGA("Image//Planet_2.tga");
	meshList[PLANET2]->material.kAmbient.Set(0.4f, 0.4f, 0.4f);
	meshList[PLANET2]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[PLANET2]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[PLANET2]->material.kShininess = 1.f;

	meshList[PLANET3] = MeshBuilder::GenerateOBJ("Planet3", "OBJ//PLanet.obj");
	meshList[PLANET3]->textureID = LoadTGA("Image//Planet_3.tga");
	meshList[PLANET3]->material.kAmbient.Set(0.4f, 0.4f, 0.4f);
	meshList[PLANET3]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[PLANET3]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[PLANET3]->material.kShininess = 1.f;

	meshList[PLANET4] = MeshBuilder::GenerateOBJ("PLanet4", "OBJ//PLanet.obj");
	meshList[PLANET4]->textureID = LoadTGA("Image//Planet_4.tga");
	meshList[PLANET4]->material.kAmbient.Set(0.4f, 0.4f, 0.4f);
	meshList[PLANET4]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[PLANET4]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[PLANET4]->material.kShininess = 1.f;

	meshList[PLANET5] = MeshBuilder::GenerateOBJ("PLanet5", "OBJ//PLanet.obj");
	meshList[PLANET5]->textureID = LoadTGA("Image//Planet_5.tga");
	meshList[PLANET5]->material.kAmbient.Set(0.4f, 0.4f, 0.4f);
	meshList[PLANET5]->material.kDiffuse.Set(0.3f, 0.3f, 0.3f);
	meshList[PLANET5]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[PLANET5]->material.kShininess = 1.f;


	rotateSwitch = -20.0f;

	rotateSwitch = 10.0f;
	translateAsteroid2 = 0.0f;
	rotateAngle7 = 0.0f;
	rotateAngle8 = 0.0f;
	translateAsteroid = 0.0f;
	
	MS_rotate = 0.f;
	MS_reverse = false;

	meshList[POSITION] = MeshBuilder::GenerateText("keymsg", 16, 16);
	meshList[POSITION]->textureID = LoadTGA("Image//Redressed.tga");

	LoadCollision("PlatformColli.txt", ColliX, ColliZ);

	translate.Set(60.f, 0.f, -30.f);
	rotate.Set(0.f, 90.f + One.rotateNPC, 0.f);


	One.attriSet(translate, rotate, "test.txt");
	Q_One.setName("Fire");
}
static float ROT_LIMIT = 45.f;
static float SCALE_LIMIT = 5.f;
static float LSPEED = 10.F;
float MS_rotate = 0.0f;
bool MS_reverse = false;

void SP2::Update(double dt)
{
	if (One.talk != One.sentences.end())
		testes = true;
	else
		testes = false;
	One.Interaction(camera, testes);
	
	One.Communication();
	if (Application::IsKeyPressed('1')) //enable back face culling
		glEnable(GL_CULL_FACE);
	if (Application::IsKeyPressed('2')) //disable back face culling
		glDisable(GL_CULL_FACE);
	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	if (Application::IsKeyPressed('E') && (camera.position.x <= -130) && (camera.position.x >= -170) && (camera.position.z <= -150))
	{
		GameMode::GetInstance()->gameState = 2;
	}

	if (Application::IsKeyPressed('E') && (camera.position.x <= -50) && (camera.position.x >= -90) && (camera.position.z <= -150))
	{
		GameMode::GetInstance()->gameState = 3;
	}

	if (Application::IsKeyPressed('E') && (camera.position.x <= 20) && (camera.position.x >= -20) && (camera.position.z <= -150))
	{
		GameMode::GetInstance()->gameState = 4;
	}

	if (Application::IsKeyPressed('E') && (camera.position.x <= 90) && (camera.position.x >= 50) && (camera.position.z <= -150))
	{
		GameMode::GetInstance()->gameState = 5;
	}

	if (Application::IsKeyPressed('E') && (camera.position.x <= 170) && (camera.position.x >= 130) && (camera.position.z <= -150))
	{
		GameMode::GetInstance()->gameState = 6;
	}
	camera.Update(dt);


	if ((Application::IsKeyPressed(VK_LBUTTON)) && (camera.position.x <= -8.f && camera.position.x >= -28.f) && (camera.position.z <= -37.5f && camera.position.z >= -62.5f))
	{
		displayOn = false;
		toggleLight = true;
		rotateSwitch = -20.0f;
	}
	if ((Application::IsKeyPressed(VK_RBUTTON)) && (camera.position.x <= -8.f && camera.position.x >= -28.f) && (camera.position.z <= -37.5f && camera.position.z >= -62.5f))
	{
		displayOn = true;
		toggleLight = false;
		rotateSwitch = 20.0f;
	}
	for (std::vector<float>::iterator xf = ColliX.begin(), zf = ColliZ.begin(); xf != ColliX.end(), zf != ColliZ.end(); ++xf, ++zf)
	{
		collisionCheck(*xf, *zf, camera, glassSideColli);
	}
	collisionCheck(0, 0, camera, glassFrontColli);

	collisionCheck(0, -200, camera, baseBackColli);
	collisionCheck(-5, -29, camera, wheelLightColli);
	collisionCheck(0, -190, camera, portalColli);

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

	rotateCase -= (float)(1500.0f * dt);
	rotatePortal += (float)(750.0f * dt);

	rotateAngle7 += (float)(100 * dt);
	rotateAngle8 += (float)(25 * dt);
	if (translateAsteroid < 2000.f)
	{
		translateAsteroid += (float)(20 * dt);
	}
	else
	{
		translateAsteroid -= (float)(20 * dt);
	}

	if (translateAsteroid2 > -2000.f)
	{
		translateAsteroid2 -= (float)(20 * dt);
	}
	else
	{
		translateAsteroid2 += (float)(20 * dt);
	}
	charMovement(MS_reverse, 20.f, MS_rotate, 3.f, dt);
	One.delayTime(dt);
	One.Interaction(camera, Vector3(60.f, 0.f, -50.f), Vector3(30.f, 0.f, 20.f));
}

void SP2::Render()
{
	std::ostringstream fps;
	std::ostringstream aaa;
	aaa << "pos: " <<camera.position.x << " " << camera.position.y << " " << camera.position.z;
	
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

	RenderSkyBox();

	////////////////////////ASTEROID//////////////////////////////
	modelStack.PushMatrix();
	modelStack.Translate(100, 20 + translateAsteroid2, 0 + translateAsteroid);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(translateAsteroid2, 5 + translateAsteroid2, 200 + translateAsteroid2);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-500 + translateAsteroid, -500 + translateAsteroid2, 0);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-750 + translateAsteroid2, 50, 100 + translateAsteroid);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID],toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(50, 50 + translateAsteroid2, 300);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(800 + translateAsteroid2, 500, -500 + translateAsteroid);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-200, -650 + translateAsteroid, 400 + translateAsteroid2);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-700 + translateAsteroid, -300 + translateAsteroid2, 500 + translateAsteroid2);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(800, 500 + translateAsteroid2, -500 + translateAsteroid);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(1000 + translateAsteroid2, 500, 800 + translateAsteroid2);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(100 + translateAsteroid, 5, -800 + translateAsteroid);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(translateAsteroid, 50, -300);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(10 + translateAsteroid, 5, 200 + translateAsteroid);
	modelStack.Rotate(rotateAngle7, 1, 1, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[ASTEROID], toggleLight); // asteroid
	modelStack.PopMatrix();


	////////////////////////ASTEROID//////////////////////////////


	////////////////////////PLANETS////////////////////////////////

	modelStack.PushMatrix();
	modelStack.Translate(500,300,2000);
	modelStack.Rotate(-30, 1, 0, 0);
	modelStack.Rotate(rotateAngle8, 0, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET1], toggleLight); // PLANET 1
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2000,600,300);
	modelStack.Rotate(30, 0, 0, 1);
	modelStack.Rotate(rotateAngle8, 0, 1, 0);
	modelStack.Scale(25, 25, 25);
	RenderMesh(meshList[PLANET2], toggleLight); // PLANET 2
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2000,-200,2000);
	modelStack.Rotate(rotateAngle8, 0, 1, 0);
	modelStack.Scale(75, 75, 75);
	RenderMesh(meshList[PLANET3], toggleLight); // PLANET 3
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-2500,100,1000);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Rotate(rotateAngle8, 1, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET4], toggleLight); // PLANET4
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-1000,-300,2000);
	modelStack.Rotate(rotateAngle8, 1, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET5], toggleLight); // PLANET5
	modelStack.PopMatrix();

	////////////////////////PLANETS////////////////////////////////


	modelStack.PushMatrix();			//PORTAL HIERARCHY
	modelStack.Translate(0.f, 8.f, -190);
	modelStack.Scale(1.5f, 1.5f, 1.5f);

	modelStack.PushMatrix();
	modelStack.Translate(-100, 0.0f, 0.f);
	modelStack.Rotate(rotateCase, 0, 0, 1);
	modelStack.Scale(5.0f, 5.0f, 5.0f);
	RenderMesh(meshList[PORTALCASE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100, 0.f, 0.f);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(rotatePortal, 0, 0, 1);
	modelStack.Scale(12.0f, 12.0f, 12.0f);
	RenderMesh(meshList[PORTAL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-50.f, 0.f, 0.f);
	modelStack.Rotate(rotateCase, 0, 0, 1);
	modelStack.Scale(5.0f, 5.0f, 5.0f);
	RenderMesh(meshList[PORTALCASE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-50.f, 0.f, 0.f);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(rotatePortal, 0, 0, 1);
	modelStack.Scale(12.0f, 12.0f, 12.0f);
	RenderMesh(meshList[PORTAL2], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 0.f);
	modelStack.Rotate(rotateCase, 0, 0, 1);
	modelStack.Scale(5.0f, 5.0f, 5.0f);
	RenderMesh(meshList[PORTALCASE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 0.f);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(rotatePortal, 0, 0, 1);
	modelStack.Scale(12.0f, 12.0f, 12.0f);
	RenderMesh(meshList[PORTAL3], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(50.f, 0.f, 0.f);
	modelStack.Rotate(rotateCase, 0, 0, 1);
	modelStack.Scale(5.0f, 5.0f, 5.0f);
	RenderMesh(meshList[PORTALCASE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(50.f, 0.f, 0.f);
	modelStack.Rotate(rotatePortal, 0, 0, -1);
	modelStack.Scale(12.0f, 12.0f, 12.0f);
	RenderMesh(meshList[PORTAL4], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(100.f, 0.f, 0.f);
	modelStack.Rotate(rotateCase, 0, 0, 1);
	modelStack.Scale(5.0f, 5.0f, 5.0f);
	RenderMesh(meshList[PORTALCASE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(100.f, 0.f, 0.f);
	modelStack.Rotate(rotatePortal, 0, 0, -1);
	modelStack.Scale(12.0f, 12.0f, 12.0f);
	RenderMesh(meshList[PORTAL5], false);
	modelStack.PopMatrix();

	modelStack.PopMatrix();			//END OF PORTAL HIERARCHY

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -100);
	modelStack.Scale(25, 21, 22);
	RenderMesh(meshList[WINGS], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();		//LIGHT SWITCH HIERARCHY
	modelStack.Translate(-18, -18, -30);

	modelStack.PushMatrix();
	modelStack.Rotate(rotateSwitch, 1, 0, 0);
	modelStack.Scale(1, 4, 4);
	RenderMesh(meshList[SWITCH], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(1.5, 1, 2);
	RenderMesh(meshList[HOLDER], false);
	modelStack.PopMatrix();

	modelStack.PopMatrix();			//END OF LIGHT SWITCH HIERARCHY

	modelStack.PushMatrix();
	modelStack.Translate(0, -3, -33);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[WHEEL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, -20, -31);
	modelStack.Scale(4, 6, 3);
	RenderMesh(meshList[STAND], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();		//PLATFORM HIERARCHY
	modelStack.Translate(0, -30, 0);
	modelStack.Scale(30, 30, 40);

	modelStack.PushMatrix();
	glBlendFunc(1, 1);
	RenderMesh(meshList[GLASS], toggleLight);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	modelStack.PopMatrix();

	if (displayOn == true)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, -0.01f);
		RenderMesh(meshList[GLASSDESIGN], false);
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	RenderMesh(meshList[BASE], false);
	modelStack.PopMatrix();

	modelStack.PopMatrix();			//END OF PLATFORM HIERARCHY

	RenderNPC(One,false);
	RenderHandOnScreen();
	fps << *One.talk;
	std::ostringstream ooo;
	ooo << One.rotateNPC;
	if (!One.b_Quest && One.b_Chat)
		RenderTextOnScreen(meshList[POSITION], fps.str(), Color(0, 1, 1), 2, 5, 5);
	RenderTextOnScreen(meshList[POSITION], aaa.str(), Color(0, 1, 1), 2, 5, 3);
	if (One.b_Quest)
	{
		QuestSystem::GetInstance()->QuestList.push_back(Q_One);
		
	}
	if (QuestSystem::GetInstance()->QuestList.size() != 0 && Application::IsKeyPressed('Q'))
	{
		vector<Quest>::iterator qt = QuestSystem::GetInstance()->QuestList.begin();
		RenderTextOnScreen(meshList[POSITION], qt->getName(), Color(0, 1, 1), 2, 5, 8);
	}

	RenderTextbox(One);

}

void SP2::RenderNPC(StopNPC &temp, bool enableLight)
{
	///////////////////////////HUMAN/////////////////////////////////

	modelStack.PushMatrix();
	modelStack.Translate(temp.getTranslate().x, temp.getTranslate().y, temp.getTranslate().z);
	modelStack.Rotate(temp.getRotate().x, 1, 0, 0);
	modelStack.Rotate(temp.getRotate().y  + temp.rotateNPC, 0, 1, 0);
	modelStack.Rotate(temp.getRotate().z, 0, 0, 1);
	modelStack.Scale(3.f, 3.f, 3.f);

	RenderMesh(meshList[ARM], enableLight);

	RenderMesh(meshList[ARM2], enableLight);

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -2.95f, 0.f);
	RenderMesh(meshList[LEG], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -2.95f, 0.f);
	RenderMesh(meshList[LEG2], enableLight);
	modelStack.PopMatrix();

	RenderMesh(meshList[CHEST], enableLight);
	RenderMesh(meshList[HEAD], enableLight);

	modelStack.PopMatrix();
	///////////////////////////HUMAN/////////////////////////////////
}

void SP2::RenderMesh(Mesh *mesh, bool enableLight)
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

void SP2::RenderSkyBox()
{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.f, 2500.f);
		modelStack.Rotate(180.f, 0, 1, 0);
		modelStack.Rotate(90.f, 1, 0, 0);
		modelStack.Scale(5000.f, 5000.f, 5000.f);
		RenderMesh(meshList[PLATFORM_FRONT], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.f, -2500.f);
		modelStack.Rotate(90.f, 1, 0, 0);
		modelStack.Scale(5000.f, 5000.f, 5000.f);
		RenderMesh(meshList[PLATFORM_BACK], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.f, 2500.f, 0.f);
		modelStack.Rotate(90.f, 0, 1, 0);
		modelStack.Rotate(180.f, 0, 0, 1);
		modelStack.Scale(5000.f, 5000.f, 5000.f);
		RenderMesh(meshList[PLATFORM_TOP], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.f, -2500.f, 0.f);
		modelStack.Rotate(-90.f, 0, 1, 0);
		modelStack.Scale(5000.f, 5000.f, 5000.f);
		RenderMesh(meshList[PLATFORM_BOTTOM], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-2500.f, 0, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(5000.f, 5000.f, 5000.f);
		RenderMesh(meshList[PLATFORM_RIGHT], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(2500.f, 0.f, 0.f);
		modelStack.Rotate(-90.f, 0, 1, 0);
		modelStack.Rotate(90.f, 1, 0, 0);
		modelStack.Scale(5000.f, 5000.f, 5000.f);
		RenderMesh(meshList[PLATFORM_LEFT], false);
		modelStack.PopMatrix();
}

void SP2::RenderText(Mesh* mesh, std::string text, Color color)
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

void SP2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
		characterSpacing.SetToTranslation(i * 0.5f, 0, 0); //1.0f is the spacing of each character, you may change this value
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

void SP2::RenderHandOnScreen()
{
	glDisable(GL_DEPTH_TEST);

	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -20, 20); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(55 + MS_rotate/2, -10 - MS_rotate/6, 15);
	modelStack.Rotate(155, 1, 0, 0);
	modelStack.Rotate(-127, 0, 1, 0);
	modelStack.Rotate(23, 0, 0, 1);
	modelStack.Scale(6, 12, 8);
	RenderMesh(meshList[ARM2], false);

	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void SP2::RenderTextbox(StopNPC temp)
{
	glDisable(GL_DEPTH_TEST);

	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -20, 20); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(40.f, 10.f, 10.f);
	modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(70, 12, 0);
	if (temp.b_Chat)
	RenderMesh(meshList[TEXTBOX], false);

	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void SP2::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
