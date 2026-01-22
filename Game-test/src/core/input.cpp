//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <array>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/key_standards.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"

#include "core/input.hpp"
#include "core/core.hpp"
#include "graphics/render.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/opengl_model.hpp"
#include "gameobject/opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::DIR_UP;

using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::ShutdownState;
using KalaWindow::Core::Input;
using KalaWindow::Core::InputCode;
using KalaWindow::Graphics::Window;

using GameTest::GameObject::Camera;
using GameTest::GameObject::OpenGL_Model;
using GameTest::GameObject::OpenGL_PointLight;

using GameTest::Core::GameTestInput;
using GameTest::Graphics::Render;

using std::to_string;
using std::array;

constexpr array<InputCode, 2> quitCombo =
{
	InputCode::FromKey(KeyboardButton::K_LEFT_CTRL),
	InputCode::FromKey(KeyboardButton::K_SPACE)
};

static void TogglePause(bool pauseState);

namespace GameTest::Core
{
	static bool isPaused = true;
	static bool canMove{};

	void GameTestInput::Update()
	{
		Input* i = Render::GetMainWindow().input;
		if (!i) return;
		
		Camera* c = Render::GetCameras()[0];

		if (i->IsComboPressed(quitCombo)) KalaWindowCore::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
		
		if (i->IsKeyPressed(KeyboardButton::K_ESC)) TogglePause(!isPaused);
		
		if (!isPaused
			&& canMove)
		{
			f32 deltaTime = static_cast<f32>(KalaWindowCore::GetDeltaTime());
			
			if (c)
			{
				//camera rotation
				
				static vec2 lastMouseDelta{};
				vec2 mouseDelta = i->GetRawMouseDelta();
				if (mouseDelta != lastMouseDelta)
				{
					lastMouseDelta = mouseDelta;
					c->UpdateCameraRotation(mouseDelta);
				}
				
				//camera position
				
				const vec3& front = c->GetFront();
				const vec3& right = c->GetRight();
				const vec3& up = c->GetUp();
				
				f32 velocity = c->GetSpeed() * deltaTime;
				f32 moveSpeed = c->GetSpeed(); //placeholder for future sprint
				
				vec3 pos = c->GetPos();
				
				if (i->IsKeyHeld(KeyboardButton::K_Q)) pos -= DIR_UP * velocity * moveSpeed;
				if (i->IsKeyHeld(KeyboardButton::K_E)) pos += DIR_UP * velocity * moveSpeed;
				
				if (i->IsKeyHeld(KeyboardButton::K_W)) pos += front * velocity * moveSpeed;
				if (i->IsKeyHeld(KeyboardButton::K_S)) pos -= front * velocity * moveSpeed;
				if (i->IsKeyHeld(KeyboardButton::K_A)) pos -= right * velocity * moveSpeed;
				if (i->IsKeyHeld(KeyboardButton::K_D)) pos += right * velocity * moveSpeed;
				
				c->SetPos(pos);
			}
			
			/*
			if (Render::models.size() > 0
				&& Render::models[0])
			{
				OpenGL_Model* m = Render::models[0];
				
				if (i->IsKeyHeld(KeyboardButton::K_ARROW_LEFT)) 
				{
					f32 opacity = m->GetOpacity();
					opacity -= 0.5f * deltaTime;
					
					m->SetOpacity(opacity);
				}
				if (i->IsKeyHeld(KeyboardButton::K_ARROW_RIGHT))
				{
					f32 opacity = m->GetOpacity();
					opacity += 0.5f * deltaTime;
					
					m->SetOpacity(opacity);
				}
			}
			*/

			auto& pls = Render::GetPointLights();
			
			if (pls.size() > 0
				&& pls[0])
			{
				OpenGL_PointLight* pl = pls[0];
				
				if (i->IsKeyHeld(KeyboardButton::K_ARROW_LEFT))
				{
					f32 intensity = pl->GetIntensity();
					intensity -= 0.5f * deltaTime;
					
					pl->SetIntensity(intensity);
					
					Log::Print("intensity: " + to_string(pl->GetIntensity()));
				}
				if (i->IsKeyHeld(KeyboardButton::K_ARROW_RIGHT))
				{
					f32 intensity = pl->GetIntensity();
					intensity += 0.5f * deltaTime;
					
					pl->SetIntensity(intensity);
					
					Log::Print("intensity: " + to_string(pl->GetIntensity()));
				}
				
				if (i->IsKeyHeld(KeyboardButton::K_ARROW_UP))
				{
					f32 maxRange = pl->GetMaxRange();
					maxRange += 0.5f * deltaTime;
					
					pl->SetMaxRange(maxRange);
					
					Log::Print("max range: " + to_string(pl->GetMaxRange()));
				}
				if (i->IsKeyHeld(KeyboardButton::K_ARROW_DOWN))
				{
					f32 maxRange = pl->GetMaxRange();
					maxRange -= 0.5f * deltaTime;
					
					pl->SetMaxRange(maxRange);
					
					Log::Print("max range: " + to_string(pl->GetMaxRange()));
				}
			}
		}
	}

	void GameTestInput::SetPauseState(bool newValue) { isPaused = newValue; }
	bool GameTestInput::IsPaused() { return isPaused; }

	void GameTestInput::SetMoveState(bool newValue) { canMove = newValue; }
	bool GameTestInput::CanMove() { return canMove; }
}

void TogglePause(bool pauseState)
{
	Input* i = Render::GetMainWindow().input;
	
	GameTestInput::SetPauseState(pauseState);
	
	if (GameTestInput::IsPaused())
	{
		i->SetMouseLockState(false);
		i->SetMouseVisibility(true);
		i->SetKeepMouseDeltaState(false);
		
		Log::Print(
			"set pause state to 'true'",
			"INPUT",
			LogType::LOG_DEBUG);
	}
	else
	{
		i->SetMouseLockState(true);
		i->SetMouseVisibility(false);
		i->SetKeepMouseDeltaState(true);
		
		Log::Print(
			"set pause state to 'false'",
			"INPUT",
			LogType::LOG_DEBUG);
	}
}