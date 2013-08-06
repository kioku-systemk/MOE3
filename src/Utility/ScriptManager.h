/*
 *  ScriptManager.h
 *  ScriptManager
 *
 *  Created by GOT on 2013/08/05.
 *  Copyright 2013 Eldorado / System K. All rights reserved.
 */
#ifndef INCLUDE_SCRIPTMANAGER_H
#define INCLUDE_SCRIPTMANAGER_H

using namespace std;

struct lua_State;

namespace MOE {
	//==============================================================
	// ScriptManager class
	//==============================================================
	class CScriptManager
	{
	private:
		static CScriptManager m_inst;

		map<string, lua_State*>	m_luastates;

		lua_State* getLuaState( string name );

		CScriptManager();

	public:
		virtual ~CScriptManager();

		//	read / release
		b8 Read( string fname, string luaname );
		void Release();

		//	stack
		b8 ExtraStack( string luaname, s32 extra_num );
		void ClearStack( string luaname );

		//	access
		//	スクリプト側にある関数実行を行った場合、lua の仕組み上、その戻り値が
		//	スタックに積まれ、スタックをクリアするまで値が残ったままになりますので、
		//	ご注意ください。(複数戻り値を指定できるためこういった状態になります)
		//	グローバル変数、テーブル用グローバル変数については、その場でスタックに
		//	値を積んだ後、値取得後にスタックから排除するので、上記の問題は出ません。

		//	lua func
		//		ExecFunc -> GetExecResultXXX
		b8 ExecFunc( string luaname, string funcname, s32 retnum = 1 );
		f32 GetExecFuncResultF32( string luaname, s32 retidx = 1 );
		s32 GetExecFuncResultS32( string luaname, s32 retidx = 1 );
		b8 GetExecFuncResultBool( string luaname, s32 retidx = 1 );
		string GetExecFuncResultString( string luaname, s32 retidx = 1 );

		//	lua global value
		//		GetGlobalValueXXX
		f32 GetGlobalValueF32( string luaname, string gvalname );
		s32 GetGlobalValueS32( string luaname, string gvalname );
		b8 GetGlobalValueBool( string luaname, string gvalname );
		string GetGlobalValueString( string luaname, string gvalname );

		//	lua table
		//		GetTableValueXXX
		f32 GetTableValueF32( string luaname, string tablename, string itemname );
		s32 GetTableValueS32( string luaname, string tablename, string itemname );
		b8 GetTableValueBool( string luaname, string tablename, string itemname );
		string GetTableValueString( string luaname, string tablename, string itemname );

		// for debug
		void DebugStackPrint( string entryname );

		// instance
		static CScriptManager* GetInstance();
	};
 
}	// namespace MOE

#endif