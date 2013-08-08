-- global value
gFloat = 100.0
gInt = 50
gBool = true
gString = "test"

-- table
gTable = {
	t0 = "test",
	t1 = 23,
	t2 = 23.5,
	t3 = false,
}

-- test func
function test()
	local a = 5.5
	local b = 2.0
	local c = 2.0
	local d = 4.0
	a = ( (a + b) * c ) / d
    return a
end

-- test2 func
function test2()
	return "abcdef"
end

-- test3 func
function test3()
	return true
end

function test4()
	print( c_Func.scrCall_Test( "test" ) )
end

function test5( x0, x1, x2, x3 )
	local t0 = x0
	local t1 = tostring(x1)
	local t2 = tostring(x2)
	local t3 = x3

	print( t0 )
	print( t1 )
	print( t2 )
	print( t3 )
end
