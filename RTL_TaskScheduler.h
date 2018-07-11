/*******************************************************************************
Header file for the WorkTask class.
*******************************************************************************/
#ifndef _RTL_TaskScheduler_h_
#define _RTL_TaskScheduler_h_


//******************************************************************************
/// The TaskScheduler class maintains a queue of tasks to execute and implements 
/// a simple dispatch loop to execute those tasks. A task wraps a function that
/// takes no arguments and returns a boolean value. 
///
/// If a task returns true it signals the scheduler to restart its task execution
/// loop, whereas returning false signals the scheduler to continue on to the next
/// task in the queue.
///
/// This simple mechanism permits the prioritized execution of tasks, where tasks
/// higher in the queue can preempt later tasks by returning true. However, it
/// also expects each task to "play nice" so that no task hogs the CPU by returning
/// true all the time and preventing any other task from executing.
///
/// Task should be designed so that they execute a small "unit of work" on each
/// iteration.
///
/// This simple design works well for controllers that need to monitor multiple
/// sensors and processes that execute in response to those sensors. For example,
/// a task may check the value of a sensor. If the sensor has triggered it initiates
/// some process and return true. Otherwise, the task just returns false. Note
/// that the initiated process itself may be another task.
///
/// Internally, the task queue is implemented as a singly-linked list. The ATask
/// class provides a pointer to the next task in the queue, while the TaskScheduler
/// maintains a pointer to the first task in the queue. The last task has its next
/// task pointer set to NULL. The TaskScheduler has methods to link and unlink
/// tasks in the queue. This mechansim avoids the need to use a pre-allocated
/// fixed-sized queue, or to use new to dynamically allocate queue entries - both
/// of which are frowned upon in an Arduino environment (pre-allocation can waste
/// precious RAM memory by allocating a queue that is larger than what is actually
/// used, while new carries a lot of overhead and also is less memory efficient).
//******************************************************************************


// Typedef of a task function
typedef bool (*TaskFunc)(void);     /* Size = 2 bytes*/


class ATask                         /* Size = 4 bytes*/
{
    friend class  TaskScheduler;

    public: virtual bool Execute() = 0;

    public: void Link(ATask& task)
    {
        task._pNextTask == _pNextTask;
        _pNextTask = &task;
    };

    protected: void Unlink(ATask& task)
    {
        if (_pNextTask == &task)
        {
            _pNextTask = task._pNextTask;
            //task._pNextTask == nullptr;
        }
    }

    private: ATask* _pNextTask;
};


class Task : public ATask           /* Size = sizeof(ATASK) + 2 = 4 + 2 = 6 bytes*/
{
    public: Task(TaskFunc func) : _function(func) { };

    public: bool Execute() { return _function(); };

    private: TaskFunc _function;
};


class TaskScheduler                 /* Size = 4 bytes*/
{
    DECLARE_CLASSNAME;

    public: TaskScheduler() : _pFirstTask(nullptr) { };

    /***************************************************************************
    Public methods
    ***************************************************************************/
    public: void Dispatch();

    public: void Add(ATask& task);

    public: void InsertAt(ATask& task, uint16_t priority);

    public: void InsertBefore(ATask& task, ATask& beforeTask);

    public: void InsertAfter(ATask& task, ATask& afterTask);

    public: void Remove(ATask& task);

    public: void DumpQueue(const __FlashStringHelper* message=nullptr);

    /***************************************************************************
    Internal implementation
    ***************************************************************************/
    private: ATask* LastTask();

    private: ATask* Find(Task* pTask);

    private: ATask* FindPrev(Task* pTask);

    private: void LinkTask(ATask* pTask, ATask* pNewTask);

    private: void LinkFirstTask(ATask* pNewTask);

    private: void LinkLastTask(ATask* pNewTask);


    /***************************************************************************
    Internal state
    ***************************************************************************/
    private: ATask* _pFirstTask;
};


//inline void TaskScheduler::LinkTask(ATask* pTask, ATask* pNewTask)
//{
//    pTask->Link(*pNewTask);
//    //pNewTask->_pNextTask = pTask->_pNextTask;
//    //pTask->_pNextTask = pNewTask;
//}


inline void TaskScheduler::LinkFirstTask(ATask* pNewTask)
{
    pNewTask->_pNextTask = _pFirstTask;
    _pFirstTask = pNewTask;
}


//inline void TaskScheduler::LinkLastTask(ATask* pNewTask)
//{
//    LastTask()->Link(*pNewTask);
//}

#endif
