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

		// read / release
		bool Read( string fname, string entryname );
		void Release();

		// access
		// ※ ExecFunc -> GetExecResultXXX -> ExecFuncResultClear
		bool ExecFunc( string luaname, string funcname, int retnum );
		float GetExecFuncResultF32( string luaname, int retidx );
		int GetExecFuncResultS32( string luaname, int retidx );
		string GetExecFuncResultString( string luaname, int retidx );
		void ExecFuncResultClear( string luaname );

		// for debug
		void DebugStackPrint( string entryname );

		// instance
		static CScriptManager* GetInstance();
	};
 
}	// namespace MOE

#endif