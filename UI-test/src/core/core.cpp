//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>

#include "KalaHeaders/log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "graphics/kw_window_global.hpp"

#include "core/ku_core.hpp"

#include "core/core.hpp"
#include "graphics/render.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaUI::Core::KalaUICore;

using UITest::Graphics::Render;

using std::string;

#ifdef ENABLE_EMIT_LOG
void KalaHeaders::KalaLog::EmitLog(string_view msg)
{
	CrashHandler::AppendToCrashLog(msg);
}
#endif

namespace UITest::Core
{
	void UITestCore::Initialize()
	{
		CrashHandler::Initialize(
			"UI test",
			Shutdown);

		KalaWindowCore::SetUserShutdownFunction(Shutdown);
		
		Render::Initialize();
	}
	
	void UITestCore::Update()
	{
		while (true)
		{
			KalaWindowCore::UpdateDeltaTime();
			Render::Update();
		}
	}
	
	void UITestCore::Shutdown()
	{
		Log::Print(
			"Shutting down UI test...",
			"CORE",
			LogType::LOG_INFO);

		KalaUICore::CleanAllResources();
	}
}