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
#include <lua/lua.hpp>

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

	b8 CScriptManager::Read( string fname, string luaname )
	{
		// lua ステートが存在しているか見る
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// luaステート作成
			pLuaState = luaL_newstate();
			if( ! pLuaState ){
				// エラーだったら内容を出力し終了
				MOELogE( "Read:luaL_newstate==NULL" );
				return false;
			}

			// lua 標準ライブラリ設定
			luaL_openlibs( pLuaState );
			// c 拡張ライブラリ設定
			userLibSetup( pLuaState );
		}
		
		// luaファイル読み込み
		if( luaL_dofile( pLuaState, fname.c_str() ) ){
			// エラーだったら内容を出力し終了
			MOELogI( "Read:%s", lua_tostring( pLuaState, lua_gettop( pLuaState ) ) );
			lua_close( pLuaState );
			return false;
		}

		// luaステートを登録
		m_luastates[luaname] = pLuaState;

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

	b8 CScriptManager::ExecFunc( string luaname, string funcname, s32 retnum, const s8 *argv, ... )
	{
		// memo:
		// c 側から lua 側の関数を実行します。
		// 引数を与える場合は、argv以降でフォーマット、引数内容を設定してください。
		// argv は "%s%c%d%f" のような感じで指定します。
		// 内訳は以下の通り。
		//	%s = 文字列
		//	%c = 文字
		//	%d = 整数
		//	%f = 小数

		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "ExecFunc:luaL_newstate==NULL" );
			return false;
		}

		// スクリプト内関数名をスタックに積む
		lua_getglobal( pLuaState, funcname.c_str() );

		// 引数取得及び lua 側に伝達
		int argnum = 0;
		if( argv ){
			va_list list;
			int i;
			char *p;

			va_start( list,  argv );
			for( p = (char *)argv, i = 0; *p != '\0'; p++, i++ ){
				if( p[0] == '%' ){
					if( p[1] == 's' ){
						// s8*
						s8 *param = va_arg( list, s8* );
						lua_pushstring( pLuaState, param );
						argnum++;
					}else if( p[1] == 'c' ){
						// s8
						s8 param[2];
						memset( param, '\0', sizeof(param) );
						param[0] = static_cast<s8>( va_arg( list, s32 ) );
						lua_pushstring( pLuaState, param );
						argnum++;
					}else if( p[1] == 'd' ){
						// s32
						s32 param = va_arg( list , s32 );
						lua_pushnumber( pLuaState, param );
						argnum++;
					}else if( p[1] == 'f' ){
						// f32
						f32 param = static_cast<f32>( va_arg( list , f64 ) );
						lua_pushnumber( pLuaState, param );
						argnum++;
					}
				}
			}
			va_end( list );
		}

		// lua側関数実行(引数はなし、返却値はユーザー指定)
		if( lua_pcall( pLuaState, argnum, retnum, 0 ) != 0 ){
			MOELogE( "ExecFunc:%s:%s function call error.", luaname.c_str(), funcname.c_str() );
			return false;
		}

		// 引数のスタックをポップ
		if( argnum > 0 ){
			lua_pop( pLuaState, 1 );
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
			return str;
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
			return str;
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

	void* CScriptManager::getTableValue( lua_State *pLuaState, s32 sch_index, s32 &now_index )
	{
		s32 index = now_index;
		void *param = NULL;
		
		if( ! pLuaState ){
			return NULL;
		}
		
		// 最初のキーはnil登録
		lua_pushnil(pLuaState);
		
		// その項目の取得が終了するまで
		while(lua_next(pLuaState, -2) != 0){
			// 該当する値か
			if( sch_index == index ){
				// 値取得
				if(lua_isstring(pLuaState, -1)){
					std::string string = static_cast<std::string>(lua_tostring(pLuaState, -1));
					param = (void*)(string.c_str());
				}else if(lua_isnumber(pLuaState, -1)){
					f32 fparam = static_cast<f32>(lua_tonumber(pLuaState, -1));
					param = static_cast<void*>(&fparam);
				}else if(lua_isboolean(pLuaState, -1)){
					int bparam = lua_toboolean(pLuaState, -1);
					param = static_cast<void*>(&bparam);
				}
				lua_pop(pLuaState, 1);
				break;
			}
			
			// 項目はテーブルか？
			if(lua_istable(pLuaState, -1)){
				// テーブル解析
				param = getTableValue(pLuaState,sch_index,index);
				if( param != NULL ){
					// パラメータを取得していたら終了
					lua_pop(pLuaState, 1);
					break;
				}
			}
			index++;

			lua_pop(pLuaState, 1);
		}
		
		// 現在のインデックス値を反映
		now_index = index;
		
		return param;
	}
	
	void* CScriptManager::GetTableValue( string luaname, string tablename, s32 sch_index, s32 &now_index )
	{
		s32 index = now_index;
		void *param = NULL;
		
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetTableValueF32:luaL_newstate==NULL" );
			return false;
		}

		// テーブル内を検索し、指定されたインデックスの値を取得
		param = getTableValue( pLuaState, sch_index, index );

		return param;
	}
	
	s32 CScriptManager::getTableValueNum( lua_State *pLuaState )
	{
		s32 num = 0;
		
		if( ! pLuaState ){
			return NULL;
		}
		
		// 最初のキーはnil登録
		lua_pushnil(pLuaState);
		
		// その項目の取得が終了するまで
		while(lua_next(pLuaState, -2) != 0){
			// 項目はテーブルか？
			if(lua_istable(pLuaState, -1)){
				// テーブル解析し、テーブル内の総数を加算
				num += getTableValueNum(pLuaState);
			// それ以外
			}else{
				// 値だったら加算
				if(lua_isstring(pLuaState, -1)){
					num++;
				}else if(lua_isnumber(pLuaState, -1)){
					num++;
				}else if(lua_isboolean(pLuaState, -1)){
					num++;
				}
			}

			lua_pop(pLuaState, 1);
		}
		
		return num;
	}
	
	s32 CScriptManager::GetTableValueNum( string luaname, string tablename )
	{
		// luaステート取得
		lua_State *pLuaState = getLuaState( luaname );
		if( ! pLuaState ){
			// エラーだったら内容を出力し終了
			MOELogE( "GetTableValueF32:luaL_newstate==NULL" );
			return false;
		}

		// テーブル内を検索し、項目数(テーブルを抜かす)を取得
		return getTableValueNum( pLuaState );
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