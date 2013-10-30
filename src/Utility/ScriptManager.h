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
struct luaL_Reg;

namespace MOE {
	//==============================================================
	// ScriptManager class
	//==============================================================
	class CScriptManager
	{
	private:
		static CScriptManager m_inst;

		static const luaL_Reg m_scrcallfuncs[];

		map<string, lua_State*>	m_luastates;

		void userLibSetup( lua_State *pLuaState );
		lua_State* getLuaState( string name );
		
		void* getTableValue( lua_State *pLuaState, s32 sch_index, s32 &now_index );
		s32 getTableValueNum( lua_State *pLuaState );

		CScriptManager();

	public:
		virtual ~CScriptManager();

		//	read / release
		b8 Read( string fname, string luaname );
		void Release();

		//	stack
		b8 ExtraStack( string luaname, s32 extra_num );
		void ClearStack( string luaname );

		//==============================================================
		//	access
		//	スクリプト側にある関数実行を行った場合、lua の仕組み上、その戻り値が
		//	スタックに積まれ、スタックをクリアするまで値が残ったままになりますので、
		//	ご注意ください。(複数戻り値を指定できるためこういった状態になります)
		//	グローバル変数、テーブル用グローバル変数については、その場でスタックに
		//	値を積んだ後、値取得後にスタックから排除するので、上記の問題は出ません。
		//==============================================================

		//	lua func
		//		ExecFunc -> GetExecResultXXX
		b8 ExecFunc( string luaname, string funcname, s32 retnum = 1, const s8 *argv = NULL, ... );
		f32 GetExecFuncResultF32( string luaname, s32 retidx = 1 );
		s32 GetExecFuncResultS32( string luaname, s32 retidx = 1 );
		b8 GetExecFuncResultBool( string luaname, s32 retidx = 1 );
		string GetExecFuncResultString( string luaname, s32 retidx = 1 );

		//	lua global value
		f32 GetGlobalValueF32( string luaname, string gvalname );
		s32 GetGlobalValueS32( string luaname, string gvalname );
		b8 GetGlobalValueBool( string luaname, string gvalname );
		string GetGlobalValueString( string luaname, string gvalname );

		//	lua table
		//		単一の項目取得用(※ネストのあるテーブルには非対応)
		f32 GetTableValueF32( string luaname, string tablename, string itemname );
		s32 GetTableValueS32( string luaname, string tablename, string itemname );
		b8 GetTableValueBool( string luaname, string tablename, string itemname );
		string GetTableValueString( string luaname, string tablename, string itemname );

		//	lua table
		//		汎用型用(※ネストのあるテーブルにも対応)
		void* GetTableValue( string luaname, string tablename, s32 sch_index, s32 &now_index );
		s32 GetTableValueNum( string luaname, string tablename );

		// for debug
		void DebugStackPrint( string entryname );

		// instance
		static CScriptManager* GetInstance();
	};
 
}	// namespace MOE

#endif