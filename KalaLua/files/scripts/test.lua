print("[LUA] script started")

-- ============================
-- CPP-DEFINED FUNCTIONS
-- ============================

-- functional + no args + no namespaces
cpp_hello()
-- free function + double
double_add(5.2435762457, 155.2472345734575)
-- float + parent namespace
ns_main.float_subtract(55666.4423, 4564.88)
-- int + nested namespace
ns_main.ns_nest.int_multiply(345, 5566)
-- bool + reused namespace
ns_other.ns_x.bool_value(true)
-- string + deep namespace
ns_one.ns_two.ns_three.ns_four.ns_five.string_value("this string came from lua")

-- get a response back from cpp
local result = cpp_multiply(5, 15)
print("[LUA] cpp multiplied a and b: ", result)

-- advanced lua function
advanced_lua()

-- ============================
-- LUA-DEFINED FUNCTIONS
-- ============================

-- print + no namespace
function lua_hello()
	print("[LUA] hello from lua!")
end
-- parent namespace
l1 = {} -- create namespace and overwrite if it already exists
function l1.number_add(a, b)
	local result = a + b
	print("[LUA] added number b to number a: ", result)
	return result
end
-- nested namespace
l1.l2 = l1.l2 or {} -- create namespace and reuse if it already exists
function l1.l2.number_subtract(a, b)
	local result = a - b
	print("[LUA] subtracted number b from number a: ", result)
	return result
end
-- different namespace
l3 = {}
l3.l4 = {}
function l3.l4.number_multiply(a, b)
	local result = a * b
	print("[LUA] multiplied number a by number b: ", result)
	return result
end
-- reused namespace
function l3.l4.bool_value(a)
	print("[LUA] bool value: ", a)
	return a
end
-- deep namespace
l5 = {}
l5.l6 = {}
l5.l6.l7 = {}
l5.l6.l7.l8 = {}
l5.l6.l7.l8.l9 = {}
function l5.l6.l7.l8.l9.string_value(a)
	print("[LUA] string value: ", a)
	return a
end

-- return number of two combined input numbers
function combine_numbers(a, b)
	return a + b;
end

-- return combined strings, using .. for string concatenation
function combine_strings(a, b, c, d, e)
	return a .. ", " .. b .. ", " .. c .. ", " .. d .. ", " .. e;
end

-- return true if a is bigger than b
function is_bigger(a, b)
	return a > b
end

print("[LUA] script ended")