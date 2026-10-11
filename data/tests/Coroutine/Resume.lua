require "etc/commands"
require "luadag"

function test()
	return 1
end

function test2(self)
	addPort("spoo", luadag.PortDirection_DIR_INTERNAL, luadag.Port_FLAGS_NONE, 1.0)
end
