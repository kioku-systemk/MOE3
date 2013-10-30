/*
 *  SceneScriptLoader.cpp
 *  SceneScriptLoader
 *
 *  Created by GOT on 2013/10/07.
 *  Copyright 2013 Eldorado / System K. All rights reserved.
 */

//==============================================================
// header
//==============================================================
#include <map>
#include <string>

#include "../Core/MOE.h"
#include "ScriptManager.h"
#include "SceneScriptLoader.h"

namespace {
	// レコード長
	const s32 _SCENESCRIPT_BUFFER_RECNUM = 5;	// Buffer
	const s32 _SCENESCRIPT_SCENE_RECNUM = 2;	// Scene
	const s32 _SCENESCRIPT_PROCESS_RECNUM = 5;	// Process
	const s32 _SCENESCRIPT_RENDER_RECNUM = 6;	// Render(@todo:本当に6かどうか、あのフォーマットだと不明)
}

namespace MOE {

	//==============================================================
	// SceneScriptLoader
	//==============================================================
	// Constractor / Destractor
	SceneScriptLoader::SceneScriptLoader( MOE::CScriptManager *sm )
	: m_sm( sm )
	{
		m_luaname.clear();
		m_buffers.clear();
		m_scenes.clear();
		m_proceses.clear();
		m_renders.clear();
	}

	SceneScriptLoader::~SceneScriptLoader()
	{
		Release();
	}

	// Load / Release
	b8 SceneScriptLoader::Load( const s8 *scriptname, const s8 *luaname )
	{
		// lua名保存
		m_luaname = luaname;
		
		// Script読み込み
		if( ! m_sm->Read( scriptname, m_luaname.c_str() ) ){
			MOELogE( "SceneScriptLoader::script read error(%s)", scriptname );
			return false;
		}
		
		// Buffer
		CBuffer *buffer = mnew CBuffer();
		if( buffer == 0 ){
			MOELogE( "SceneScriptLoader::CBuffer memory alloc error" );
			return false;
		}
		
		s32 len = m_sm->GetTableValueNum( luaname, "Buffer" );
		s32 start_idx = 0;
		for( s32 i = 0; i < len; i+=_SCENESCRIPT_BUFFER_RECNUM ){
			// 1 record read & entry
			buffer->m_name   = static_cast<s8*>(m_sm->GetTableValue( luaname, "Buffer", i+0, start_idx ));
			buffer->m_color  = static_cast<s8*>(m_sm->GetTableValue( luaname, "Buffer", i+1, start_idx ));
			buffer->m_depth  = static_cast<s8*>(m_sm->GetTableValue( luaname, "Buffer", i+2, start_idx ));
			buffer->m_width  = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Buffer", i+3, start_idx ));
			buffer->m_height = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Buffer", i+4, start_idx ));
			addBuffer(buffer->m_name, buffer);
		}
		
		// Scene
		CScene *scene = mnew CScene();
		if( scene == 0 ){
			MOELogE( "SceneScriptLoader::CScene memory alloc error" );
			return false;
		}
		
		start_idx = 0;
		len = m_sm->GetTableValueNum( luaname, "Scene" );
		for( s32 i = 0; i < len; i+=_SCENESCRIPT_SCENE_RECNUM ){
			// 1 record read & entry
			scene->m_name    = static_cast<s8*>(m_sm->GetTableValue( luaname, "Scene", i+0, start_idx ));
			scene->m_mrzpath = static_cast<s8*>(m_sm->GetTableValue( luaname, "Scene", i+1, start_idx ));
			addScene(scene->m_name, scene);
		}
		
		// Process
		CProcess *process = mnew CProcess();
		if( process == 0 ){
			MOELogE( "SceneScriptLoader::CProcess memory alloc error" );
			return false;
		}

		start_idx = 0;
		len = m_sm->GetTableValueNum( luaname, "Process" );
		for( s32 i = 0; i < len; i+=_SCENESCRIPT_PROCESS_RECNUM ){
			// 1 record read & entry
			process->m_name            = static_cast<s8*>(m_sm->GetTableValue( luaname, "Process", i+0, start_idx ));
			process->m_demo_starttime  = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Process", i+1, start_idx ));
			process->m_demo_endtime    = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Process", i+2, start_idx ));
			process->m_scene_starttime = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Process", i+3, start_idx ));
			process->m_scene_endtime   = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Process", i+4, start_idx ));
			addProcess(process->m_name, process);
		}

		// Render
		CRender *render = mnew CRender();
		if( render == 0 ){
			MOELogE( "SceneScriptLoader::CRender memory alloc error" );
			return false;
		}

		start_idx = 0;
		len = m_sm->GetTableValueNum( luaname, "Render" );
		for( s32 i = 0; i < len; i+=_SCENESCRIPT_RENDER_RECNUM ){
			// 1 record read & entry
			render->m_src_name               = static_cast<s8*>(m_sm->GetTableValue( luaname, "Render", i+0, start_idx ));
			render->m_target_name            = static_cast<s8*>(m_sm->GetTableValue( luaname, "Render", i+1, start_idx ));
			render->m_demo_starttime         = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Render", i+2, start_idx ));
			render->m_demo_endtime           = *static_cast<f32*>(m_sm->GetTableValue( luaname, "Render", i+3, start_idx ));
			render->m_shader_name            = static_cast<s8*>(m_sm->GetTableValue( luaname, "Render", i+4, start_idx ));
			render->m_param.m_srcbuffer_name = static_cast<s8*>(m_sm->GetTableValue( luaname, "Render", i+5, start_idx ));
			addRender(i, render);
		}

		return true;
	}

	b8 SceneScriptLoader::Release()
	{
		// Buffer
	    std::map<std::string, CBuffer*>::iterator bit, ebit = m_buffers.end();
		for (bit = m_buffers.begin(); bit != ebit; ++bit) {
			delete bit->second;
		}
		m_buffers.clear();
		
		// Scene
	    std::map<std::string, CScene*>::iterator sit, esit = m_scenes.end();
		for (sit = m_scenes.begin(); sit != esit; ++sit) {
			delete sit->second;
		}
		m_scenes.clear();

		// Process
	    std::map<std::string, CProcess*>::iterator pit, epit = m_proceses.end();
		for (pit = m_proceses.begin(); pit != epit; ++pit) {
			delete pit->second;
		}
		m_proceses.clear();

		// Render
	    std::map<s32, CRender*>::iterator rit, reit = m_renders.end();
		for (rit = m_renders.begin(); rit != reit; ++rit) {
			delete rit->second;
		}
		m_renders.clear();
		
		// LuaName
		m_luaname.clear();

		return true;
	}

	// Entry Container
	void SceneScriptLoader::addBuffer(std::string key, CBuffer *buf)
	{
		m_buffers[key] = buf;
	}
	
	void SceneScriptLoader::addScene(std::string key, CScene *scene)
	{
		m_scenes[key] = scene;
	}
	
	void SceneScriptLoader::addProcess(std::string key, CProcess *process)
	{
		m_proceses[key] = process;
	}
	
	void SceneScriptLoader::addRender(s32 key, CRender *render)
	{
		m_renders[key] = render;
	}

	// Buffer
	const SceneScriptLoader::CBuffer* SceneScriptLoader::GetBuffer( const s8 *buffer_name )
	{
		std::string name = buffer_name;
		std::map<std::string, SceneScriptLoader::CBuffer*>::const_iterator it = m_buffers.find(name);
		
		if (it != m_buffers.end()){
			// 見つかった
			return it->second;
		}
		
		// 見つからない
		return NULL;
	}

	// Scene
	const SceneScriptLoader::CScene* SceneScriptLoader::GetScene( const s8 *scene_name )
	{
		std::string name = scene_name;
		std::map<std::string, SceneScriptLoader::CScene*>::const_iterator it = m_scenes.find(name);
		
		if (it != m_scenes.end()){
			// 見つかった
			return it->second;
		}
		
		// 見つからない
		return NULL;
	}

	// Process
	const SceneScriptLoader::CProcess* SceneScriptLoader::GetProcess( const s8 *scene_name )
	{
		std::string name = scene_name;
		std::map<std::string, SceneScriptLoader::CProcess*>::const_iterator it = m_proceses.find(name);
		
		if (it != m_proceses.end()){
			// 見つかった
			return it->second;
		}
		
		// 見つからない
		return NULL;
	}

	// Render
	const SceneScriptLoader::CRender* SceneScriptLoader::GetRender( s32 render_no )
	{
		std::map<s32, SceneScriptLoader::CRender*>::const_iterator it = m_renders.find(render_no);
		
		if (it != m_renders.end()){
			// 見つかった
			return it->second;
		}
		
		// 見つからない
		return NULL;
	}

}	// namespace MOE
