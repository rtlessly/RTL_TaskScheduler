#pragma once
/*******************************************************************************
Header file for the TaskManager class.
*******************************************************************************/

#include <RTL_StdLib.h>
#include "TaskBase.h"
#include "StateBase.h"
#include "Event.h"
#include "EventSource.h"
#include "EventQueue.h"


//******************************************************************************
/// The TaskManager is a static singleton class that implements a dispatch loop
/// to execute a list of tasks.
///
/// The list of tasks is supplied to the TaskManager via the SetTaskList()
/// method. TaskManager only handles immutable task lists, i.e., no tasks can 
/// be added or removed from the task list. However, individual tasks in the task
/// list can be suspended to effectively remove them from execution. In addition,
/// the task list can be changed at any time via SetTaskList() so that you can
/// dynamically supply different task lists as needed.
/// 
/// The TaskManager class also provides a basic state machine implementation.
/// A state is just a special task that is a subclass of the StateBase class 
/// (itself a sublcass of TaskBase). TaskManager maintains a pointer to a single
/// StateBase that repersents the currently active state, which is set by the 
/// SetCurrentState() method. The TaskManager::Dispatch() method polls the active
/// state after all other task have been polled.
/// ============================================================================
/// IMPORTANT: The task list *MUST* be terminiated with a null entry to mark the
///            end of the list.
//******************************************************************************
class TaskManager           /* Size = 6 bytes (16 bit) or 12 bytes (32 bit) */
{
    DECLARE_CLASSNAME;

    /*--------------------------------------------------------------------------
    Constructors
    --------------------------------------------------------------------------*/
    //**************************************************************************
    /// Private constructor to enforce static singleton semantics.
    //**************************************************************************
    private: TaskManager() {};


    /*--------------------------------------------------------------------------
    Public interface
    --------------------------------------------------------------------------*/
    //**************************************************************************
    /// Dispatches the next task.
    //**************************************************************************
    public: static void Dispatch();

    //**************************************************************************
    /// Sets the current task list. Returns the previously active task list;
    ///
    /// IMPORTANT: The task list *MUST* be terminiated with a null entry to mark
    ///            the end of the list.
    //**************************************************************************
    public: static TaskBase** SetTaskList(TaskBase* newTaskList[], bool autoeResume=true, bool autoSuspend=true);

    //**************************************************************************
    /// Sets the current state machine state task. Returns the previously active
    /// state task;
    //**************************************************************************
    public: static StateBase* SetCurrentState(StateBase* pNewState);
    public: static StateBase* SetCurrentState(StateBase& newState) { return SetCurrentState(&newState); };

    //**************************************************************************
    /// Determines if a task is in the scheduler queue.
    //**************************************************************************
    public: static bool IsScheduled(TaskBase* pTask);

    //**************************************************************************
    /// Diagnostic method to display the current list of tasks.
    //**************************************************************************
    public: static void DumpTaskList(const __FlashStringHelper* message = nullptr);


    /*--------------------------------------------------------------------------
    Internal implementation
    --------------------------------------------------------------------------*/
    /// The task list array containing pointers to tasks. 
    /// IMPORTANT: The task list *MUST* be terminiated with a null entry to
    ///            mark the end of the list.
    private: static TaskBase** _taskList;

    /// The pointer to the next task pointer in the task list to execute.
    private: static TaskBase** _taskPointer;

    /// The pointer to the current state machine state task.
    private: static StateBase* _pCurrentState;
};
