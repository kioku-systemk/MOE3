/*
 *  BaseScriptLoader.h
 *  BaseScriptLoader
 *
 *  Created by GOT on 2013/10/07.
 *  Copyright 2013 Eldorado / System K. All rights reserved.
 */
#ifndef INCLUDE_BASESCRIPTLOADER_H
#define INCLUDE_BASESCRIPTLOADER_H

namespace MOE {
	//==============================================================
	// BaseScriptLoader class
	//==============================================================
	class BaseScriptLoader
	{
	public:
		BaseScriptLoader(){}
		virtual ~BaseScriptLoader(){}
		
		virtual b8 Load( const s8 *scriptname ) = 0;
		virtual b8 Release() = 0;
	};
}

#endif