#pragma once

#include <Event.h>
#include "TaskBase.h"

//enum StateEvent
//{
//    Resuming,
//    Running,
//    Suspending,
//    Suspended,
//};
//

//******************************************************************************
/// The base class for a state machine state.
///
/// The StateBase class defines a virtual Poll() method that should be implemented 
/// in a derived class. This method contains the actual functionality of the state. 
/// The Poll() method is called by the TaskManager on each iteration through its 
/// task queue. As such, the Poll() method should be designed so that it executes
/// only a small unit of work on each iteration. In addition, it should be designed
/// to "fail fast" so that task exits as quickly as possible if it has no work to do.
///
/// A StateBase can also handle events by overriding the OnEvent() method. When a 
/// StateBase is the active state machine state it is automatically the target of
/// any events that are posted to the EventQueue. Note that a dormant state
/// (i.e., NOT the active state) does not recieve any events - only the currently
/// active state does.
//******************************************************************************
class StateBase : public TaskBase           /* Size = 10 bytes */
{
    friend class TaskManager;

    /*--------------------------------------------------------------------------
    Constructors
    --------------------------------------------------------------------------*/
    public: StateBase() {}

    /*--------------------------------------------------------------------------
     Public interface
    --------------------------------------------------------------------------*/
    //**************************************************************************
    /// Handles events when the state is the active state.
    //**************************************************************************
    public: virtual void OnEvent(const Event* pEvent) { };

    /*--------------------------------------------------------------------------
     Overrides
    --------------------------------------------------------------------------*/
    public: const __FlashStringHelper* Name() override { return F("StateBase"); };
};
