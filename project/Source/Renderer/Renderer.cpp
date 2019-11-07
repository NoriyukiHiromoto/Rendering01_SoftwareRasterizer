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
	, _pDepthBuffer(nullptr)
{
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
void Renderer::BeginDraw(ColorBuffer* pColorBuffer, DepthBuffer* pDepthBuffer, const Matrix& mView, const Matrix& mProj)
{
	_pColorBuffer = pColorBuffer;
	_pDepthBuffer = pDepthBuffer;
	_ViewMatrix = mView;
	_ProjMatrix = mProj;

	_RenderMeshDatas.clear();

	_CurrentTextureId = 0;
	_Textures.clear();
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
			auto pNormalTbl = Mesh.pMeshData->GetNormal();
			const auto VertexCount = Mesh.pMeshData->GetVertexCount();
			ASSERT(VertexCount <= MAX_VERTEX_CACHE_SIZE);
			static Vector4 Positions[MAX_VERTEX_CACHE_SIZE];
			for (auto i = 0; i < VertexCount; ++i)
			{
				Matrix_Transform4x4(Positions[i], pPosTbl[i], mWorldViewProj);
			}
			static Vector3 Normals[MAX_VERTEX_CACHE_SIZE];
			for (auto i = 0; i < VertexCount; ++i)
			{
				Matrix_Transform3x3(Normals[i], pNormalTbl[i], Mesh.mWorld);
			}

			RenderTriangle(
				Mesh.TextureId,
				Mesh.pMeshData,
				Positions,
				Normals,
				Mesh.pMeshData->GetTexCoord(),
				VertexCount,
				Mesh.pMeshData->GetIndex(),
				Mesh.pMeshData->GetIndexCount());
		}
	}
}

//======================================================================================================
//
//======================================================================================================
void Renderer::RasterizeTriangle(uint16 TextureId, InternalVertex v0, InternalVertex v1, InternalVertex v2)
{
	// 三角形の各位置
	auto& p0 = v0.Position;
	auto& p1 = v1.Position;
	auto& p2 = v2.Position;

	// 三角形の各法線
	auto& n0 = v0.Normal;
	auto& n1 = v1.Normal;
	auto& n2 = v2.Normal;

	// 三角形の各UV
	auto& t0 = v0.TexCoord;
	auto& t1 = v1.TexCoord;
	auto& t2 = v2.TexCoord;

	// 外積から面の向きを求めて、裏向きなら破棄する（backface-culling)
	const auto Denom = EdgeFunc(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
	if (Denom <= 0.0f) return;

	const auto InvDenom = 1.0f / Denom;
	p0.z *= p0.w;
	p1.z *= p1.w;
	p2.z *= p2.w;

	// ポリゴンのバウンディングを求める
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

	auto pDepthBuffer = _pDepthBuffer->GetPixelPointer(x0, y0);
	auto pColorBuffer = _pColorBuffer->GetPixelPointer(x0, y0);

	fp32 LastU = 0.0f, LastV = 0.0f;

	// 求めたバウンディング内をforで回す
	fp32 py = fp32(y0) + 0.5f;
	for (int32 y = y0; y <= y1; ++y, py += 1.0f)
	{
		bool bFirstSampling = true;
		int32 x_offset = 0;
		fp32 px = fp32(x0) + 0.5f;
		for (auto x = x0; x <= x1; ++x, px += 1.0f, ++x_offset)
		{
			// 辺1-2に対して内外判定
			auto b0 = EdgeFunc(p1.x, p1.y, p2.x, p2.y, px, py);
			if (b0 < 0.0f) continue;
			// 辺2-0に対して内外判定
			auto b1 = EdgeFunc(p2.x, p2.y, p0.x, p0.y, px, py);
			if (b1 < 0.0f) continue;
			// 辺0-1に対して内外判定
			auto b2 = EdgeFunc(p0.x, p0.y, p1.x, p1.y, px, py);
			if (b2 < 0.0f) continue;

			b0 *= InvDenom;
			b1 *= InvDenom;
			b2 *= InvDenom;

			// 重心座標系でz値を求める
			const auto Depth = (b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z);
			auto& DepthBuf = pDepthBuffer[x_offset];
			if (Depth >= DepthBuf) continue;
			DepthBuf = Depth;

			// 重心座標系で法線を求める
			Vector3 Normal = {
				((b0 * n0.x) + (b1 * n1.x) + (b2 * n2.x)),
				((b0 * n0.y) + (b1 * n1.y) + (b2 * n2.y)),
				((b0 * n0.z) + (b1 * n1.z) + (b2 * n2.z)),
			};

			// 適当な感じでライティングする
			Vector_Normalize(Normal, Normal);
			const auto NdotL = Vector_DotProduct(Normal, _DirectionalLight) * 0.25f + 0.75f;

			const auto Brightness = uint32(NdotL * 128.0f);

			// 重心座標系でwを求める
			const auto w = 1.0f / ((b0 * p0.w) + (b1 * p1.w) + (b2 * p2.w));

			// 重心座標系でUVを求める
			// パースペクティブコレクト対応のため1/wする
			Vector2 TexCoord = {
				((b0 * t0.x) + (b1 * t1.x) + (b2 * t2.x)) * w,
				((b0 * t0.y) + (b1 * t1.y) + (b2 * t2.y)) * w,
			};

			// 求めたUVからテクスチャの色をとってくる
			auto pTexture = _Textures[TextureId];
			Color texel = bFirstSampling
				? pTexture->Sample(TexCoord.x, TexCoord.y)
				: pTexture->Sample(TexCoord.x, TexCoord.y, LastU - TexCoord.x, LastV - TexCoord.y);
			bFirstSampling = false;
			LastU = TexCoord.x;
			LastV = TexCoord.y;

			// テクスチャの色にライtの結果を乗算
			auto& ColorBuff = pColorBuffer[x_offset];
			ColorBuff.r = (texel.r * Brightness) >> 7;
			ColorBuff.g = (texel.g * Brightness) >> 7;
			ColorBuff.b = (texel.b * Brightness) >> 7;
		}

		pDepthBuffer += SCREEN_WIDTH;
		pColorBuffer += SCREEN_WIDTH;
	}
}

//======================================================================================================
//
//======================================================================================================
fp32 Renderer::EdgeFunc(const Vector2& a, const Vector2& b, const Vector2& c)
{
	Vector2 d1, d2;
	return Vector_CrossProduct(Vector_Sub(d1, b, a), Vector_Sub(d2, c, a));
}

//======================================================================================================
//
//======================================================================================================
fp32 Renderer::EdgeFunc(const fp32 ax, const fp32 ay, const fp32 bx, const fp32 by, const fp32 cx, const fp32 cy)
{
	return ((bx - ax) * (cy - ay)) - ((by - ay) * (cx - ax));
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
	_RenderMeshDatas.emplace_back(RenderMeshData{ pMeshData, _CurrentTextureId, mWorld });
}

//======================================================================================================
//
//======================================================================================================
void Renderer::RenderTriangle(uint16_t TextureId, const IMeshData* pMeshData, const Vector4 Positions[], const Vector3 Normals[], const Vector2 Texcoord[], const int32 VertexCount, const uint16* pIndex, const int32 IndexCount)
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

	const auto WidthF  = SCREEN_WIDTH_F;
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
			pt.Position.x = (+pt.Position.x * invW * 0.5f + 0.5f) * WidthF;
			pt.Position.y = (-pt.Position.y * invW * 0.5f + 0.5f) * HeightF;
			pt.Position.w = invW;
			pt.TexCoord.x *= invW;
			pt.TexCoord.y *= invW;
		}

		auto table = index_table[PointCount];
		auto& cv0 = TempA[0];
		for (int32 j = 1; j < PointCount - 1; ++j)
		{
			auto& cv1 = TempA[j];
			auto& cv2 = TempA[table[j]];	// [(i + 1) % PointCount]
			RasterizeTriangle(TextureId, cv0, cv1, cv2);
		}
	}
}
