/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Game.cpp
Purpose: Game class cpp file that handles all other classes to create a project.
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_3
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Game.h"
#include <filesystem>
#include <iostream>

Game::Game(const char* title) :
	window(nullptr), framebufferWidth(WINDOW_WIDTH), framebufferHeight(WINDOW_HEIGHT),
	dt(0.f), curTime(0.f), lastTime(0.f),
	camera(glm::vec3(0.f, 0.f, 10.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)),
	camPosition(glm::vec3(0.f, 0.f, 10.f)),
	worldUp(glm::vec3(0.f, 1.f, 0.1f)),
	camFront(glm::vec3(0.f, 0.f, -1.f)),
	fov(45.f), nearPlane(0.1f), farPlane(100.f),
	drawNormal(false), drawFaceNormal(false)
{
	mouseX = mouseY = 0.0;
	lastMouseX = lastMouseY = 0.0;
	mouseOffsetX = mouseOffsetY = 0.0;
	firstMouse = true;
	calcMappingInGPU = false;
	useNormalForMapping = false;
	mappingType = TEXTURE_MAPPING::CYLINDERICAL;
	fogColor[0] = 0.f;
	fogColor[1] = 0.f;
	fogColor[2] = 0.f;

	shouldReflect = true;
	shouldRefract = false;
	shouldEnvPhong = false;
	activateRefractRGB = false;
	K_t = 1.f;
	Eta[0] = 0.65f;
	Eta[1] = 0.67f;
	Eta[2] = 0.69f;
	
	lightColors.emplace_back(255 / 255.f, 0, 0);
	lightColors.emplace_back(255 / 255.f, 93 / 255.f, 0);
	lightColors.emplace_back(255 / 255.f, 106 / 255.f, 0);
	lightColors.emplace_back(255 / 255.f, 212 / 255.f, 0);
	lightColors.emplace_back(204 / 255.f, 255 / 255.f, 0);
	lightColors.emplace_back(84 / 255.f, 255 / 255.f, 0);
	lightColors.emplace_back(0, 255 / 255.f, 0);
	lightColors.emplace_back(0, 255 / 255.f, 127 / 255.f);
	lightColors.emplace_back(0, 255 / 255.f, 221 / 255.f);
	lightColors.emplace_back(0, 169 / 255.f, 255 / 255.f);
	lightColors.emplace_back(0, 76 / 255.f, 255 / 255.f);
	lightColors.emplace_back(42 / 255.f, 0, 255 / 255.f);
	lightColors.emplace_back(135 / 255.f, 0, 255 / 255.f);
	lightColors.emplace_back(255 / 255.f, 0, 255 / 255.f);
	lightColors.emplace_back(255 / 255.f, 0, 161 / 255.f);
	lightColors.emplace_back(255 / 255.f, 0, 42 / 255.f);

	Initialize(title);
	InitImGUI();
	InitMatrices();
	InitShaders();
	InitTextures();
	InitMaterials();
	InitLights();
	InitModels();
	InitUniforms();

	glGenFramebuffers(1, &FBO);
}

Game::~Game()
{
	glfwDestroyWindow(window);
	glfwTerminate();

	for (size_t i = 0; i < shaders.size(); i++)
		delete shaders[i];

	for (size_t i = 0; i < textures.size(); i++)
		delete textures[i];

	std::unordered_map<std::string, Object*>::iterator itr1 = objects.begin();
	if (itr1 != objects.end())
	{
		delete itr1->second;
		objects.erase(itr1);
	}

	std::unordered_map<std::string, Mesh*>::iterator itr2 = meshes.begin();
	if (itr2 != meshes.end())
	{
		delete itr2->second;
		meshes.erase(itr2);
	}

	for (size_t i = 0; i < materials.size(); i++)
		delete materials[i];

	for (size_t i = 0; i < lights.size(); i++)
		delete lights[i];

	ImGui_ImplOpenGL4_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Game::Update()
{
	UpdateDt();
	UpdateInput();

	if (rotateLights)
	{
		for (Object* light : lightObjs)
		{
			if(light->GetCanRotate())
				light->MoveByAngle(0.01f, 2.5f);
		}
	}
}

void Game::Render()
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	UpdateUniforms();
	
	//Bind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	Camera cam = camera;
	fov = 90.f;
	for(int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, centraObj->GetCubeTexture(i)->getID(), 0);

		camera.SetCameraDirPos(static_cast<FACING_DIR>(i), centraObj->GetPosition());
		UpdateUniforms();
		
		//skyboxObj->SetPosition(camera.GetPosition());
		skyboxObj->RenderSkyBox(shaders[SHADER_SKYBOX], skyboxMesh);
		orbitObj->RenderOrbit(shaders[SHADER_BASIC], orbitMesh);

		for (int j = 0; j < numLights; ++j)
		{
			lightObjs[j]->Render(shaders[SHADER_BASIC], lightMesh);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//UNBIND FBO

	camera = cam;
	//skyboxObj->SetPosition(camera.GetPosition());

	fov = 45.f;
	UpdateUniforms();
	///////////////////////////////////////////////////
	//camera.SetCameraDirPos(static_cast<FACING_DIR>(FACING_DIR::CAM_LEFT), camera.GetPosition());

	skyboxObj->RenderSkyBox(shaders[SHADER_SKYBOX], skyboxMesh);
	
	for (int i = 0; i < numLights; ++i)
	{
		if (lightObjs[i]->GetCanRotate())
		{
			lights[i]->SetPosition(lightObjs[i]->GetRawPosition());
			lights[i]->SetDirection(centraObj->GetPosition() - lightObjs[i]->GetPosition());
		}
		lightObjs[i]->Render(shaders[SHADER_BASIC], lightMesh);
	}

	centraObj->SetLight(lights);

	centraObj->Render(shaders[shaderType], meshes[centraObj->GetMeshName()]);
	orbitObj->RenderOrbit(shaders[SHADER_BASIC], orbitMesh);

	if (drawNormal)
		centraObj->RenderNormal(shaders[SHADER_BASIC], meshes[centraObj->GetMeshName()]);
	if (drawFaceNormal)
		centraObj->RenderFaceNormal(shaders[SHADER_BASIC], meshes[centraObj->GetMeshName()]);

	///////////////////////////////////////////////////
	UpdateImGUI();

	glfwSwapBuffers(window);
	glFlush();

	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::UpdateDt()
{
	curTime = static_cast<float>(glfwGetTime());
	dt = curTime - lastTime;
	lastTime = curTime;
}

void Game::UpdateInput()
{
	glfwPollEvents();
	UpdateMouseInput();
	UpdateKeyboardInput();
}

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

void Game::UpdateImGUI()
{
	ImGui_ImplOpenGL4_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Editor");

	if (ImGui::Button("Recompile Shader"))
	{
		InitShaders();
	}

	if (ImGui::CollapsingHeader("'___'"))
	{
		static int current_mesh = 0;
		if (ImGui::Combo("Meshes", &current_mesh, vector_getter, static_cast<void*>(&meshNames), meshNames.size()))
			centraObj->SetMesh(meshNames[current_mesh]);

		ImGui::NewLine();

		ImGui::SliderFloat("Rotation X", &centraObj->GetRotation()->x, 0.f, 360.f);
		ImGui::SliderFloat("Rotation Y", &centraObj->GetRotation()->y, 0.f, 360.f);
		ImGui::SliderFloat("Rotation Z", &centraObj->GetRotation()->z, 0.f, 360.f);

		ImGui::NewLine();

		ImGui::Checkbox("Draw Vertex Normals", &drawNormal);
		ImGui::Checkbox("Draw Face Normals", &drawFaceNormal);
	}

	if (ImGui::CollapsingHeader("\"___\""))
	{
		if (ImGui::SliderInt("Number of Lights", &numLights, 1, 16))
		{
			UpdateLightNumber(numLights);
			if (numLights <= currentLight) currentLight = numLights-1;
		}
		ImGui::SliderInt("Current light-1", &currentLight, 0, numLights - 1);

		ImGui::NewLine();
		ImGui::Checkbox("Rotate Lights", &rotateLights);

		ImGui::NewLine();
		ImGui::Text("Texture Mapping: ");
		ImGui::Checkbox("GPU uv Calculation", &calcMappingInGPU);
		
		if (ImGui::Button("Vertex Position"))
		{
			useNormalForMapping = false;
			for (auto& m : meshes)
				m.second->ChangeMappingKind(mappingType, useNormalForMapping);
		}
		ImGui::SameLine();
		if (ImGui::Button("Vertex Normal"))
		{
			useNormalForMapping = true;
			for (auto& m : meshes)
				m.second->ChangeMappingKind(mappingType, useNormalForMapping);
		}
		
		if (ImGui::Button("Cylinderical"))
		{
			mappingType = TEXTURE_MAPPING::CYLINDERICAL;
			for (auto & m : meshes)
				m.second->ChangeMappingKind(mappingType, useNormalForMapping);
		}
		ImGui::SameLine();
		if (ImGui::Button("Spherical"))
		{
			mappingType = TEXTURE_MAPPING::SPHERICAL;
			for (auto & m : meshes)
				m.second->ChangeMappingKind(mappingType, useNormalForMapping);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cube"))
		{
			mappingType = TEXTURE_MAPPING::CUBE;
			for (auto & m : meshes)
				m.second->ChangeMappingKind(mappingType, useNormalForMapping);
		}

		ImGui::NewLine();
		ImGui::ColorEdit3("Fog Color", fogColor);
		ImGui::NewLine();
		ImGui::ColorEdit3("Global Ambient", globalAmbient);

		ImGui::NewLine();
		ImGui::Text("Shader Programs: ");
		if (ImGui::Button("Phong Shading")) shaderType = SHADER_PHONG_SHADING;
		ImGui::SameLine();
		if (ImGui::Button("Phong Lighting")) shaderType = SHADER_PHONG_LIGHTING;
		ImGui::SameLine();
		if (ImGui::Button("Blinn Shading")) shaderType = SHADER_BLINN_SHADING;

		ImGui::NewLine();
		ImGui::Text("Scenarios : ");
		if (ImGui::Button("Scenario 1"))
		{
			scenario = SCENARIO1;
			UpdateLightNumber(16);
		}
		ImGui::SameLine();
		if (ImGui::Button("Scenario 2"))
		{
			scenario = SCENARIO2;
			UpdateLightNumber(16);
		}
		ImGui::SameLine();
		if (ImGui::Button("Scenario 3"))
		{
			scenario = SCENARIO3;
			UpdateLightNumber(16);
		}

		if (scenario == SCENARIO2 || scenario == SCENARIO1)
		{
			ImGui::Text("Light type: ");
			if (ImGui::Button("Directional"))
			{
				for (Light* l : lights)
					l->GetLightType() = LIGHTTYPE::DIRECTIONAL;
			}
			ImGui::SameLine();
			if (ImGui::Button("Point"))
			{
				for (Light* l : lights)
					l->GetLightType() = LIGHTTYPE::POINT;
			}
			ImGui::SameLine();
			if (ImGui::Button("Spotlight"))
			{
				for (Light* l : lights)
					l->GetLightType() = LIGHTTYPE::SPOTLIGHT;
			}
		}
	}


	if (ImGui::CollapsingHeader(";___;"))
	{
		if (ImGui::Button("Reflect"))
		{
			shouldReflect = true;
			shouldRefract = false;
			shouldEnvPhong = false;
		}
		if (ImGui::Button("Refract"))
		{
			shouldReflect = false;
			shouldRefract = true;
			shouldEnvPhong = false;
		}
		if (ImGui::Button("Reflect + Refract"))
		{
			shouldReflect = true;
			shouldRefract = true;
			shouldEnvPhong = false;
		}
		if (ImGui::Button("Phong + Environment Mapping"))
		{
			shouldReflect = false;
			shouldRefract = false;
			shouldEnvPhong = true;
		}
		ImGui::Checkbox("Activate Refraction RGB", &activateRefractRGB);

		ImGui::NewLine();
		
		ImGui::Text("Refractive Index:");
		ImGui::SliderFloat("", &K_t, 0, 100);
		if (ImGui::Button("Air")) K_t = 1.000132f;
		if (ImGui::Button("Hydrogen")) K_t = 1.000132f;
		if (ImGui::Button("Water")) K_t = 1.333f;
		if (ImGui::Button("Olive Oil")) K_t = 1.47f;
		if (ImGui::Button("Ice")) K_t = 1.31f;
		if (ImGui::Button("Quartz")) K_t = 1.46f;
		if (ImGui::Button("Diamond")) K_t = 2.42f;
		if (ImGui::Button("Acrylic / plexiglas / Lucite")) K_t = 1.49f;

		ImGui::NewLine();

		ImGui::SliderFloat("Refract R", &Eta[0], 0.f, 1.f);
		ImGui::SliderFloat("Refract G", &Eta[1], 0.f, 1.f);
		ImGui::SliderFloat("Refract B", &Eta[2], 0.f, 1.f);
	}
	
	ImGui::End();

	////////////////////////////////////////////////////////////////////////
	
	ImGui::Begin("Light");

	if (ImGui::SliderFloat3("Position", lights[currentLight]->GetPositionFloat(), -10.f, 10.f))
	{
		lightObjs[currentLight]->GetCanRotate() = false;
		lights[currentLight]->UpdatePosition();
		lightObjs[currentLight]->SetPosition(lights[currentLight]->GetPosition());
	}
	if (ImGui::SliderFloat3("Direction", lights[currentLight]->GetDirectionFloat(), -10.f, 10.f))
	{
		lightObjs[currentLight]->GetCanRotate() = false;
		lights[currentLight ]->UpdateDirection();
	}
	
	ImGui::NewLine();
	ImGui::Text("Light Types:");
	if (ImGui::Checkbox("Directional", &lights[currentLight ]->GetIsDirectional()))
		lights[currentLight ]->UpdateLightType(LIGHTTYPE::DIRECTIONAL);
	ImGui::SameLine();
	if (ImGui::Checkbox("Point", &lights[currentLight ]->GetIsPoint()))
		lights[currentLight ]->UpdateLightType(LIGHTTYPE::POINT);
	ImGui::SameLine();
	if (ImGui::Checkbox("Spotlight", &lights[currentLight ]->GetIsSpotlight()))
		lights[currentLight ]->UpdateLightType(LIGHTTYPE::SPOTLIGHT);

	ImGui::NewLine();
	ImGui::Text("Color:");
	if (ImGui::ColorEdit3("Ambient: ", lights[currentLight ]->GetAmbientFloat()))
		lights[currentLight ]->UpdateAmbient();
	if (ImGui::ColorEdit3("Diffuse: ", lights[currentLight ]->GetDiffuseFloat()))
	{
		lights[currentLight ]->UpdateDiffuse();
		lightObjs[currentLight ]->SetColor(lights[currentLight ]->GetDiffuse());
	}
	if (ImGui::ColorEdit3("Specular: ", lights[currentLight ]->GetSpecularFloat()))
		lights[currentLight ]->UpdateSpecular();

	if (lights[currentLight]->GetIsSpotlight())
	{
		ImGui::SliderFloat("Inner Angle: ", &lights[currentLight]->GetInnerAngle(), 0.f, lights[currentLight]->GetOuterAngle());
		if (ImGui::SliderFloat("Outer Angle: ", &lights[currentLight]->GetOuterAngle(), 0.f, 30.f))
		{
			if (lights[currentLight]->GetInnerAngle() > lights[currentLight]->GetOuterAngle())
				lights[currentLight]->GetInnerAngle() = lights[currentLight]->GetOuterAngle();
		}
		ImGui::SliderFloat("Falloff: ", &lights[currentLight]->GetFallOff(), 1.f, 32.f);
	}
	
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL4_RenderDrawData(ImGui::GetDrawData());
}

void Game::UpdateMouseInput()
{
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (firstMouse)
	{
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		firstMouse = false;
	}

	mouseOffsetX = mouseX - lastMouseX;
	mouseOffsetY = mouseY - lastMouseY;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

void Game::UpdateKeyboardInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	//Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.Move(dt, FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.Move(dt, BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.Move(dt, LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.Move(dt, RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		camera.Move(dt, UP);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		camera.Move(dt, DOWN);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		this->camera.UpdateInput(dt, -1, this->mouseOffsetX, this->mouseOffsetY);
	}
}

void Game::UpdateUniforms()
{
	ViewMatrix = camera.GetViewMatrix();

	for (int s = SHADER_BASIC; s <= SHADER_BLINN_SHADING; s += 1)
	{
		shaders[s]->SetMat4fv(ViewMatrix, "ViewMatrix");
		shaders[s]->SetVec3f(camera.GetPosition(), "cameraPos");
	}

	shaders[SHADER_SKYBOX]->SetMat4fv(glm::mat4(glm::mat3(ViewMatrix)), "ViewMatrix");
	
	//Update framebuffer size and projection matrix
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

	if (framebufferHeight)
	{
		ProjectionMatrix = glm::perspective(glm::radians(fov),
			static_cast<float>(framebufferWidth / framebufferHeight),
			nearPlane, farPlane);
	}
	
	shaders[SHADER_BASIC]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
	shaders[SHADER_SKYBOX]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
	shaders[SHADER_PHONG_SHADING]->Set1f(shouldRefract, "shouldRefract");
	shaders[SHADER_PHONG_SHADING]->Set1f(shouldReflect, "shouldReflect");
	shaders[SHADER_PHONG_SHADING]->Set1f(shouldEnvPhong, "shouldEnvPhong");
	shaders[SHADER_PHONG_SHADING]->Set1f(activateRefractRGB, "activateRefractRGB");
	shaders[SHADER_PHONG_SHADING]->Set1f(K_t, "K_t");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(Eta, "Eta");

	for(int s = SHADER_PHONG_SHADING; s <= SHADER_BLINN_SHADING; s+=1)
	{
		shaders[s]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
		shaders[s]->Set1i(numLights, "numLights");
		shaders[s]->Set1i(calcMappingInGPU, "texMappingGPU");
		shaders[s]->Set1i(useNormalForMapping, "useNormalForMapping");
		shaders[s]->Set1i(static_cast<int>(mappingType), "mappingKind");
		shaders[s]->SetVec3f(fogColor, "fogColor");
		shaders[s]->SetVec3f(globalAmbient, "globalAmbient");
	}
}

void Game::Initialize(const char* title)
{
	//Init GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW INIT FAILED \n";
		glfwTerminate();
	}

	//Init Window
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, false); //resizable

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //for MAX OS

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "ERROR::GLFW CREATE WINDOW FAILED\n";
		glfwTerminate();
	}

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, Game::Framebuffer_resize_callback);

	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::GLEW INIT FAILED \n";
		glfwTerminate();
	}

	//Input
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::InitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL4_Init("#version 450");
	ImGui::StyleColorsDark();
}

void Game::InitMatrices()
{
	ViewMatrix = glm::mat4(1.f);
	ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

	ProjectionMatrix = glm::mat4(1.f);
	ProjectionMatrix = glm::perspective(
		glm::radians(fov),
		static_cast<float>(framebufferWidth / framebufferHeight),
		nearPlane,
		farPlane
	);
}

void Game::InitShaders()
{
	for (Shader* s : shaders)
		delete s;
	
	shaders.clear();

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/basic_vertex.glsl", "Shaders/basic_fragment.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/phong_shading_vertex.glsl", "Shaders/phong_shading_fragment.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/phong_lighting_vertex.glsl", "Shaders/phong_lighting_fragment.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/blinn_shading_vertex.glsl", "Shaders/blinn_shading_fragment.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/skyBox_vertex.glsl", "Shaders/skyBox_fragment.glsl"));
}

void Game::InitTextures()
{
	textures.push_back(new Texture("Images/right.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/left.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/top.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/bottom.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/front.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/back.png", GL_TEXTURE_2D));

	textures.push_back(new Texture("Images/right.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/left.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/top.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/bottom.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/front.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/back.png", GL_TEXTURE_2D));
}

void Game::InitMaterials()
{
	materials.push_back(new Material(32.f, 0, 1));
}

void Game::InitModels()
{
	std::string name;
	for (const auto& entry : std::filesystem::directory_iterator("models/"))
	{
		name = entry.path().string();
		name = name.substr(name.find_first_of("/") + 1, name.find_first_of(".") - name.find_first_of("/") - 1);
		AddMesh("OBJ_" + name, new Mesh(entry.path().string()));
	}

	centraObj = new Object(glm::vec3(0.f), materials[0], lights, textures[TEX_RIGHT], textures[TEX_RIGHT], "OBJ_sphere");
	
	for (int t = TEX_CUBE_RIGHT; t <= TEX_CUBE_BACK; t += 1)
	{
		centraObj->SetCubeMapTextures(textures[t]);
	}

	for (int i = 0; i < 6; ++i)
	{
		centraObj->GetCubeTexture(i)->bind(TEX_CUBE_RIGHT + i);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, centraObj->GetCubeTexture(i)->getID(), 0);
	}
	
	lightMesh = new Mesh(new Sphere(36, 36, 0.1f));
	UpdateLightNumber(3);

	orbitMesh = new Mesh();
	orbitObj = new Object(glm::vec3(0.f), materials[0], std::vector<Light*>(), nullptr, nullptr, "Orbit");

	skyboxMesh = new Mesh(new SkyBox());
	skyboxObj = new Object(camera.GetPosition(), materials[0], lights, nullptr, nullptr, "");
	for (int t = TEX_RIGHT; t <= TEX_BACK; t += 1)
	{
		skyboxObj->SetSkyBoxTextures(textures[t]);
	}
}

void Game::InitLights()
{
	float dis = 2.5f;
	float step = 2 * PI / numLights;

	for (int i = 0; i < numLights; ++i)
	{
		lights.push_back(new Light(glm::vec3{ dis * cosf(i * step), 0.f, dis * sinf(i * step) }, glm::vec3(0.f),
			0.1f / numLights, 0.4f / numLights, 1.f / numLights,
			8.f, 8.f, LIGHTTYPE::DIRECTIONAL));
	}
}

void Game::InitUniforms()
{
	shaders[SHADER_BASIC]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_BASIC]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
	shaders[SHADER_BASIC]->Set1i(numLights, "numLights");

	shaders[SHADER_SKYBOX]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_SKYBOX]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");

	shaders[SHADER_PHONG_SHADING]->Set1f(shouldRefract, "shouldRefract");
	shaders[SHADER_PHONG_SHADING]->Set1f(shouldReflect, "shouldReflect");
	shaders[SHADER_PHONG_SHADING]->Set1f(shouldEnvPhong, "shouldEnvPhong");
	shaders[SHADER_PHONG_SHADING]->Set1f(activateRefractRGB, "activateRefractRGB");
	shaders[SHADER_PHONG_SHADING]->Set1f(K_t, "K_t");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(Eta, "Eta");

	for (int s = SHADER_PHONG_SHADING; s <= SHADER_BLINN_SHADING; s += 1)
	{
		shaders[s]->SetMat4fv(ViewMatrix, "ViewMatrix");
		shaders[s]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
		shaders[s]->Set1i(numLights, "numLights");
		shaders[s]->Set1i(calcMappingInGPU, "texMappingGPU");
		shaders[s]->Set1i(static_cast<int>(mappingType), "mappingKind");
		shaders[s]->Set1i(useNormalForMapping, "useNormalForMapping");
		shaders[s]->SetVec3f(fogColor, "fogColor");
		shaders[s]->SetVec3f(globalAmbient, "globalAmbient");
	}
}

void Game::Framebuffer_resize_callback(GLFWwindow* /*window*/, int fbw, int fbh)
{
	glViewport(0, 0, fbw, fbh);
}

int Game::GetWindowShouldClose() const { return glfwWindowShouldClose(window); }
void Game::SetWindowShouldClose() const { glfwSetWindowShouldClose(window, GLFW_TRUE); }

void Game::AddMesh(std::string name, Mesh* mesh)
{
	meshes[name] = mesh;
	meshNames.push_back(name);
}

void Game::AddObject(std::string name, std::string meshName)
{
	objects[name] = new Object(glm::vec3(0.f), materials[0], lights, nullptr, nullptr, meshName);
	objectNames.push_back(name);
}

void Game::UpdateLightNumber(int number, bool typeFixed)
{
	numLights = number;

	float dis = 2.5f;
	float step = 2 * PI / numLights;

	std::vector<LIGHTTYPE> types;
	if (typeFixed)
	{
		for (Light* l : lights)
			types.push_back(l->GetLightType());
	}
	int preTypeSize = types.size() - 1;

	lights.clear();
	LIGHTTYPE type = LIGHTTYPE::DIRECTIONAL;

	for (int i = 0; i < numLights; ++i)
	{
		if (scenario == SCENARIO3) type = static_cast<LIGHTTYPE>(rand() % 3);
		if (typeFixed) type = preTypeSize < i ? LIGHTTYPE::POINT : types[i];

		lights.push_back(new Light(glm::vec3{ dis * cosf(i * step), 0.f, dis * sinf(i * step) }, glm::vec3(0.f),
			0.1f / numLights, 0.4f / numLights, 1.f / numLights,
			8.f, 8.f, type));
	}

	lightObjs.clear();

	float d = LIGHT_MAX / static_cast<float>(number);
	float light_c = 0.f;
	glm::vec3 color(0.f);

	if (scenario == SCENARIO1)
	{
		color = lightColors[rand() % 16];
	}

	for (int i = 0; i < numLights; ++i)
	{
		if (scenario == SCENARIO2)
		{
			color = lightColors[static_cast<int>(light_c)];
			light_c += d;
		}
		else if (scenario == SCENARIO3)
		{
			color = lightColors[static_cast<int>(light_c)];
			light_c += d;
		}

		lightObjs.push_back(new Object(glm::vec3(0.f), materials[0], std::vector<Light*>(), nullptr, nullptr, ""));
		lightObjs.back()->SetColor(color);
		lightObjs.back()->SetPosition(lights[i]->GetPosition());
		lightObjs.back()->SetAngle(step * i);
		lights[i]->GetDiffuse() = color;
		lights[i]->UpdateDiffuseF();
		lights[i]->SetDirection(glm::vec3(0));
	}
}
