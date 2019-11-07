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
#include <Renderer/Texture.h>

//======================================================================================================
//
//======================================================================================================
Texture::Texture()
{
	Release();
}

//======================================================================================================
//
//======================================================================================================
Texture::~Texture()
{
	Release();
}

//======================================================================================================
//
//======================================================================================================
void Texture::Release()
{
	_Width = 0;
	_Height = 0;
	_SurfaceCount = 0;
	memset(_Surface, 0, sizeof(_Surface));
}

//======================================================================================================
//
//======================================================================================================
bool Texture::Create(int32 w, int32 h, int32 level)
{
	auto& Src = _Surface[level];

	Src.Color.resize(w * h);

	Src.Width = w;
	Src.Height = h;

	return true;
}

//======================================================================================================
//
//======================================================================================================
bool Texture::Create(int32 w, int32 h)
{
	Release();

	_Width = w;
	_Height = h;
	_SurfaceCount = 1;
	return Create(w, h, 0);
}

//======================================================================================================
//
//======================================================================================================
bool Texture::Load(const char* pFileName)
{
	struct DDPIXELFORMAT
	{
		uint32	dwSize;
		uint32	dwFlags;
		uint32	dwFourCC;
		uint32	dwRGBBitCount;
		uint32	dwRBitMask;
		uint32	dwGBitMask;
		uint32	dwBBitMask;
		uint32	dwRGBAlphaBitMask;
	};
	struct DDCAPS2
	{
		uint32	dwCaps1;
		uint32	dwCaps2;
		uint32	Reserved[2];
	};
	struct DDSURFACEDESC2
	{
		uint32			dwSize;
		uint32			dwFlags;
		uint32			dwHeight;
		uint32			dwWidth;
		uint32			dwPitchOrLinearSize;
		uint32			dwDepth;
		uint32			dwMipMapCount;
		uint32			dwReserved1[11];
		DDPIXELFORMAT	ddpfTexelFormat;
		DDCAPS2			ddsCaps;
		uint32			dwReserved2;
	};

	DWORD ReadedBytes;
	uint32 MagicNumber;
	DDSURFACEDESC2 DDSHeader;

	bool bSucceeded = false;

	Release();

	HANDLE hFile = ::CreateFileA(pFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) goto EXIT;

	// マジックナンバー
	::ReadFile(hFile, &MagicNumber, sizeof(uint32), &ReadedBytes, nullptr);
	if (MagicNumber != ' SDD') goto EXIT;

	// ヘッダ
	::ReadFile(hFile, &DDSHeader, sizeof(DDSURFACEDESC2), &ReadedBytes, nullptr);

	if (DDSHeader.ddpfTexelFormat.dwRGBBitCount != 32) goto EXIT;
	if (DDSHeader.ddpfTexelFormat.dwRGBAlphaBitMask != 0xFF000000) goto EXIT;
	if (DDSHeader.ddpfTexelFormat.dwRBitMask != 0x00FF0000) goto EXIT;
	if (DDSHeader.ddpfTexelFormat.dwGBitMask != 0x0000FF00) goto EXIT;
	if (DDSHeader.ddpfTexelFormat.dwBBitMask != 0x000000FF) goto EXIT;

	_Width = DDSHeader.dwWidth;
	_Height = DDSHeader.dwHeight;
	_SurfaceCount = std::max(1, int32(DDSHeader.dwMipMapCount) - 2);
	for (int32 i = 0; i < _SurfaceCount; ++i)
	{
		const int32 SrcW = std::max(1, int32(DDSHeader.dwWidth >> i));
		const int32 SrcH = std::max(1, int32(DDSHeader.dwHeight >> i));

		// サーフェイス生成
		if (!Create(SrcW, SrcH, i))
		{
			goto EXIT;
		}

		auto& Src = _Surface[i];

		// ピクセルデータ読み込み
		::ReadFile(hFile, &(Src.Color[0]), sizeof(Color) * SrcW * SrcH, &ReadedBytes, nullptr);
	}

	bSucceeded = true;

EXIT:
	// ファイル閉じる
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile);
	}

	if (!bSucceeded)
	{
		Release();
	}

	return bSucceeded;
}

//======================================================================================================
//
//======================================================================================================
Color Texture::Sample(fp32 u, fp32 v) const
{
	u += 256.0f;
	v += 256.0f;

	const auto& Src = _Surface[0];

	const auto uf = u * fp32(Src.Width);
	const auto vf = v * fp32(Src.Height);
	const auto ui0 = int32(uf);
	const auto vi0 = int32(vf);

	const auto mui0 = ui0 % Src.Width;
	const auto mui1 = (ui0 + 1) % Src.Width;
	const auto mvi0 = vi0 % Src.Height;
	const auto mvi1 = (vi0 + 1) % Src.Height;

	const auto x0y0 = Src.Color[mui0 + (mvi0 * Src.Width)];
	const auto x1y0 = Src.Color[mui1 + (mvi0 * Src.Width)];
	const auto x0y1 = Src.Color[mui0 + (mvi1 * Src.Width)];
	const auto x1y1 = Src.Color[mui1 + (mvi1 * Src.Width)];

	return Color::Lerp(x0y0, x1y0, x0y1, x1y1, uf - fp32(ui0), vf - fp32(vi0));
}