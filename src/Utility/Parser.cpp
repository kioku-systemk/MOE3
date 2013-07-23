/*
 *  Parser.cpp
 *  Parser for Script.
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
//==============================================================
// define
//==============================================================
#pragma warning (disable : 4786)

//==============================================================
// header
//==============================================================
#include "../Core/Log.h"
#include "../Core/Memory.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Range.h"
#include "Parameters.h"
#include "BPM.h"
#include "Event.h"
#include "Trigger.h"
#include "StringUtils.h"
#include "Parser.h"

namespace MOE {

	//==============================================================
	// CParser class
	//==============================================================
	CParser::CParser()
	{
		mLine = 0;
		string mCurrentNamespace;
		mCurrentLineIndex = 0;
		mTotalLineCount = 0;
		mFile.clear();
	}

	CParser::~CParser()
	{
		mFile.clear();
	}

	void CParser::_Error(const char *msg)
	{
		//the getNextLine() has always been called before this is, so it's safe to use the -1 indexing
		MOELogW( "!! parse error on line(%d) %s\n", mFile[mCurrentLineIndex-1].first, string(msg) );
	}

	b8 CParser::_ReadFile(string filename)
	{
		ifstream script(filename.c_str(), ios::in);
		s32 linenumber = 0;

		if (script.is_open()){
			//clear out old file
			mFile.clear();
			b8 comment = false;
		
			//read in new file
			while (!script.eof()){
				string line;
				getline(script, line);
				linenumber++;

				//skip empty lines
				if (!line.empty()){
					// skip return code
					if( line[0] == '\n' || (line[0] == '\r' && line[1] == '\n') ){
						line.clear();
					}else{
						//check for a comment block
						if (line.length() > 1){
							if (line[0] == '/' && line[1] == '*'){
								//comment starts
								comment = true;
								continue; //go to next line without processing
							}else if (line[0] == '*' && line[1] == '/'){
								//comment ends
								comment = false;
								continue; //go to next line without processing
							}
						}

						//check that the line is not a single comment or inside a comment block
						if (!comment && (line[0] != ';' && line[0] != '#')){
							//convert it into lowercase
							transform(line.begin(), line.end(), line.begin(), tolower);
							//remove all crap that the programmer might put there by custom
							CStringUtils::RemoveChar(line, ';');
							CStringUtils::RemoveChar(line, '\t'); //remove tabs

							//TODO: check for lines that are only whitespace
							//push the processed line into the file
							if (line.size() > 0){
								mFile.push_back(pair<s32, string>(linenumber, line));
							}
						}
					}
				}
			}
			mTotalLineCount = (s32)mFile.size();
		}else{
			MOELogW( "CParser::readFile() - cannot open file %s for reading!", filename );
			return false;
		}
		script.close(); 

		return true;
	}

	b8 CParser::_GetNextLine(vector<string> &tokens)
	{
		if (mCurrentLineIndex < mTotalLineCount){
			//fetch the next line and split it into tokens. The tokenization is done here because it would
			//convolute the data structure further if it was done during loading. It's as fast anyway.. 
			tokens.clear();
			string &line = mFile[mCurrentLineIndex].second;
			CStringUtils::Tokenize(line, tokens);
			mCurrentLineIndex++;
			return true;
		}else{
			//we're done
			return false;
		}
	}

	b8 CParser::_ParseNamespace(CParameters &params)
	{
		vector<string> tokens;

		_GetNextLine(tokens);
		if(tokens[0] != "{"){
			_Error("namespace must start with {");
			return false;
		}

		//read until the closing curly brace
		while(true){
			tokens.clear();
			_GetNextLine(tokens);
			//break on ending curly brace
			if (tokens[0] == "}"){
				break;
			}
			//check for the equal sign that is in every namespace value
			if (tokens[2] != "="){
				_Error("syntax error");
			}
			//read in all kinds of values
			if (tokens[0] == "float"){
				f32 value = 0.0f;
		
				if (CStringUtils::ConvertStringToNumber<f32>(value, tokens[3], std::dec)){
					if (mCurrentNamespace != ""){
						params.AddF32(mCurrentNamespace, tokens[1], value);
					}else{
						_Error("trying to add parameter to an empty namespace!");
					}
				}else{
					_Error("cannot parse float");
				}
			}else if (tokens[0] == "int"){
				s32 value = 0;
				if (CStringUtils::ConvertStringToNumber<s32>(value, tokens[3], std::dec)){
					if (mCurrentNamespace != ""){
						params.AddS32(mCurrentNamespace, tokens[1], value);
					}else{
						_Error("trying to add parameter to an empty namespace!");
					}
				}else{
					_Error("cannot parse int");
				}
			}else if (tokens[0] == "bool"){
				s32 value = 0;
				if (mCurrentNamespace != ""){
					if (tokens[3] == "true"){
						params.AddB8(mCurrentNamespace, tokens[1], true);
					}else if (tokens[3] == "false"){
						params.AddB8(mCurrentNamespace, tokens[1], false);
					}else{
						_Error("cannot parse bool");
					}
				}else{
					_Error("trying to add parameter to an empty namespace!");
				}
			}else if (tokens[0] == "vector3"){
				f32 x, y, z;
				if (CStringUtils::ConvertStringToNumber<f32>(x, tokens[3], std::dec) &&
					CStringUtils::ConvertStringToNumber<f32>(y, tokens[4], std::dec) &&
					CStringUtils::ConvertStringToNumber<f32>(z, tokens[5], std::dec)){
					vec3 v = vec3(x, y, z);
					if (mCurrentNamespace != ""){
						params.AddVector3(mCurrentNamespace, tokens[1], v);
					}else{
						_Error("trying to add parameter to an empty namespace!");
					}
				}else{
					_Error("cannot parse vector3");
				}
			}else if (tokens[0] == "color"){
				f32 r, g, b, a;
				if (CStringUtils::ConvertStringToNumber<f32>(r, tokens[3], std::dec) &&
					CStringUtils::ConvertStringToNumber<f32>(g, tokens[4], std::dec) &&
					CStringUtils::ConvertStringToNumber<f32>(b, tokens[5], std::dec) &&
					CStringUtils::ConvertStringToNumber<f32>(a, tokens[6], std::dec)){
					vec4 col = vec4(r, g, b, a);
					if (mCurrentNamespace != ""){
						params.AddColor(mCurrentNamespace, tokens[1], col);
					}else{
						_Error("trying to add parameter to an empty namespace!");
					}
				}else{
					_Error("cannot parse color");
				}
			}else if (tokens[0] == "string"){
				if (mCurrentNamespace != ""){
					string str = tokens[3];

					//check for multiple words. They will be separated with space.
					bool multiple = false;
					for (vector<string>::iterator it = tokens.begin() + 4; it < tokens.end(); it++){
						if (!multiple){
							str += " "; //add space after the first word that was stripped while tokenization. Ugly.. 
							multiple = true;
						}
						str += *it;
						str += " ";
					}
					//take out the last space (yes, this is kind of hackish but..)
					if (multiple){
						str = str.substr(0, str.size() -1);
					}

					params.AddString(mCurrentNamespace, tokens[1], str);
				}else{
					_Error("trying to add parameter to an empty namespace!");
				}
			}else if (tokens[0] == "range"){
				f32 minimum, maximum;
				if (CStringUtils::ConvertStringToNumber<f32>(minimum, tokens[3], std::dec) &&
					CStringUtils::ConvertStringToNumber<f32>(maximum, tokens[4], std::dec)){
					CRange r = CRange(minimum, maximum);
					if (mCurrentNamespace != ""){
						params.AddRange(mCurrentNamespace, tokens[1], r);
					}else{
						_Error("trying to add parameter to an empty namespace!");
					}
				}else{
					_Error("cannot parse range");
				}
			}else{
				string ErrMsg = string("unknown symbol \"")+tokens[0]+string("\"");
				_Error(ErrMsg.c_str());
			}
		}

		return true;
	}

	b8 CParser::_ParseBPM(CBPM &beat)
	{
		vector<string> tokens;

		float bpm = 0;
		int offset = 0;

		_GetNextLine(tokens);
		//first non-empty line
		if (tokens[0] != "{"){
			_Error("BPM must start with {");
			return false;
		}

		//read until the closing curly brace
		while (true){
			tokens.clear();
			_GetNextLine(tokens);
			if (tokens[0] == "}"){
				break;
			}
			if (tokens[1] != "="){
				_Error("syntax error");
				return false;
			}
			//read in values
			if (tokens[0] == "bpm"){
				if (CStringUtils::ConvertStringToNumber<float>(bpm, tokens[2], std::dec)){
					beat.SetBPM(bpm);
				}else{
					_Error("cannot parse BPM value!");
				}
			}else if (tokens[0] == "offset"){
				if (CStringUtils::ConvertStringToNumber<int>(offset, tokens[2], std::dec)){
					beat.SetOffset(offset);
				}else{
					_Error("cannot parse BPM offset!");
				}
			}else{
				std::string err = std::string("unknown symbol \"")+tokens[0]+std::string("\"");
				_Error(err.c_str());
			}
		}

		return true;
	}

	b8 CParser::_ParseTrigger(CTriggerSystem &triggers)
	{
		vector<string> tokens;

		_GetNextLine(tokens);
		if (tokens[0] != "{"){
			_Error("BPM must start with {");
			return false;
		}

		//read until the closing curly brace
		while (true){
			tokens.clear();
			_GetNextLine(tokens);
			if (tokens[0] == "}"){
				break;
			}
			CTrigger trigger;

			if (CStringUtils::ConvertStringToNumber<int>(trigger.mTime, tokens[0], std::dec)){
				// though
			}else{
				_Error("cannot parse trigger time!");
			}
			if (CStringUtils::ConvertStringToNumber<int>(trigger.mLength, tokens[1], std::dec)){
				// though
			}else{
				_Error("cannot parse trigger length!");
			}
			triggers.AddTrigger(trigger);
		}
		return true;
	}

	b8 CParser::_ParseEvent(CEvent &e)
	{
		vector<string> tokens;

		int time = 0;
		int length = 0;

		_GetNextLine(tokens);

		//first non-empty line
		if (tokens[0] != "{"){
			_Error("Event must start with {");
			return false;
		}

		//read until the closing curly brace
		while(true){
			tokens.clear();
			_GetNextLine(tokens);
			if (tokens[0] == "}"){
				break;
			}
			if (tokens[1] != "="){
				_Error("syntax error");
				return false;
			}

			//read in values
			if (tokens[0] == "time"){
				if (CStringUtils::ConvertStringToNumber<s32>(time, tokens[2], std::dec)){
					e.mTime = time;
					//g_debug << "parsed event time = " << e.m_time << "\n";
				}else{
					_Error("cannot parse event time!");
				}
			}else if (tokens[0] == "length"){
				if (CStringUtils::ConvertStringToNumber<s32>(length, tokens[2], std::dec)){
					e.mLength = length;
				}else{
					_Error("cannot parse event value!");
				}
			}else{
				string err = string("unknown symbol \"")+tokens[0]+string("\"");
				_Error(err.c_str());
			}
		}
		return true;
	}

	vec3 CParser::_ConvertToVector3(string token1, string token2, string token3)
	{
		f32 x, y, z;
		if (CStringUtils::ConvertStringToNumber<f32>(x, token1, std::dec) &&
			CStringUtils::ConvertStringToNumber<f32>(y, token2, std::dec) &&
			CStringUtils::ConvertStringToNumber<f32>(z, token3, std::dec)){
			vec3 v = vec3(x, y, z);
			return v;
		}else{
			string ErrMsg = string("cannot parse {") + token1 + string(",") + token2 + string(",") + token3 + string("} into a Vector3!");
			_Error(ErrMsg.c_str());
			return vec3(0, 0, 0);
		}
	}

	b8 CParser::Parse(string filename, CParameters &params)
	{
		MOELogI( "loading script %s\n", filename.c_str() );

		if (_ReadFile(filename)){
			//file was found and read into the memory, so it can be processed
			mCurrentLineIndex = 0;

			params.Clear();

			vector<string> tokens;
			while (_GetNextLine(tokens)){
				//==============================================================
				// @todo:
				//	命令が追加された場合、こちらを追加する
				//==============================================================
				if (tokens[0] == "namespace"){
					if (tokens.size() != 2){
						_Error("namespace declaration must have exactly 2 tokens");
					}else{
						mCurrentNamespace = tokens[1];
						_ParseNamespace(params);
					}
				}else if (tokens[0] == "bpm"){
					CBPM *bpm = mnew CBPM();
					if( ! bpm ){
						_Error( "bpm is null." );
					}else{
						bpm->setName(tokens[1]);
						_ParseBPM(*bpm);
						params.AddBeat(tokens[1], bpm);
					}
				}else if (tokens[0] == "trigger"){
					CTriggerSystem *triggers = mnew CTriggerSystem();
					if( ! triggers ){
						_Error( "trigger is null." );
					}else{
						_ParseTrigger(*triggers);
						params.AddTriggers(tokens[1], triggers);
					}
				}else if (tokens[0] == "event"){
					CEvent *e = mnew CEvent();
					if( ! e ){
						_Error( "event is null." );
					}else{
						_ParseEvent(*e);
						params.AddEvent(tokens[1], e);
					}
				}else{
					_Error("unknown block");
				}
			}
		}else{
			string ErrMsg = string("cannot open script file ") + filename + string(" for reading!" );
			_Error(ErrMsg.c_str());
		}
		MOELogI( "Parsing done!\n" );
		return true;
	}

}	// namespace MOE