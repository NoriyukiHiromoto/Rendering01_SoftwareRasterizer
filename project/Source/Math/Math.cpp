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
#include <Math/Math.h>

//======================================================================================================
//
//======================================================================================================
const Vector2	Vector2::ZERO = { 0.0f, 0.0f };
const Vector2	Vector2::X = { 1.0f, 0.0f };
const Vector2	Vector2::Y = { 0.0f, 1.0f };

const Vector3	Vector3::ZERO = { 0.0f, 0.0f, 0.0f };
const Vector3	Vector3::X = { 1.0f, 0.0f, 0.0f };
const Vector3	Vector3::Y = { 0.0f, 1.0f, 0.0f };
const Vector3	Vector3::Z = { 0.0f, 0.0f, 1.0f };

const Vector4	Vector4::ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
const Vector4	Vector4::X = { 1.0f, 0.0f, 0.0f, 0.0f };
const Vector4	Vector4::Y = { 0.0f, 1.0f, 0.0f, 0.0f };
const Vector4	Vector4::Z = { 0.0f, 0.0f, 1.0f, 0.0f };
const Vector4	Vector4::W = { 0.0f, 0.0f, 0.0f, 1.0f };

const Matrix	Matrix::IDENTITY = { Vector4::X, Vector4::Y, Vector4::Z, Vector4::W };

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Set(Vector2& Result, fp32 x, fp32 y, fp32 z)
{
	Result.x = x;
	Result.y = y;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Add(Vector2& Result, const Vector2& lhs, const Vector2& rhs)
{
	Result.x = lhs.x + rhs.x;
	Result.y = lhs.y + rhs.y;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Sub(Vector2& Result, const Vector2& lhs, const Vector2& rhs)
{
	Result.x = lhs.x - rhs.x;
	Result.y = lhs.y - rhs.y;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Mul(Vector2& Result, const Vector2& lhs, const Vector2& rhs)
{
	Result.x = lhs.x * rhs.x;
	Result.y = lhs.y * rhs.y;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Div(Vector2& Result, const Vector2& lhs, const Vector2& rhs)
{
	Result.x = lhs.x / rhs.x;
	Result.y = lhs.y / rhs.y;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Add(Vector2& Result, const Vector2& lhs, const fp32 rhs)
{
	Result.x = lhs.x + rhs;
	Result.y = lhs.y + rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Sub(Vector2& Result, const Vector2& lhs, const fp32 rhs)
{
	Result.x = lhs.x - rhs;
	Result.y = lhs.y - rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Mul(Vector2& Result, const Vector2& lhs, const fp32 rhs)
{
	Result.x = lhs.x * rhs;
	Result.y = lhs.y * rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Div(Vector2& Result, const Vector2& lhs, const fp32 rhs)
{
	Result.x = lhs.x / rhs;
	Result.y = lhs.y / rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Min(Vector2& Result, const Vector2& lhs, const Vector2& rhs)
{
	Result.x = std::min(lhs.x, rhs.x);
	Result.y = std::min(lhs.y, rhs.y);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Max(Vector2& Result, const Vector2& lhs, const Vector2& rhs)
{
	Result.x = std::max(lhs.x, rhs.x);
	Result.y = std::max(lhs.y, rhs.y);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Normalize(Vector2& Result, const Vector2& Input)
{
	const fp32 Length = 1.0f / Vector_Length(Input);
	Result.x = Input.x * Length;
	Result.y = Input.y * Length;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_NormalizeSafe(Vector2& Result, const Vector2& Input)
{
	const fp32 Length = 1.0f / Vector_Length(Input);
	if (Length > FLT_EPSILON)
	{
		Result.x = Input.x * Length;
		Result.y = Input.y * Length;
	}
	return Result;
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_CrossProduct(const Vector2& lhs, const Vector2& rhs)
{
	return (lhs.x * rhs.y) - (lhs.y * rhs.x);
}

//======================================================================================================
//
//======================================================================================================
Vector2& Vector_Lerp(Vector2& Result, const Vector2& lhs, const Vector2& rhs, const fp32 rate)
{
	Result.x = lhs.x + (rhs.x - lhs.x) * rate;
	Result.y = lhs.y + (rhs.y - lhs.y) * rate;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_DotProduct(const Vector2& lhs, const Vector2& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_LengthSq(const Vector2& Input)
{
	return (Input.x * Input.x) + (Input.y * Input.y);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_Length(const Vector2& Input)
{
	return sqrtf(Vector_LengthSq(Input));
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_Length3(const Vector2& Input)
{
	return sqrtf(Vector_LengthSq(Input));
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Set(Vector3& Result, fp32 x, fp32 y, fp32 z)
{
	Result.x = x;
	Result.y = y;
	Result.z = z;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Add(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = lhs.x + rhs.x;
	Result.y = lhs.y + rhs.y;
	Result.z = lhs.z + rhs.z;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Sub(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = lhs.x - rhs.x;
	Result.y = lhs.y - rhs.y;
	Result.z = lhs.z - rhs.z;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Mul(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = lhs.x * rhs.x;
	Result.y = lhs.y * rhs.y;
	Result.z = lhs.z * rhs.z;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Div(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = lhs.x / rhs.x;
	Result.y = lhs.y / rhs.y;
	Result.z = lhs.z / rhs.z;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Add(Vector3& Result, const Vector3& lhs, const fp32 rhs)
{
	Result.x = lhs.x + rhs;
	Result.y = lhs.y + rhs;
	Result.z = lhs.z + rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Sub(Vector3& Result, const Vector3& lhs, const fp32 rhs)
{
	Result.x = lhs.x - rhs;
	Result.y = lhs.y - rhs;
	Result.z = lhs.z - rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Mul(Vector3& Result, const Vector3& lhs, const fp32 rhs)
{
	Result.x = lhs.x * rhs;
	Result.y = lhs.y * rhs;
	Result.z = lhs.z * rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Div(Vector3& Result, const Vector3& lhs, const fp32 rhs)
{
	Result.x = lhs.x / rhs;
	Result.y = lhs.y / rhs;
	Result.z = lhs.z / rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Min(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = std::min(lhs.x, rhs.x);
	Result.y = std::min(lhs.y, rhs.y);
	Result.z = std::min(lhs.z, rhs.z);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Max(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = std::max(lhs.x, rhs.x);
	Result.y = std::max(lhs.y, rhs.y);
	Result.z = std::max(lhs.z, rhs.z);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Normalize(Vector3& Result, const Vector3& Input)
{
	const auto Length = Vector_Length(Input);
	const auto InvLength = 1.0f / Length;
	Result.x = Input.x * InvLength;
	Result.y = Input.y * InvLength;
	Result.z = Input.z * InvLength;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_NormalizeSafe(Vector3& Result, const Vector3& Input)
{
	const auto Length = Vector_Length(Input);
	if (Length > FLT_EPSILON)
	{
		const auto InvLength = 1.0f / Length;
		Result.x = Input.x * InvLength;
		Result.y = Input.y * InvLength;
		Result.z = Input.z * InvLength;
	}
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_CrossProduct(Vector3& Result, const Vector3& lhs, const Vector3& rhs)
{
	Result.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
	Result.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
	Result.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector3& Vector_Lerp(Vector3& Result, const Vector3& lhs, const Vector3& rhs, const fp32 rate)
{
	Result.x = lhs.x + (rhs.x - lhs.x) * rate;
	Result.y = lhs.y + (rhs.y - lhs.y) * rate;
	Result.z = lhs.z + (rhs.z - lhs.z) * rate;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_DotProduct(const Vector3& lhs, const Vector3& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_LengthSq(const Vector3& Input)
{
	return (Input.x * Input.x) + (Input.y * Input.y) + (Input.z * Input.z);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_Length(const Vector3& Input)
{
	return sqrtf(Vector_LengthSq(Input));
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_Length3(const Vector3& Input)
{
	return sqrtf(Vector_LengthSq(Input));
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Set(Vector4& Result, fp32 x, fp32 y, fp32 z, fp32 w)
{
	Result.x = x;
	Result.y = y;
	Result.z = z;
	Result.w = w;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Add(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = lhs.x + rhs.x;
	Result.y = lhs.y + rhs.y;
	Result.z = lhs.z + rhs.z;
	Result.w = lhs.w + rhs.w;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Sub(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = lhs.x - rhs.x;
	Result.y = lhs.y - rhs.y;
	Result.z = lhs.z - rhs.z;
	Result.w = lhs.w - rhs.w;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Mul(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = lhs.x * rhs.x;
	Result.y = lhs.y * rhs.y;
	Result.z = lhs.z * rhs.z;
	Result.w = lhs.w * rhs.w;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Div(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = lhs.x / rhs.x;
	Result.y = lhs.y / rhs.y;
	Result.z = lhs.z / rhs.z;
	Result.w = lhs.w / rhs.w;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Add(Vector4& Result, const Vector4& lhs, const fp32 rhs)
{
	Result.x = lhs.x + rhs;
	Result.y = lhs.y + rhs;
	Result.z = lhs.z + rhs;
	Result.w = lhs.w + rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Sub(Vector4& Result, const Vector4& lhs, const fp32 rhs)
{
	Result.x = lhs.x - rhs;
	Result.y = lhs.y - rhs;
	Result.z = lhs.z - rhs;
	Result.w = lhs.w - rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Mul(Vector4& Result, const Vector4& lhs, const fp32 rhs)
{
	Result.x = lhs.x * rhs;
	Result.y = lhs.y * rhs;
	Result.z = lhs.z * rhs;
	Result.w = lhs.w * rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Div(Vector4& Result, const Vector4& lhs, const fp32 rhs)
{
	Result.x = lhs.x / rhs;
	Result.y = lhs.y / rhs;
	Result.z = lhs.z / rhs;
	Result.w = lhs.w / rhs;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Min(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = std::min(lhs.x, rhs.x);
	Result.y = std::min(lhs.y, rhs.y);
	Result.z = std::min(lhs.z, rhs.z);
	Result.w = std::min(lhs.w, rhs.w);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Max(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = std::max(lhs.x, rhs.x);
	Result.y = std::max(lhs.y, rhs.y);
	Result.z = std::max(lhs.z, rhs.z);
	Result.w = std::max(lhs.w, rhs.w);
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Normalize3(Vector4& Result, const Vector4& Input)
{
	const auto Length = Vector_Length(Input);
	const auto InvLength = 1.0f / Length;
	Result.x = Input.x * InvLength;
	Result.y = Input.y * InvLength;
	Result.z = Input.z * InvLength;
	Result.w = 1.0;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_NormalizeSafe3(Vector4& Result, const Vector4& Input)
{
	const auto Length = Vector_Length(Input);
	if (Length > FLT_EPSILON)
	{
		const auto InvLength = 1.0f / Length;
		Result.x = Input.x * InvLength;
		Result.y = Input.y * InvLength;
		Result.z = Input.z * InvLength;
		Result.w = 1.0;
	}
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Normalize(Vector4& Result, const Vector4& Input)
{
	const auto Length = Vector_Length(Input);
	const auto InvLength = 1.0f / Length;
	Result.x = Input.x * InvLength;
	Result.y = Input.y * InvLength;
	Result.z = Input.z * InvLength;
	Result.w = Input.w * InvLength;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_NormalizeSafe(Vector4& Result, const Vector4& Input)
{
	const auto Length = Vector_Length(Input);
	if (Length > FLT_EPSILON)
	{
		const auto InvLength = 1.0f / Length;
		Result.x = Input.x * InvLength;
		Result.y = Input.y * InvLength;
		Result.z = Input.z * InvLength;
		Result.w = Input.w * InvLength;
	}
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_CrossProduct(Vector4& Result, const Vector4& lhs, const Vector4& rhs)
{
	Result.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
	Result.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
	Result.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
	Result.w = 1.0f;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
Vector4& Vector_Lerp(Vector4& Result, const Vector4& lhs, const Vector4& rhs, const fp32 rate)
{
	Result.x = lhs.x + (rhs.x - lhs.x) * rate;
	Result.y = lhs.y + (rhs.y - lhs.y) * rate;
	Result.z = lhs.z + (rhs.z - lhs.z) * rate;
	Result.w = lhs.w + (rhs.w - lhs.w) * rate;
	return Result;
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_DotProduct(const Vector4& lhs, const Vector4& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_DotProduct3(const Vector4& lhs, const Vector4& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_LengthSq(const Vector4& Input)
{
	return (Input.x * Input.x) + (Input.y * Input.y) + (Input.z * Input.z) + (Input.w * Input.w);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_LengthSq3(const Vector4& Input)
{
	return (Input.x * Input.x) + (Input.y * Input.y) + (Input.z * Input.z);
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_Length(const Vector4& Input)
{
	return sqrtf(Vector_LengthSq(Input));
}

//======================================================================================================
//
//======================================================================================================
fp32 Vector_Length3(const Vector4& Input)
{
	return sqrtf(Vector_LengthSq3(Input));
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Inverse(Matrix& Result, const Matrix& Input)
{
	const fp32 a11 = Input.x.x;
	const fp32 a12 = Input.x.y;
	const fp32 a13 = Input.x.z;
	const fp32 a14 = Input.x.w;
	const fp32 a21 = Input.y.x;
	const fp32 a22 = Input.y.y;
	const fp32 a23 = Input.y.z;
	const fp32 a24 = Input.y.w;
	const fp32 a31 = Input.z.x;
	const fp32 a32 = Input.z.y;
	const fp32 a33 = Input.z.z;
	const fp32 a34 = Input.z.w;
	const fp32 a41 = Input.w.x;
	const fp32 a42 = Input.w.y;
	const fp32 a43 = Input.w.z;
	const fp32 a44 = Input.w.w;

	// 行列式の計算
	const fp32 b11 = +a22 * (a33 * a44 - a43 * a34) - a23 * (a32 * a44 - a42 * a34) + a24 * (a32 * a43 - a42 * a33);
	const fp32 b12 = -a12 * (a33 * a44 - a43 * a34) + a13 * (a32 * a44 - a42 * a34) - a14 * (a32 * a43 - a42 * a33);
	const fp32 b13 = +a12 * (a23 * a44 - a43 * a24) - a13 * (a22 * a44 - a42 * a24) + a14 * (a22 * a43 - a42 * a23);
	const fp32 b14 = -a12 * (a23 * a34 - a33 * a24) + a13 * (a22 * a34 - a32 * a24) - a14 * (a22 * a33 - a32 * a23);

	const fp32 b21 = -a21 * (a33 * a44 - a43 * a34) + a23 * (a31 * a44 - a41 * a34) - a24 * (a31 * a43 - a41 * a33);
	const fp32 b22 = +a11 * (a33 * a44 - a43 * a34) - a13 * (a31 * a44 - a41 * a34) + a14 * (a31 * a43 - a41 * a33);
	const fp32 b23 = -a11 * (a23 * a44 - a43 * a24) + a13 * (a21 * a44 - a41 * a24) - a14 * (a21 * a43 - a41 * a23);
	const fp32 b24 = +a11 * (a23 * a34 - a33 * a24) - a13 * (a21 * a34 - a31 * a24) + a14 * (a21 * a33 - a31 * a23);

	const fp32 b31 = +a21 * (a32 * a44 - a42 * a34) - a22 * (a31 * a44 - a41 * a34) + a24 * (a31 * a42 - a41 * a32);
	const fp32 b32 = -a11 * (a32 * a44 - a42 * a34) + a12 * (a31 * a44 - a41 * a34) - a14 * (a31 * a42 - a41 * a32);
	const fp32 b33 = +a11 * (a22 * a44 - a42 * a24) - a12 * (a21 * a44 - a41 * a24) + a14 * (a21 * a42 - a41 * a22);
	const fp32 b34 = -a11 * (a22 * a34 - a32 * a24) + a12 * (a21 * a34 - a31 * a24) - a14 * (a21 * a32 - a31 * a22);

	const fp32 b41 = -a21 * (a32 * a43 - a42 * a33) + a22 * (a31 * a43 - a41 * a33) - a23 * (a31 * a42 - a41 * a32);
	const fp32 b42 = +a11 * (a32 * a43 - a42 * a33) - a12 * (a31 * a43 - a41 * a33) + a13 * (a31 * a42 - a41 * a32);
	const fp32 b43 = -a11 * (a22 * a43 - a42 * a23) + a12 * (a21 * a43 - a41 * a23) - a13 * (a21 * a42 - a41 * a22);
	const fp32 b44 = +a11 * (a22 * a33 - a32 * a23) - a12 * (a21 * a33 - a31 * a23) + a13 * (a21 * a32 - a31 * a22);

	// 行列式の逆数をかける
	const fp32 Det = (a11 * b11) + (a12 * b21) + (a13 * b31) + (a14 * b41);
	if ((-FLT_MIN <= Det) && (Det <= +FLT_MIN))
	{
		Result = Input;
		return;
	}

	const fp32 InvDet = 1.0f / Det;
	Result.x.x = b11 * InvDet;  Result.x.y = b12 * InvDet;  Result.x.z = b13 * InvDet;  Result.x.w = b14 * InvDet;
	Result.y.x = b21 * InvDet;  Result.y.y = b22 * InvDet;  Result.y.z = b23 * InvDet;  Result.y.w = b24 * InvDet;
	Result.z.x = b31 * InvDet;  Result.z.y = b32 * InvDet;  Result.z.z = b33 * InvDet;  Result.z.w = b34 * InvDet;
	Result.w.x = b41 * InvDet;  Result.w.y = b42 * InvDet;  Result.w.z = b43 * InvDet;  Result.w.w = b44 * InvDet;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transpose(Matrix& Result, const Matrix& Input)
{
	if (&Result == &Input)
	{
		Matrix temp = Input;
		Result.x.x = temp.x.x;  Result.x.y = temp.y.x;  Result.x.z = temp.z.x;  Result.x.w = temp.w.x;
		Result.y.x = temp.x.y;  Result.y.y = temp.y.y;  Result.y.z = temp.z.y;  Result.y.w = temp.w.y;
		Result.z.x = temp.x.z;  Result.z.y = temp.y.z;  Result.z.z = temp.z.z;  Result.z.w = temp.w.z;
		Result.w.x = temp.x.w;  Result.w.y = temp.y.w;  Result.w.z = temp.z.w;  Result.w.w = temp.w.w;
	}
	else
	{
		Result.x.x = Input.x.x;  Result.x.y = Input.y.x;  Result.x.z = Input.z.x;  Result.x.w = Input.w.x;
		Result.y.x = Input.x.y;  Result.y.y = Input.y.y;  Result.y.z = Input.z.y;  Result.y.w = Input.w.y;
		Result.z.x = Input.x.z;  Result.z.y = Input.y.z;  Result.z.z = Input.z.z;  Result.z.w = Input.w.z;
		Result.w.x = Input.x.w;  Result.w.y = Input.y.w;  Result.w.z = Input.z.w;  Result.w.w = Input.w.w;
	}
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Translation(Matrix& Result, fp32 x, fp32 y, fp32 z)
{
	Result.x.x = 1.0f; Result.x.y = 0.0f; Result.x.z = 0.0f; Result.x.w = 0.0f;
	Result.y.x = 0.0f; Result.y.y = 1.0f; Result.y.z = 0.0f; Result.y.w = 0.0f;
	Result.z.x = 0.0f; Result.z.y = 0.0f; Result.z.z = 1.0f; Result.z.w = 0.0f;
	Result.w.x = x;    Result.w.y = y;    Result.w.z = z;    Result.w.w = 1.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Multiply4x4(Matrix& Result, const Matrix& lhs, const Matrix& rhs)
{
	const fp32 result_x_x = (lhs.x.x * rhs.x.x) + (lhs.x.y * rhs.y.x) + (lhs.x.z * rhs.z.x) + (lhs.x.w * rhs.w.x);
	const fp32 result_x_y = (lhs.x.x * rhs.x.y) + (lhs.x.y * rhs.y.y) + (lhs.x.z * rhs.z.y) + (lhs.x.w * rhs.w.y);
	const fp32 result_x_z = (lhs.x.x * rhs.x.z) + (lhs.x.y * rhs.y.z) + (lhs.x.z * rhs.z.z) + (lhs.x.w * rhs.w.z);
	const fp32 result_x_w = (lhs.x.x * rhs.x.w) + (lhs.x.y * rhs.y.w) + (lhs.x.z * rhs.z.w) + (lhs.x.w * rhs.w.w);
	const fp32 result_y_x = (lhs.y.x * rhs.x.x) + (lhs.y.y * rhs.y.x) + (lhs.y.z * rhs.z.x) + (lhs.y.w * rhs.w.x);
	const fp32 result_y_y = (lhs.y.x * rhs.x.y) + (lhs.y.y * rhs.y.y) + (lhs.y.z * rhs.z.y) + (lhs.y.w * rhs.w.y);
	const fp32 result_y_z = (lhs.y.x * rhs.x.z) + (lhs.y.y * rhs.y.z) + (lhs.y.z * rhs.z.z) + (lhs.y.w * rhs.w.z);
	const fp32 result_y_w = (lhs.y.x * rhs.x.w) + (lhs.y.y * rhs.y.w) + (lhs.y.z * rhs.z.w) + (lhs.y.w * rhs.w.w);
	const fp32 result_z_x = (lhs.z.x * rhs.x.x) + (lhs.z.y * rhs.y.x) + (lhs.z.z * rhs.z.x) + (lhs.z.w * rhs.w.x);
	const fp32 result_z_y = (lhs.z.x * rhs.x.y) + (lhs.z.y * rhs.y.y) + (lhs.z.z * rhs.z.y) + (lhs.z.w * rhs.w.y);
	const fp32 result_z_z = (lhs.z.x * rhs.x.z) + (lhs.z.y * rhs.y.z) + (lhs.z.z * rhs.z.z) + (lhs.z.w * rhs.w.z);
	const fp32 result_z_w = (lhs.z.x * rhs.x.w) + (lhs.z.y * rhs.y.w) + (lhs.z.z * rhs.z.w) + (lhs.z.w * rhs.w.w);
	const fp32 result_w_x = (lhs.w.x * rhs.x.x) + (lhs.w.y * rhs.y.x) + (lhs.w.z * rhs.z.x) + (lhs.w.w * rhs.w.x);
	const fp32 result_w_y = (lhs.w.x * rhs.x.y) + (lhs.w.y * rhs.y.y) + (lhs.w.z * rhs.z.y) + (lhs.w.w * rhs.w.y);
	const fp32 result_w_z = (lhs.w.x * rhs.x.z) + (lhs.w.y * rhs.y.z) + (lhs.w.z * rhs.z.z) + (lhs.w.w * rhs.w.z);
	const fp32 result_w_w = (lhs.w.x * rhs.x.w) + (lhs.w.y * rhs.y.w) + (lhs.w.z * rhs.z.w) + (lhs.w.w * rhs.w.w);

	Result.x.x = result_x_x;
	Result.x.y = result_x_y;
	Result.x.z = result_x_z;
	Result.x.w = result_x_w;
	Result.y.x = result_y_x;
	Result.y.y = result_y_y;
	Result.y.z = result_y_z;
	Result.y.w = result_y_w;
	Result.z.x = result_z_x;
	Result.z.y = result_z_y;
	Result.z.z = result_z_z;
	Result.z.w = result_z_w;
	Result.w.x = result_w_x;
	Result.w.y = result_w_y;
	Result.w.z = result_w_z;
	Result.w.w = result_w_w;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_RotationX(Matrix& Result, fp32 Radian)
{
	const fp32 c = cosf(Radian);
	const fp32 s = sinf(Radian);

	Result.x.x = 1.0f; Result.x.y = 0.0f; Result.x.z = 0.0f; Result.x.w = 0.0f;
	Result.y.x = 0.0f; Result.y.y = +c;   Result.y.z = +s;   Result.y.w = 0.0f;
	Result.z.x = 0.0f; Result.z.y = -s;   Result.z.z = +c;   Result.z.w = 0.0f;
	Result.w.x = 0.0f; Result.w.y = 0.0f; Result.w.z = 0.0f; Result.w.w = 1.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_RotationY(Matrix& Result, fp32 Radian)
{
	const fp32 c = cosf(Radian);
	const fp32 s = sinf(Radian);

	Result.x.x = +c;   Result.x.y = 0.0f; Result.x.z = -s;   Result.x.w = 0.0f;
	Result.y.x = 0.0f; Result.y.y = 1.0f; Result.y.z = 0.0f; Result.y.w = 0.0f;
	Result.z.x = +s;   Result.z.y = 0.0f; Result.z.z = +c;   Result.z.w = 0.0f;
	Result.w.x = 0.0f; Result.w.y = 0.0f; Result.w.z = 0.0f; Result.w.w = 1.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_RotationZ(Matrix& Result, fp32 Radian)
{
	const fp32 c = cosf(Radian);
	const fp32 s = sinf(Radian);

	Result.x.x = +c;   Result.x.y = +s;   Result.x.z = 0.0f; Result.x.w = 0.0f;
	Result.y.x = -s;   Result.y.y = +c;   Result.y.z = 0.0f; Result.y.w = 0.0f;
	Result.z.x = 0.0f; Result.z.y = 0.0f; Result.z.z = 1.0f; Result.z.w = 0.0f;
	Result.w.x = 0.0f; Result.w.y = 0.0f; Result.w.z = 0.0f; Result.w.w = 1.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_CreateLookAt(Matrix& Result, const Vector4& Position, const Vector4& Target, const Vector4& Up)
{
	Vector4 AxisX, AxisY, AxisZ;

	Vector_Sub(AxisZ, Target, Position);
	Vector_CrossProduct(AxisX, Up, AxisZ);
	Vector_CrossProduct(AxisY, AxisZ, AxisX);

	Vector_Normalize3(AxisX, AxisX);
	Vector_Normalize3(AxisY, AxisY);
	Vector_Normalize3(AxisZ, AxisZ);

	Result.x.x = AxisX.x;  Result.x.y = AxisX.y;  Result.x.z = AxisX.z;  Result.x.w = 0.0f;
	Result.y.x = AxisY.x;  Result.y.y = AxisY.y;  Result.y.z = AxisY.z;  Result.y.w = 0.0f;
	Result.z.x = AxisZ.x;  Result.z.y = AxisZ.y;  Result.z.z = AxisZ.z;  Result.z.w = 0.0f;
	Result.w.x = Position.x;  Result.w.y = Position.y;  Result.w.z = Position.z;  Result.w.w = 1.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_CreateLookAtView(Matrix& Result, const Vector4& Position, const Vector4& Target, const Vector4& Up)
{
	Vector4 AxisX, AxisY, AxisZ;

	Vector_Sub(AxisZ, Target, Position);
	Vector_CrossProduct(AxisX, Up, AxisZ);
	Vector_CrossProduct(AxisY, AxisZ, AxisX);

	Vector_Normalize3(AxisX, AxisX);
	Vector_Normalize3(AxisY, AxisY);
	Vector_Normalize3(AxisZ, AxisZ);

	Result.x.x = AxisX.x; Result.x.y = AxisY.x; Result.x.z = AxisZ.x; Result.x.w = 0.0f;
	Result.y.x = AxisX.y; Result.y.y = AxisY.y; Result.y.z = AxisZ.y; Result.y.w = 0.0f;
	Result.z.x = AxisX.z; Result.z.y = AxisY.z; Result.z.z = AxisZ.z; Result.z.w = 0.0f;

	Result.w.x = -Vector_DotProduct3(Position, AxisX);
	Result.w.y = -Vector_DotProduct3(Position, AxisY);
	Result.w.z = -Vector_DotProduct3(Position, AxisZ);
	Result.w.w = 1.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_CreateProjection(Matrix& Result, fp32 NearClip, fp32 FarClip, fp32 FovY, fp32 Aspect)
{
	const fp32 yScale = 1.0f / tanf(FovY * 0.5f);
	const fp32 xScale = yScale / Aspect;

	Result.x.x = xScale;
	Result.x.y = 0.0f;
	Result.x.z = 0.0f;
	Result.x.w = 0.0f;

	Result.y.x = 0.0f;
	Result.y.y = yScale;
	Result.y.z = 0.0f;
	Result.y.w = 0.0f;

	Result.z.x = 0.0f;
	Result.z.y = 0.0f;
	Result.z.z = FarClip / (FarClip - NearClip);
	Result.z.w = 1.0f;

	Result.w.x = 0.0f;
	Result.w.y = 0.0f;
	Result.w.z = -NearClip * FarClip / (FarClip - NearClip);
	Result.w.w = 0.0f;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transform3x3(Vector3& Result, const Vector3 lhs, const Matrix& rhs)
{
	Result.x = (lhs.x * rhs.x.x) + (lhs.y * rhs.y.x) + (lhs.z * rhs.z.x);
	Result.y = (lhs.x * rhs.x.y) + (lhs.y * rhs.y.y) + (lhs.z * rhs.z.y);
	Result.z = (lhs.x * rhs.x.z) + (lhs.y * rhs.y.z) + (lhs.z * rhs.z.z);
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transform3x3(Vector4& Result, const Vector4 lhs, const Matrix& rhs)
{
	Result.x = (lhs.x * rhs.x.x) + (lhs.y * rhs.y.x) + (lhs.z * rhs.z.x);
	Result.y = (lhs.x * rhs.x.y) + (lhs.y * rhs.y.y) + (lhs.z * rhs.z.y);
	Result.z = (lhs.x * rhs.x.z) + (lhs.y * rhs.y.z) + (lhs.z * rhs.z.z);
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transform4x4(Vector4& Result, const Vector4 lhs, const Matrix& rhs)
{
	Result.x = (lhs.x * rhs.x.x) + (lhs.y * rhs.y.x) + (lhs.z * rhs.z.x) + (lhs.w * rhs.w.x);
	Result.y = (lhs.x * rhs.x.y) + (lhs.y * rhs.y.y) + (lhs.z * rhs.z.y) + (lhs.w * rhs.w.y);
	Result.z = (lhs.x * rhs.x.z) + (lhs.y * rhs.y.z) + (lhs.z * rhs.z.z) + (lhs.w * rhs.w.z);
	Result.w = (lhs.x * rhs.x.w) + (lhs.y * rhs.y.w) + (lhs.z * rhs.z.w) + (lhs.w * rhs.w.w);
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transform4x4(Vector4& Result, const Vector3 lhs, const Matrix& rhs)
{
	Result.x = (lhs.x * rhs.x.x) + (lhs.y * rhs.y.x) + (lhs.z * rhs.z.x) + rhs.w.x;
	Result.y = (lhs.x * rhs.x.y) + (lhs.y * rhs.y.y) + (lhs.z * rhs.z.y) + rhs.w.y;
	Result.z = (lhs.x * rhs.x.z) + (lhs.y * rhs.y.z) + (lhs.z * rhs.z.z) + rhs.w.z;
	Result.w = (lhs.x * rhs.x.w) + (lhs.y * rhs.y.w) + (lhs.z * rhs.z.w) + rhs.w.w;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transform4x4Projection(Vector4& Result, const Vector4 lhs, const Matrix& rhs)
{
	Result.x = (lhs.x * rhs.x.x) + (lhs.y * rhs.y.x) + (lhs.z * rhs.z.x) + (lhs.w * rhs.w.x);
	Result.y = (lhs.x * rhs.x.y) + (lhs.y * rhs.y.y) + (lhs.z * rhs.z.y) + (lhs.w * rhs.w.y);
	Result.z = (lhs.x * rhs.x.z) + (lhs.y * rhs.y.z) + (lhs.z * rhs.z.z) + (lhs.w * rhs.w.z);
	Result.w = (lhs.x * rhs.x.w) + (lhs.y * rhs.y.w) + (lhs.z * rhs.z.w) + (lhs.w * rhs.w.w);
	const fp32 InvW = 1.0f / Result.w;
	Result.x *= InvW;
	Result.y *= InvW;
	Result.z *= InvW;
}

//======================================================================================================
//
//======================================================================================================
void Matrix_Transform4x4Projection(Vector4& Result, const Vector3 lhs, const Matrix& rhs)
{
	Result.x = (lhs.x * rhs.x.x) + (lhs.y * rhs.y.x) + (lhs.z * rhs.z.x) + rhs.w.x;
	Result.y = (lhs.x * rhs.x.y) + (lhs.y * rhs.y.y) + (lhs.z * rhs.z.y) + rhs.w.y;
	Result.z = (lhs.x * rhs.x.z) + (lhs.y * rhs.y.z) + (lhs.z * rhs.z.z) + rhs.w.z;
	Result.w = (lhs.x * rhs.x.w) + (lhs.y * rhs.y.w) + (lhs.z * rhs.z.w) + rhs.w.w;
	const fp32 InvW = 1.0f / Result.w;
	Result.x *= InvW;
	Result.y *= InvW;
	Result.z *= InvW;
}
