//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <filesystem>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/file_utils.hpp"
#include "KalaHeaders/import_kmd.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"
#include "opengl/kw_opengl_functions_windows.hpp"
#include "opengl/kw_opengl_shader.hpp"

#include "core/ku_core.hpp"
#include "opengl/ku_opengl_functions.hpp"

#include "graphics/render.hpp"
#include "core/core.hpp"
#include "core/input.hpp"
#include "gameobject/camera.hpp"

using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaMath::wrap;
using KalaHeaders::KalaMath::radians;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::mat4;
using KalaHeaders::KalaMath::PosTarget;
using KalaHeaders::KalaMath::RotTarget;
using KalaHeaders::KalaMath::SizeTarget;
using KalaHeaders::KalaModelData::Vertex;
using KalaHeaders::KalaFile::ReadTextFromFile;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowState;
using KalaWindow::Graphics::WindowMode;
using KalaWindow::Graphics::WindowData;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::OpenGL::OpenGL_Context;
using KalaWindow::OpenGL::VSyncState;
using KalaWindow::OpenGL::OpenGL_Shader;
using KalaWindow::OpenGL::OpenGL_ShaderType;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;
using KalaWindow::OpenGL::OpenGLFunctions::DebugCallback;

using KalaUI::Core::KalaUICore;
using KalaUI::OpenGL::OpenGLFunctions::GL_UI;
using KalaUI::OpenGL::OpenGLFunctions::OpenGL_Functions_UI;

using GameTest::Core::GameTestCore;
using GameTest::Core::GameTestInput;
using GameTest::Graphics::MainWindow;
using GameTest::Graphics::Render;
using GameTest::GameObject::Camera;

using std::string;
using std::vector;
using std::filesystem::path;
using std::filesystem::current_path;
using std::filesystem::exists;

//light blue background color
constexpr vec3 NORMALIZED_BACKGROUND_COLOR = vec3(0.29f, 0.36f, 0.85f);

static void CreateNewWindow(const string& windowName);

static void Redraw();
static void Resize();

static void AssignUIFunctions();

namespace GameTest::Graphics
{
	static MainWindow mainWindow{};

	static vector<Camera*> cameras{};
	static vector<OpenGL_Model*> models{};
	static vector<OpenGL_PointLight*> pointLights{};

	void Render::Initialize()
	{
		Log::Print(
			"initializing render...",
			"RENDER",
			LogType::LOG_DEBUG);
			
		Window_Global::Initialize();
		OpenGL_Global::Initialize();

		AssignUIFunctions();
		
		CreateNewWindow("game test");

		GameTestCore::SyncID();

		Camera* camera = Camera::Initialize(
			"camera",
			0.0f,
			0.0f,
			Render::GetMainWindow().window->GetClientRectSize(),
			90.0f,
			0.25f);
		Render::GetCameras().push_back(camera);
		
		const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

		//enable depth testing
		coreFunc->glEnable(GL_DEPTH_TEST);
		//render closest object to camera in front
		coreFunc->glDepthFunc(GL_LESS);
		//enable face culling
		coreFunc->glEnable(GL_CULL_FACE);
		//cull backfaces of mesh
		coreFunc->glCullFace(GL_BACK);
		//set normal winding counterclockwise
		coreFunc->glFrontFace(GL_CCW);
		
		OpenGL_Context* context = mainWindow.context;

		//
		// LOAD MODEL SHADER
		//

		path vModel = current_path() / "files" / "shaders" / "model.vert";
		if (!exists(vModel))
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				"Failed to initialize model shader because the model.vert shader file was not found!");

			return;
		}

		string vModelText{};
		string vModelResult = ReadTextFromFile(vModel, vModelText);

		if (!vModelResult.empty())
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				vModelResult);

			return;
		}

		path fModel = current_path() / "files" / "shaders" / "model.frag";
		if (!exists(fModel))
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				"Failed to initialize model shader because the model.frag shader file was not found!");

			return;
		}

		string fModelText{};
		string fModelResult = ReadTextFromFile(fModel, fModelText);

		if (!fModelResult.empty())
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				fModelResult);

			return;
		}
		
		OpenGL_Shader* shader_model = OpenGL_Shader::Initialize(
			context,
			"shader_model",
			{ {
				{.shaderData = vModelText, .type = OpenGL_ShaderType::SHADER_VERTEX },
				{.shaderData = fModelText, .type = OpenGL_ShaderType::SHADER_FRAGMENT }
			} });

		//
		// LOAD DEBUG SHAPE SHADER
		//

		path vShape = current_path() / "files" / "shaders" / "debug_shape.vert";
		if (!exists(vShape))
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				"Failed to initialize debug shape shader because debug_shape.vert was not found!");

			return;
		}

		string vShapeText{};
		string vShapeResult = ReadTextFromFile(vShape, vShapeText);

		if (!vShapeResult.empty())
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				vShapeResult);

			return;
		}

		path fShape = current_path() / "files" / "shaders" / "debug_shape.frag";
		if (!exists(fShape))
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				"Failed to initialize debug shape shader because debug_shape.frag was not found!");

			return;
		}

		string fShapeText{};
		string fShapeResult = ReadTextFromFile(fShape, fShapeText);

		if (!fShapeResult.empty())
		{
			KalaWindowCore::ForceClose(
				"Shader load error",
				fShapeResult);

			return;
		}
			
		OpenGL_Shader* shader_debug_shape = OpenGL_Shader::Initialize(
			context,
			"shader_debug_shape",
			{ {
				{.shaderData = string(vShapeText), .type = OpenGL_ShaderType::SHADER_VERTEX },
				{.shaderData = string(fShapeText), .type = OpenGL_ShaderType::SHADER_FRAGMENT }
			} });

		//
		// LOAD TEST MODEL
		//
		
		path testModel = current_path() / "files" / "models" / "crusher.kmd";
		if (!exists(testModel))
		{
			KalaWindowCore::ForceClose(
				"Model load error",
				"Failed to initialize model because crusher.kmd was not found!");

			return;
		}

		Render::GetModels() = OpenGL_Model::InitializeAll(
			testModel.string(),
			context,
			shader_model);

		OpenGL_Model* m = Render::GetModels()[0];

		vec3 newSize = m->GetSize(SizeTarget::SIZE_WORLD) * 0.01f;
		m->SetSize(SizeTarget::SIZE_WORLD, newSize);

		vec3 newPos = m->GetPos(PosTarget::POS_WORLD) + vec3(0.0f, 0.0f, -0.05f);
		m->SetPos(PosTarget::POS_WORLD, newPos);

		m->SetNormalizedDiffuseColor(0.2f);

		//
		// LOAD POINT LIGHT
		//
		
		OpenGL_PointLight* pl = OpenGL_PointLight::Initialize("light01", context);
		
		vec3 newLightPos = pl->GetPos(PosTarget::POS_WORLD) + vec3(0.0f, 0.0f, 1.0f);
		pl->SetPos(PosTarget::POS_WORLD, newLightPos);
		
		pl->SetIntensity(0.2f);
		pl->SetRGBColor(vec3(84, 255, 98));
		
		Render::GetPointLights().push_back(pl);

		{
			const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

			coreFunc->glClearColor(
				NORMALIZED_BACKGROUND_COLOR.x,
				NORMALIZED_BACKGROUND_COLOR.y,
				NORMALIZED_BACKGROUND_COLOR.z,
				1.0f);

			coreFunc->glClear(
				GL_COLOR_BUFFER_BIT
				| GL_DEPTH_BUFFER_BIT);

			mainWindow.context->SwapOpenGLBuffers(mainWindow.window->GetWindowData().hwnd);

			mainWindow.window->SetWindowMode(WindowMode::WINDOWMODE_EXCLUSIVE);
		}
	}

	MainWindow& Render::GetMainWindow() { return mainWindow; }
	vector<Camera*>& Render::GetCameras() { return cameras; }
	vector<OpenGL_Model*>& Render::GetModels() { return models; }
	vector<OpenGL_PointLight*>& Render::GetPointLights() { return pointLights; }
	
	void Render::Update()
	{
		auto& mw = Render::GetMainWindow();

		if (!mw.window) return;
		
		Window* w = mw.window;
		Input* i = mw.input;
		
		w->Update();
		
		GameTestInput::Update();
		
		if (!w->IsIdle()
			&& !w->IsResizing())
		{
			Redraw();
		}
		
		if (i) i->EndFrameUpdate();
	}
}

void CreateNewWindow(const string& name)
{
	auto& mw = Render::GetMainWindow();

	mw.window = Window::Initialize(
		name,
		vec2(1280, 720));
		
	mw.window->SetRedrawCallback([](){ Redraw(); });
	mw.window->SetResizeCallback([](){ Resize(); });
	
	u32 windowID = mw.window->GetID();
	mw.context = OpenGL_Context::Initialize(windowID, 0);
	
	u32 contextID = mw.context->GetID();
	
	mw.context->SetVSyncState(VSyncState::VSYNC_ON);
	
	mw.input = Input::Initialize(windowID);
	
	Log::Print(
		"Created new window '" + name + "'!",
		"RENDER",
		LogType::LOG_DEBUG);
}

void Redraw()
{
	auto& mw = Render::GetMainWindow();

	if (!mw.window) return;
	
	Window* w = mw.window;
	const WindowData& wData = w->GetWindowData();
	vec2 vpSize = w->GetClientRectSize();
	OpenGL_Context* c = mw.context;
	
	Camera* cam = Render::GetCameras()[0];
	
	//--------------------
	// gl draw start
	//--------------------
	
	u32 glID = c->GetID();
	uintptr_t handle = wData.hdc;
	
	OpenGL_Global::MakeContextCurrent(c, handle);
	
	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	coreFunc->glClearColor(
		NORMALIZED_BACKGROUND_COLOR.x,
		NORMALIZED_BACKGROUND_COLOR.y,
		NORMALIZED_BACKGROUND_COLOR.z,
		1.0f);
	coreFunc->glClear(
		GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);
		
	//--------------------
	// render stuff
	//--------------------
		
	mat4 view = cam->GetViewMatrix();	
	mat4 perspective = cam->GetPerspectiveMatrix(vpSize);
	
	f32 deltaTime = static_cast<f32>(KalaWindowCore::GetDeltaTime());
		
	for (const auto& m : Render::GetModels())
	{
		/*
		const vec3& right = m->GetRight();
		vec3 rot = m->GetRot(RotTarget::ROT_COMBINED);
		rot.y += right * 1.0f * deltaTime;
		
		m->AddRot(RotTarget::ROT_WORLD, rot);
		*/
		
		m->Render(
			cam->GetPos(),
			view,
			perspective);
	}
	
	for (const auto& pl : Render::GetPointLights())
	{
		static f32 angle{};
		const f32 speed = 125.0f; //degrees per second
		
		angle += speed * deltaTime;
		angle = wrap(angle);
		
		f32 radius = 2.0f;
		
		vec3 center = Render::GetModels()[0]->GetPos(PosTarget::POS_COMBINED);
		
		f32 angleRad = radians(angle);
		
		vec3 newPos{};
		newPos.x = center.x + cosf(angleRad) * radius;
		newPos.z = center.z + sinf(angleRad) * radius;
		newPos.y = center.y;
		
		pl->SetPos(PosTarget::POS_WORLD, newPos);
		
		pl->Render(
			view,
			perspective);
	}
		
	c->SwapOpenGLBuffers(handle);
}

void Resize()
{
	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	vec2 vpSize = Render::GetMainWindow().window->GetClientRectSize();

	coreFunc->glViewport(
		0,
		0,
		vpSize.x,
		vpSize.y);
}

void AssignUIFunctions()
{
	static GL_UI uiFunc{};
	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	uiFunc.glEnable             = coreFunc->glEnable;
	uiFunc.glDisable            = coreFunc->glDisable;
	uiFunc.glBlendFunc          = coreFunc->glBlendFunc;
	uiFunc.glDepthMask          = coreFunc->glDepthMask;
	uiFunc.glBufferData         = coreFunc->glBufferData;
	uiFunc.glDrawElements       = coreFunc->glDrawElements;
	uiFunc.glGenVertexArrays    = coreFunc->glGenVertexArrays;
	uiFunc.glGenBuffers         = coreFunc->glGenBuffers;
	uiFunc.glBindBuffer         = coreFunc->glBindBuffer;
	uiFunc.glDeleteVertexArrays = coreFunc->glDeleteVertexArrays;
	uiFunc.glDeleteBuffers      = coreFunc->glDeleteBuffers;
	uiFunc.glGetError           = coreFunc->glGetError;

	uiFunc.glCreateShader       = coreFunc->glCreateShader;
	uiFunc.glCreateProgram      = coreFunc->glCreateProgram;
	uiFunc.glValidateProgram    = coreFunc->glValidateProgram;
	uiFunc.glAttachShader       = coreFunc->glAttachShader;
	uiFunc.glLinkProgram        = coreFunc->glLinkProgram;
	uiFunc.glGetProgramiv       = coreFunc->glGetProgramiv;
	uiFunc.glGetProgramInfoLog  = coreFunc->glGetProgramInfoLog;
	uiFunc.glGetShaderInfoLog   = coreFunc->glGetShaderInfoLog;
	uiFunc.glIsProgram          = coreFunc->glIsProgram;
	uiFunc.glUseProgram         = coreFunc->glUseProgram;
	uiFunc.glShaderSource       = coreFunc->glShaderSource;
	uiFunc.glCompileShader      = coreFunc->glCompileShader;
	uiFunc.glGetUniformLocation = coreFunc->glGetUniformLocation;
	uiFunc.glGetIntegerv        = coreFunc->glGetIntegerv;
	uiFunc.glGetShaderiv        = coreFunc->glGetShaderiv;
	uiFunc.glUniform1i          = coreFunc->glUniform1i;
	uiFunc.glUniform1f          = coreFunc->glUniform1f;
	uiFunc.glUniform1fv         = coreFunc->glUniform1fv;
	uiFunc.glUniform2fv         = coreFunc->glUniform2fv;
	uiFunc.glUniform3fv         = coreFunc->glUniform3fv;
	uiFunc.glUniform4fv         = coreFunc->glUniform4fv;
	uiFunc.glUniformMatrix2fv   = coreFunc->glUniformMatrix2fv;
	uiFunc.glUniformMatrix3fv   = coreFunc->glUniformMatrix3fv;
	uiFunc.glUniformMatrix4fv   = coreFunc->glUniformMatrix4fv;
	uiFunc.glDetachShader       = coreFunc->glDetachShader;
	uiFunc.glDeleteShader       = coreFunc->glDeleteShader;
	uiFunc.glDeleteProgram      = coreFunc->glDeleteProgram;

	uiFunc.glActiveTexture           = coreFunc->glActiveTexture;
	uiFunc.glEnableVertexAttribArray = coreFunc->glEnableVertexAttribArray;
	uiFunc.glVertexAttribPointer     = coreFunc->glVertexAttribPointer;
	uiFunc.glBindTexture             = coreFunc->glBindTexture;
	uiFunc.glBindVertexArray         = coreFunc->glBindVertexArray;
	uiFunc.glGenTextures             = coreFunc->glGenTextures;
	uiFunc.glTexParameteri           = coreFunc->glTexParameteri;
	uiFunc.glPixelStorei             = coreFunc->glPixelStorei;
	uiFunc.glTexImage2D              = coreFunc->glTexImage2D;
	uiFunc.glTexSubImage2D           = coreFunc->glTexSubImage2D;
	uiFunc.glTexStorage2D            = coreFunc->glTexStorage2D;
	uiFunc.glGenerateMipmap          = coreFunc->glGenerateMipmap;
	uiFunc.glDeleteTextures          = coreFunc->glDeleteTextures;

	OpenGL_Functions_UI::SetGLUI(&uiFunc);
}