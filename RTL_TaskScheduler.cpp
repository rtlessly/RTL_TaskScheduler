/*******************************************************************************
Implementation file for the TaskScheduler class.
*******************************************************************************/
#define DEBUG 0

#include <Arduino.h>
#include <avr/wdt.h>
#include <RTL_Stdlib.h>
#include "RTL_TaskScheduler.h"


void TaskScheduler::Dispatch()
{
    for (auto pTask = _pFirstTask; pTask; pTask = pTask->_pNextTask)
    {
        // The watchdog timer (wdt) will restart the Arduino unless it is periodically reset
        wdt_reset();
        TRACE(Logger() << F("Scheduler.Dispatch: Task=") << PTR(pTask) << endl);

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
        LinkLastTask(&task);
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
                LinkTask(p, &task);
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
            LinkTask(prevTask, &task);
        }
    }
    
    TRACE(DumpQueue(F("InsertBefore:After")));
}


void TaskScheduler::InsertAfter(ATask& task, ATask& afterTask)
{
    TRACE(DumpQueue(F("InsertAfter:Before")));

    if (auto pTask = Find(&afterTask))
    {
        LinkTask(pTask, &task);
    }
    
    TRACE(DumpQueue(F("InsertAfter:After")));
}


void TaskScheduler::Remove(ATask& task)
{
    if (_pFirstTask == nullptr) return;

    TRACE(DumpQueue(F("Remove:Before")));

    if (_pFirstTask == &task)
    {
        _pFirstTask = _pFirstTask->_pNextTask;
        TRACE(Logger() << F("Remove: Removed first task ") << PTR(&task) << endl);
    }
    else
    {
        auto prevTask = FindPrev(&task);

        if (prevTask)
        {
            prevTask->_pNextTask = task._pNextTask;
            TRACE(Logger() << F("Remove: Removed task ") << PTR(&task) << F(" after task ") << PTR(prevTask) << endl);
        }
        else
            TRACE(Logger() << F("Remove: task ") << PTR(&task) << F(" not found") << endl);
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
    int i = 1;

    Logger() << F("TaskScheduler:  ") << message << endl;

    for (auto p = _pFirstTask; p; p = p->_pNextTask)
    {
        Logger() << i++ << F(".    ") << PTR(p) << endl;
    }
}

