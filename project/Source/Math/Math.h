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
static const fp32 PI = 3.1415926535897932384626433832795f;
static const fp32 PI2 = 6.283185307179586476925286766559f;

inline fp32 ToDegree(fp32 value) { return value * 180.0f / PI; }
inline fp32 ToRadian(fp32 value) { return value / 180.0f * PI; }

//======================================================================================================
//
//======================================================================================================
typedef struct _Vector2
{
	static const _Vector2 ZERO;
	static const _Vector2 X;
	static const _Vector2 Y;

	fp32 x, y;
}
Vector2;

//======================================================================================================
//
//======================================================================================================
typedef struct _Vector3
{
	static const _Vector3 ZERO;
	static const _Vector3 X;
	static const _Vector3 Y;
	static const _Vector3 Z;

	fp32 x, y, z;

	Vector2& RefVec2() { return *reinterpret_cast<Vector2*>(this); }
	const Vector2& RefVec2() const { return *reinterpret_cast<const Vector2*>(this); }
}
Vector3;

//======================================================================================================
//
//======================================================================================================
typedef struct _Vector4
{
	static const _Vector4 ZERO;
	static const _Vector4 X;
	static const _Vector4 Y;
	static const _Vector4 Z;
	static const _Vector4 W;

	fp32 x, y, z, w;

	Vector2& RefVec2() { return *reinterpret_cast<Vector2*>(this); }
	const Vector2& RefVec2() const { return *reinterpret_cast<const Vector2*>(this); }
	Vector3& RefVec3() { return *reinterpret_cast<Vector3*>(this); }
	const Vector3& RefVec3() const { return *reinterpret_cast<const Vector3*>(this); }
}
Vector4;

//======================================================================================================
//
//======================================================================================================
typedef struct _Matrix4x4
{
	static const _Matrix4x4 IDENTITY;

	Vector4 x;
	Vector4 y;
	Vector4 z;
	Vector4 w;
}
Matrix;

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Set(Vector2& Result, fp32 x, fp32 y, fp32 z);
Vector2& Vector_Add(Vector2& Result, const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Sub(Vector2& Result, const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Mul(Vector2& Result, const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Div(Vector2& Result, const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Add(Vector2& Result, const Vector2& lhs, const fp32 rhs);
Vector2& Vector_Sub(Vector2& Result, const Vector2& lhs, const fp32 rhs);
Vector2& Vector_Mul(Vector2& Result, const Vector2& lhs, const fp32 rhs);
Vector2& Vector_Div(Vector2& Result, const Vector2& lhs, const fp32 rhs);
Vector2& Vector_Min(Vector2& Result, const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Max(Vector2& Result, const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Normalize(Vector2& Result, const Vector2& Input);
Vector2& Vector_NormalizeSafe(Vector2& Result, const Vector2& Input);
fp32 Vector_CrossProduct(const Vector2& lhs, const Vector2& rhs);
Vector2& Vector_Lerp(Vector2& Result, const Vector2& lhs, const Vector2& rhs, const fp32 rate);
fp32 Vector_DotProduct(const Vector2& lhs, const Vector2& rhs);
fp32 Vector_LengthSq(const Vector2& Input);
fp32 Vector_Length(const Vector2& Input);

Vector3& Vector_Set(Vector3& Result, fp32 x, fp32 y, fp32 z);
Vector3& Vector_Add(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Sub(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Mul(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Div(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Add(Vector3& Result, const Vector3& lhs, const fp32 rhs);
Vector3& Vector_Sub(Vector3& Result, const Vector3& lhs, const fp32 rhs);
Vector3& Vector_Mul(Vector3& Result, const Vector3& lhs, const fp32 rhs);
Vector3& Vector_Div(Vector3& Result, const Vector3& lhs, const fp32 rhs);
Vector3& Vector_Min(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Max(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Normalize(Vector3& Result, const Vector3& Input);
Vector3& Vector_NormalizeSafe(Vector3& Result, const Vector3& Input);
Vector3& Vector_CrossProduct(Vector3& Result, const Vector3& lhs, const Vector3& rhs);
Vector3& Vector_Lerp(Vector3& Result, const Vector3& lhs, const Vector3& rhs, const fp32 rate);
fp32 Vector_DotProduct(const Vector3& lhs, const Vector3& rhs);
fp32 Vector_LengthSq(const Vector3& Input);
fp32 Vector_Length(const Vector3& Input);

Vector4& Vector_Set(Vector4& Result, fp32 x, fp32 y, fp32 z, fp32 w);
Vector4& Vector_Add(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Sub(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Mul(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Div(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Add(Vector4& Result, const Vector4& lhs, const fp32 rhs);
Vector4& Vector_Sub(Vector4& Result, const Vector4& lhs, const fp32 rhs);
Vector4& Vector_Mul(Vector4& Result, const Vector4& lhs, const fp32 rhs);
Vector4& Vector_Div(Vector4& Result, const Vector4& lhs, const fp32 rhs);
Vector4& Vector_Min(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Max(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Normalize(Vector4& Result, const Vector4& Input);
Vector4& Vector_NormalizeSafe(Vector4& Result, const Vector4& Input);
Vector4& Vector_Normalize3(Vector4& Result, const Vector4& Input);
Vector4& Vector_NormalizeSafe3(Vector4& Result, const Vector4& Input);
Vector4& Vector_CrossProduct(Vector4& Result, const Vector4& lhs, const Vector4& rhs);
Vector4& Vector_Lerp(Vector4& Result, const Vector4& lhs, const Vector4& rhs, const fp32 rate);
fp32 Vector_DotProduct(const Vector4& lhs, const Vector4& rhs);
fp32 Vector_DotProduct3(const Vector4& lhs, const Vector4& rhs);
fp32 Vector_LengthSq(const Vector4& Input);
fp32 Vector_LengthSq3(const Vector4& Input);
fp32 Vector_Length(const Vector4& Input);
fp32 Vector_Length3(const Vector4& Input);

void Matrix_CreateLookAt(Matrix& Result, const Vector4& Position, const Vector4& Target, const Vector4& Up);
void Matrix_CreateLookAtView(Matrix& Result, const Vector4& Position, const Vector4& Target, const Vector4& Up);
void Matrix_CreateProjection(Matrix& Result, fp32 NearClip, fp32 FarClip, fp32 FovY, fp32 Aspect);
void Matrix_Inverse(Matrix& Result, const Matrix& Input);
void Matrix_Transpose(Matrix& Result, const Matrix& Input);
void Matrix_Multiply4x4(Matrix& Result, const Matrix& lhs, const Matrix& rhs);
void Matrix_Translation(Matrix& Result, fp32 x, fp32 y, fp32 z);
void Matrix_RotationX(Matrix& Result, fp32 Radian);
void Matrix_RotationY(Matrix& Result, fp32 Radian);
void Matrix_RotationZ(Matrix& Result, fp32 Radian);
void Matrix_Transform3x3(Vector3& Result, const Vector3 lhs, const Matrix& rhs);
void Matrix_Transform3x3(Vector4& Result, const Vector4 lhs, const Matrix& rhs);
void Matrix_Transform4x4(Vector4& Result, const Vector4 lhs, const Matrix& rhs);
void Matrix_Transform4x4(Vector4& Result, const Vector3 lhs, const Matrix& rhs);
void Matrix_Transform4x4Projection(Vector4& Result, const Vector4 lhs, const Matrix& rhs);
void Matrix_Transform4x4Projection(Vector4& Result, const Vector3 lhs, const Matrix& rhs);
