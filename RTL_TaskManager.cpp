/*******************************************************************************
Implementation file for the TaskManager class.
*******************************************************************************/
#define DEBUG 0

#include <Arduino.h>
#include <RTL_Stdlib.h>
#include <EventQueue.h>
#include "RTL_TaskManager.h"


DEFINE_CLASSNAME(TaskManager);

static TaskBase* EMPTY_TASK_LIST[] = { nullptr };


TaskBase** TaskManager::_taskList = EMPTY_TASK_LIST;
TaskBase** TaskManager::_taskPointer = EMPTY_TASK_LIST;
StateBase* TaskManager::_pCurrentState = nullptr;


//******************************************************************************
// Runs all tasks in the task list, dispacthes all events in the event queue,
// and runs the current state.
//******************************************************************************
void TaskManager::Dispatch()
{
    // Run all tasks in the task list
    for (auto p = _taskList; *p; p++)
    {
        TRACE(Logger(_classname_, F("Dispatch Task ")) << (*p)->Name() << '[' << PTR(*p) << ']' << endl);

        (*p)->Run();
    }

    // Dispatch all events that were queued up to this point to the current state.
    // NOTE: This loop is specifically constructed to only go around the event queue
    // one time. It does NOT dispatch any new events added as a result of processing
    // a dispatched event. Those will get processed on the next go-around. Otherwise, 
    // we could create an endless loop where object A posts an event that object B 
    // receives who, in turn, posts an event that object A receives, etc... In such 
    // a scenario the event queue would never empty and the dispatch loop would go 
    // on forever.
    for (auto i = EventQueue::Length(); i > 0; i--)
    {
        Event event;

        if (EventQueue::Dequeue(event))
        {
            TRACE(Logger(_classname_, F("Dispatch Event")) << F("ID=") << event.EventID << F(", Srce=") << PTR(event.Source) << endl);

            if (_pCurrentState) _pCurrentState->OnEvent(&event);
        }
    }

    // Run the current state
    if (_pCurrentState) _pCurrentState->Run();
}


//******************************************************************************
// Sets the task list to execute.
// NOTE: Attempting to set the task list to NULL (nullptr) will cause it to be
//       set to an internal empty task list.
//******************************************************************************
TaskBase** TaskManager::SetTaskList(TaskBase* newTaskList[], bool autoResume, bool autoSuspend)
{
    auto oldTaskList = _taskList;

    // Suspend all tasks in the current task list
    if (autoSuspend)
    {
        for (auto p = _taskList; *p; p++)
        {
            (*p)->Suspend();
        }
    }

    // Switch task lists and reset task pointer
    _taskList = (newTaskList != nullptr) ? newTaskList : EMPTY_TASK_LIST;
    _taskPointer = _taskList;

    // Resume all tasks in the new task list
    if (autoResume)
    {
        for (auto p = _taskList; *p; p++)
        {
            (*p)->Resume();
        }
    }

    return oldTaskList;
}


//******************************************************************************
// Sets the current state machine state
//******************************************************************************
StateBase* TaskManager::SetCurrentState(StateBase* pNewState)
{
    if (pNewState == _pCurrentState) return nullptr;
        
    auto oldState = _pCurrentState;

    if (_pCurrentState != nullptr) _pCurrentState->Suspend();

    _pCurrentState = pNewState;

    if (_pCurrentState != nullptr) _pCurrentState->Resume();

    return oldState;
}


//******************************************************************************
// Determines if the specified task is in the task list.
//******************************************************************************
bool TaskManager::IsScheduled(TaskBase* pTask)
{
    for (auto p = _taskList; *p; p++)
    {
        if ((*p) == pTask) return true;
    }

    return false;
}


void TaskManager::DumpTaskList(const __FlashStringHelper* message)
{
    auto i = 1;

    Logger(_classname_) << F("Task Queue - ") << message << endl;

    for (auto p = _taskList; *p; p++)
    {
        auto pTask = *p;
        Logger(_classname_) << i++ << F(".    ") << (*p)->Name() << '[' << PTR(*p) << ']' << endl;
    }
}

