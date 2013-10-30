/*
 *  SceneScriptLoader.h
 *  SceneScriptLoader
 *
 *  Created by GOT on 2013/10/07.
 *  Copyright 2013 Eldorado / System K. All rights reserved.
 */
#ifndef INCLUDE_SCENESCRIPTLOADER_H
#define INCLUDE_SCENESCRIPTLOADER_H

#include "BaseScriptLoader.h"

namespace MOE {
	//==============================================================
	// SceneScriptLoader class
	//==============================================================
	class SceneScriptLoader : public BaseScriptLoader
	{
	public:
		// Buffer
		class CBuffer
		{
		public:
			std::string m_name;
			std::string m_color;
			std::string m_depth;
			f32 m_width;
			f32 m_height;
			
			CBuffer()
			: m_width( 0.0f )
			, m_height( 0.0f )
			{
				m_name.clear();
				m_color.clear();
				m_depth.clear();
			}
		};
		
		// Scene
		class CScene
		{
		public:
			std::string m_name;
			std::string m_mrzpath;
			
			CScene()
			{
				m_name.clear();
				m_mrzpath.clear();
			}
		};
		
		// Process
		class CProcess
		{
		public:
			std::string m_name;
			f32 m_demo_starttime;
			f32 m_demo_endtime;
			f32 m_scene_starttime;
			f32 m_scene_endtime;
			
			CProcess()
			: m_demo_starttime( 0.0f )
			, m_demo_endtime( 0.0f )
			, m_scene_starttime( 0.0f )
			, m_scene_endtime( 0.0f )
			{
				m_name.clear();
			}
		};
		
		// Render
		class CRender
		{
		private:
			struct SParam
			{
				std::string m_srcbuffer_name;
			};
		
		public:
			std::string m_src_name;
			std::string m_target_name;
			f32 m_demo_starttime;
			f32 m_demo_endtime;
			std::string m_shader_name;
			SParam m_param;
			
			CRender()
			: m_demo_starttime( 0.0f )
			, m_demo_endtime( 0.0f )
			{
				m_src_name.clear();
				m_target_name.clear();
				m_param.m_srcbuffer_name.clear();
			}
		};

	private:
		std::string							m_luaname;
		std::map<std::string, CBuffer*>		m_buffers;
		std::map<std::string, CScene*>		m_scenes;
		std::map<std::string, CProcess*>	m_proceses;
		std::map<s32, CRender*>				m_renders;
		MOE::CScriptManager					*m_sm;

	private:
		// Entry Container
		void addBuffer( std::string key, CBuffer *buf );
		void addScene( std::string key, CScene *scene );
		void addProcess( std::string key, CProcess *process );
		void addRender( s32 key, CRender *render );

	public:
		// Constractor / Destractor
		SceneScriptLoader( MOE::CScriptManager *sm );
		virtual ~SceneScriptLoader();
		
		// Load / Release
		virtual b8 Load( const s8 *scriptname, const s8 *luaname );
		virtual b8 Release();
		
		// Buffer
		const CBuffer* GetBuffer( const s8 *buffer_name );
		
		// Scene
		const CScene* GetScene( const s8 *scene_name );
		
		// Process
		const CProcess* GetProcess( const s8 *scene_name );
		
		// Render
		const CRender* GetRender( s32 render_no );
	};
}

#endif
