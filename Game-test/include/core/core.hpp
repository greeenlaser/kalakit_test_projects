//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace GameTest::Core
{
	class GameTestCore
	{
	public:
		static void Initialize();

		//Synchronizes kalaui, kalaphysics, kalaaudio id to current kalawindow id
		static void SyncID();
		
		static void Update();
		
		static void Shutdown();
	};
}