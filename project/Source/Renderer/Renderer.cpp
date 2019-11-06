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
#include <Renderer/Renderer.h>

//======================================================================================================
//
//======================================================================================================
Renderer::Renderer()
	: _pColorBuffer(nullptr)
{
}

//======================================================================================================
//
//======================================================================================================
Renderer::~Renderer()
{
}

//======================================================================================================
//
//======================================================================================================
void Renderer::BeginDraw(ColorBuffer* pColorBuffer, const Matrix& mView, const Matrix& mProj)
{
	_pColorBuffer = pColorBuffer;
	_ViewMatrix = mView;
	_ProjMatrix = mProj;

	_RenderMeshDatas.clear();
}

//======================================================================================================
//
//======================================================================================================
void Renderer::EndDraw()
{
	Matrix mViewProj;
	Matrix_Multiply4x4(mViewProj, _ViewMatrix, _ProjMatrix);

	// メッシュ毎に処理する
	{
		const int32 MeshCount = int32(_RenderMeshDatas.size());
		for (int32 i = 0; i < MeshCount; ++i)
		{
			const auto& Mesh = _RenderMeshDatas[i];

			Matrix mWorldViewProj;
			Matrix_Multiply4x4(mWorldViewProj, Mesh.mWorld, mViewProj);

			auto pPosTbl = Mesh.pMeshData->GetPosition();
			const auto VertexCount = Mesh.pMeshData->GetVertexCount();
			ASSERT(VertexCount <= MAX_VERTEX_CACHE_SIZE);
			static Vector4 Positions[MAX_VERTEX_CACHE_SIZE];
			for (auto i = 0; i < VertexCount; ++i)
			{
				Matrix_Transform4x4(Positions[i], pPosTbl[i], mWorldViewProj);
			}

			RenderTriangle(
				Mesh.pMeshData,
				Positions,
				VertexCount,
				Mesh.pMeshData->GetIndex(),
				Mesh.pMeshData->GetIndexCount());
		}
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::DrawIndexed(const IMeshData* pMeshData, const Matrix& mWorld)
{
	_RenderMeshDatas.emplace_back(RenderMeshData{ pMeshData, mWorld });
}

//======================================================================================================
//
//======================================================================================================
void Renderer::RenderTriangle(const IMeshData* pMeshData, const Vector4 Positions[], const int32 VertexCount, const uint16* pIndex, const int32 IndexCount)
{
	const auto WidthF  = SCREEN_WIDTH_F  - 1.0f;
	const auto HeightF = SCREEN_HEIGHT_F - 1.0f;

	const int32 PointCount = 3;
	Vector4 Temp[PointCount];

	auto index = 0;
	while (index < IndexCount)
	{
		const auto i0 = pIndex[index++];
		const auto i1 = pIndex[index++];
		const auto i2 = pIndex[index++];

		Temp[0] = Positions[i0];
		Temp[1] = Positions[i1];
		Temp[2] = Positions[i2];

		// 画面外にはみ出すポリゴンは破棄する
		if (Temp[0].w <= 0.0f) continue;
		if (Temp[1].w <= 0.0f) continue;
		if (Temp[2].w <= 0.0f) continue;
		if (Temp[0].x < -Temp[0].w) continue;
		if (Temp[1].x < -Temp[1].w) continue;
		if (Temp[2].x < -Temp[2].w) continue;
		if (Temp[0].x > +Temp[0].w) continue;
		if (Temp[1].x > +Temp[1].w) continue;
		if (Temp[2].x > +Temp[2].w) continue;
		if (Temp[0].y < -Temp[0].w) continue;
		if (Temp[1].y < -Temp[1].w) continue;
		if (Temp[2].y < -Temp[2].w) continue;
		if (Temp[0].y > +Temp[0].w) continue;
		if (Temp[1].y > +Temp[1].w) continue;
		if (Temp[2].y > +Temp[2].w) continue;

		for (int32 j = 0; j < PointCount; ++j)
		{
			auto& pt = Temp[j];
			pt.x = (+pt.x / pt.w * 0.5f + 0.5f) * WidthF;
			pt.y = (-pt.y / pt.w * 0.5f + 0.5f) * HeightF;
		}

		DrawLine(int32(Temp[0].x + 0.5f), int32(Temp[0].y + 0.5f), int32(Temp[1].x + 0.5f), int32(Temp[1].y + 0.5f));
		DrawLine(int32(Temp[1].x + 0.5f), int32(Temp[1].y + 0.5f), int32(Temp[2].x + 0.5f), int32(Temp[2].y + 0.5f));
		DrawLine(int32(Temp[2].x + 0.5f), int32(Temp[2].y + 0.5f), int32(Temp[0].x + 0.5f), int32(Temp[0].y + 0.5f));
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::DrawLine(int32 x0, int32 y0, int32 x1, int32 y1)
{
	const int32 dx = std::abs(x1 - x0);
	const int32 dy = std::abs(y1 - y0);
	if (dx > dy)
	{
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		y0 <<= 16;
		y1 <<= 16;
		const int32 add = dx == 0 ? 0 : (y1 - y0) / dx;
		int32 y = y0;
		for (int32 x = x0; x <= x1; ++x)
		{
			const auto dx = x;
			const auto dy = y >> 16;
			_pColorBuffer->SetPixel(dx, dy, 0xFFFFFFFF);
			y += add;
		}
	}
	else
	{
		if (y0 > y1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		x0 <<= 16;
		x1 <<= 16;
		const int32 add = dx == 0 ? 0 : (x1 - x0) / dy;
		int32 x = x0;
		for (int32 y = y0; y <= y1; ++y)
		{
			const auto dx = x >> 16;
			const auto dy = y;
			_pColorBuffer->SetPixel(dx, dy, 0xFFFFFFFF);
			x += add;
		}
	}
}
