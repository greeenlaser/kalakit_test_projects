//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>

#include "KalaHeaders/log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "graphics/kw_window_global.hpp"

#include "core/ku_core.hpp"

#include "core/kp_core.hpp"

#include "core/ka_core.hpp"

#include "core/core.hpp"
#include "core/input.hpp"
#include "graphics/render.hpp"
#include "graphics/opengl_texture.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/opengl_model.hpp"
#include "gameobject/opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::CrashHandler;
using KalaWindow::Core::KalaWindowCore;

using KalaUI::Core::KalaUICore;

using KalaPhysics::Core::KalaPhysicsCore;

using KalaAudio::Core::KalaAudioCore;

using GameTest::Graphics::Render;
using GameTest::Graphics::OpenGL_Texture;
using GameTest::GameObject::Camera;
using GameTest::GameObject::OpenGL_Model;
using GameTest::GameObject::OpenGL_PointLight;

using std::cin;
using std::string;

namespace GameTest::Core
{
	void GameTestCore::Initialize()
	{
		Log::Print(
			"initializing core...",
			"CORE",
			LogType::LOG_DEBUG);
		
		CrashHandler::Initialize("game test", Shutdown);
		
		KalaWindowCore::SetUserShutdownFunction(Shutdown);
		
		Render::Initialize();
		
		Update();
	}

	void GameTestCore::SyncID()
	{
		u32 highest = KalaWindowCore::GetGlobalID();

		//fix individual desync

		if (KalaUICore::GetGlobalID() > highest)      highest = KalaUICore::GetGlobalID();
		if (KalaPhysicsCore::GetGlobalID() > highest) highest = KalaPhysicsCore::GetGlobalID();
		if (KalaAudioCore::GetGlobalID() > highest)   highest = KalaAudioCore::GetGlobalID();

		//bump all to new highest value

		KalaUICore::SetGlobalID(highest);
		KalaPhysicsCore::SetGlobalID(highest);
		KalaAudioCore::SetGlobalID(highest);

		//and finally sync window id

		KalaWindowCore::SetGlobalID(highest);
	}
	
	void GameTestCore::Update()
	{
		Log::Print(
			"starting update loop...",
			"CORE",
			LogType::LOG_DEBUG);
			
		//force-enable move for now
		GameTestInput::SetMoveState(true);
		
		while (true)
		{
			KalaWindowCore::UpdateDeltaTime();
			Render::Update();
		}
	}
	
	void GameTestCore::Shutdown()
	{
		Log::Print(
			"Shutting down game...",
			"CORE",
			LogType::LOG_INFO);
		
		Camera::GetRegistry().RemoveAllContent();
		OpenGL_Model::GetRegistry().RemoveAllContent();
		OpenGL_PointLight::GetRegistry().RemoveAllContent();

		OpenGL_Texture::GetRegistry().RemoveAllContent();
		
		KalaUICore::CleanAllResources();
		KalaPhysicsCore::CleanAllResources();
		KalaAudioCore::CleanAllResources();
	}
}