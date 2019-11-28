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
class Texture
{
private:
	enum {
		SURFACE_COUNT = 16,
	};

	struct Surface
	{
		std::vector<Color>	Color;
		int32				Width;
		int32				Height;
		fp32				WidthF;
		fp32				HeightF;
		int32				UMask;
		int32				VMask;
		int32				UBit;
		int32				VBit;
	};

private:
	fp32	_WidthF;
	fp32	_HeightF;
	int32	_SurfaceCount;
	Surface	_Surface[SURFACE_COUNT];

public:
	Texture();
	~Texture();

private:
	bool Create(int32 w, int32 h, int32 level);

public:
	bool Create(int32 w, int32 h);
	bool Load(const char* pFileName);
	void Release();

public:
	Color* GetTexelPtr(int32 MipLevel = 0)
	{
		auto& Src = _Surface[MipLevel];
		return Src.Color.empty() ? nullptr : &(Src.Color[0]);
	}

	const Color* GetTexelPtr(int32 MipLevel = 0) const
	{
		auto& Src = _Surface[MipLevel];
		return Src.Color.empty() ? nullptr : &(Src.Color[0]);
	}

	int32 GetWidth(int32 MipLevel = 0) const
	{
		return _Surface[MipLevel].Width;
	}

	int32 GetHeight(int32 MipLevel = 0) const
	{
		return _Surface[MipLevel].Height;
	}

	int32 GetSurfaceCount() const
	{
		return _SurfaceCount;
	}

	virtual Color Sample(fp32 u, fp32 v) const;
	virtual Color Sample(fp32 u, fp32 v, fp32 du, fp32 dv) const;
};
