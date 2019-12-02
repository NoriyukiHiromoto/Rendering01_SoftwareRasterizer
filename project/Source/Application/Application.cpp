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
#include <Application/Application.h>
#include <Math/Math.h>

//======================================================================================================
// 初期化処理
//======================================================================================================
bool Application::OnInitialize()
{
	// レンダラーの生成
	_pRenderer = new Renderer();

	// カメラの初期状態
	_CameraDistance = 9.65f;
	Vector_Set(_CameraTarget, 0.0f, 3.0f, 0.0f, 1.0f);
	Vector_Set(_CameraAngle, -0.29f, 1.76f, 0.0f, 0.0f);

	// モデルの読み込み
	ModelLoad("resources\\sponza.mbin");

	return true;
}

//======================================================================================================
// 終了処理
//======================================================================================================
void Application::OnFinalize()
{
	delete _pRenderer;
}

//======================================================================================================
// 更新処理
//======================================================================================================
void Application::OnUpdate(fp32 FrameTime)
{
	//--------------------------------------------------------------------
	// カメラの行列を作る
	//--------------------------------------------------------------------
	// ビュー行列
	Matrix mRotateX, mRotateY;
	Matrix_RotationX(mRotateX, _CameraAngle.x);
	Matrix_RotationY(mRotateY, _CameraAngle.y);

	Vector4 Eye = { 0.0f, 0.0f, _CameraDistance, 1.0f };
	Matrix_Transform3x3(Eye, Eye, mRotateX);
	Matrix_Transform3x3(Eye, Eye, mRotateY);
	Vector4 At = _CameraTarget;
	Vector_Add(Eye, Eye, At);
	Matrix_CreateLookAtView(_mView, Eye, At, Vector4::Y);
	// プロジェクション行列
	Matrix_CreateProjection(_mProj, 0.01f, 200.0f, ToRadian(45.0f), SCREEN_WIDTH_F / SCREEN_HEIGHT_F);
}

//======================================================================================================
// レンダリング処理
//======================================================================================================
void Application::OnRendering(ColorBuffer* pColorBuffer, DepthBuffer* pDepthBuffer, GBuffer* pGBuffer)
{
	_VertexCount = 0;
	_TriangleCount = 0;

	// 描画を開始する
	_pRenderer->BeginDraw(pColorBuffer, pDepthBuffer, pGBuffer, _mView, _mProj);

	_pRenderer->SetDirectionalLight(Vector3{ 1.0f, -2.0f, 5.0f });

	// メッシュの描画
	for (auto&& Mesh : _MeshDatas)
	{
		_pRenderer->SetTexture(Mesh._Texture);
		_pRenderer->DrawIndexed(&Mesh, Matrix::IDENTITY);

		_VertexCount += Mesh.GetVertexCount();
		_TriangleCount += Mesh.GetIndexCount() / 3;
	}

	// 描画を完了する
	_pRenderer->EndDraw();
}

//======================================================================================================
// マウス左ドラッグ
//======================================================================================================
void Application::OnLefeMouseDrag(int32 x, int32 y)
{
	_CameraAngle.x -= fp32(y) * 0.005f;
	_CameraAngle.y += fp32(x) * 0.005f;

	if (_CameraAngle.x < ToRadian(-88.0f)) _CameraAngle.x = ToRadian(-88.0f);
	if (_CameraAngle.x > ToRadian(+88.0f)) _CameraAngle.x = ToRadian(+88.0f);
	while (_CameraAngle.y < ToRadian(-180.0f)) _CameraAngle.y += ToRadian(360.0f);
	while (_CameraAngle.y > ToRadian(+180.0f)) _CameraAngle.y -= ToRadian(360.0f);
}

//======================================================================================================
// マウス右ドラッグ
//======================================================================================================
void Application::OnRightMouseDrag(int32 x, int32 y)
{
	_CameraDistance += fp32(y - x) * 0.001f * _CameraDistance;
	if (_CameraDistance < 0.001f) _CameraDistance = 0.001f;
}

//======================================================================================================
// マウスホイールのドラッグ
//======================================================================================================
void Application::OnWheelMouseDrag(int32 x, int32 y)
{
	_CameraTarget.x -= _mView.x.x * fp32(x) * 0.003f;
	_CameraTarget.y -= _mView.y.x * fp32(x) * 0.003f;
	_CameraTarget.z -= _mView.z.x * fp32(x) * 0.003f;
	_CameraTarget.x += _mView.x.y * fp32(y) * 0.003f;
	_CameraTarget.y += _mView.y.y * fp32(y) * 0.003f;
	_CameraTarget.z += _mView.z.y * fp32(y) * 0.003f;
}

//======================================================================================================
// ファイルの読み込み処理
//======================================================================================================
void Application::ModelLoad(const char* pFileName)
{
	// 同一頂点をマージするためのローカル関数
	std::map<VertexData, uint16> VertexMap;
	auto AddVertex = [&](VertexData v, MeshData& Dst)
	{
		// すでに存在する場合はインデックスを返す
		auto it = VertexMap.find(v);
		if (it != VertexMap.end())
		{
			return (*it).second;
		}

		// 新規に追加してそのインデックスを返す
		const uint16 LocalIndex = uint16(Dst._Position.size());

		Dst._Position.push_back(Vector3{ v.Position[0], v.Position[1], v.Position[2] });
		Dst._Normal.push_back(Vector3{ v.Normal[0], v.Normal[1], v.Normal[2] });
		Dst._TexCoord.push_back(Vector2{ v.TexCoord[0], v.TexCoord[1] });

		VertexMap.insert(std::make_pair(v, LocalIndex));
		return LocalIndex;
	};

	// ファイルパスの作成
	std::string FullPath = pFileName;
	size_t path_i = FullPath.find_last_of("\\") + 1;
	size_t ext_i = FullPath.find_last_of(".");
	std::string Dir = FullPath.substr(0, path_i);
	std::string ExtName = FullPath.substr(ext_i, FullPath.size() - ext_i);
	std::string FileName = FullPath.substr(path_i, ext_i - path_i);

	_MeshDatas = std::vector<MeshData>();

	// メッシュファイルを読み込み
	HANDLE hFile = ::CreateFileA(pFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD ReadedBytes;

		MeshFileBinaryHead Head;
		::ReadFile(hFile, &Head, sizeof(Head), &ReadedBytes, nullptr);

		if (Head.GUID == 'MBIN')
		{
			_MeshDatas.resize(Head.MeshCount);
			for (auto iMesh = 0U; iMesh < Head.MeshCount; ++iMesh)
			{
				auto& Dst = _MeshDatas[iMesh];

				MeshFileBinary MeshBin;
				::ReadFile(hFile, &MeshBin, sizeof(MeshBin), &ReadedBytes, nullptr);

				// テクスチャの読み込み
				Dst._Texture.Load((Dir + MeshBin.TextureName + ".dds").c_str());

				// ジオメトリデータ読み込み
				std::vector<VertexData> VertexDatas(MeshBin.TriangleVertexCount);
				::ReadFile(hFile, &(VertexDatas[0]), sizeof(VertexData) * MeshBin.TriangleVertexCount, &ReadedBytes, nullptr);

				// 頂点データ＆頂点インデックス
				for (auto&& v : VertexDatas)
				{
					Dst._Index.push_back(AddVertex(v, Dst));
				}
			}
		}

		::CloseHandle(hFile);
	}
}
