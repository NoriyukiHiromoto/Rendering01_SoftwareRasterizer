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
#include <TaskSystem/TaskSystem.h>

//======================================================================================================
//
//======================================================================================================
Renderer::Renderer()
	: _pColorBuffer(nullptr)
	, _pDepthBuffer(nullptr)
{
	// タイルごとのポリゴンデータの入れ物
	// 並列処理されるので最初に入れ物を用意しておく
	for (int32 y = 0; y < MAX_TILE_COUNT_Y; ++y)
	{
		for (int32 x = 0; x < MAX_TILE_COUNT_X; ++x)
		{
			_RasterizeDatas[y][x].Count = 0;
			_RasterizeDatas[y][x].Triangles.resize(MAX_TILE_TRIANGLE_COUNT);
		}
	}

	// テクスチャがセットされない場合用の白のダミーテクスチャ
	_DummyTexture.Create(2, 2);
	memset(_DummyTexture.GetTexelPtr(), 0x80, sizeof(uint32[2][2]));
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
void Renderer::BeginDraw(ColorBuffer* pColorBuffer, DepthBuffer* pDepthBuffer, GBuffer* pGBuffer, const Matrix& mView, const Matrix& mProj)
{
	_pColorBuffer = pColorBuffer;
	_pDepthBuffer = pDepthBuffer;
	_pGBuffer = pGBuffer;
	_ViewMatrix = mView;
	_ProjMatrix = mProj;

	_RenderMeshDatas.clear();

	_Textures.clear();
	_Textures.push_back(nullptr);

	_CurrentTextureId = int32(_Textures.size());
	_CurrentTriangleId = 0;
}

//======================================================================================================
//
//======================================================================================================
void Renderer::EndDraw()
{
	Matrix_Multiply4x4(_mViewProj, _ViewMatrix, _ProjMatrix);

	// メッシュ毎にジョブを作って並列処理する
	// ・座標変換
	// ・シザリング
	// ・レンダリングする可能性のあるタイルへのデータの追加
	{
		const int32 MeshCount = int32(_RenderMeshDatas.size());
		for (int32 i = 0; i < MeshCount; ++i)
		{
			TaskSystem::Instance().PushQue([&](void* pData) {
				auto* pMesh = reinterpret_cast<RenderMeshData*>(pData);
				const auto VertexCount = pMesh->pMeshData->GetVertexCount();
				ASSERT(VertexCount <= MAX_VERTEX_CACHE_SIZE);

				const auto mWorld = pMesh->mWorld;
				const auto mViewProj = _mViewProj;

				thread_local static Vector4 Positions[MAX_VERTEX_CACHE_SIZE];
				auto pPosTbl = pMesh->pMeshData->GetPosition();
				for (auto i = 0; i < VertexCount; ++i)
				{
					Matrix_Transform4x4(Positions[i], pPosTbl[i], mViewProj);
				}

				thread_local static Vector3 Normals[MAX_VERTEX_CACHE_SIZE];
				auto pNormalTbl = pMesh->pMeshData->GetNormal();
				for (auto i = 0; i < VertexCount; ++i)
				{
					Matrix_Transform3x3(Normals[i], pNormalTbl[i], mWorld);
				}

				RenderTriangle(
					pMesh->TriangleId,
					pMesh->TextureId,
					pMesh->pMeshData,
					Positions,
					Normals,
					pMesh->pMeshData->GetTexCoord(),
					VertexCount,
					pMesh->pMeshData->GetIndex(),
					pMesh->pMeshData->GetIndexCount());
			}, &_RenderMeshDatas[i]);
		}

		TaskSystem::Instance().PushBarrier();
	}

	// タイルごとにジョブを作って並列処理する
	// ・三角形のラスタライズをする
	// ・ピクセルごとの深度テストをする
	// ・ピクセルごとの法線とUVをとマテリアル情報をGBufferに書き込む
	{
		const int32 txCount = (SCREEN_WIDTH  + BUFFER_TILE_SIZE_X - 1) / BUFFER_TILE_SIZE_X;
		const int32 tyCount = (SCREEN_HEIGHT + BUFFER_TILE_SIZE_Y - 1) / BUFFER_TILE_SIZE_Y;
		for (int32 y = 0; y < tyCount; ++y)
		{
			for (int32 x = 0; x < txCount; ++x)
			{
				union PackedPosition {
					struct {
						int32 x, y;
					};
					int64 packed;
				};
				PackedPosition Position;
				Position.x = x;
				Position.y = y;

				TaskSystem::Instance().PushQue([this](void* pData) {
					PackedPosition Pos;
					Pos.packed = (int64)pData;
					RasterizeTile(Pos.x, Pos.y);
				}, (void*)Position.packed);
			}
		}

		TaskSystem::Instance().PushBarrier();
	}

	// GBufferの内容をもとにシェーディングを行うジョブを作って並列処理をする
	// ・ピクセルごとのマテリアル情報を元にテクスチャマッピングとライティングを行う
	{
		const int32 w = SCREEN_WIDTH;
		const int32 h = 5;
		const int32 yn = SCREEN_HEIGHT / h;

		for (int32 y = 0; y < yn; ++y)
		{
			union PackedRect {
				struct {
					int16 x, y, w, h;
				};
				int64 packed;
			};
			PackedRect Rect;
			Rect.x = 0;
			Rect.y = y * h;
			Rect.w = w;
			Rect.h = h;

			TaskSystem::Instance().PushQue([this](void* pData) {
				PackedRect Rc;
				Rc.packed = (int64)pData;
				DeferredShading(Rc.x, Rc.y, Rc.w, Rc.h);
			}, (void*)Rect.packed);
		}
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::RenderTriangle(uint16 TriangleId, uint16_t TextureId, const IMeshData* pMeshData, const Vector4 Positions[], const Vector3 Normals[], const Vector2 Texcoord[], const int32 VertexCount, const uint16* pIndex, const int32 IndexCount)
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

	const auto WidthF = SCREEN_WIDTH_F;
	const auto HeightF = SCREEN_HEIGHT_F;

	InternalVertex TempA[8], TempB[8];

	auto index = 0;
	while (index < IndexCount)
	{
		const auto i0 = pIndex[index++];
		const auto i1 = pIndex[index++];
		const auto i2 = pIndex[index++];

		TempA[0] = InternalVertex{ Positions[i0], Normals[i0], Texcoord[i0] };
		TempA[1] = InternalVertex{ Positions[i1], Normals[i1], Texcoord[i1] };
		TempA[2] = InternalVertex{ Positions[i2], Normals[i2], Texcoord[i2] };
		int32 PointCount = 3;

		PointCount = ClipPoints(TempB, TempA, PointCount, [](const Vector4& v) { return v.w - v.y; });
		PointCount = ClipPoints(TempA, TempB, PointCount, [](const Vector4& v) { return v.w + v.y; });
		PointCount = ClipPoints(TempB, TempA, PointCount, [](const Vector4& v) { return v.w - v.x; });
		PointCount = ClipPoints(TempA, TempB, PointCount, [](const Vector4& v) { return v.w + v.x; });
		PointCount = ClipPoints(TempB, TempA, PointCount, [](const Vector4& v) { return v.w - v.z; });
		PointCount = ClipPoints(TempA, TempB, PointCount, [](const Vector4& v) { return v.w + v.z; });

		for (int32 j = 0; j < PointCount; ++j)
		{
			auto& pt = TempA[j];
			auto invW = 1.0f / pt.Position.w;
			auto screen_x = (+pt.Position.x * invW * 0.5f + 0.5f) * WidthF;
			auto screen_y = (-pt.Position.y * invW * 0.5f + 0.5f) * HeightF;
			pt.Position.x = floorf(screen_x * 16.0f) / 16.0f;
			pt.Position.y = floorf(screen_y * 16.0f) / 16.0f;
			pt.Position.w = invW;
			pt.Position.z *= invW;
			pt.TexCoord.x *= invW;
			pt.TexCoord.y *= invW;
		}

		auto table = index_table[PointCount];
		auto& cv0 = TempA[0];
		for (int32 j = 1; j < PointCount - 1; ++j)
		{
			auto& cv1 = TempA[j];
			auto& cv2 = TempA[table[j]];	// [(i + 1) % PointCount]
			RasterizeTriangle(TriangleId, TextureId, cv0, cv1, cv2);
		}

		TriangleId++;
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::RasterizeTriangle(uint16 TriangleId, uint16 TextureId, InternalVertex v0, InternalVertex v1, InternalVertex v2)
{
	// 三角形の各位置
	auto& p0 = v0.Position;
	auto& p1 = v1.Position;
	auto& p2 = v2.Position;

	// 外積から面の向きを求めて、裏向きなら破棄する（backface-culling)
	const auto Denom = ((p1.x - p0.x) * (p2.y - p0.y)) - ((p1.y - p0.y) * (p2.x - p0.x));
	if (Denom <= 0.0f) return;

	// 補間処理の事前計算
	auto& t0 = v0.TexCoord;
	auto& t1 = v1.TexCoord;
	auto& t2 = v2.TexCoord;

	const auto InvDenom = 1.0f / Denom;
	p0.z *= InvDenom;
	p1.z *= InvDenom;
	p2.z *= InvDenom;
	p0.w *= InvDenom;
	p1.w *= InvDenom;
	p2.w *= InvDenom;
	t0.x *= InvDenom;
	t0.y *= InvDenom;
	t1.x *= InvDenom;
	t1.y *= InvDenom;
	t2.x *= InvDenom;
	t2.y *= InvDenom;

	auto bbMinX = p0.x, bbMinY = p0.y, bbMaxX = p0.x, bbMaxY = p0.y;
	if (bbMaxX < p1.x) bbMaxX = p1.x;
	if (bbMaxX < p2.x) bbMaxX = p2.x;
	if (bbMaxY < p1.y) bbMaxY = p1.y;
	if (bbMaxY < p2.y) bbMaxY = p2.y;
	if (bbMinX > p1.x) bbMinX = p1.x;
	if (bbMinX > p2.x) bbMinX = p2.x;
	if (bbMinY > p1.y) bbMinY = p1.y;
	if (bbMinY > p2.y) bbMinY = p2.y;

	const auto x0 = int16(bbMinX);
	const auto x1 = int16(bbMaxX);
	const auto y0 = int16(bbMinY);
	const auto y1 = int16(bbMaxY);

	const auto tx0 = x0 / BUFFER_TILE_SIZE_X;
	const auto tx1 = std::min(x1 / BUFFER_TILE_SIZE_X, MAX_TILE_COUNT_X - 1);
	const auto ty0 = y0 / BUFFER_TILE_SIZE_Y;
	const auto ty1 = std::min(y1 / BUFFER_TILE_SIZE_Y, MAX_TILE_COUNT_Y - 1);

	for (auto ty = ty0; ty <= ty1; ++ty)
	{
		for (auto tx = tx0; tx <= tx1; ++tx)
		{
			PushTriangleToTile(
				tx, ty,
				bbMinX, bbMinY, bbMaxX, bbMaxY,
				InvDenom,
				TriangleId, TextureId,
				v0, v1, v2);
		}
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::PushTriangleToTile(
	int32 tx, int32 ty,
	fp32 bbMinX, fp32 bbMinY, fp32 bbMaxX, fp32 bbMaxY, fp32 InvDenom,
	uint16 TriangleId, uint16 TextureId, InternalVertex v0, InternalVertex v1, InternalVertex v2)
{
	const auto minTileX = tx * BUFFER_TILE_SIZE_X;
	const auto minTileY = ty * BUFFER_TILE_SIZE_Y;
	const auto maxTileX = minTileX + BUFFER_TILE_SIZE_X - 1;
	const auto maxTileY = minTileY + BUFFER_TILE_SIZE_Y - 1;

	auto& Dst = _RasterizeDatas[ty][tx];
	auto& Tri = Dst.Triangles[Dst.Count.Increment() - 1];
	Tri.bbMinX		= std::max(int16(bbMinX), int16(minTileX));
	Tri.bbMinY		= std::max(int16(bbMinY), int16(minTileY));
	Tri.bbMaxX		= std::min(int16(bbMaxX), int16(maxTileX));
	Tri.bbMaxY		= std::min(int16(bbMaxY), int16(maxTileY));
	Tri.TriangleId	= TriangleId;
	Tri.TextureId	= TextureId;
	Tri.InvDenom	= InvDenom;
	Tri.v0			= v0;
	Tri.v1			= v1;
	Tri.v2			= v2;
}

//======================================================================================================
//
//======================================================================================================
void Renderer::RasterizeTile(int32 tx, int32 ty)
{
	auto& Src = _RasterizeDatas[ty][tx];
	const int32_t Count = Src.Count.Load();

	for (int32 i = 0; i < Count; ++i)
	{
		const auto& Tri = Src.Triangles[i];

		const auto p0 = Tri.v0.Position;
		const auto p1 = Tri.v1.Position;
		const auto p2 = Tri.v2.Position;
		const auto n0 = Tri.v0.Normal;
		const auto n1 = Tri.v1.Normal;
		const auto n2 = Tri.v2.Normal;
		const auto t0 = Tri.v0.TexCoord;
		const auto t1 = Tri.v1.TexCoord;
		const auto t2 = Tri.v2.TexCoord;

		const auto InvDenom		= Tri.InvDenom;
		const auto TextureId	= Tri.TextureId;
		const auto TriangleId	= Tri.TriangleId;

		const auto x0 = Tri.bbMinX;
		const auto x1 = Tri.bbMaxX;
		const auto y0 = Tri.bbMinY;
		const auto y1 = Tri.bbMaxY;

		const auto beign_x = fp32(x0) + 0.5f;
		const auto beign_y = fp32(y0) + 0.5f;

		const auto p2_p1_x = p2.x - p1.x;
		const auto p2_p1_y = p2.y - p1.y;
		const auto p0_p2_x = p0.x - p2.x;
		const auto p0_p2_y = p0.y - p2.y;
		const auto p1_p0_x = p1.x - p0.x;
		const auto p1_p0_y = p1.y - p0.y;

		auto b0_row = (p2_p1_x * (beign_y - p1.y)) - (p2_p1_y * (beign_x - p1.x));
		auto b1_row = (p0_p2_x * (beign_y - p2.y)) - (p0_p2_y * (beign_x - p2.x));
		auto b2_row = (p1_p0_x * (beign_y - p0.y)) - (p1_p0_y * (beign_x - p0.x));

		auto pDepthBuffer = _pDepthBuffer->GetPixelPointer(0, y0);
		auto pGBuffer = _pGBuffer->GetPixelPointer(0, y0);

		for (auto y = y0; y <= y1; ++y)
		{
			auto bRasterized = false;

			auto b0 = b0_row;
			auto b1 = b1_row;
			auto b2 = b2_row;

			for (auto x = x0; x <= x1; ++x, b0 -= p2_p1_y, b1 -= p0_p2_y, b2 -= p1_p0_y)
			{
				if (b0 < 0.0f || b1 < 0.0f || b2 < 0.0f) if (bRasterized) break; else continue;

				bRasterized = true;

				const auto z = (b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z);
				auto& DepthBuf = pDepthBuffer[x];
				if (DepthBuf <= z) continue;
				DepthBuf = z;

				const auto w = 1.0f / ((b0 * p0.w) + (b1 * p1.w) + (b2 * p2.w));
				auto& GBuff = pGBuffer[x];
				GBuff.TextureId  = TextureId;
				GBuff.TriangleId = TriangleId;
				GBuff.Normal.x   = (b0 * n0.x) + (b1 * n1.x) + (b2 * n2.x);
				GBuff.Normal.y   = (b0 * n0.y) + (b1 * n1.y) + (b2 * n2.y);
				GBuff.Normal.z   = (b0 * n0.z) + (b1 * n1.z) + (b2 * n2.z);
				GBuff.TexCoord.x = (b0 * t0.x) + (b1 * t1.x) + (b2 * t2.x);
				GBuff.TexCoord.y = (b0 * t0.y) + (b1 * t1.y) + (b2 * t2.y);
				GBuff.TexCoord.x *= w;
				GBuff.TexCoord.y *= w;
			}

			b0_row += p2_p1_x;
			b1_row += p0_p2_x;
			b2_row += p1_p0_x;

			pDepthBuffer += SCREEN_WIDTH;
			pGBuffer += SCREEN_WIDTH;
		}
	}

	Src.Count = 0;
}

//======================================================================================================
//
//======================================================================================================
void Renderer::DeferredShading(int32 x, int32 y, int32 w, int32 h)
{
	auto pGPixel = _pGBuffer->GetPixelPointer(x, y);
	auto pColorBuffer = _pColorBuffer->GetPixelPointer(x, y);

	fp32 LastU = 0.0f, LastV = 0.0f;
	uint16 TriangleId = 0xFFFF;
	const int32 count = w * h;
	for (int32 i = 0; i < count; ++i, ++pGPixel, ++pColorBuffer)
	{
		const auto GBuff = *pGPixel;

		auto bChangedTriangle = TriangleId - GBuff.TriangleId;
		TriangleId = GBuff.TriangleId;

		if (GBuff.TextureId == 0xFFFF) continue;

		Vector3 Normal;
		Vector_Normalize(Normal, GBuff.Normal);
		const auto NdotL = Vector_DotProduct(Normal, _DirectionalLight) * 0.25f + 0.75f;

		auto pTexture = _Textures[GBuff.TextureId];
		Color texel = bChangedTriangle
			? pTexture->Sample(GBuff.TexCoord.x, GBuff.TexCoord.y)
			: pTexture->Sample(GBuff.TexCoord.x, GBuff.TexCoord.y, LastU - GBuff.TexCoord.x, LastV - GBuff.TexCoord.y);
		LastU = GBuff.TexCoord.x;
		LastV = GBuff.TexCoord.y;

		const auto Brightness = uint32(NdotL * 128.0f);
		pColorBuffer->r = (texel.r * Brightness) >> 7;
		pColorBuffer->g = (texel.g * Brightness) >> 7;
		pColorBuffer->b = (texel.b * Brightness) >> 7;
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::SetTexture(Texture& Texture)
{
	if (Texture.GetSurfaceCount() == 0)
	{
		_CurrentTexture = &_DummyTexture;
		_CurrentTextureId = int32(_Textures.size());
		_Textures.push_back(&_DummyTexture);
	}
	else
	{
		_CurrentTexture = &Texture;
		_CurrentTextureId = int32(_Textures.size());
		_Textures.push_back(&Texture);
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::SetDirectionalLight(const Vector3& Direction)
{
	Vector_Normalize(_DirectionalLight, Direction);
	Vector_Mul(_DirectionalLight, _DirectionalLight, -1.0f);
}

//======================================================================================================
//
//======================================================================================================
void Renderer::DrawIndexed(const IMeshData* pMeshData, const Matrix& mWorld)
{
	_RenderMeshDatas.emplace_back(RenderMeshData{ pMeshData,_CurrentTriangleId++, _CurrentTextureId, mWorld });
}
