#pragma once
#ifndef __aod_project_lua_h_
#define __aod_project_lua_h_
#include <vector>
struct LuaBindable {
	static inline std::vector<LuaBindable*> bindables{};
	virtual void Bind() { return; };
	explicit LuaBindable() {
		bindables.push_back(this);
	}
};
// Alias for LuaBindable
typedef LuaBindable luab;

#endif