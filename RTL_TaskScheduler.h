/*******************************************************************************
Header file for the TaskScheduler class.
*******************************************************************************/
#ifndef _RTL_TaskScheduler_h_
#define _RTL_TaskScheduler_h_


// Typedef of a task function
typedef bool (*TaskFunc)(void);     /* Size = 2 bytes*/


//******************************************************************************
/// The abstact base class for a simple task. A task wraps a function that takes
/// no arguments and returns a boolean value. 
///
/// If a task returns true it signals the TaskScheduler to restart its task execution
/// loop, whereas returning false signals the scheduler to continue on to the next
/// task in the queue.
///
/// The ATask class defines a abstact Execute() method that must be implemented 
/// in a derived class. This method contains the actual functionality of the task. 
/// The Execute() method is called by the TaskScheduler on each iteration through 
/// its task queue. As such, the Execute() method should be designed so that it 
/// executes only a small unit of work on each iteration. In addition, it should
/// be designed to "fail fast" so that task exits as quickly as possible (and 
/// returns false) if it has no work to do.
//******************************************************************************
class ATask                         /* Size = 4 bytes*/
{
    friend class  TaskScheduler;

    //**************************************************************************
    /// The function that the task executes. Derived classes must implement this 
    /// function.
    //**************************************************************************
    public: virtual bool Execute() = 0;

    //**************************************************************************
    /// Links a new task after this task.
    //**************************************************************************
    public: void Link(ATask& task)
    {
        task._pNextTask = _pNextTask;
        _pNextTask = &task;
    };

    //**************************************************************************
    /// Unlinks a task if it is linked to this task.
    //**************************************************************************
    protected: void Unlink(ATask& task)
    {
        if (_pNextTask == &task)
        {
            _pNextTask = task._pNextTask;
        }
    }

    /// The pointer to the next task in the queue. 
    private: ATask* _pNextTask;
};


//******************************************************************************
/// An implmentation of ATask that executes the function passed to the constuctor
/// as the task function.
//******************************************************************************
class Task : public ATask           /* Size = sizeof(ATASK) + 2 = 4 + 2 = 6 bytes*/
{
    //**************************************************************************
    /// Constructs an instance of the Task class with the function to execute.
    //**************************************************************************
    public: Task(TaskFunc func) : _function(func) { };

    //**************************************************************************
    /// Executes the function bound to this instance in the constructor.
    //**************************************************************************
    public: bool Execute() { return _function(); };

    /// The function to execute for the task.
    private: TaskFunc _function;
};


//******************************************************************************
/// The TaskScheduler class maintains a queue of tasks to execute and implements 
/// a simple dispatch loop to execute those tasks.  
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
/// This design works well for controllers that need to monitor multiple sensors 
/// and processes that execute in response to those sensors. For example, a task
/// may check the value of a sensor. If the sensor has triggered it initiates some
/// process and return true. Otherwise, the task just returns false. Note that the
/// initiated process itself may be another task.
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
class TaskScheduler                 /* Size = 4 bytes*/
{
    DECLARE_CLASSNAME;

    //**************************************************************************
    /// Constructs an instance of TaskScheduler.
    //**************************************************************************
    public: TaskScheduler() : _pFirstTask(nullptr) { };

    //**************************************************************************
    /// Dispatches the next task.
    //**************************************************************************
    public: void Dispatch();

    //**************************************************************************
    /// Adds a task to the scheduler.
    //**************************************************************************
    public: void Add(ATask& task);

    //**************************************************************************
    /// Adds a task at a specific priority (index into the scheduler queue).
    //**************************************************************************
    public: void InsertAt(ATask& task, uint16_t priority);

    //**************************************************************************
    /// Adds a task before another task.
    //**************************************************************************
    public: void InsertBefore(ATask& task, ATask& beforeTask);

    //**************************************************************************
    /// Adds a task after another task.
    //**************************************************************************
    public: void InsertAfter(ATask& task, ATask& afterTask);

    //**************************************************************************
    /// Removes a task from the scheduler.
    //**************************************************************************
    public: void Remove(ATask& task);

    //**************************************************************************
    /// Diagnostic method to display the current queue of tasks.
    //**************************************************************************
    public: void DumpQueue(const __FlashStringHelper* message=nullptr);

    
    //**************************************************************************
    /// Returns the last task in scheduler queue.
    //**************************************************************************
    private: ATask* LastTask();

    //**************************************************************************
    /// Finds a particular task in the scheduler queue.
    //**************************************************************************
    private: ATask* Find(Task* pTask);

    //**************************************************************************
    /// Returns the task previous to the specified task.
    //**************************************************************************
    private: ATask* FindPrev(Task* pTask);

    //**************************************************************************
    /// Links a task into the queue after the specified task.
    //**************************************************************************
    private: void LinkTask(ATask* pTask, ATask* pNewTask);

    //**************************************************************************
    /// Links a task  as the first task in the queue.
    //**************************************************************************
    private: void LinkFirstTask(ATask* pNewTask);

    //**************************************************************************
    /// Links a task as the last task in the queue.
    //**************************************************************************
    private: void LinkLastTask(ATask* pNewTask);


    /// The first task in the linked lists of task (i.e., the head of the list)
    private: ATask* _pFirstTask;
};


//******************************************************************************
/// Links a task  as the first task in the queue.
//******************************************************************************
inline void TaskScheduler::LinkFirstTask(ATask* pNewTask)
{
    pNewTask->_pNextTask = _pFirstTask;
    _pFirstTask = pNewTask;
}

#endif
