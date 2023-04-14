/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Game.cpp
Purpose: Game class cpp file that handles all other classes to create a project.
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Game.h"
#include <filesystem>
#include <iostream>

Game::Game(const char* title) :
	window(nullptr), framebufferWidth(WINDOW_WIDTH), framebufferHeight(WINDOW_HEIGHT),
	dt(0.f), curTime(0.f), lastTime(0.f),
	camera(glm::vec3(0.f, 0.f, 7.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)),
	camPosition(glm::vec3(0.f, 0.f, 7.f)),
	worldUp(glm::vec3(0.f, 1.f, 0.1f)),
	camFront(glm::vec3(0.f, 0.f, -1.f)),
	fov(45.f), nearPlane(0.1f), farPlane(100.f),
	drawNormal(false), drawFaceNormal(false)
{
	mouseX = mouseY = 0.0;
	lastMouseX = lastMouseY = 0.0;
	mouseOffsetX = mouseOffsetY = 0.0;
	firstMouse = true;

	Initialize(title);
	InitImGUI();
	InitMatrices();
	InitShaders();
	InitTextures();
	InitMaterials();
	InitModels();
	InitLights();
	InitUniforms();
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
}

void Game::Render()
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	UpdateUniforms();

	centraObj->Render(shaders[SHADER_CORE], meshes[centraObj->GetMeshName()]);
	if (drawNormal)
		centraObj->RenderNormal(shaders[SHADER_LINE], meshes[centraObj->GetMeshName()]);
	if (drawFaceNormal)
		centraObj->RenderFaceNormal(shaders[SHADER_LINE], meshes[centraObj->GetMeshName()]);
	for (auto* o : spheres)
	{
		o->Render(shaders[SHADER_CORE], sphereMeshS);
	}
	orbitObj->RenderOrbit(shaders[SHADER_LINE], orbitMesh);

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
	UpdateKeyboardInput();
	UpdateMouseInput();
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

	static int current_mesh = 0;
	if (ImGui::Combo("Meshes", &current_mesh, vector_getter, static_cast<void*>(&meshNames), meshNames.size()))
		centraObj->SetMesh(meshNames[current_mesh]);

	ImGui::SliderFloat("Rotation X", &centraObj->GetRotation()->x, 0.f, 360.f);
	ImGui::SliderFloat("Rotation Y", &centraObj->GetRotation()->y, 0.f, 360.f);
	ImGui::SliderFloat("Rotation Z", &centraObj->GetRotation()->z, 0.f, 360.f);

	ImGui::NewLine();

	ImGui::Checkbox("Draw Vertex Normals", &drawNormal);
	ImGui::Checkbox("Draw Face Normals", &drawFaceNormal);

	ImGui::NewLine();

	ImGui::SliderFloat3("Ambient", glm::value_ptr(materials[0]->GetAmbient()), 0.1f, 1.0f);
	if (ImGui::SliderFloat("Ambientf1", materials[0]->GetAmbient1f(), 0.1f, 1.f)) materials[0]->ResetAmbient();
	ImGui::Separator();
	ImGui::SliderFloat3("Diffuse", glm::value_ptr(materials[0]->GetDiffuse()), 0.1f, 1.0f);
	if (ImGui::SliderFloat("Diffusef1", materials[0]->GetDiffuse1f(), 0.1f, 1.f)) materials[0]->ResetDiffuse();
	ImGui::Separator();
	ImGui::SliderFloat3("Specular", glm::value_ptr(materials[0]->GetSpecular()), 0.1f, 1.0f);
	if (ImGui::SliderFloat("Specularf1", materials[0]->GetSpecular1f(), 0.1f, 1.f))materials[0]->ResetSpecular();
	ImGui::Separator();
	ImGui::SliderFloat("Shininess", materials[0]->GetShininess(), 0.1f, 32.f);

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
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		camPosition.y -= 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camPosition.y += 0.05f;
	}
}

void Game::UpdateUniforms()
{
	ViewMatrix = camera.GetViewMatrix();

	shaders[SHADER_CORE]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_LINE]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_CORE]->SetVec3f(camera.GetPosition(), "cameraPos");

	//Update framebuffer size and projection matrix
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

	if (framebufferHeight)
	{
		ProjectionMatrix = glm::perspective(glm::radians(fov),
			static_cast<float>(framebufferWidth / framebufferHeight),
			nearPlane, farPlane);
	}
	shaders[SHADER_CORE]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
	shaders[SHADER_LINE]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
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

	//glViewport(0, 0, framebufferWidth, framebufferHeight);

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
	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"vertex_core.glsl", "fragment_core.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"vertex_line.glsl", "fragment_line.glsl"));
}

void Game::InitTextures()
{
	textures.push_back(new Texture("Images/hanji.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/hanji_specular.png", GL_TEXTURE_2D));
}

void Game::InitMaterials()
{
	materials.push_back(new Material(glm::vec3(0.6f), glm::vec3(0.5f), glm::vec3(0.5f), 32.f, 0, 1));
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
	orbitMesh = new Mesh();
	sphereMeshS = new Mesh(new Sphere(36, 36, 0.1f));
	
	centraObj = new Object(glm::vec3(0.f), materials[0], nullptr, nullptr, meshNames.front());
	orbitObj = new Object(glm::vec3(0.f), materials[0], nullptr, nullptr, "Orbit");
	
	float dis = 2.f;
	float step = 2 * PI / 8.f;
	for (int i = 0; i < 8; i++)
		spheres.push_back(new Object(glm::vec3{ dis * cosf(i * step),dis * sinf(i * step), 0.f }, materials[0], nullptr, nullptr, "Sphere"));
}

void Game::InitLights()
{
	lights.push_back(new glm::vec3(-1.f, 1.5f, 2.f));
}

void Game::InitUniforms()
{
	shaders[SHADER_CORE]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_CORE]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");

	shaders[SHADER_CORE]->SetVec3f(*lights[0], "lightPos");

	shaders[SHADER_LINE]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_LINE]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
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
	objects[name] = new Object(glm::vec3(0.f), materials[0], nullptr, nullptr, meshName);
	objectNames.push_back(name);
}
