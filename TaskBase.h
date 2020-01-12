#pragma once
/*******************************************************************************
Header file for the TaskBase class.
*******************************************************************************/

#include <RTL_StdLib.h>


enum TaskState
{
    Resuming,
    Running,
    Suspending,
    Suspended,
};


//******************************************************************************
/// The base class for a pollable task.
///
/// The TaskBase class defines a virtual Poll() method that should be implemented 
/// in a derived class. This method contains the actual functionality of the task. 
/// The Poll() method is called by the TaskManager on each iteration through its 
/// task queue. As such, the Poll() method should be designed so that it executes
/// only a small unit of work on each iteration. In addition, it should be designed
/// to "fail fast" so that task exits as quickly as possible if it has no work to do.
//******************************************************************************
class TaskBase                         /* Size = 4 bytes */
{
    friend class TaskManager;

    /*--------------------------------------------------------------------------
     Constructors
    --------------------------------------------------------------------------*/
    protected: TaskBase() : TaskBase(TaskState::Resuming) { };

    protected: TaskBase(TaskState startingState);

    /*--------------------------------------------------------------------------
     Public interface
    --------------------------------------------------------------------------*/
    //**************************************************************************
    /// The function that the task executes. Derived classes must implement this 
    /// function to do useful work.
    //**************************************************************************
    public: virtual void Poll() {};

    //**************************************************************************
    /// <summary>
    /// Notified when a task is changing state.
    /// </summary>
    /// <param name="state"> The new state the task is transitioning to. </param>
    /// <remarks>
    /// The possible states values, and the meanings, are as follows:
    ///
    /// State value             Meaning
    /// --------------------    ------------------------------------------------
    /// TaskState::Resuming     Signals that the task is starting, or resuming from
    ///                         a suspended state. The task should perform any actions
    ///                         necessary to activate/reactivate the task, such as
    ///                         initialization.
    /// TaskState::Suspending   Signals that the task is about to be suspended. The
    ///                         Task should perform any actions necessary to suspend
    ///                         the task.
    /// </remarks>
    //**************************************************************************
    public: virtual void StateChanging(TaskState newState) {};

    //**************************************************************************
    /// Suspends execution of a running task. The task's Poll() method will not 
    /// be called as long as it is suspended. A task can only be suspended while 
    /// in the starting or running state. Calls to Suspend() while in any 
    /// other state are ignored.
    //**************************************************************************
    public: void Suspend();

    //**************************************************************************
    /// Reusmes execution of a suspended task. This transitions the task back into
    /// the running state. A task can only be resumed if it is in the suspended 
    /// state. Calls to Resume() while in any other state are ignored.
    //**************************************************************************
    public: void Resume();

    //**************************************************************************
    /// Runs the task according to its current state. Returns true if the task
    /// executed; otherwise, false is returned.
    //**************************************************************************
    public: bool Run();

    //**************************************************************************
    /// Indicates if a task is currently in the running state.
    //**************************************************************************
    public: bool IsRunning() { return _taskState == TaskState::Running; };

    //**************************************************************************
    /// Returns the name of the task (i.e., the class name).
    //**************************************************************************
    public: virtual const __FlashStringHelper* Name() { return F("TaskBase"); };

    /*--------------------------------------------------------------------------
     Internal implementation
    --------------------------------------------------------------------------*/
    /// The current task state
    private: TaskState _taskState;
};
