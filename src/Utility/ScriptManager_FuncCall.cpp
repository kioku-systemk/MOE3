/*
 *  ScriptManager_FuncCall.cpp
 *  ScriptManager
 *
 *  Created by GOT / Eldorado on 2013/08/07.
 *  Copyright 2013 Eldorado / System K. All rights reserved.
 */
//==============================================================
// header
//==============================================================
#include <map>
#include <string>
#include <sstream>
#include <lua/lua.hpp>

#include "../Core/MOE.h"
#include "ScriptManager.h"

namespace MOE {

	//==============================================================
	// Memo:
	//	こちらには、 C 側から lua 側に提供するシステムコールが記載されています。
	//	ここで記載した関数は、lua 側で命令として呼び出すことが可能です。
	//	lua 側としては、
	//
	//		c_Func.XXXXX(XXXXX=システムコール命令名)
	//
	//	と記載することで、該当関数を呼び出せれます。
	//	拡張する場合は、m_scrcallfuncs に lua で使用する関数を記載し、
	//	関数内容を、スタティック関数として登録して下さい。
	//	関数内での lua への伝達はスタックを用いますが、このスタックは、
	//	lua 側で勝手にポップするので、アプリ側で制御する必要はありません。
	//==============================================================

	//==============================================================
	// C Entry Function
	//==============================================================
	int scrCall_Test( lua_State *pLuaState )
	{
		// lua から与えられたデータ(文字列)を取得
		const char *s = luaL_checkstring( pLuaState, 1 );

		// s -> <test>s</test> に変換
		std::stringstream ss;
		ss << "<test>" << s << "</test>";

		// lua に編集後のデータ(文字列)を受け渡し
		lua_pushstring( pLuaState, ss.str().c_str() );

		return 1;
	}

	//==============================================================
	// C Entry Function Table
	//==============================================================
	const luaL_Reg CScriptManager::m_scrcallfuncs[] = {
		//	lua 側命令名	C 側関数名
		{	"scrCall_Test",	scrCall_Test	},
		{	NULL,			NULL			}
	};




	//==============================================================
	// ScriptManager class userLibSetup Section
	//==============================================================
	void CScriptManager::userLibSetup( lua_State *pLuaState )
	{
#if LUA_VERSION_NUM >= 502	// lua 5.2 以降
		// c 側拡張ライブラリ追加
		luaL_newlib( pLuaState, m_scrcallfuncs );
		lua_setglobal( pLuaState, "c_Func" );
#else						// lua 5.2 以前
		// c 側拡張ライブラリ追加
		luaL_register( pLuaState, "c_Func", m_scrcallfuncs );
#endif
	}

}
