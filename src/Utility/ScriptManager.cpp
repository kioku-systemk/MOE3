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

	b8 CScriptManager::Read( string fname, string entryname )
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
		// c 拡張ライブラリ設定
		userLibSetup( pLuaState );
		
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

	b8 CScriptManager::ExtraStack( string luaname, s32 extra_num )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFunc:luaL_newstate==NULL" );
			return false;
		}

		// stack 拡張
		if( lua_checkstack( pLuaState, extra_num ) == 0 ){
			MOELogE( "Error: stack allocation" );
			return false;
		}

		return true;
	}

	void CScriptManager::ClearStack( string luaname )
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

	bool CScriptManager::ExecFunc( string luaname, string funcname, s32 retnum )
	{
		// memo:
		// luaベースの関数実行を行います。
		// lua内で関数実行後、C側で値を取得できます。
		// ※ 関数実行後、別な関数を実行する前に必ず ClearStack() を呼び出してください

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

	f32 CScriptManager::GetExecFuncResultF32( string luaname, s32 retidx )
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
		for( int i = ret_num; i >= 0; i-- ){
			if( i == retidx && lua_type( pLuaState, i ) == LUA_TNUMBER ){
				// 見つかった
				return static_cast<f32>( lua_tonumber( pLuaState, i ) );
			}
		}

		// 見つからない
		return 0.0f;
	}

	s32 CScriptManager::GetExecFuncResultS32( string luaname, s32 retidx )
	{
		return static_cast<s32>( GetExecFuncResultF32( luaname, retidx ) );
	}

	b8 CScriptManager::GetExecFuncResultBool( string luaname, s32 retidx )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFuncResultBool:luaL_newstate==NULL" );
			return false;
		}

		// 返却されたスタックから値を検索
		const s32 ret_num = lua_gettop( pLuaState );
		for( s32 i = ret_num; i >= 0; i-- ){
			if( i == retidx && lua_type( pLuaState, i ) == LUA_TBOOLEAN ){
				// 見つかった
				return ( lua_toboolean( pLuaState, i ) )? true : false;
			}
		}

		// 見つからない
		return false;
	}

	string CScriptManager::GetExecFuncResultString( string luaname, s32 retidx )
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
		const s32 ret_num = lua_gettop( pLuaState );
		for( s32 i = ret_num; i >= 0; i-- ){
			if( i == retidx && lua_type( pLuaState, i ) == LUA_TSTRING ){
				// 見つかった
				str += lua_tostring( pLuaState, i );
				break;
			}
		}

		return str;
	}

	f32 CScriptManager::GetGlobalValueF32( string luaname, string gvalname )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetGlobalValueF32:luaL_newstate==NULL" );
			return false;
		}

		// グローバル変数をスタックに積む
		lua_getglobal( pLuaState, gvalname.c_str() );
		// 種類チェック
		if( ! lua_isnumber( pLuaState, -1 ) ){
			MOELogE( "GetGlobalValueF32:is not number" );
			return 0.0f;
		}

		// 見つかった
		f32 ret = static_cast<f32>( lua_tonumber( pLuaState, -1 ) );

		// スタックをポップ
		lua_pop( pLuaState, 1 );

		return ret;
	}

	s32 CScriptManager::GetGlobalValueS32( string luaname, string gvalname )
	{
		return static_cast<s32>( GetGlobalValueF32( luaname, gvalname ) );
	}

	b8 CScriptManager::GetGlobalValueBool( string luaname, string gvalname )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetGlobalValueBool:luaL_newstate==NULL" );
			return false;
		}

		// グローバル変数をスタックに積む
		lua_getglobal( pLuaState, gvalname.c_str() );

		// 種類チェック
		if( ! lua_isboolean( pLuaState, -1 ) ){
			MOELogE( "GetGlobalValueBool:is not boolean" );
			return false;
		}

		// 見つかった
		b8 ret = ( lua_toboolean( pLuaState, -1 ) )? true : false;

		// スタックをポップ
		lua_pop( pLuaState, 1 );

		return ret;
	}

	string CScriptManager::GetGlobalValueString( string luaname, string gvalname )
	{
		string str;

		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFuncResultF32:luaL_newstate==NULL" );
			return false;
		}

		// グローバル変数をスタックに積む
		lua_getglobal( pLuaState, gvalname.c_str() );

		// 種類チェック
		if( ! lua_isstring( pLuaState, -1 ) ){
			MOELogE( "GetGlobalValueBool:is not boolean" );
			return str;
		}

		// 見つかった
		str += lua_tostring( pLuaState, -1 );

		// スタックをポップ
		lua_pop( pLuaState, 1 );

		return str;
	}

	f32 CScriptManager::GetTableValueF32( string luaname, string tablename, string itemname )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetTableValueF32:luaL_newstate==NULL" );
			return false;
		}

		// グローバル変数をスタックに積む
		lua_getglobal( pLuaState, tablename.c_str() );
		// テーブルチェック
		if( ! lua_istable( pLuaState, -1 ) ){
			MOELogE( "GetTableValueF32:is not table data" );
			return 0.0f;
		}

		// テーブルの指定項目をスタックに積む
		lua_pushstring( pLuaState, itemname.c_str() );
		lua_gettable( pLuaState, -2 );
		// 数値チェック
		if( ! lua_isnumber( pLuaState, -1 ) ){
			MOELogE( "GetTableValueF32:is not number" );
			return 0.0f;
		}

		// 値取得
		f32 ret = static_cast<f32>( lua_tonumber( pLuaState, -1 ) );

		// スタックをポップ : table & value
		lua_pop( pLuaState, 2 );

		return ret;
	}

	s32 CScriptManager::GetTableValueS32( string luaname, string tablename, string itemname )
	{
		return static_cast<s32>( GetTableValueF32( luaname, tablename, itemname ) );
	}

	b8 CScriptManager::GetTableValueBool( string luaname, string tablename, string itemname )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetTableValueF32:luaL_newstate==NULL" );
			return false;
		}

		// グローバル変数をスタックに積む
		lua_getglobal( pLuaState, tablename.c_str() );
		// テーブルチェック
		if( ! lua_istable( pLuaState, -1 ) ){
			MOELogE( "GetTableValueF32:is not table data" );
			return false;
		}

		// テーブルの指定項目をスタックに積む
		lua_pushstring( pLuaState, itemname.c_str() );
		lua_gettable( pLuaState, -2 );
		// 数値チェック
		if( ! lua_isboolean( pLuaState, -1 ) ){
			MOELogE( "GetTableValueBool:is not boolean" );
			return false;
		}

		// 値取得
		b8 ret = ( lua_toboolean( pLuaState, -1 ) ) ? true : false;

		// スタックをポップ : table & value
		lua_pop( pLuaState, 2 );

		return ret;
	}

	string CScriptManager::GetTableValueString( string luaname, string tablename, string itemname )
	{
		string ret;

		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetTableValueF32:luaL_newstate==NULL" );
			return ret;
		}

		// テーブルをスタックに積む
		lua_getglobal( pLuaState, tablename.c_str() );
		// テーブルチェック
		if( ! lua_istable( pLuaState, -1 ) ){
			MOELogE( "GetTableValueF32:is not table data" );
			return ret;
		}

		// テーブルの指定項目をスタックに積む
		lua_pushstring( pLuaState, itemname.c_str() );
		lua_gettable( pLuaState, -2 );
		// 数値チェック
		if( ! lua_isstring( pLuaState, -1 ) ){
			MOELogE( "GetTableValueBool:is not string" );
			return ret;
		}

		// 値取得
		ret += lua_tostring( pLuaState, -1 );

		// スタックをポップ : table & value
		lua_pop( pLuaState, 2 );

		return ret;
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