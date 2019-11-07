/*
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
#pragma once

//======================================================================================================
//
//======================================================================================================
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

//======================================================================================================
//
//======================================================================================================
#include <windows.h>
#include <stdio.h>
#include <map>
#include <mutex>
#include <string>
#include <atomic>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>

//======================================================================================================
//
//======================================================================================================
typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
typedef long long			int64;
typedef unsigned long long	uint64;
typedef float				fp32;
typedef double				fp64;

//======================================================================================================
//
//======================================================================================================
static const wchar_t*	APPLICATION_TITLE		= L"Rasterizer";
static const int32		SCREEN_WIDTH			= 1280;
static const int32		SCREEN_HEIGHT			= 720;

//======================================================================================================
//
//======================================================================================================
static const fp32		SCREEN_WIDTH_F			= fp32(SCREEN_WIDTH);
static const fp32		SCREEN_HEIGHT_F			= fp32(SCREEN_HEIGHT);
static const int32		SCREEN_WIDTH_HALF		= SCREEN_WIDTH  / 2;
static const int32		SCREEN_HEIGHT_HALF		= SCREEN_HEIGHT / 2;
static const fp32		SCREEN_WIDTH_HALF_F		= fp32(SCREEN_WIDTH_HALF);
static const fp32		SCREEN_HEIGHT_HALF_F	= fp32(SCREEN_HEIGHT_HALF);

static const int32		MAX_VERTEX_CACHE_SIZE	= 0x0000FFFF;

//======================================================================================================
//
//======================================================================================================
#if defined(_DEBUG)
#define ASSERT(c)		if (!(c)) { *reinterpret_cast<int32*>(0) = 0; }
#else//defined(_DEBUG)
#define ASSERT(c)
#endif//defined(_DEBUG)

//======================================================================================================
//
//======================================================================================================
union Color
{
	struct
	{
		uint8 b, g, r, a;
	};
	uint32 data;

	Color()
	{
	}
	Color(uint32 _data)
		: data(_data)
	{
	}

	static Color Lerp(Color lhs, Color rhs, fp32 rate)
	{
		// result
		Color result;
		result.r = uint8(lhs.r + fp32(rhs.r - lhs.r) * rate);
		result.g = uint8(lhs.g + fp32(rhs.g - lhs.g) * rate);
		result.b = uint8(lhs.b + fp32(rhs.b - lhs.b) * rate);
		result.a = uint8(lhs.a + fp32(rhs.a - lhs.a) * rate);
		return result;
	}

	static Color Lerp(Color x0y0, Color x1y0, Color x0y1, Color x1y1, fp32 rateX, fp32 rateY)
	{
		// y0
		Color y0;
		y0.r = uint8(x0y0.r + fp32(x1y0.r - x0y0.r) * rateX);
		y0.g = uint8(x0y0.g + fp32(x1y0.g - x0y0.g) * rateX);
		y0.b = uint8(x0y0.b + fp32(x1y0.b - x0y0.b) * rateX);
		y0.a = uint8(x0y0.a + fp32(x1y0.a - x0y0.a) * rateX);
		// y1
		Color y1;
		y1.r = uint8(x0y1.r + fp32(x1y1.r - x0y1.r) * rateX);
		y1.g = uint8(x0y1.g + fp32(x1y1.g - x0y1.g) * rateX);
		y1.b = uint8(x0y1.b + fp32(x1y1.b - x0y1.b) * rateX);
		y1.a = uint8(x0y1.a + fp32(x1y1.a - x0y1.a) * rateX);
		// result
		Color result;
		result.r = uint8(y0.r + fp32(y1.r - y0.r) * rateY);
		result.g = uint8(y0.g + fp32(y1.g - y0.g) * rateY);
		result.b = uint8(y0.b + fp32(y1.b - y0.b) * rateY);
		result.a = uint8(y0.a + fp32(y1.a - y0.a) * rateY);
		return result;
	}
};

//======================================================================================================
//
//======================================================================================================
struct VertexData
{
	fp32	Position[3];
	fp32	Normal[3];
	fp32	TexCoord[2];
};

//======================================================================================================
//
//======================================================================================================
inline bool operator < (const VertexData& lhs, const VertexData& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(VertexData)) < 0;
}

//======================================================================================================
//
//======================================================================================================
struct MeshFileBinary
{
	char		TextureName[32];
	uint32		TriangleVertexCount;
};

struct MeshFileBinaryHead
{
	uint32			GUID;		// 'MBIN'
	uint32			MeshCount;
};
