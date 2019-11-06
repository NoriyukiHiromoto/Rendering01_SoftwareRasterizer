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
#include <Math/Math.h>
#include <Renderer/FrameBuffer.h>
#include <Renderer/Texture.h>

//======================================================================================================
//
//======================================================================================================
struct IMeshData
{
	virtual const Texture* GetTexture() const = 0;

	virtual const int32 GetVertexCount() const = 0;
	virtual const int32 GetIndexCount() const = 0;

	virtual const Vector3* const GetPosition() const = 0;
	virtual const Vector3* const GetNormal() const = 0;
	virtual const Vector2* const GetTexCoord() const = 0;
	virtual const uint16* const GetIndex() const = 0;
};

struct MeshData : public IMeshData
{
	Texture					_Texture;
	std::vector<Vector3>	_Position;
	std::vector<Vector3>	_Normal;
	std::vector<Vector2>	_TexCoord;
	std::vector<uint16>		_Index;

	virtual const Texture* GetTexture() const { return &_Texture; }

	virtual const int32 GetVertexCount() const { return int32(_Position.size()); }
	virtual const int32 GetIndexCount() const { return int32(_Index.size()); }

	virtual const Vector3* const GetPosition() const { return &_Position[0]; }
	virtual const Vector3* const GetNormal() const { return &_Normal[0]; }
	virtual const Vector2* const GetTexCoord() const { return &_TexCoord[0]; }
	virtual const uint16* const GetIndex() const { return &_Index[0]; }
};

struct InternalVertex
{
	Vector4		Position;
	Vector3		Normal;
	Vector2		TexCoord;
};

struct RenderMeshData
{
	const IMeshData*	pMeshData;
	uint16				TextureId;
	Matrix				mWorld;
};

//======================================================================================================
//
//======================================================================================================
typedef FrameBuffer<Color>			ColorBuffer;
typedef FrameBuffer<fp32>			DepthBuffer;

//======================================================================================================
//
//======================================================================================================
class Renderer
{
	ColorBuffer*				_pColorBuffer;
	DepthBuffer*				_pDepthBuffer;
	std::vector<RenderMeshData>	_RenderMeshDatas;
	Matrix						_ViewMatrix;
	Matrix						_ProjMatrix;
	Vector3						_DirectionalLight;
	Texture						_DummyTexture;
	Texture*					_CurrentTexture;
	std::vector<Texture*>		_Textures;
	uint16						_CurrentTextureId;

public:
	Renderer();
	~Renderer();

private:
	template <typename FUNC>
	int32 ClipPoints(InternalVertex Dst[], const InternalVertex Src[], const int32 PointCount, FUNC Compare)
	{
		static const uint8 index_table[8][8] = {
			{ 0, 0, 0, 0, 0, 0, 0 },	// 0: -
			{ 0, 0, 0, 0, 0, 0, 0 },	// 1: 0
			{ 1, 0, 0, 0, 0, 0, 0 },	// 2: 0 1 0
			{ 1, 2, 0, 0, 0, 0, 0 },	// 3: 0 1 2 0
			{ 1, 2, 3, 0, 0, 0, 0 },	// 4: 0 1 2 3 0
			{ 1, 2, 3, 4, 0, 0, 0 },	// 5: 0 1 2 3 4 0
			{ 1, 2, 3, 4, 5, 0, 0 },	// 6: 0 1 2 3 4 5 0
			{ 1, 2, 3, 4, 5, 6, 0 },	// 7: 0 1 2 3 4 5 6 0
		};
		auto table = index_table[PointCount];

		int32 NewPointCount = 0;

		for (int32 i = 0; i < PointCount; ++i)
		{
			const auto& v1 = Src[i];
			const auto& v2 = Src[table[i]];	// [(i + 1) % PointCount]

			const auto d1 = Compare(v1.Position);
			const auto d2 = Compare(v2.Position);

			const auto Intersect = [&](InternalVertex& v)
			{
				const fp32 rate = d1 / (d1 - d2);
				Vector_Lerp(v.Position, v1.Position, v2.Position, rate);
				Vector_Lerp(v.Normal, v1.Normal, v2.Normal, rate);
				Vector_Lerp(v.TexCoord, v1.TexCoord, v2.TexCoord, rate);
			};

			if (d1 > 0.0f)
			{
				if (d2 > 0.0f)
				{
					Dst[NewPointCount++] = v2;
				}
				else
				{
					Intersect(Dst[NewPointCount++]);
				}
			}
			else
			{
				if (d2 > 0.0f)
				{
					Intersect(Dst[NewPointCount++]);
					Dst[NewPointCount++] = v2;
				}
			}
		}

		return NewPointCount;
	}

	fp32 EdgeFunc(const Vector2& a, const Vector2& b, const Vector2& c);
	fp32 EdgeFunc(const fp32 ax, const fp32 ay, const fp32 bx, const fp32 by, const fp32 cx, const fp32 cy);
	void RasterizeTriangle(uint16 TextureId, InternalVertex v0, InternalVertex v1, InternalVertex v2);
	void RenderTriangle(uint16_t TextureId, const IMeshData* pMeshData, const Vector4 Positions[], const Vector3 Normals[], const Vector2 Texcoord[], const int32 VertexCount, const uint16* pIndex, const int32 IndexCount);

public:
	void BeginDraw(ColorBuffer* pColorBuffer, DepthBuffer* pDepthBuffer, const Matrix& mView, const Matrix& mProj);
	void EndDraw();
	void SetTexture(Texture& Texture);
	void SetDirectionalLight(const Vector3& Direction);

	void DrawIndexed(const IMeshData* pMeshData, const Matrix& mWorld);
};
