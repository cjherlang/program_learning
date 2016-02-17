--[[
function fact( n )
	if n == 0 then
		return 1
	else
		return n * fact(n -1)
	end
end

print("enter a num:")
a = io.read("*num")
print(fact(a))
--]]
print("10" + 1)
print(10 .. 20)

line = "hello"
n = tonumber(line)
if n == nil then
	print(line .. " is not num")
	print(line .. " lenth " .. #line)
else
	print(n * 2)
end

print("table test:")
a = {
   "11sdfsdf",
   2,
   3,
   4,
}
a[3] = 3
a[8] = 8
a[9] = 9
a[10] = 10
a[0] = 0
a["1"] = 11
a["2"] = 12
a["a"] = 13
a["4"] = 14

for key, value in pairs(a) do
	if(key == 1) then
		print(a[key])
	end
end

print("table a length :" .. #a)
print("next key value: ")
print(next(a, "1"))


function add( ... )
	local  s = 0
	for i,v in ipairs {...} do
		s = s + v
	end
	return s
end

print(add(1, 2))
print(add(1, 22, 3))