//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "opengl/kw_opengl.hpp"

#include "gameobject/camera.hpp"
#include "gameobject/opengl_model.hpp"
#include "gameobject/opengl_point_light.hpp"

namespace GameTest::Graphics
{
	using std::vector;
	
	using KalaWindow::Core::Input;
	using KalaWindow::Graphics::Window;
	using KalaWindow::OpenGL::OpenGL_Context;
	
	using GameTest::GameObject::Camera;
	using GameTest::GameObject::OpenGL_Model;
	using GameTest::GameObject::OpenGL_PointLight;
	
	struct MainWindow
	{
		Window* window{};
		Input* input{};
		OpenGL_Context* context{};
	};
	
	class Render
	{
	public:
		static void Initialize();

		static MainWindow& GetMainWindow();
		static vector<Camera*>& GetCameras();
		static vector<OpenGL_Model*>& GetModels();
		static vector<OpenGL_PointLight*>& GetPointLights();
		
		static void Update();
	};
}