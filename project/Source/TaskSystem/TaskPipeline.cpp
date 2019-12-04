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
#include <TaskSystem/TaskPipeline.h>
#include <TaskSystem/TaskSystem.h>

//======================================================================================================
//
//======================================================================================================
TaskPipeline::TaskPipeline(int32 CoreNo)
	: _CoreNo(CoreNo)
	, _bRunning(false)
{
	_Threading = std::thread([this]() {
		_bRunning = true;
		this->Loop();
	});

	while (!_bRunning)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(10000));
	}
}

//======================================================================================================
//
//======================================================================================================
TaskPipeline::~TaskPipeline()
{
	_bRunning = false;
	_Semaphore.Notify();

	while (!_Threading.joinable())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(10000));
	}

	_Threading.join();
}

//======================================================================================================
//
//======================================================================================================
void TaskPipeline::Kick()
{
	_Semaphore.Notify();
}

//======================================================================================================
//
//======================================================================================================
void TaskPipeline::Loop()
{
	auto& System = TaskSystem::Instance();
	for (;;)
	{
		_Semaphore.Wait();
		if (!_bRunning) return;

		TaskSystem::QueData* pQueData;
		while (System.GetQueData(pQueData))
		{
			System.ExecuteQue(pQueData);
		}

		System.Completed(_CoreNo);
	}
}
