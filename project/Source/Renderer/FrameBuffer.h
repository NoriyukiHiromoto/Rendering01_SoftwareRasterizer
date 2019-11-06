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
template <typename T>
class FrameBuffer
{
private:
	T*		_pPixel;
	int32	_Width;
	int32	_Height;
	bool	_IsInternal;

public:
	FrameBuffer()
		: _pPixel(nullptr)
		, _Width(0)
		, _Height(0)
		, _IsInternal(true)
	{
	}
	FrameBuffer(T* pPixel, int32 Width, int32 Height)
		: _pPixel(pPixel)
		, _Width(Width)
		, _Height(Height)
		, _IsInternal(pPixel == nullptr)
	{
		if (_IsInternal)
		{
			_pPixel = new T[Width * Height];
		}
	}
	~FrameBuffer()
	{
		Release();
	}

	void Release()
	{
		if (_IsInternal)
		{
			delete[] _pPixel;
		}
		_pPixel = nullptr;
		_Width = 0;
		_Height = 0;
		_IsInternal = true;
	}

public:
	void Clear(T pixel)
	{
		int32 count = int32(_Width * _Height);
		while (count-- > 0)
		{
			_pPixel[count] = pixel;
		}
	}

	void Resize(int32 Width, int32 Height)
	{
		Release();

		_Width = Width;
		_Height = Height;
		_IsInternal = true;
		_pPixel = new T[Width * Height];
	}

	bool SetPixel(int32 x, int32 y, T color)
	{
		ASSERT(0 <= x);
		ASSERT(0 <= y);
		ASSERT(x < _Width);
		ASSERT(y < _Height);
		_pPixel[x + y * _Width] = color;
		return true;
	}

	T& GetPixelRef(int32 x, int32 y)
	{
		ASSERT(0 <= x);
		ASSERT(0 <= y);
		ASSERT(x < _Width);
		ASSERT(y < _Height);
		return _pPixel[x + y * _Width];
	}

	T GetPixel(int32 x, int32 y) const
	{
		ASSERT(0 <= x);
		ASSERT(0 <= y);
		ASSERT(x < _Width);
		ASSERT(y < _Height);
		return _pPixel[x + y * _Width];
	}

	T* GetPixelPointer(int32 x = 0, int32 y = 0)
	{
		ASSERT(0 <= x);
		ASSERT(0 <= y);
		ASSERT(x < _Width);
		ASSERT(y < _Height);
		return _pPixel + x + y * _Width;
	}

	const T* GetPixelPointer(int32 x = 0, int32 y = 0) const
	{
		ASSERT(0 <= x);
		ASSERT(0 <= y);
		ASSERT(x < _Width);
		ASSERT(y < _Height);
		return _pPixel + x + y * _Width;
	}

	uint32 GetWidth() const
	{
		return _Width;
	}

	uint32 GetHeight() const
	{
		return _Height;
	}
};
