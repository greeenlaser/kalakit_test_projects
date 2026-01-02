//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <functional>
#include <filesystem>
#include <string>

#include "core/kl_lua.hpp"

using KalaLua::Core::Lua;

using std::cout;
using std::cin;
using std::function;
using std::filesystem::current_path;
using std::filesystem::path;
using std::string;
using std::string_view;
using std::to_string;
using std::setprecision;

static void Double_Add(double a, double b)
{
	double result = a + b;
	cout << "[CPP] added double b to double a: '" << setprecision(15) << result << "'\n";
}
static void Float_Subtract(float a, float b)
{
	float result = a - b;
	cout << "[CPP] subtracted float b from float a: '" << setprecision(6) << result << "'\n";
}
static void Int_Multiply(int a, int b)
{
	int result = a * b;
	cout << "[CPP] multiplied int a by int b: '" << setprecision(0) << result << "'\n";
}
static void Bool_Value(bool a)
{
	string value = a ? "true" : "false";

	cout << "[CPP] bool value: '"  << value << "'\n";
}
static void String_Value(string a)
{
	cout << "[CPP] string value: '" << a << "'\n";
}

int main()
{
	Lua::Initialize();

	//test functional + no args + no namespaces
	Lua::RegisterFunction(
		"cpp_hello",
		"",
		function<void()>([]() { cout << "[CPP] hello from cpp!\n"; }));

	//test lambda + double + parent namespace.
	//note: lambdas without args are not supported in KalaLua,
	//functionals and free functions can be ran with no arguments
	Lua::RegisterFunction<double, double>(
		"double_add",
		"ns_main",
		[](double a, double b) 
			{ 
				Double_Add(a, b); 
			});

	//test free function + float + nested namespace
	Lua::RegisterFunction(
		"float_subtract",
		"ns_main.ns_nest",
		Float_Subtract);

	//test int + different namespace
	Lua::RegisterFunction(
		"int_multiply",
		"ns_other.ns_x",
		Int_Multiply);

	//test bool + reused namespace
	Lua::RegisterFunction(
		"bool_value",
		"ns_other.ns_x",
		Bool_Value);

	//test string + deep namespace
	Lua::RegisterFunction(
		"string_value",
		"ns_one.ns_two.ns_three.ns_four.ns_five",
		String_Value);

	//test advanced lua function
	Lua::RegisterFunction(
		"advanced_lua",
		"",
		function<int(lua_State*)>(
			[](lua_State* L) -> int
			{
				cout << "[CPP] hello from advanced lua function\n";
				return 0;
			}));

	//load a lua script
	Lua::LoadScript(
		{
			path(current_path() / "files" / "scripts" / "test.lua").string()
		});

	//test lua functions with the same structure

	Lua::CallFunction("lua_hello",      "");
	Lua::CallFunction("double_add",     "l1",             { 53546750.534674, 1834570.56354675 });
	Lua::CallFunction("float_subtract", "l1.l2",          { 550.4f, 180.565f });
	Lua::CallFunction("int_multiply",   "l3.l4",          { 35467, 576 });
	Lua::CallFunction("bool_value",     "l3.l4",          { false });
	Lua::CallFunction("string_value",   "l5.l6.l7.l8.l9", { "this string came from cpp" });

	//test returnable int, return 0 on failure
	auto intResult = Lua::CallFunction<int>(
		"combine_numbers", 
		"", 
		{ 50, 250 }).value_or(0);

	cout << "[CPP] added two ints together and lua returned: '" << intResult << "'\n";

	//test returnable string, return error string on failure
	auto strResult = Lua::CallFunction<string>(
		"combine_strings",
		"",
		{ "string 1", "string 2", "string 3", "string 4", "string 5"}).value_or("<STRING CONCAT ERROR>");

	cout << "[CPP] added five strings together and lua returned: '" << strResult << "'\n";

	//test returnable bool, return false on failure
	auto boolResult = Lua::CallFunction<bool>(
		"is_bigger",
		"",
		{ 35, 15 }).value_or(false);

	string_view boolResultValue = boolResult ? "true" : "false";
	cout << "[CPP] compared a against b and lua returned: '" << boolResultValue << "'\n";

	cout << "ready...\n";

	cin.get();

	cout << "shutting down...\n";

	Lua::Shutdown();

	return 0;
}