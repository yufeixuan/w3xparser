#include "slk.h" 
#include "txt.h" 
#include "ini.h" 
#include "tonumber.h" 
#include "mdxopt.h" 

namespace w3x
{
	static bool is_string(std::string_view& s)
	{
		bool r = false;
		if (!s.empty() && s.front() == '"') {
			s.remove_prefix(1);
			r = true;
		}
		if (!s.empty() && s.back() == '"') {
			s.remove_suffix(1);
			r = true;
		}
		return r;
	}

	int parse_slk(lua_State* L)
	{
		slk l(L, luaL_checkstring(L, 1));
		const char* file = luaL_optstring(L, 2, "...");
		if (!l.parse()) {
			return luaL_error(L, "\n%s:%d: %s", file, (int)lua_tointeger(L, -2), lua_tostring(L, -1));
		}
		for (size_t y = 0; y < l.maxy; ++y) {
			is_string(l.col[y]);
		}
		lua_createtable(L, 0, l.maxy);
		for (size_t y = 1; y < l.maxy; ++y) {
			if (l.col[y].empty()) {
				continue;
			}
			lua_pushlstring(L, l.col[y].data(), l.col[y].size());
			lua_createtable(L, 0, l.maxx);
			for (size_t x = 1; x < l.maxx; ++x) {
				if (l.row[x].empty()) {
					continue;
				}
				lua_pushlstring(L, l.row[x].data(), l.row[x].size());
				auto& data = l.data[x][y];
				if (is_string(data)) {
					lua_pushlstring(L, data.data(), data.size());
				}
				else if (!data.empty()) {
					lua_pushlstring(L, data.data(), data.size());
					if (!lua_stringtonumber(L, lua_tostring(L, -1))) {
						lua_pushinteger(L, 0);
					}
					lua_remove(L, -2);
				}
				else {
					lua_pushnil(L);
				}
				lua_rawset(L, -3);
			}
			lua_rawset(L, -3);
		}
		return 1;
	}

	int parse_txt(lua_State* L)
	{
		txt l(L, luaL_checkstring(L, 1));
		const char* file = luaL_optstring(L, 2, "...");
		if (lua_gettop(L) < 3) {
			lua_newtable(L);
		}
		if (!l.parse()) {
			return luaL_error(L, "\n%s:%d: %s", file, (int)lua_tointeger(L, -2), lua_tostring(L, -1));
		}
		return 1;
	}

	int parse_ini(lua_State* L)
	{
		ini l(L, luaL_checkstring(L, 1));
		const char* file = luaL_optstring(L, 2, "...");
		if (!l.parse()) {
			return luaL_error(L, "\n%s:%d: %s", file, (int)lua_tointeger(L, -2), lua_tostring(L, -1));
		}
		return 1;
	}

	int tonumber(lua_State* L)
	{
		number l(L, luaL_checkstring(L, 1));
		l.parse();
		return 1;
	}

	int mdxopt(lua_State* L)
	{
		size_t len = 0;
		const char* buf = luaL_checklstring(L, 1, &len);
		char* newbuf = (char*)lua_newuserdata(L, len);
		memcpy(newbuf, buf, len);
		mdx::opt(newbuf, len);
		lua_pushlstring(L, newbuf, len);
		return 1;
	}
}

extern "C" __declspec(dllexport)
int luaopen_w3xparser(lua_State* L)
{
	static luaL_Reg l[] = {
	    { "slk", w3x::parse_slk },
		{ "txt", w3x::parse_txt },
		{ "ini", w3x::parse_ini },
		{ "tonumber", w3x::tonumber },
		{ "mdxopt", w3x::mdxopt },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
