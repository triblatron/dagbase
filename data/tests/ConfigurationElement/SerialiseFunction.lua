root=
{
	foo=function ()
		print("test")
	end,
	bar=function ()		
		print("test")
	end,
}

function foo()
	print("test")
end

function bar()
	print("test")
end

print(foo==bar)

--print(root.foo==root.bar)
--print(rawequal(root.foo,root.bar))
--root.foo()
--root.bar()
