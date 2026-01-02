print("[LUA] script started")

-- functions that originate from the kalalua test program

-- no namespace
cpp_hello()
-- parent namespace
ns_main.double_add(5.2435762457, 155.2472345734575)
-- nested namespace
ns_main.ns_nest.float_subtract(55666.4423, 4564.88)
-- different namespace
ns_other.ns_x.int_multiply(345, 5566)
-- reused namespace
ns_other.ns_x.bool_value(true)
-- deep namespace
ns_one.ns_two.ns_three.ns_four.ns_five.string_value("this string came from lua")

-- functions that originate from lua

-- no namespace
function lua_hello()
	print("[LUA] hello from lua!")
end
-- parent namespace
l1 = {} -- create namespace and overwrite if it already exists
function l1.double_add(a, b)
	local result = a + b
	print("[LUA] added double b to double a: ", result)
	return result
end
-- nested namespace
l1.l2 = l1.l2 or {} -- create namespace and reuse if it already exists
function l1.l2.float_subtract(a, b)
	local result = a - b
	print("[LUA] subtracted float b from float a: ", result)
	return result
end
-- different namespace
l3 = {}
l3.l4 = {}
function l3.l4.int_multiply(a, b)
	local result = a * b
	print("[LUA] multiplied int a by int b: ", result)
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

print("[LUA] script ended")