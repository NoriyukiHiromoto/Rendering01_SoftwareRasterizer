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

	auto index = 0;
	while (index < IndexCount)
	{
		auto pt = Positions[pIndex[index++]];
		pt.x = (+pt.x / pt.w * 0.5f + 0.5f) * WidthF;
		pt.y = (-pt.y / pt.w * 0.5f + 0.5f) * HeightF;

		auto dx = int32(pt.x + 0.5f);
		auto dy = int32(pt.y + 0.5f);
		if ((0 <= dx) && (dx < SCREEN_WIDTH) && (0 <= dy) && (dy < SCREEN_HEIGHT))
		{
			_pColorBuffer->SetPixel(dx, dy, 0xFFFFFFFF);
		}
	}
}
