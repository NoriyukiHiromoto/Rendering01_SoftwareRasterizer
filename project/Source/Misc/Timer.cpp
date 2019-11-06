﻿/*
 * MIT License
 *  Copyright (c) 2019 SPARKCREATIVE
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  @author Noriyuki Hiromoto <hrmtnryk@sparkfx.jp>
*/

//======================================================================================================
//
//======================================================================================================
#include <Misc/Timer.h>

//======================================================================================
//
//======================================================================================
bool											Timer::_IsInitialized;
std::chrono::high_resolution_clock::time_point	Timer::_Origin;

//======================================================================================
//
//======================================================================================
Timer::Timer()
	: _Start(0)
{
	if (_IsInitialized) return;
	_IsInitialized = true;
	_Origin = std::chrono::high_resolution_clock::now();
}

//======================================================================================
//
//======================================================================================
uint64 Timer::GetMicro()
{
	std::chrono::high_resolution_clock::time_point TimeNow = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(TimeNow - _Origin).count();
}

//======================================================================================
//
//======================================================================================
uint64 Timer::Start()
{
	_Start = GetMicro();
	return _Start;
}

//======================================================================================
//
//======================================================================================
uint64 Timer::Elapsed()
{
	const uint64 Now = GetMicro();
	const uint64 Result = Now - _Start;
	_Start = Now;
	return Result;
}