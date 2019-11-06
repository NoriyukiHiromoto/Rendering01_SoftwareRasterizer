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

//======================================================================================================
//
//======================================================================================================
struct IMeshData
{
	virtual const int32 GetVertexCount() const = 0;
	virtual const int32 GetIndexCount() const = 0;

	virtual const Vector3* const GetPosition() const = 0;
	virtual const uint16* const GetIndex() const = 0;
};

struct MeshData : public IMeshData
{
	std::vector<Vector3>	_Position;
	std::vector<uint16>		_Index;

	virtual const int32 GetVertexCount() const { return int32(_Position.size()); }
	virtual const int32 GetIndexCount() const { return int32(_Index.size()); }

	virtual const Vector3* const GetPosition() const { return &_Position[0]; }
	virtual const uint16* const GetIndex() const { return &_Index[0]; }
};

struct RenderMeshData
{
	const IMeshData*	pMeshData;
	Matrix				mWorld;
};

//======================================================================================================
//
//======================================================================================================
typedef FrameBuffer<Color>			ColorBuffer;

//======================================================================================================
//
//======================================================================================================
class Renderer
{
	ColorBuffer*				_pColorBuffer;
	std::vector<RenderMeshData>	_RenderMeshDatas;
	Matrix						_ViewMatrix;
	Matrix						_ProjMatrix;

public:
	Renderer();
	~Renderer();

private:
	void RenderTriangle(const IMeshData* pMeshData, const Vector4 Positions[], const int32 VertexCount, const uint16* pIndex, const int32 IndexCount);

public:
	void BeginDraw(ColorBuffer* pColorBuffer, const Matrix& mView, const Matrix& mProj);
	void EndDraw();

	void DrawIndexed(const IMeshData* pMeshData, const Matrix& mWorld);
};
