#include "Planet5.h"
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
#include "Movement.h"

#include <sstream>
#include <iomanip>
#include <fstream>
PLANET5::PLANET5()
{
}

PLANET5::~PLANET5()
{
}

void PLANET5::Init()
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
	camera.Init(Vector3(0, 5, 100), Vector3(0, 5, 0), Vector3(0, 1, 0));

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

	meshList[PICFRAME] = MeshBuilder::GenerateOBJ("frame", "OBJ//PicFrame.obj");
	meshList[PICFRAME]->textureID = LoadTGA("Image//Painting.tga");

	meshList[SPIN] = MeshBuilder::GenerateOBJ("spin", "OBJ//spin.obj");
	meshList[SPIN]->textureID = LoadTGA("Image//spin.tga");
	meshList[SPIN]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[SPIN]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[SPIN]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[SPIN]->material.kShininess = 1.5f;

	meshList[SPINCAP] = MeshBuilder::GenerateOBJ("spincap", "OBJ//spincap.obj");
	meshList[SPINCAP]->textureID = LoadTGA("Image//spincap.tga");
	meshList[SPINCAP]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[SPINCAP]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[SPINCAP]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[SPINCAP]->material.kShininess = 1.5f;

	meshList[BUTTON] = MeshBuilder::GenerateOBJ("button", "OBJ//button.obj");
	meshList[BUTTON]->textureID = LoadTGA("Image//redbutton.tga");
	meshList[BUTTON]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[BUTTON]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[BUTTON]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[BUTTON]->material.kShininess = 1.5f;

	meshList[BUTTONSTAND] = MeshBuilder::GenerateOBJ("spincap", "OBJ//buttonstand.obj");
	meshList[BUTTONSTAND]->textureID = LoadTGA("Image//buttonstand.tga");
	meshList[BUTTONSTAND]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[BUTTONSTAND]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[BUTTONSTAND]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[BUTTONSTAND]->material.kShininess = 1.5f;

	meshList[ARM2] = MeshBuilder::GenerateOBJ("arm2", "OBJ//arm2.obj");
	meshList[ARM2]->textureID = LoadTGA("Image//Human.tga");
	meshList[ARM2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kShininess = 1.5f;

	rotatespin = 0;
	rotategem1 = 0;
	rotategem2 = 0;
	rotategem3 = 0;
	rotategem4 = 0;
	spin1 = false;
	gem1 = false;
	gem2 = false;
	gem3 = false;
	gem4 = false;
	complete1 = false;
	complete2 = false;
	complete3 = false;
	complete4 = false;
	translateButton = 3.55f;
	translategem1 = 3.55f;
	translategem2 = 3.55f;
	translategem3 = 3.55f;
	translategem4 = 3.55f;
	flygem1 = 6.0f;
	flygem2 = 6.3f;
	flygem3 = 5.7f;
	flygem4 = 5.5f;
	scaleFinish = 0.1f;
	MS_rotate = 0.f;
	MS_reverse = false;

	meshList[POSITION] = MeshBuilder::GenerateText("keymsg", 16, 16);
	meshList[POSITION]->textureID = LoadTGA("Image//Redressed.tga");

	meshList[GALLERY_WALL] = MeshBuilder::GenerateOBJ("asteroid", "OBJ//GalleryBox.obj");
	meshList[GALLERY_WALL]->textureID = LoadTGA("Image//BiegeWall.tga");

	meshList[GEM] = MeshBuilder::GenerateOBJ("gem", "OBJ//gem1.obj");
	meshList[GEM]->textureID = LoadTGA("Image//portal.tga");
	meshList[GEM]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM]->material.kShininess = 1.5f;


	meshList[GEM2] = MeshBuilder::GenerateOBJ("gem2", "OBJ//gem2.obj");
	meshList[GEM2]->textureID = LoadTGA("Image//portal2.tga");
	meshList[GEM2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM2]->material.kShininess = 1.5f;


	meshList[GEM3] = MeshBuilder::GenerateOBJ("gem3", "OBJ//gem3.obj");
	meshList[GEM3]->textureID = LoadTGA("Image//portalcase.tga");
	meshList[GEM3]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM3]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM3]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM3]->material.kShininess = 1.5f;


	meshList[GEM4] = MeshBuilder::GenerateOBJ("gem4", "OBJ//gem4.obj");
	meshList[GEM4]->textureID = LoadTGA("Image//portal4.tga");
	meshList[GEM4]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM4]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM4]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[GEM4]->material.kShininess = 1.5f;

	meshList[GEMCASE] = MeshBuilder::GenerateOBJ("gemcase", "OBJ//Barrier.obj");
	meshList[GEMCASE]->textureID = LoadTGA("Image//BarrierTextures.tga");
	meshList[GEMCASE]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GEMCASE]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GEMCASE]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[GEMCASE]->material.kShininess = 1.5f;

	meshList[END] = MeshBuilder::GenerateOBJ("end", "OBJ//END.obj");
	meshList[END]->textureID = LoadTGA("Image//END.tga");
	meshList[END]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[END]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[END]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[END]->material.kShininess = 1.5f;
}

static float ROT_LIMIT = 45.f;
static float SCALE_LIMIT = 5.f;
static float LSPEED = 10.F;


void PLANET5::Update(double dt)
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
		GameMode::GetInstance()->gameState = 1;
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

	if (Application::IsKeyPressed(VK_LBUTTON) && ((camera.position.x < 10) && (camera.position.x > 0) && (camera.position.z < 52) && (camera.position.z > 40)))
	{
		spin1 = true;
	}
	if (spin1 == true)
	{
		rotatespin += (float)(25 * dt);

	}

	if ((translateButton > 3.4f) && (spin1 == true))
	{
		translateButton -= (float)(1 * dt);
	}

	if (Application::IsKeyPressed(VK_RBUTTON) && ((camera.position.x < 10) && (camera.position.x > 0) && (camera.position.z < 52) && (camera.position.z > 40)))
	{
		spin1 = false;
	}
	if ((translateButton < 3.55f) && (spin1 == false))
	{
		translateButton += (float)(1 * dt);
	}


	if (Application::IsKeyPressed(VK_LBUTTON) && ((camera.position.x < -45) && (camera.position.x > -55) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem1 = true;
	}
	if (gem1 == true)
	{
		if (flygem1 <= 8.0f)
		{
			flygem1 += (float)(3 * dt);
		}

		if (flygem1 > 7.99f)
		{
			rotategem1 += (float)(150 * dt);
		}
		complete1 = true;

	}
	if ((translategem1 > 3.4f) && (gem1 == true))
	{
		translategem1 -= (float)(1 * dt);
	}


	if (Application::IsKeyPressed(VK_RBUTTON) && ((camera.position.x < -45) && (camera.position.x > -55) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem1 = false;
	}
	if ((translategem1 < 3.55f) && (gem1 == false))
	{
		translategem1 += (float)(1 * dt);

	}
	if (gem1 == false)
	{
		if (flygem1 > 6.0f)
		{
			flygem1 -= (float)(3 * dt);
		}
	}


	if (Application::IsKeyPressed(VK_LBUTTON) && ((camera.position.x < -20) && (camera.position.x > -30) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem2 = true;
	}
	if (gem2 == true)
	{
		if (flygem2 <= 8.1f)
		{
			flygem2 += (float)(3 * dt);
		}

		if (flygem2 > 8.09f)
		{
			rotategem2 += (float)(150 * dt);
		}
		complete2 = true;
	}
	if ((translategem2 > 3.4f) && (gem2 == true))
	{
		translategem2 -= (float)(1 * dt);
	}

	if (Application::IsKeyPressed(VK_RBUTTON) && ((camera.position.x < -20) && (camera.position.x > -30) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem2 = false;
	}
	if ((translategem2 < 3.55f) && (gem2 == false))
	{
		translategem2 += (float)(1 * dt);

	}
	if (gem2 == false)
	{
		if (flygem2 > 6.3f)
		{
			flygem2 -= (float)(3 * dt);
		}
	}


	if (Application::IsKeyPressed(VK_LBUTTON) && ((camera.position.x < 5) && (camera.position.x > -5) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem3 = true;
	}
	if (gem3 == true)
	{
		if (flygem3 <= 7.5f)
		{
			flygem3 += (float)(3 * dt);
		}

		if (flygem3 > 7.49f)
		{
			rotategem3 += (float)(150 * dt);
		}
		complete3 = true;
	}
	if ((translategem3 > 3.4f) && (gem3 == true))
	{
		translategem3 -= (float)(1 * dt);
	}


	if (Application::IsKeyPressed(VK_RBUTTON) && ((camera.position.x < 5) && (camera.position.x > -5) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem3 = false;
	}
	if ((translategem3 < 3.55f) && (gem3 == false))
	{
		translategem3 += (float)(1 * dt);

	}
	if (gem3 == false)
	{
		if (flygem3 > 5.5f)
		{
			flygem3 -= (float)(3 * dt);
		}
	}



	if (Application::IsKeyPressed(VK_LBUTTON) && ((camera.position.x < 30) && (camera.position.x > 20) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem4 = true;
	}
	if (gem4 == true)
	{
		if (flygem4 <= 7.5f)
		{
			flygem4 += (float)(3 * dt);
		}

		if (flygem4 > 7.49f)
		{
			rotategem4 += (float)(150 * dt);
		}
		complete4 = true;
	}
	if ((translategem4 > 3.4f) && (gem4 == true))
	{
		translategem4 -= (float)(1 * dt);
	}

	if (Application::IsKeyPressed(VK_RBUTTON) && ((camera.position.x < 30) && (camera.position.x > 20) && (camera.position.z < -69.5) && (camera.position.z > -93.5)))
	{
		gem4 = false;
	}
	if ((translategem4 < 3.55f) && (gem4 == false))
	{
		translategem4 += (float)(1 * dt);

	}
	if (gem4 == false)
	{
		if (flygem4 > 5.5f)
		{
			flygem4 -= (float)(3 * dt);
		}
	}

	if (complete1 == true && complete2 == true && complete3 == true && complete4 == true)
	{
		if (scaleFinish < 10)
		{
			scaleFinish += (float)(1 * dt);
		}
		translateEND = 20.0f;
	}

	charMovement(MS_reverse, 20.f, MS_rotate, 3.f, dt);
	collisionCheck(125.f, 0.f, camera, Vector3(2.f, 0.f, 126.f));
	collisionCheck(-125.f, 0.f, camera, Vector3(2.f, 0.f, 126.f));
	collisionCheck(0.f, 125.f, camera, Vector3(126.f, 0.f, 2.f));
	collisionCheck(0.f, -125.f, camera, Vector3(126.f, 0.f, 2.f));
	collisionCheck(0.f, 0.f, camera, Vector3(40.f, 0.f, 40.f));
	collisionCheck(5.f, 45.f, camera, Vector3(3.f, 0.f, 3.f));
}

void PLANET5::Render()
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

	/*modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	/*	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 0.f);
	RenderMesh(meshList[ASTEROID], false);
	modelStack.PopMatrix();*/

	std::ostringstream fps;
	fps << camera.position.x << " " << camera.position.y << " " << camera.position.z;
	std::ostringstream fpss;
	fpss << camera.target.x << " " << camera.target.y << " " << camera.target.z;
	RenderSkyBox();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, -35.0f, 0.f);
	modelStack.Rotate(rotatespin - 45, 0, 1, 0);
	modelStack.Scale(22.0f, 24.0f, 22.0f);
	RenderMesh(meshList[SPIN], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 8.0f, 0.f);
	modelStack.Scale(10.0f, 10.f, 10.f);
	RenderMesh(meshList[SPINCAP], false);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(5.0f, translateButton - 6.f, 43.5f);
	modelStack.Rotate(25.0f, 1, 0, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[BUTTON], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5.0f, -6.0f, 45.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[BUTTONSTAND], false);
	modelStack.PopMatrix();

	//////////////////////GEMS ARE OUTRAGREOUS//////////////////////////


	modelStack.PushMatrix();
	modelStack.Translate(-50.0f, translategem1 - 5.7f, -81.5f);
	modelStack.Rotate(22.0f, 1, 0, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[BUTTON], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-50.0f, -6.0f, -80.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[BUTTONSTAND], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-50.0f, flygem1, -90.0f);
	modelStack.Rotate(rotategem1, 0, 1, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[GEM], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-50.0f, -5.0f, -90.0f);
	modelStack.Scale(2.f, 2.0f, 2.f);
	RenderMesh(meshList[GEMCASE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-25.0f, translategem2 - 5.7f, -81.5f);
	modelStack.Rotate(22.0f, 1, 0, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[BUTTON], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-25.0f, -6.0f, -80.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[BUTTONSTAND], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-25.0f, flygem2, -90.0f);
	modelStack.Rotate(rotategem2, 0, 1, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[GEM2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-25.0f, -5.0f, -90.0f);
	modelStack.Scale(2.f, 2.0f, 2.f);
	RenderMesh(meshList[GEMCASE], true);
	modelStack.PopMatrix();



	modelStack.PushMatrix();
	modelStack.Translate(0.0f, translategem3 - 5.7f, -81.5f);
	modelStack.Rotate(22.0f, 1, 0, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[BUTTON], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, -6.0f, -80.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[BUTTONSTAND], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, flygem3, -90.0f);
	modelStack.Rotate(rotategem3, 0, 1, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[GEM3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, -5.0f, -90.0f);
	modelStack.Scale(2.f, 2.0f, 2.f);
	RenderMesh(meshList[GEMCASE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25.0f, translategem4 - 5.7f, -81.5f);
	modelStack.Rotate(22.0f, 1, 0, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[BUTTON], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25.0f, -6.0f, -80.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[BUTTONSTAND], false);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(25.0f, flygem4, -90.0f);
	modelStack.Rotate(rotategem4, 0, 2, 0);
	modelStack.Scale(2.f, 3.0f, 2.f);
	RenderMesh(meshList[GEM4], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25.0f, -5.0f, -90.0f);
	modelStack.Scale(2.f, 2.0f, 2.f);
	RenderMesh(meshList[GEMCASE], true);
	modelStack.PopMatrix();
	//////////////////////THEY ARE TRULY, TRULY, TRULY OUTRAGREOUS//////////////////////////


	modelStack.PushMatrix();
	modelStack.Translate(-10.0f, translateEND, -123.0f);
	modelStack.Scale(scaleFinish, scaleFinish, scaleFinish);
	RenderMesh(meshList[END], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(124.0f, 12.0f, -40.f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(10.0f, 10.0f, 3.0f);
	RenderMesh(meshList[PICFRAME], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(124.0f, 12.0f, 40.f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(10.0f, 10.0f, 3.0f);
	RenderMesh(meshList[PICFRAME], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-124.0f, 12.0f, -40.f);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Scale(10.0f, 10.0f, 3.0f);
	RenderMesh(meshList[PICFRAME], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-124.0f, 12.0f, 40.f);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Scale(10.0f, 10.0f, 3.0f);
	RenderMesh(meshList[PICFRAME], false);
	modelStack.PopMatrix();

	RenderHandOnScreen();

	RenderTextOnScreen(meshList[POSITION], fps.str(), Color(0, 1, 1), 3, 10, 10);
	RenderTextOnScreen(meshList[POSITION], fpss.str(), Color(0, 1, 1), 3, 10, 7);

}

void PLANET5::RenderMesh(Mesh *mesh, bool enableLight)
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

void PLANET5::RenderSkyBox()
{
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 30.f, 0.f);
	modelStack.Scale(5.f, 3.f, 5.f);
	RenderMesh(meshList[GALLERY_WALL], false);
	modelStack.PopMatrix();
}

void PLANET5::RenderText(Mesh* mesh, std::string text, Color color)
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

void PLANET5::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

void PLANET5::RenderHandOnScreen()
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
	modelStack.Translate(55 + MS_rotate / 2, -10 - MS_rotate / 6, 15);
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

void PLANET5::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
