/*
 *  LuaUtil.h
 * 
 */

#ifndef INCLUDE_MOE_LUAUTIL_H
#define INCLUDE_MOE_LUAUTIL_H

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#include <assert.h>


template<typename T>
inline T luaX_Cast(lua_State* L, int i) {
    return lua_tonumber(L, i);
}
template<>
inline std::string luaX_Cast<std::string>(lua_State* L, int i)
{
	const char* s = lua_tostring(L, i);
	if (!s)
		return std::string();
    return std::string(s);
}

template<typename T>
inline bool isLuaType(lua_State* L,int idx) {
	assert(0); // Unkown type
	return false;
}
template<> inline bool isLuaType<int>(lua_State* L, int idx)        { return lua_isnumber(L, idx); }
template<> inline bool isLuaType<std::string>(lua_State* L, int idx){ return lua_isstring(L, idx); }
template<> inline bool isLuaType<float>(lua_State* L, int idx)      { return lua_isnumber(L, idx); }
template<> inline bool isLuaType<double>(lua_State* L, int idx)     { return lua_isnumber(L, idx); }

template<typename T>
inline T eval(lua_State* L, const char* str, ...)
{
    char buf[128] = {};
    
    va_list args;
    va_start(args, str);
    vsprintf(buf, str, args);
    va_end(args);
    
    luaL_dostring(L, buf);
    T val = luaX_Cast<T>(L, -1);
    lua_pop(L, 1);
    return val;
}

template<typename T>
inline bool eval(lua_State* L, T& val, const char* str, ...)
{
    char buf[128] = {};
    
    va_list args;
    va_start(args, str);
    vsprintf(buf, str, args);
    va_end(args);
    
    luaL_dostring(L, buf);
	if (isLuaType<T>(L,-1)) {
		val = luaX_Cast<T>(L, -1);
		lua_pop(L, 1);
		return true;
	}
    lua_pop(L, 1);
    return false;
}

inline void dumpStack(lua_State* L)
{
    const int stackSize = lua_gettop(L);
    for(int i = stackSize; i >= 1; i-- ) {
        int type = lua_type(L, i);
        printf("Stack[%2d-%10s] : ", i, lua_typename(L,type) );
        switch( type ) {
            case LUA_TNUMBER:  printf("%f", lua_tonumber(L, i) ); break;
            case LUA_TBOOLEAN: printf("%s", lua_toboolean(L, i) ? "true" : "false"); break;
            case LUA_TSTRING:  printf("%s", lua_tostring(L, i) ); break;
            case LUA_TNIL:     break;
            default:           printf("%s", lua_typename(L, type)); break;
        }
        printf("\n");
    }
    printf("\n");
}
inline int getTableNum(lua_State* L, const char* tablename)
{
#if 1
	std::string buf = std::string("local n = 0; for i,v in pairs(") + std::string(tablename) + std::string(") do n=n+1 end return n;");
	luaL_dostring(L, buf.c_str());
	int n = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return n;
#else
	lua_getglobal(L, tablename);
	if (!lua_istable(L, -1))
		return 0;
	lua_pushnil(L);
	int n = 0;
	while(lua_next(L, -2) != 0){
		++n;
		lua_pop(L, 1);
	}
	return n;
#endif
}

inline int getTableValues(lua_State* L, const char* tablename, std::map<std::string,std::string>& vals)
{
	//lua_getglobal(L, tablename);
	int r = luaL_dostring(L, (std::string("return ") + tablename).c_str());
	if (!lua_istable(L, -1))
		return 0;
	lua_pushnil(L);
	while(lua_next(L, -2) != 0){
		if(lua_isstring(L, -1)){
			std::string fld = static_cast<std::string>(lua_tostring(L, -2));
			std::string str = static_cast<std::string>(lua_tostring(L, -1));
			vals[std::string(fld)] = str;
		}
		lua_pop(L, 1);
	}
	return static_cast<int>(vals.size());
}

inline lua_State* createLua()
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    return L;
}

inline bool doLua(lua_State* L, const char* lua)
{
    int r = luaL_dostring(L, lua);
    if (r){
        const char *err = lua_tostring(L, -1);
        printf("ERROR: %s\n", err);
        return false;
    }
    return true;
}

inline void closeLua(lua_State* L)
{
    lua_close(L);
}

#endif // INCLUDE_MOE_LUAUTIL_H
