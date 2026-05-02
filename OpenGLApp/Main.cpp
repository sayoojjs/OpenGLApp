#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"



const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture brickRoughness;
Texture brickNormal;
Texture dirtTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

Shader  billboardShader;
Mesh* billboardMesh;
Texture iconDirectional;
Texture iconPoint;

double FinalTime = glfwGetTime();
int frameCount = 0;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";


//UNCLEAR REWATCH  UNCLEAR REWATCH	UNCLEAR REWATCH	UNCLEAR REWATCH	UNCLEAR REWATCH	UNCLEAR REWATCH	UNCLEAR REWATCH	UNCLEAR REWATCH 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 1, 2,
		1, 3, 2
	};

	
 GLfloat floorVertices[] = {
    -10.0f, 0.0f, -10.0f,  0.0f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
     10.0f, 0.0f, -10.0f,  10.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
    -10.0f, 0.0f,  10.0f,  0.0f,  10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
     10.0f, 0.0f,  10.0f,  10.0f, 10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
};


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12, false);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12, false);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 56, 6, true);
	meshList.push_back(obj3);



}


//billboard test 


void DrawLightIcon(Mesh* quad, Shader& shader, Texture& icon,
	glm::vec3 position, glm::vec3 tintColour,
	glm::mat4 view, glm::mat4 projection, float size = 0.4f)
{
	glm::vec3 right = glm::vec3(view[0][0], view[1][0], view[2][0]);
	glm::vec3 up = glm::vec3(view[0][1], view[1][1], view[2][1]);

	glm::mat4 model = glm::mat4(1.0f);
	model[0] = glm::vec4(right * size, 0.0f);
	model[1] = glm::vec4(up * size, 0.0f);
	model[2] = glm::vec4(0.0f, 0.0f, size, 0.0f);
	model[3] = glm::vec4(position, 1.0f);

	shader.UseShader();

	glUniform1i(glGetUniformLocation(shader.GetShaderID(), "iconTexture"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(glGetUniformLocation(shader.GetShaderID(), "iconColour"),
		tintColour.r, tintColour.g, tintColour.b);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	icon.UseTexture();
	quad->RenderMesh();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

Mesh* CreateBillboardQuad()
{
	
	GLfloat verts[] = {
		-0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
	};
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	Mesh* m = new Mesh();
	m->CreateMesh(verts, indices, 32, 6, false);  // 4 verts * 8 floats = 32
	return m;
}


//----------------------------------------------------------------------------



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1280, 720);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTexture();
	brickRoughness = Texture("Textures/brickRoughness.png");
	brickRoughness.LoadTexture();
	brickNormal = Texture("Textures/brickNormal.png");
	brickNormal.LoadTexture();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTexture();
	
	//ICONS
	billboardMesh = CreateBillboardQuad();
	billboardShader.CreateFromFiles("Shaders/billboard.vert", "Shaders/billboard.frag");

	iconDirectional = Texture("Textures/icon_directional.png");
	iconDirectional.LoadTextureA();  // load with alpha

	iconPoint = Texture("Textures/icon_point.png");
	iconPoint.LoadTextureA();


	shinyMaterial = Material(16.0f, 128);
	dullMaterial = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, -1.0f, -2.0f);

	unsigned int pointLightCount = 0;

	pointLights[0] = PointLight(0.8f, 0.1f, 0.1f,
		0.6f, 9.0f,
		4.0f, -4.0f, 3.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(0.6f, 1.0f, 0.5f,
		0.6f, 6.0f,
		2.0f, -3.0f, 1.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;
	pointLights[2] = PointLight(0.0f, 0.2f, 0.8f,
		0.0f, 15.0f,
		9.0f, -4.0f, 9.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;






	// ── ImGui init ──────────────────────────────────────────────────────────
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.07f, 0.08f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.06f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.07f, 0.08f, 1.00f);

	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.85f, 0.90f, 0.50f);

	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.10f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.03f, 0.22f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.03f, 0.32f, 0.36f, 1.00f);

	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.05f, 0.06f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.02f, 0.20f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03f, 0.05f, 0.06f, 0.80f);

	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.03f, 0.05f, 0.06f, 1.00f);

	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.05f, 0.06f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 0.60f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.75f, 0.85f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.90f, 1.00f, 1.00f);

	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.95f, 1.00f, 1.00f);

	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.75f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.95f, 1.00f, 1.00f);

	style.Colors[ImGuiCol_Button] = ImVec4(0.02f, 0.20f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.02f, 0.45f, 0.52f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.70f, 0.82f, 1.00f);

	style.Colors[ImGuiCol_Header] = ImVec4(0.02f, 0.20f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.02f, 0.40f, 0.48f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.65f, 0.76f, 1.00f);

	style.Colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.85f, 0.90f, 0.40f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.02f, 0.20f, 0.24f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.60f, 0.72f, 0.80f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.90f, 1.00f, 1.00f);

	style.Colors[ImGuiCol_Tab] = ImVec4(0.05f, 0.10f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.02f, 0.45f, 0.52f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.02f, 0.25f, 0.30f, 1.00f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.70f, 0.98f, 1.00f, 1.00f);  // icy cyan text
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.20f, 0.45f, 0.50f, 1.00f);

	// -- Sharp edges for cyberpunk feel --
	style.WindowRounding = 2.0f;
	style.FrameRounding = 2.0f;
	style.ScrollbarRounding = 2.0f;
	style.GrabRounding = 2.0f;
	style.TabRounding = 2.0f;
	style.FramePadding = ImVec2(8.0f, 4.0f);
	style.ItemSpacing = ImVec2(8.0f, 6.0f);
	style.WindowPadding = ImVec2(10.0f, 10.0f);


	ImGui_ImplGlfw_InitForOpenGL(mainWindow.GetGLFWwindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// ── ImGui-controlled state ──────────────────────────────────────────────
	// Surface maps
	float normalStrength = 10.0f;
	float roughnessIntensity = 1.0f;

	// Directional light — mirror initial values
	float dirAmbient = 0.0f;
	float dirDiffuse = 0.0f;
	float dirDirection[3] = { 1.0f, -1.0f, -2.0f };
	float dirColour[3] = { 1.0f, 1.0f, 1.0f };

	// Point lights — mirror initial values
	struct PLState {
		float pos[3];
		float ambient;
		float diffuse;
		float colour[3];
	};
	PLState pl[MAX_POINT_LIGHTS] = {
		{{ 4.0f, 1.0f,  3.0f }, 0.6f, 9.0f,  { 0.8f, 0.1f, 0.1f }},
	    {{ 2.0f, 1.0f,  1.0f }, 0.6f, 6.0f,  { 0.6f, 1.0f, 0.5f }},
	    {{ 9.0f, 1.0f,  9.0f }, 0.0f, 15.0f, { 0.0f, 0.2f, 0.8f }},
	};
	const char* plNames[MAX_POINT_LIGHTS] = { "Red Light", "Green Light", "Blue Light" };

	bool  showUI = true;   // toggle with Tab
	bool  uiHasMouse = false;







	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePoisiton = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();


		// ── Tab toggles UI / camera mode ────────────────────────────────────
		static bool tabWasDown = false;
		bool* keys = mainWindow.getsKeys();
		if (keys[GLFW_KEY_TAB] && !tabWasDown)
		{
			showUI = !showUI;
			mainWindow.SetCursorEnabled(showUI);
		}
		tabWasDown = keys[GLFW_KEY_TAB];

		uiHasMouse = showUI && io.WantCaptureMouse;

		if (!showUI)
		{
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			camera.keyControl(keys, deltaTime);
		}
		else
		{
			mainWindow.getXChange();
			mainWindow.getYChange(); // consume deltas
		}



		// ── FPS title ────────────────────────────────────────────────────────
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - FinalTime >= 1.0)
		{
			double fps = frameCount / (currentTime - FinalTime);
			glfwSetWindowTitle(mainWindow.GetGLFWwindow(),
				("FPS: " + std::to_string((int)fps)).c_str());
			frameCount = 0;
			FinalTime = currentTime;
		}

		// ── Push ImGui state to lights ───────────────────────────────────────
		mainLight.SetDirection(dirDirection[0], dirDirection[1], dirDirection[2]);
		mainLight.SetAmbientIntensity(dirAmbient);
		mainLight.SetDiffuseIntensity(dirDiffuse);
		mainLight.SetColour(dirColour[0], dirColour[1], dirColour[2]);

		for (int i = 0; i < (int)pointLightCount; i++)
		{
			pointLights[i].SetPosition(pl[i].pos[0], pl[i].pos[1], pl[i].pos[2]);
			pointLights[i].SetAmbientIntensity(pl[i].ambient);
			pointLights[i].SetDiffuseIntensity(pl[i].diffuse);
			pointLights[i].SetColour(pl[i].colour[0], pl[i].colour[1], pl[i].colour[2]);
		}

		// ── ImGui frame ──────────────────────────────────────────────────────
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showUI)
		{
			ImGui::SetNextWindowSize(ImVec2(380, 0), ImGuiCond_Once);
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
			ImGui::Begin("PBR Renderer");

			//ImGui::TextDisabled("Press Tab to switch between UI and camera");
			ImGui::Separator();

			// Surface maps
			if (ImGui::CollapsingHeader("Surface Maps", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SliderFloat("Normal Strength", &normalStrength, 0.0f, 20.0f);
				ImGui::SliderFloat("Roughness Intensity", &roughnessIntensity, 0.0f, 3.0f);
			}

			// Directional light
			if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::ColorEdit3("Colour##dir", dirColour);
				ImGui::SliderFloat("Ambient##dir", &dirAmbient, 0.0f, 1.0f);
				ImGui::SliderFloat("Diffuse##dir", &dirDiffuse, 0.0f, 2.0f);
				ImGui::SliderFloat3("Direction", dirDirection, -5.0f, 5.0f);
			}

			// Point lights
			if (ImGui::CollapsingHeader("Point Lights", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (int i = 0; i < (int)pointLightCount; i++)
				{
					ImGui::PushID(i);
					if (ImGui::TreeNode(plNames[i]))
					{
						ImGui::ColorEdit3("Colour", pl[i].colour);
						ImGui::SliderFloat("Ambient", &pl[i].ambient, 0.0f, 2.0f);
						ImGui::SliderFloat("Diffuse", &pl[i].diffuse, 0.0f, 20.0f);
						ImGui::SliderFloat("X", &pl[i].pos[0], -20.0f, 20.0f);
						ImGui::SliderFloat("Y", &pl[i].pos[1], -20.0f, 20.0f);
						ImGui::SliderFloat("Z", &pl[i].pos[2], -20.0f, 20.0f);
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
			}

			ImGui::End();
		}
		

			// Clear the window
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			shaderList[0].UseShader();
			uniformModel = shaderList[0].GetModelLocation();
			uniformProjection = shaderList[0].GetProjectionLocation();
			uniformView = shaderList[0].GetViewLocation();
			uniformEyePoisiton = shaderList[0].GetEyePositionLocation();
			uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
			uniformShininess = shaderList[0].GetShininessLocation();
			GLuint uniformRougnessMap = shaderList[0].GetRoughnessMapLocation();
			GLuint uniformUseRoughnessMap = shaderList[0].GetUseRoughnessMapLocation();
			shaderList[0].SetUseNormalMap(true);
			shaderList[0].SetNormalStrength(normalStrength);
			shaderList[0].SetRoughnessIntensity(roughnessIntensity);

			glUniform1i(uniformRougnessMap, 2);


			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(pointLights, pointLightCount);


			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePoisiton, camera.getCameraPoisiton().x, camera.getCameraPoisiton().y, camera.getCameraPoisiton().z);


			glm::mat4 model(1.0f);

			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			brickTexture.UseTexture();
			shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			//meshList[0]->RenderMesh();

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			dirtTexture.UseTexture();
			dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			//meshList[1]->RenderMesh();

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			brickTexture.UseTexture();
			brickNormal.UseNormalMap();

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, brickRoughness.GetID());
			glUniform1f(uniformUseRoughnessMap, 2);



			shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[2]->RenderMesh();

			//CALLING DRAW ICON
			glm::mat4 viewMatrix = camera.calculateViewMatrix();

			// Directional light icon
			glm::vec3 dirIconPos = glm::vec3(0.0f, 3.0f, 0.0f);
			DrawLightIcon(billboardMesh, billboardShader, iconDirectional,
				dirIconPos,
				mainLight.GetColour(),
				viewMatrix, projection);

			// Point light icons
			for (int i = 0; i < (int)pointLightCount; i++)
			{
				DrawLightIcon(billboardMesh, billboardShader, iconPoint,
					pointLights[i].GetPosition(),
					pointLights[i].GetColour(),
					viewMatrix, projection);
			}


			glUseProgram(0);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			mainWindow.swapBuffers();
		}
		return 0;


	
}