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
#include "Movement.h"

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
	camera.Init(Vector3(20, 5, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));

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

	meshList[BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 0, 0), 36, 36);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//Redressed.tga");

	meshList[ASTEROID] = MeshBuilder::GenerateOBJ("asteroid", "OBJ//asteroid.obj");
	meshList[ASTEROID]->textureID = LoadTGA("Image//asteroid.tga");
	meshList[ASTEROID]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ASTEROID]->material.kShininess = 1.5f;

	meshList[WALL] = MeshBuilder::GenerateOBJ("wall", "OBJ//oldWall.obj");
	meshList[WALL]->textureID = LoadTGA("Image//oldWall.tga");
	meshList[WALL]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[WALL]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[WALL]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[WALL]->material.kShininess = 1.5f;

	meshList[GUN] = MeshBuilder::GenerateOBJ("wall", "OBJ//Gun.obj");
	meshList[GUN]->textureID = LoadTGA("Image//Gun.tga");
	meshList[GUN]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GUN]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GUN]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[GUN]->material.kShininess = 1.5f;

	meshList[GROUND] = MeshBuilder::GenerateQuad("ground", Color(0, 0, 0));
	meshList[GROUND]->textureID = LoadTGA("Image//ground2.tga");


	meshList[ARM] = MeshBuilder::GenerateOBJ("arm", "OBJ//arm1.obj");
	meshList[ARM]->textureID = LoadTGA("Image//Alien1.tga");
	meshList[ARM]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM]->material.kShininess = 1.5f;

	meshList[ARM2] = MeshBuilder::GenerateOBJ("arm2", "OBJ//arm2.obj");
	meshList[ARM2]->textureID = LoadTGA("Image//Alien1.tga");
	meshList[ARM2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[ARM2]->material.kShininess = 1.5f;

	meshList[LEG] = MeshBuilder::GenerateOBJ("leg", "OBJ//leg1.obj");
	meshList[LEG]->textureID = LoadTGA("Image//Alien1.tga");
	meshList[LEG]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG]->material.kShininess = 1.5f;

	meshList[LEG2] = MeshBuilder::GenerateOBJ("leg2", "OBJ//leg2.obj");
	meshList[LEG2]->textureID = LoadTGA("Image//Alien1.tga");
	meshList[LEG2]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG2]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG2]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[LEG2]->material.kShininess = 1.5f;


	meshList[HEAD] = MeshBuilder::GenerateOBJ("head", "OBJ//head.obj");
	meshList[HEAD]->textureID = LoadTGA("Image//Alien1.tga");
	meshList[HEAD]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[HEAD]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[HEAD]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[HEAD]->material.kShininess = 1.5f;

	meshList[CHEST] = MeshBuilder::GenerateOBJ("chest", "OBJ//chest.obj");
	meshList[CHEST]->textureID = LoadTGA("Image//Alien1.tga");
	meshList[CHEST]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[CHEST]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[CHEST]->material.kSpecular.Set(0.8f, 0.8f, 0.8f);
	meshList[CHEST]->material.kShininess = 1.5f;

	meshList[FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[FRONT]->textureID = LoadTGA("Image//redplanet_ft.tga");

	meshList[RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[RIGHT]->textureID = LoadTGA("Image//redplanet_rt.tga");

	meshList[TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[TOP]->textureID = LoadTGA("Image//redplanet_up.tga");


	meshList[BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[BOTTOM]->textureID = LoadTGA("Image//redplanet_dn.tga");

	meshList[LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[LEFT]->textureID = LoadTGA("Image//redplanet_lf.tga");

	meshList[BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[BACK]->textureID = LoadTGA("Image//redplanet_bk.tga");

	meshList[POSITION] = MeshBuilder::GenerateText("keymsg", 16, 16);
	meshList[POSITION]->textureID = LoadTGA("Image//Redressed.tga");

	MS_rotate = 0.f;
	MS_reverse = false;
	SimpleVariables();
	
}

static float ROT_LIMIT = 45.f;
static float SCALE_LIMIT = 5.f;
static float LSPEED = 10.F;

void PLANET2::SimpleVariables()
{
	inRange = false;
	holdingGun = false;
	GunX = 202.f;
	GunZ = 0.f;
	rotateA = 5.f;
	rotateB = 110.f;
	rotateC = 5.f;
}

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
		GameMode::GetInstance()->gameState = 1;
	camera.Update(dt);

	MethCalculations();

	if ((rangeGunX <= 70) && (rangeGunZ <= 30) && (inRange == false))
	{
		inRange = true;
		if ((Application::IsKeyPressed('E')))
		{
			holdingGun = true;
		}
	}
	else
	{
		inRange = false;
	}

	//if (Application::IsKeyPressed('7'))
	//{
	//	rotateA += 1;
	//	if (rotateA > 360)
	//	{
	//		rotateA = 0;
	//	}
	//}
	//if (Application::IsKeyPressed('8'))
	//{
	//	rotateB += 1;
	//	if (rotateB > 360)
	//	{
	//		rotateB = 0;
	//	}
	//}
	//if (Application::IsKeyPressed('9'))
	//{
	//	rotateC += 1;
	//	if (rotateC > 360)
	//	{
	//		rotateC = 0;
	//	}
	//}

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

	charMovement(MS_reverse, 20.f, MS_rotate, 3.f, dt);

	collisionCheck(0.f, -190.f, camera, Vector3(200.f, 0.f, 20.f));
	collisionCheck(0.f, 210.f, camera, Vector3(200.f, 0.f, 20.f));
	collisionCheck(210.f, 0.f, camera, Vector3(20.f, 0.f, 200.f));
	collisionCheck(210.f, 210.f, camera, Vector3(50.f, 0.f, 50.f));
	collisionCheck(-210.f, -190.f, camera, Vector3(50.f, 0.f, 50.f));
	collisionCheck(210.f, 190.f, camera, Vector3(50.f, 0.f, 50.f));
	collisionCheck(190.f, -210.f, camera, Vector3(50.f, 0.f, 50.f));
	collisionCheck(-190.f, 210.f, camera, Vector3(50.f, 0.f, 50.f));
	collisionCheck(170.f, 0.f, camera, Vector3(30.f, 0.f, 60.f));

	//if (pla2npc.rotateNPC < pla2npc.Interaction(camera, 15.f))
	//	pla2npc.rotateNPC += (float)(50.0 * dt);

}

void PLANET2::MethCalculations()
{
	rangeGunX = camera.position.x - GunX;
	if (rangeGunX <= 0)
	{
		rangeGunX *= -1;
	}
	rangeGunZ = camera.position.z - GunZ;
	if (rangeGunZ <= 0)
	{
		rangeGunZ *= -1;
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

	RenderSkyBox();

	///////////////////////     GROUND      ///////////////////////////
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -20.f, 0.f);
	modelStack.Scale(2500.f, 0.f, 2500.f);
	RenderMesh(meshList[GROUND], false);
	modelStack.PopMatrix();
	///////////////////////     GROUND      ///////////////////////////

	///////////////////////   GIANT WALLS   ///////////////////////////
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -20.f, 200.f);
	modelStack.Scale(50.f, 20.f, 50.f);
	RenderMesh(meshList[WALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -20.f, -200.f);
	modelStack.Scale(50.f, 20.f, 50.f);
	RenderMesh(meshList[WALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(200.f, -20.f, 0.f);
	modelStack.Rotate(90.f, 0, 1, 0);
	modelStack.Scale(50.f, 20.f, 50.f);
	RenderMesh(meshList[WALL], false);
	modelStack.PopMatrix();
	///////////////////////   GIANT WALLS   ///////////////////////////

	///////////////////////   SMALL WALLS   ///////////////////////////
	modelStack.PushMatrix();
	modelStack.Translate(150.f, -20.f, 0.f);
	modelStack.Rotate(90.f, 0, 1, 0);
	modelStack.Scale(12.f, 6.f, 12.f);
	RenderMesh(meshList[WALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(180.f, -20.f, 50.f);
	modelStack.Scale(5.f, 6.f, 5.f);
	RenderMesh(meshList[WALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(180.f, -20.f, -50.f);
	modelStack.Scale(5.f, 6.f, 5.f);
	RenderMesh(meshList[WALL], false);
	modelStack.PopMatrix();
	///////////////////////   SMALL WALLS   ///////////////////////////

	RenderNPC(false);
	RenderHandOnScreen();

	if (holdingGun == false)
	{
		std::ostringstream inRangeGunz;
		inRangeGunz << "Press E to pick up the Gun ";
		if (inRange == true)
		{
			RenderTextOnScreen(meshList[POSITION], inRangeGunz.str(), Color(0, 0, 0), 2, 10, 15);
		}

		modelStack.PushMatrix();
		modelStack.Translate(GunX, 0.f, GunZ);
		modelStack.Rotate(90.f, 0, 1, 0);
		modelStack.Rotate(15.f, 0, 0, 1);
		modelStack.Scale(5.f, 5.f, 5.f);
		RenderMesh(meshList[GUN], false);
		modelStack.PopMatrix();

	}
	else if (holdingGun == true)
	{
		renderGunUI();
		renderGunOnHand();
		std::ostringstream Ammunition;
		Ammunition << "You have Unlimited Power. Its ok :)";
		RenderTextOnScreen(meshList[POSITION], Ammunition.str(), Color(0, 1, 1), 2, 1, 28);
	}

	std::ostringstream position;
	position << "pos: X= " << camera.position.x << " Y= " << camera.position.y << " Z= " << camera.position.z;
	RenderTextOnScreen(meshList[POSITION], position.str(), Color(0, 1, 1), 2, 1, 29);

	/*std::ostringstream Testing;
	Testing << "RotateA= " << rotateA << " RotateB= " << rotateB << " RotateC= " << rotateC;
	RenderTextOnScreen(meshList[POSITION], Testing.str(), Color(0, 0, 0), 2, 10, 17);*/
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
	RenderMesh(meshList[FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, -2500.f);
	modelStack.Rotate(90.f, 1, 0, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 2500.f, 0.f);
	modelStack.Rotate(-90.f, 0, 1, 0);
	modelStack.Rotate(180.f, 0, 0, 1);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -2500.f, 0.f);
	modelStack.Rotate(-90.f, 0, 1, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-2500.f, 0, 0);
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(2500.f, 0.f, 0.f);
	modelStack.Rotate(-90.f, 0, 1, 0);
	modelStack.Rotate(90.f, 1, 0, 0);
	modelStack.Scale(5000.f, 5000.f, 5000.f);
	RenderMesh(meshList[LEFT], false);
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

void PLANET2::RenderNPC(bool enableLight)
{
	///////////////////////////HUMAN/////////////////////////////////

	modelStack.PushMatrix();
	//modelStack.Translate(temp.tx, temp.ty, temp.tz);
	//modelStack.Rotate(temp.r_angle - temp.rotateNPC, 0, 1, 0);
	modelStack.Scale(3.f, 4.f, 3.f);

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

void PLANET2::RenderHandOnScreen()
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

void PLANET2::renderGunUI()
{
	glDisable(GL_DEPTH_TEST);

	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -40, 40); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(5, 50, 15);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GUN], false);

	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void PLANET2::renderGunOnHand()
{
	glDisable(GL_DEPTH_TEST);

	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -40, 40); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(55 + MS_rotate / 2, 10 - MS_rotate / 6, 0);
	modelStack.Rotate(rotateA, 1, 0, 0);
	modelStack.Rotate(rotateB, 0, 1, 0);
	modelStack.Rotate(rotateC, 0, 0, 1);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[GUN], false);

	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void PLANET2::shooting()
{
	
}

void PLANET2::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
