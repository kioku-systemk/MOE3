/*
 *  ScriptManager.cpp
 *  ScriptManager
 *
 *  Created by GOT on 2013/08/05.
 *  Copyright 2013 Eldorado / System K. All rights reserved.
 */
//==============================================================
// header
//==============================================================
#include <map>
#include <string>
#include <lua.hpp>

#include "../Core/MOE.h"
#include "ScriptManager.h"

namespace MOE {

	//==============================================================
	// static data
	//==============================================================
	CScriptManager CScriptManager::m_inst;	// singleton

	//==============================================================
	// CScriptManager class
	//==============================================================
	CScriptManager::CScriptManager()
	{
		m_luastates.clear();
	}

	CScriptManager::~CScriptManager()
	{
	}

	lua_State* CScriptManager::getLuaState( string name )
	{
		// luaステート検索
		if( m_luastates.find( name ) != m_luastates.end() ){
			// 見つかった
			return m_luastates[name];
		}

		// 見つからない
		return NULL;
	}

	bool CScriptManager::Read( string fname, string entryname )
	{
		// luaステート作成
		lua_State *pLuaState = luaL_newstate();
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "Read:luaL_newstate==NULL" );
			return false;
		}

		// lua 標準ライブラリ設定
		luaL_openlibs( pLuaState );
		
		// luaファイル読み込み
		if( luaL_dofile( pLuaState, fname.c_str() ) ){
			// エラーだったら内容を出力し終了
			MOELogI( "Read:%s", lua_tostring( pLuaState, lua_gettop( pLuaState ) ) );
			lua_close( pLuaState );
			return false;
		}

		// luaステートを登録
		m_luastates[entryname] = pLuaState;

		return true;
	}

	void CScriptManager::Release()
	{
		// luaステート全クローズ
		map<string, lua_State*>::iterator it;
		for( it = m_luastates.begin(); it != m_luastates.end(); it++ ){
			lua_State *pLuaState = it->second;
			lua_close( pLuaState );
		}

		// lua保持していたマップテンプレートもクリア
		m_luastates.clear();
	}

	bool CScriptManager::ExecFunc( string luaname, string funcname, int retnum )
	{
		// memo:
		// luaベースの関数実行を行います。
		// lua内で関数実行後、C側で値を取得できます。
		// ※ 関数実行後、別な関数を実行する前に必ず ExecFuncResultClear() を呼び出してください

		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFunc:luaL_newstate==NULL" );
			return false;
		}

		// スクリプト内関数名をスタックに積む
		lua_getglobal( pLuaState, funcname.c_str() );
		// lua側関数実行(引数はなし、返却値はユーザー指定)
		if( lua_pcall( pLuaState, 0, retnum, 0 ) != 0 ){
			MOELogE( "ExecFunc:%s:%s function call error.", luaname.c_str(), funcname.c_str() );
			return false;
		}

		return true;
	}

	float CScriptManager::GetExecFuncResultF32( string luaname, int retidx )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFuncResultF32:luaL_newstate==NULL" );
			return false;
		}

		// 返却されたスタックから値を検索
		const int ret_num = lua_gettop( pLuaState );
		for( int i = ret_num; i < 0; i-- ){
			if( i == retidx && lua_type( pLuaState, i ) == LUA_TNUMBER ){
				// 見つかった
				return static_cast<float>( lua_tonumber( pLuaState, i ) );
			}
		}

		// 見つからない
		return 0.0f;
	}

	int CScriptManager::GetExecFuncResultS32( string luaname, int retidx )
	{
		return static_cast<int>( GetExecFuncResultF32( luaname, retidx ) );
	}

	string CScriptManager::GetExecFuncResultString( string luaname, int retidx )
	{
		string str;

		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFuncResultF32:luaL_newstate==NULL" );
			return false;
		}

		// 返却されたスタックから値を検索
		const int ret_num = lua_gettop( pLuaState );
		for( int i = ret_num; i < 0; i-- ){
			if( i == retidx && lua_type( pLuaState, i ) == LUA_TSTRING ){
				// 見つかった
				str += lua_tostring( pLuaState, i );
				break;
			}
		}

		return str;
	}

	void CScriptManager::ExecFuncResultClear( string luaname )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFuncResultClear:luaL_newstate==NULL" );
			return;
		}

		// スタックをクリア
		lua_settop( pLuaState, 0 );
	}

	CScriptManager* CScriptManager::GetInstance()
	{
		return &m_inst;
	}
	
	void CScriptManager::DebugStackPrint( string entryname )
	{
		lua_State *pLuaState = getLuaState( entryname );
		if( pLuaState ){
			const int num = lua_gettop( pLuaState );
			if (num == 0) {
				MOELogI("No stack.");
				return;
			}
			for (int i = num; i >= 1; i--) {
				MOELogI( "%03d(%04d): ", i, -num + i - 1 );
				int type = lua_type( pLuaState, i );
				switch( type ){
				case LUA_TNIL:
					MOELogI( "NIL" );
					break;
				case LUA_TBOOLEAN:
					MOELogI( "BOOLEAN %s", lua_toboolean( pLuaState, i ) ? "true" : "false" );
					break;
				case LUA_TLIGHTUSERDATA:
					MOELogI( "LIGHTUSERDATA" );
					break;
				case LUA_TNUMBER:
					MOELogI( "NUMBER %f", lua_tonumber( pLuaState, i ) );
					break;
				case LUA_TSTRING:
					MOELogI( "STRING %s", lua_tostring( pLuaState, i ) );
					break;
				case LUA_TTABLE:
					MOELogI( "TABLE" );
					break;
				case LUA_TFUNCTION:
					MOELogI( "FUNCTION" );
					break;
				case LUA_TUSERDATA:
					MOELogI( "USERDATA" );
					break;
				case LUA_TTHREAD:
					MOELogI( "THREAD" );
					break;
				}
			}
			MOELogI("-----------------------------\n");
		}
	}

}	// namespace MOE