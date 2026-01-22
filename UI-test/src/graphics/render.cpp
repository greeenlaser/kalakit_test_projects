//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#else
//TODO: define for linux
#endif

#include <string>
#include <vector>
#include <filesystem>
#include <sstream>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_kfd.hpp"
#include "KalaHeaders/key_standards.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "core/kw_registry.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"
#include "opengl/kw_opengl_functions_windows.hpp"
#include "core/kw_messageloop_windows.hpp"

#include "core/ku_core.hpp"
#include "core/ku_registry.hpp"
#include "core/ku_font.hpp"
#include "opengl/ku_opengl_manager.hpp"
#include "opengl/ku_opengl_texture.hpp"
#include "opengl/ku_opengl_shader.hpp"
#include "opengl/ku_opengl_functions.hpp"
#include "opengl/shaders/ku_shader_quad.hpp"
#include "opengl/shaders/ku_shader_text2.hpp"
#include "opengl/ui/ku_opengl_widget.hpp"
#include "opengl/ui/ku_opengl_image.hpp"
#include "opengl/ui/ku_opengl_text.hpp"

#include "graphics/render.hpp"

using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::mat4;
using KalaHeaders::KalaMath::ortho;
using KalaHeaders::KalaMath::Transform2D;
using KalaHeaders::KalaMath::PosTarget;
using KalaHeaders::KalaMath::RotTarget;
using KalaHeaders::KalaMath::SizeTarget;
using KalaHeaders::KalaFontData::GlyphHeader;
using KalaHeaders::KalaFontData::GlyphBlock;
using KalaHeaders::KalaKeyStandards::GetUTFByValue;
using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowRegistry;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowState;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::OpenGL::OpenGL_Context;
using KalaWindow::OpenGL::VSyncState;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;
using KalaWindow::OpenGL::OpenGLFunctions::DebugCallback;
using KalaWindow::Core::MessageLoop;

using KalaUI::Core::KalaUICore;
using KalaUI::Core::KalaUIRegistry;
using KalaUI::Core::Font;
using KalaUI::OpenGL::OpenGL_Manager;
using KalaUI::OpenGL::OpenGL_Texture;
using KalaUI::OpenGL::TextureFormat;
using KalaUI::OpenGL::OpenGL_Shader;
using KalaUI::OpenGL::OpenGL_ShaderType;
using KalaUI::OpenGL::OpenGL_Shaders::shader_quad_vertex;
using KalaUI::OpenGL::OpenGL_Shaders::shader_quad_fragment;
using KalaUI::OpenGL::OpenGL_Shaders::shader_text_vertex2;
using KalaUI::OpenGL::OpenGL_Shaders::shader_text_fragment2;
using KalaUI::OpenGL::UI::OpenGL_Widget;
using KalaUI::OpenGL::UI::OpenGL_Image;
using KalaUI::OpenGL::UI::OpenGL_Text;
using KalaUI::OpenGL::OpenGLFunctions::GL_UI;
using KalaUI::OpenGL::OpenGLFunctions::OpenGL_Functions_UI;

using std::string;
using std::vector;
using std::filesystem::path;
using std::filesystem::current_path;
using std::ostringstream;

static void Redraw(Window* window);
static void Resize(Window* window);

static Window* CreateNewWindow(const string& name);
	
//light blue background color
constexpr vec3 NORMALIZED_BACKGROUND_COLOR = vec3(0.29f, 0.36f, 0.85f);

constexpr vec2 BASE_SIZE = vec2(1280.0f, 720.0f);

//Deletes image and text widgets for the selected window
static void CleanWindowUIContent(u32 windowID)
{
	KalaUIRegistry<OpenGL_Widget>::RemoveAllWindowContent(windowID);
}

static void AssignUIFunctions();

namespace UITest::Graphics
{
	void Render::Initialize()
	{
		Window_Global::Initialize();
		OpenGL_Global::Initialize();
		
		AssignUIFunctions();
		
		Window* window = CreateNewWindow("UI test");

		u32 windowID = window->GetID();

		const vector<OpenGL_Context*>& contexts = KalaWindowRegistry<OpenGL_Context>::GetAllWindowContent(windowID);
			
		OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

		if (!context)
		{
			KalaWindowCore::ForceClose(
				"Initialization error",
				"Failed to attach an OpenGL context to window '" + window->GetTitle() + "'!");

			return;
		}
		
#ifdef _WIN32
		uintptr_t handle = window->GetWindowData().hdc;
		uintptr_t glContext = context->GetContext();
#else
		//TODO: define for linux
#endif

		u32 glID = window->GetGLID();

		OpenGL_Global::MakeContextCurrent(context, handle);

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

#ifdef _DEBUG
		coreFunc->glEnable(GL_DEBUG_OUTPUT);
		coreFunc->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Ensures callbacks run immediately
		coreFunc->glDebugMessageCallback(DebugCallback, nullptr);
#endif

		//bump KalaUI id to current kalawindow id
		KalaUICore::SetGlobalID(KalaWindowCore::GetGlobalID());

		string texPath = (current_path() / "files" / "UI" / "image1.png").string();
		
		OpenGL_Texture* tex01 = OpenGL_Texture::Initialize(
			glContext,
			"tex01",
			texPath,
			TextureFormat::Format_RGBA8);

		OpenGL_Shader* shader01 = OpenGL_Shader::Initialize(
			glContext,
			"shader01",
			{ {
				{.shaderData = string(shader_quad_vertex), .type = OpenGL_ShaderType::SHADER_VERTEX },
				{.shaderData = string(shader_quad_fragment), .type = OpenGL_ShaderType::SHADER_FRAGMENT }
			} });
			
		OpenGL_Shader* shader02 = OpenGL_Shader::Initialize(
			glContext,
			"shader02",
			{ {
				{.shaderData = string(shader_text_vertex2), .type = OpenGL_ShaderType::SHADER_VERTEX },
				{.shaderData = string(shader_text_fragment2), .type = OpenGL_ShaderType::SHADER_FRAGMENT }
			} });

		OpenGL_Image* image = OpenGL_Image::Initialize(
			"image_load_icon",
			windowID,
			glContext,
			0.0f,
			0.0f,
			100.0f,
			tex01,
			shader01);
			
		path fontPath = current_path() / "files" / "fonts" / "times-bold.kfd";
		
		Font* font = Font::Initialize(
			"times-bold", 
			fontPath.string(),
			glContext);

		if (font)
		{
			OpenGL_Text* t = OpenGL_Text::Initialize(
				"text_widget",
				windowID,
				glContext,
				{},
				font->GetID(),
				vec2(50.0f),
				0.0f,
				25.0f,
				shader02);

			t->SetUpscale(1.1f);

			//add letter
			MessageLoop::SetAddCharCallback([t](u32 c) { t->AddLetter(c); });

			//remove last letter
			MessageLoop::SetRemoveFromBackCallback(
				[t]() 
				{ 
					Log::Print(
						"Removed last letter from Text widget '" + t->GetName() + "'",
						"UI_TEST",
						LogType::LOG_DEBUG);

					const auto& letters = t->GetLetters();
					if (!letters.empty()) t->RemoveLetterFromSlot(letters.size() - 1); 
				});

			//add new line
			MessageLoop::SetAddNewLineCallback(
				[t]() 
				{ 
					Log::Print(
						"Added new line to Text widget '" + t->GetName() + "'",
						"UI_TEST",
						LogType::LOG_DEBUG);

					t->AddNewLine(); 
				});

			//add tab
			MessageLoop::SetAddTabCallback(
				[t]() 
				{ 
					Log::Print(
						"Added tab to Text widget '" + t->GetName() + "'",
						"UI_TEST",
						LogType::LOG_DEBUG);

					t->AddTab(); 
				});
		}

		{
			coreFunc->glClearColor(
				NORMALIZED_BACKGROUND_COLOR.x,
				NORMALIZED_BACKGROUND_COLOR.y,
				NORMALIZED_BACKGROUND_COLOR.z,
				1.0f);

			coreFunc->glClear(
				GL_COLOR_BUFFER_BIT
				| GL_DEPTH_BUFFER_BIT);

			context->SwapOpenGLBuffers(window->GetWindowData().hwnd);

			window->SetWindowState(WindowState::WINDOW_MAXIMIZE);
		}
		
		Log::Print(
			"Reached end of Solin initialization!",
			"RENDER",
			LogType::LOG_DEBUG);
	}
	
	void Render::Update()
	{
		for (const auto& window : Window::GetRegistry().runtimeContent)
		{
			if (!window) continue;
			
			window->Update();
			u32 windowID = window->GetID();

			const vector<Input*>& inputs = KalaWindowRegistry<Input>::GetAllWindowContent(windowID);
			Input* input = inputs.empty() ? nullptr : inputs.front();

			if (!window->IsIdle()
				&& !window->IsResizing())
			{
				Redraw(window);
			}

			if (input) input->EndFrameUpdate();
		}
	}
}

void Redraw(Window* window)
{
	if (!window) return;

	u32 windowID = window->GetID();
	
#ifdef _WIN32
	uintptr_t handle = window->GetWindowData().hdc;
#else
	//TODO: define for linux
#endif

	mat4 projection = ortho(window->GetClientRectSize());

	auto contexts = KalaWindowRegistry<OpenGL_Context>::GetAllWindowContent(windowID);
	OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

	if (!context) return;
	
	OpenGL_Global::MakeContextCurrent(context, handle);

	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	coreFunc->glClearColor(
		NORMALIZED_BACKGROUND_COLOR.x,
		NORMALIZED_BACKGROUND_COLOR.y,
		NORMALIZED_BACKGROUND_COLOR.z,
		1.0f);
	coreFunc->glClear(
		GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);

	coreFunc->glDisable(GL_CULL_FACE);
	
	const vector<Input*>& inputs = KalaWindowRegistry<Input>::GetAllWindowContent(windowID);
	Input* input = inputs.empty() ? nullptr : inputs.front();
	
	vec2 mousePos{};
	if (input) mousePos = input->GetMousePosition();
	
	/*
	ostringstream oss{};
	
	oss << "mouse pos: " << mousePos.x << ", " << mousePos.y << "\n";
	*/
	
	const vector<OpenGL_Widget*>& widgets = KalaUIRegistry<OpenGL_Widget>::GetAllWindowContent(windowID);
		
	for (const auto& w : widgets)
	{
		if (auto* i = dynamic_cast<OpenGL_Image*>(w))
		{
			i->MoveWidget(
				window->GetClientRectSize(),
				vec2(1.0f, 1.0f));

			i->Render(
				projection,
				window->GetClientRectSize().y);
				
			/*
			Transform2D* t2 = i->GetTransform();
			vec2 pos = t2->GetPos(PosTarget::POS_WORLD);
			vec2 size = t2->GetSize(SizeTarget::SIZE_WORLD);
				
			oss << "image pos:  " << pos.x << ", " << pos.y << "\n"
				<< "image size: " << size.x << ", " << size.y << "\n";
			*/
				
			if (i->IsHovered(mousePos)
				&& input->IsMouseButtonPressed(MouseButton::M_LEFT))
			{
				/*
				Log::Print(
					"hovering over image!",
					"UI_TEST",
					LogType::LOG_INFO);
				*/
				
				Log::Print(
					"clicked on image!",
					"UI_TEST",
					LogType::LOG_INFO);
			}
		}
		else if (auto* t = dynamic_cast<OpenGL_Text*>(w))
		{
			/*
			t->MoveWidget(
				window->GetClientRectSize(),
				vec2(0.75f, 0.75f));
			*/

			t->Render(
				projection,
				window->GetClientRectSize().y);
				
			/*
			Transform2D* t2 = t->GetTransform();
			vec2 pos = t2->GetPos(PosTarget::POS_COMBINED);
			vec2 size = t2->GetSize(SizeTarget::SIZE_COMBINED);
				
			oss << "text pos:  " << pos.x << ", " << pos.y << "\n"
				<< "text size: " << size.x << ", " << size.y << "\n----------\n";
			*/
				
			if (t->IsHovered(mousePos)
				&& input->IsMouseButtonPressed(MouseButton::M_LEFT))
			{
				/*
				Log::Print(
					"hovering over text!",
					"UI_TEST",
					LogType::LOG_INFO);
				*/
				
				Log::Print(
					"clicked on text!",
					"UI_TEST",
					LogType::LOG_INFO);
			}
		}
	}
	
	//Log::Print(oss.str());
	
	coreFunc->glEnable(GL_CULL_FACE);

	context->SwapOpenGLBuffers(handle);
}

void Resize(Window* window)
{
	vec2 vpSize = window->GetClientRectSize();

	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	coreFunc->glViewport(
		0,
		0,
		vpSize.x,
		vpSize.y);
}

Window* CreateNewWindow(const string& name)
{
	Window* window = Window::Initialize(
		name,
		BASE_SIZE);

	if (!window)
	{
		KalaWindowCore::ForceClose(
			"Initialization error",
			"Failed to create a window!");

		return nullptr;
	}
	
	window->SetRedrawCallback([window]() { Redraw(window); });
	window->SetResizeCallback([window]() { Resize(window); });
	
	window->SetCleanExternalContent(CleanWindowUIContent);

	u32 windowID = window->GetID();

	OpenGL_Context* context = OpenGL_Context::Initialize(windowID, 0);

	if (!context)
	{
		KalaWindowCore::ForceClose(
			"Initialization error",
			"Failed to attach an OpenGL context to window '" + window->GetTitle() + "'!");

		return nullptr;
	}

	u32 glID = context->GetID();
	
	context->SetVSyncState(VSyncState::VSYNC_ON);

	/*
	if (!windowIconTexture)
	{
		if (!exists(windowIconPath))
		{
			KalaWindowCore::ForceClose(
				"Initialization error",
				"Failed to attach icon to window '" + window->GetTitle() + "' because the icon was not found!");

			return nullptr;
		}
		else
		{
			windowIconTexture = OpenGL_Texture::LoadTexture(
				windowID,
				"exeIcon",
				windowIconPath.data(),
				OpenGL_TextureType::Type_2D,
				OpenGL_TextureFormat::Format_RGBA8);
		}
	}
	else window->SetIcon(windowIconTexture->GetID());
	*/

	Input* input = Input::Initialize(windowID);

	if (!input)
	{
		KalaWindowCore::ForceClose(
			"Initialization error",
			"Failed to attach an Input context to window '" + window->GetTitle() + "'!");

		return nullptr;
	}

	return window;
}

void AssignUIFunctions()
{
	static GL_UI uiFunc{};
	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	//state

	uiFunc.glEnable      = coreFunc->glEnable;
	uiFunc.glDisable     = coreFunc->glDisable;
	uiFunc.glBlendFunc   = coreFunc->glBlendFunc;
	uiFunc.glDepthMask   = coreFunc->glDepthMask;
	uiFunc.glGetError    = coreFunc->glGetError;
	uiFunc.glGetIntegerv = coreFunc->glGetIntegerv;

	//buffers / VAOs

	uiFunc.glGenBuffers    = coreFunc->glGenBuffers;
	uiFunc.glDeleteBuffers = coreFunc->glDeleteBuffers;
	uiFunc.glBindBuffer    = coreFunc->glBindBuffer;
	uiFunc.glBufferData    = coreFunc->glBufferData;

	uiFunc.glGenVertexArrays    = coreFunc->glGenVertexArrays;
	uiFunc.glDeleteVertexArrays = coreFunc->glDeleteVertexArrays;
	uiFunc.glBindVertexArray    = coreFunc->glBindVertexArray;

	uiFunc.glEnableVertexAttribArray  = coreFunc->glEnableVertexAttribArray;
	uiFunc.glVertexAttribPointer      = coreFunc->glVertexAttribPointer;
	uiFunc.glDisableVertexAttribArray = coreFunc->glDisableVertexAttribArray;

	//drawing

	uiFunc.glDrawElements = coreFunc->glDrawElements;

	//shaders / programs

	uiFunc.glCreateShader     = coreFunc->glCreateShader;
	uiFunc.glShaderSource     = coreFunc->glShaderSource;
	uiFunc.glCompileShader    = coreFunc->glCompileShader;
	uiFunc.glGetShaderiv      = coreFunc->glGetShaderiv;
	uiFunc.glGetShaderInfoLog = coreFunc->glGetShaderInfoLog;
	uiFunc.glDeleteShader     = coreFunc->glDeleteShader;

	uiFunc.glCreateProgram     = coreFunc->glCreateProgram;
	uiFunc.glAttachShader      = coreFunc->glAttachShader;
	uiFunc.glDetachShader      = coreFunc->glDetachShader;
	uiFunc.glLinkProgram       = coreFunc->glLinkProgram;
	uiFunc.glValidateProgram   = coreFunc->glValidateProgram;
	uiFunc.glUseProgram        = coreFunc->glUseProgram;
	uiFunc.glIsProgram         = coreFunc->glIsProgram;
	uiFunc.glGetProgramiv      = coreFunc->glGetProgramiv;
	uiFunc.glGetProgramInfoLog = coreFunc->glGetProgramInfoLog;
	uiFunc.glDeleteProgram     = coreFunc->glDeleteProgram;

	uiFunc.glGetUniformLocation = coreFunc->glGetUniformLocation;
	uiFunc.glUniform1i          = coreFunc->glUniform1i;
	uiFunc.glUniform1f          = coreFunc->glUniform1f;
	uiFunc.glUniform1fv         = coreFunc->glUniform1fv;
	uiFunc.glUniform2fv         = coreFunc->glUniform2fv;
	uiFunc.glUniform3fv         = coreFunc->glUniform3fv;
	uiFunc.glUniform4fv         = coreFunc->glUniform4fv;
	uiFunc.glUniformMatrix2fv   = coreFunc->glUniformMatrix2fv;
	uiFunc.glUniformMatrix3fv   = coreFunc->glUniformMatrix3fv;
	uiFunc.glUniformMatrix4fv   = coreFunc->glUniformMatrix4fv;
	
	//textures
	
	uiFunc.glActiveTexture   = coreFunc->glActiveTexture;
	uiFunc.glGenTextures     = coreFunc->glGenTextures;
	uiFunc.glDeleteTextures  = coreFunc->glDeleteTextures;
	uiFunc.glBindTexture     = coreFunc->glBindTexture;
	uiFunc.glTexParameteri   = coreFunc->glTexParameteri;
	uiFunc.glPixelStorei     = coreFunc->glPixelStorei;
	uiFunc.glTexImage2D      = coreFunc->glTexImage2D;
	uiFunc.glTexSubImage2D   = coreFunc->glTexSubImage2D;
	uiFunc.glTexSubImage3D   = coreFunc->glTexSubImage3D;
	uiFunc.glTexStorage2D    = coreFunc->glTexStorage2D;
	uiFunc.glTexStorage3D    = coreFunc->glTexStorage3D;
	uiFunc.glGenerateMipmap  = coreFunc->glGenerateMipmap;
	uiFunc.glVertexAttribI1i = coreFunc->glVertexAttribI1i;

	OpenGL_Functions_UI::SetGLUI(&uiFunc);
}