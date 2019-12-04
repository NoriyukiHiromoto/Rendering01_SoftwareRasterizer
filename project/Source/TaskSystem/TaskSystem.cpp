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
#include <TaskSystem/TaskSystem.h>

//======================================================================================================
//
//======================================================================================================
void TaskSystem::Initialize()
{
	_PipelineCount = std::thread::hardware_concurrency();
	_PipelineCount = std::max(1, _PipelineCount - 1);
	for (int32 i = 0; i < _PipelineCount; ++i)
	{
		_TaskPipelines.push_back(new TaskPipeline(i + 1));
	}

	_TaskData.Que.resize(QUE_COUNT_MAX);
	_TaskData.QueCount = 0;
	_TaskData.WriteOffset = 0;
	_TaskData.ReadOffset = 0;
	_TaskData.RunningPipelineCount = 0;
	_TaskData.IsTaskCompleted = true;
	_TaskData.BarrierCount.clear();
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::Finalize()
{
	for (int32 i = 0; i < _PipelineCount; ++i)
	{
		delete _TaskPipelines[i];
	}
	_TaskPipelines.clear();
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::Execute()
{
	_TaskData.IsTaskCompleted = false;
	_TaskData.RunningPipelineCount = _PipelineCount;
	_TaskData.QueCount = _TaskData.WriteOffset.Load();
	_TaskData.ReadOffset = 0;
	_TaskData.WriteOffset = 0;

	for (int32 i = 0; i < _PipelineCount; ++i)
	{
		_TaskPipelines[i]->Kick();
	}

	TaskSystem::QueData* pQueData;
	while (GetQueData(pQueData))
	{
		ExecuteQue(pQueData);
	}

	while (!_TaskData.IsTaskCompleted)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(0));
	}

	_TaskData.BarrierCount.clear();
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::ExecuteSingle()
{
	_TaskData.IsTaskCompleted = false;
	_TaskData.RunningPipelineCount = _PipelineCount;
	_TaskData.QueCount = _TaskData.WriteOffset.Load();
	_TaskData.ReadOffset = 0;
	_TaskData.WriteOffset = 0;

	TaskSystem::QueData* pQueData;
	while (GetQueData(pQueData))
	{
		ExecuteQue(pQueData);
	}

	_TaskData.BarrierCount.clear();
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::ExecuteQue(TaskSystem::QueData* pQueData)
{
	if (pQueData->BarrierID != 0)
	{
		const int32 Index = pQueData->BarrierID - 1;
		auto& Barrier = _TaskData.BarrierCount[Index];
		Barrier.Decrement();
		while (Barrier.Load())
		{
			std::this_thread::sleep_for(std::chrono::microseconds(0));
		}
	}
	else
	{
		pQueData->Callback(pQueData->pData);
	}
}

//======================================================================================================
//
//======================================================================================================
bool TaskSystem::GetQueData(QueData*& pQueData)
{
	const auto Read = _TaskData.ReadOffset.Increment();
	if (Read > _TaskData.QueCount) return false;

	pQueData = &_TaskData.Que[Read - 1];
	return true;
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::Completed(int32 CoreNo)
{
	if (_TaskData.RunningPipelineCount.Decrement() == 0)
	{
		_TaskData.IsTaskCompleted = true;
	}
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::PushQue(std::function<void(void*)> Callback, void* pData)
{
	const auto Write = _TaskData.WriteOffset.Increment() - 1;
	auto& Dst = _TaskData.Que[Write];
	Dst.Callback = Callback;
	Dst.pData = pData;
	Dst.BarrierID = 0;
}

//======================================================================================================
//
//======================================================================================================
void TaskSystem::PushBarrier()
{
	const int32 QueueCount = _PipelineCount + 1;

	_TaskData.BarrierCount.push_back(QueueCount);
	const int32 BarrierId = int32(_TaskData.BarrierCount.size());

	for (int32 i = 0; i < QueueCount; ++i)
	{
		const auto Write = _TaskData.WriteOffset.Increment() - 1;
		auto& Dst = _TaskData.Que[Write];
		Dst.Callback = [](void*) {};
		Dst.pData = nullptr;
		Dst.BarrierID = BarrierId;
	}
}
