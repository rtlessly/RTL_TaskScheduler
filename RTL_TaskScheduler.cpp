/*******************************************************************************
Implementation file for the TaskScheduler class.
*******************************************************************************/
#define DEBUG 0

#include <Arduino.h>
#include <avr/wdt.h>
#include <RTL_Stdlib.h>
#include "RTL_TaskScheduler.h"


DEFINE_CLASSNAME(TaskScheduler);


void TaskScheduler::Dispatch()
{
    for (auto pTask = _pFirstTask; pTask; pTask = pTask->_pNextTask)
    {
        // The watchdog timer (wdt) will restart the Arduino unless it is periodically reset
        wdt_reset();
        TRACE(Logger(_classname_, F("Dispatch")) << F("Task=") << ToHex(pTask) << endl);

        if (pTask->Execute()) break;
    }
}


void TaskScheduler::Add(ATask& task)
{
    TRACE(DumpQueue(F("Add:Before")));

    if (_pFirstTask == nullptr)
    {
        LinkFirstTask(&task);
    }
    else
    {
        LastTask()->Link(task);
    }

    TRACE(DumpQueue(F("Add:After")));
}


void TaskScheduler::InsertAt(ATask& task, uint16_t priority)
{
    TRACE(DumpQueue(F("InsertAt:Before")));

    if (priority <= 1)
    {
        LinkFirstTask(&task);
    }
    else
    {
        auto i = 2;

        for (auto p = _pFirstTask; p; p = p->_pNextTask)
        {
            if (i++ == priority)
            {
                p->Link(task);
                break;
            }
        }
    }
    
    TRACE(DumpQueue(F("InsertAt:After")));
}


void TaskScheduler::InsertBefore(ATask& task, ATask& beforeTask)
{
    TRACE(DumpQueue(F("InsertBefore:Before")));

    if (_pFirstTask == nullptr) return;

    if (&beforeTask == _pFirstTask)
    {
        LinkFirstTask(&task);
    }
    else
    {
        if (auto prevTask = FindPrev(&beforeTask))
        {
            prevTask->Link(task);
        }
    }
    
    TRACE(DumpQueue(F("InsertBefore:After")));
}


void TaskScheduler::InsertAfter(ATask& task, ATask& afterTask)
{
    TRACE(DumpQueue(F("InsertAfter:Before")));

    afterTask.Link(task);
    
    TRACE(DumpQueue(F("InsertAfter:After")));
}


void TaskScheduler::Remove(ATask& task)
{
    if (_pFirstTask == nullptr) return;

    TRACE(DumpQueue(F("Remove:Before")));

    if (_pFirstTask == &task)
    {
        _pFirstTask = _pFirstTask->_pNextTask;
        TRACE(Logger(_classname_, F("Remove")) << F("Removed first task ") << ToHex(&task) << endl);
    }
    else
    {
        if (auto prevTask = FindPrev(&task))
        {
            prevTask->Unlink(task);
            TRACE(Logger(_classname_, F("Remove")) << F("Removed task ") << ToHex(&task) << endl);
        }
        else
            TRACE(Logger(_classname_, F("Remove")) << F("task not found: ") << ToHex(&task) << endl);
    }

    TRACE(DumpQueue(F("Remove:After")));
}


ATask* TaskScheduler::LastTask()
{
    for (auto p = _pFirstTask; p; p = p->_pNextTask)
        if (p->_pNextTask == nullptr) return p;

    return nullptr;
}


ATask* TaskScheduler::Find(Task* pTask)
{
    for (auto p = _pFirstTask; p; p = p->_pNextTask)
    {
        if (p == pTask) return p;
    }
}


ATask* TaskScheduler::FindPrev(Task* pTask)
{
    for (auto p = _pFirstTask; p; p = p->_pNextTask)
    {
        if (p->_pNextTask == pTask) return p;
    }
}


void TaskScheduler::DumpQueue(const __FlashStringHelper* message)
{
    auto i = 1;

    Logger(_classname_) << F("Task Queue - ") << message << endl;

    for (auto p = _pFirstTask; p; p = p->_pNextTask)
    {
        Logger(_classname_) << i++ << F(".    ") << ToHex(p) << endl;
    }
}

