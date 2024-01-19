-- put utility functions to name space 'luaaa'
luaaa = {}

-- create subclass for base, obj can be exist table or nil
function luaaa:extend(base, obj)
	derived = obj or {}
	derived.new = function(self, ...)
		-- in next version, below line will be changed to 'o = base:new(...)'
		o = base.new(...)
		setmetatable(self, getmetatable(o))
		self["@"] = o
		return self
	end
	return derived
end

-- get base class of obj
function luaaa:base(obj)
	if (type(obj) == "table") then
		return obj["@"]
	end
	return nil
end