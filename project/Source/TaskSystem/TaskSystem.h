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
#include <TaskSystem/TaskPipeline.h>
#include <Misc/Atomic.h>

//======================================================================================================
//
//======================================================================================================
class TaskSystem
{
public:
	struct QueData
	{
		std::function<void(void*)>	Callback;
		void*						pData;
		uint32						BarrierID;
	};

private:
	enum { QUE_COUNT_MAX = 30000 };

	struct TaskData
	{
		std::vector<QueData>	Que;
		int32					QueCount;
		Atomic					WriteOffset;
		Atomic					ReadOffset;
		Atomic					RunningPipelineCount;
		bool					IsTaskCompleted;
		std::vector<Atomic>		BarrierCount;
	};

private:
	std::vector<TaskPipeline*>	_TaskPipelines;
	int32						_PipelineCount;
	TaskData					_TaskData;
	Atomic						_ExecutePipelineCount;

private:
	TaskSystem() {}
	~TaskSystem() {}

public:
	void Initialize();
	void Finalize();
	void Execute();
	void ExecuteSingle();

	bool GetQueData(QueData*& pQueData);
	void ExecuteQue(QueData* pQueData);
	void Completed(int32 CoreNo);

	void PushQue(std::function<void(void*)> Callback, void* pData);
	void PushBarrier();

public:
	static TaskSystem& Instance()
	{
		static TaskSystem _Instance;
		return _Instance;
	}
};
