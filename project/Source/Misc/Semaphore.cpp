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
#include <Misc/Semaphore.h>

//======================================================================================
//
//======================================================================================
Semaphore::Semaphore()
{
	m_BlockingCount = 0;
}

//======================================================================================
//
//======================================================================================
Semaphore::~Semaphore()
{
}

//======================================================================================
//
//======================================================================================
void Semaphore::Wait()
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_ConditionVariable.wait(lock, [this](){ return m_BlockingCount != 0; });
	--m_BlockingCount;
}

//======================================================================================
//
//======================================================================================
void Semaphore::Notify()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_ConditionVariable.notify_all();
	++m_BlockingCount;
}
