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

	static void Blend(Color& result, Color value, fp32 rate)
	{
		const auto rate128 = int32(rate * 128.0f);

		const auto mask_1010 = 0xFF00FF00;
		const auto mask_0101 = 0x00FF00FF;
		const auto mask = (uint64(mask_1010) << 24) | uint64(mask_0101);

		const auto packed = (uint64(value.data & mask_1010) << 24) | uint64(value.data & mask_0101);
		const auto color = ((packed * rate128) >> 7) & mask;

		result.data = uint32(color >> 24) | uint32(color);
	}

	static Color Lerp(Color l, Color r, fp32 rate)
	{
		const auto rate128 = int32(rate * 128.0f);
		const auto rate128inv = 0x80 - rate128;

		const auto mask_1010 = 0xFF00FF00;
		const auto mask_0101 = 0x00FF00FF;
		const auto mask = (uint64(mask_1010) << 24) | uint64(mask_0101);

		auto r_packed = (uint64(r.data & mask_1010) << 24) | uint64(r.data & mask_0101);
		auto l_packed = (uint64(l.data & mask_1010) << 24) | uint64(l.data & mask_0101);

		r_packed = ((r_packed * rate128) >> 7) & mask;
		l_packed = ((l_packed * rate128inv) >> 7) & mask;

		const auto color = l_packed + r_packed;

		Color result;
		result.data = uint32(color >> 24) | uint32(color);
		return result;
	}

	static Color Lerp(Color x0y0, Color x1y0, Color x0y1, Color x1y1, fp32 rateX, fp32 rateY)
	{
		// ＿＿＿＿＿
		// |R|G|B|A|             32bit RGBA
		// ￣￣￣￣￣
		//     ↓
		// ＿＿＿＿＿  ＿＿＿＿＿
		// |R| |B| |  | |G| |A|  32bit R_G_  32bit _G_A
		// ￣￣￣￣￣  ￣￣￣￣￣
		//     ↓
		// ＿＿＿＿＿＿＿＿
		// |R| |B| |G| |A|       64bit packed R_B_G_A
		// ￣￣￣￣￣￣￣￣

		const auto rateX128 = int32(rateX * 128.0f);
		const auto rateX128inv = 0x80 - rateX128;
		const auto rateY128 = int32(rateY * 128.0f);
		const auto rateY128inv = 0x80 - rateY128;

		const auto mask_1010 = 0xFF00FF00;
		const auto mask_0101 = 0x00FF00FF;
		const auto mask = (uint64(mask_1010) << 24) | uint64(mask_0101);

		auto x0y0_packed = (uint64(x0y0.data & mask_1010) << 24) | uint64(x0y0.data & mask_0101);
		auto x1y0_packed = (uint64(x1y0.data & mask_1010) << 24) | uint64(x1y0.data & mask_0101);
		auto x0y1_packed = (uint64(x0y1.data & mask_1010) << 24) | uint64(x0y1.data & mask_0101);
		auto x1y1_packed = (uint64(x1y1.data & mask_1010) << 24) | uint64(x1y1.data & mask_0101);

		x1y0_packed = ((x1y0_packed * rateX128) >> 7) & mask;
		x1y1_packed = ((x1y1_packed * rateX128) >> 7) & mask;
		x0y0_packed = ((x0y0_packed * rateX128inv) >> 7) & mask;
		x0y1_packed = ((x0y1_packed * rateX128inv) >> 7) & mask;

		auto y0_packed = x0y0_packed + x1y0_packed;
		auto y1_packed = x0y1_packed + x1y1_packed;

		y1_packed = ((y1_packed * rateY128) >> 7) & mask;
		y0_packed = ((y0_packed * rateY128inv) >> 7) & mask;

		const auto color = y0_packed + y1_packed;

		Color result;
		result.data = uint32(color >> 24) | uint32(color);
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
