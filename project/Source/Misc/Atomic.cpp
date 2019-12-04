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
#include <Misc/Atomic.h>

//======================================================================================
//
//======================================================================================
Atomic::Atomic()
{
	m_Value.store(0);
}

//======================================================================================
//
//======================================================================================
Atomic::Atomic(int32 value)
{
	m_Value.store(value);
}

//======================================================================================
//
//======================================================================================
Atomic::Atomic(const Atomic& value)
{
	m_Value.store(value.Load());
}

//======================================================================================
//
//======================================================================================
Atomic::~Atomic()
{
}

//======================================================================================
//
//======================================================================================
int32 Atomic::Increment()
{
	return ++m_Value;
}

//======================================================================================
//
//======================================================================================
int32 Atomic::Decrement()
{
	return --m_Value;
}

//======================================================================================
//
//======================================================================================
int32 Atomic::Add(const int32 value)
{
	return m_Value.fetch_add(value, std::memory_order_seq_cst);
}

//======================================================================================
//
//======================================================================================
int32 Atomic::Load() const
{
	return m_Value.load(std::memory_order_seq_cst);
}

//======================================================================================
//
//======================================================================================
void Atomic::Store(const int32 value)
{
	m_Value.store(value);
}

//======================================================================================
//
//======================================================================================
int32 Atomic::operator = (const int32 value)
{
	Store(value);
	return value;
}

//======================================================================================
//
//======================================================================================
bool Atomic::operator == (const int32 value)
{
	return Load() == value;
}

//======================================================================================
//
//======================================================================================
bool Atomic::operator != (const int32 value)
{
	return Load() != value;
}
