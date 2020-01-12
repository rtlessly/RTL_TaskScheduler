# RTL_TaskManager

An Arduino library that performs simple task and state management and scheduling.

The TaskManager class is a static singleton clas that implements a simple
dispatch loop to execute a list of tasks. The list of tasks is supplied to
the TaskManager via the TaskManager::SetTaskList() method, which takes an
array of pointers to tasks (the end of the array must contain a null pointer
to mark the end of the task list). 

TaskManager only handles immutable task lists, i.e., no tasks can be added or 
removed from the task list. However, individual tasks in the task list can be 
suspended to effectively remove them from execution. In addition,the task list 
can be changed at any time via SetTaskList() so that you can dynamically supply 
different task lists as needed.

The task disptach is handled by the TaskManager::Dispacth() method, which 
should be invoked on every iteration through the Arduino loop() function. 
Every invocation of the Dispatch() method loops through the task list one 
time, calling each task's Poll() method (if the task is active).

The TaskBase class is the abstract base class for tasks that are managed
by the TaskManager. A concrete task class derives from TaskBase and overrides
the Poll() method, which executes the task-specific functionality (such as 
reading a sensor). Since the Poll() method is called by the TaskManager on
every iteration through the Arduino loop() function, the Poll() method should
be designed to only execute a small "unit of work" on each ivocation.

In addition to overriding the Poll() method, a task can also override the 
TaskChangeState() method to monitor changes in it's state - specifically when
it is being activated (TaskState::Resuming) or deactivated (TaskState::Suspending).
These notifications give the task a chance to perform some initialization on
activation (resuming) or to quiesce itself on deactivation (suspending).

The TaskManager class also provides a basic state machine implementation.
A state is just a special task that is a subclass of the StateBase class 
(itself a sublcass of TaskBase). TaskManager maintains a pointer to a single
StateBase that repersents the currently active state, which is set by the 
SetCurrentState() method. The TaskManager::Dispatch() method polls the active
state (via its Poll() method) after all other task have been polled. As with 
a task's Poll() method, the state's Poll() method should also be designed to 
only execute a small "unit of work" on each ivocation.

Similar to a task, a state also has a OnStateChanged() method that notifies when
the state is being activated or deactivated. These notifications give the state 
a chance to perform some initialization on activation (resuming) or to quiesce 
itself on deactivation (suspending).

A state usually has a set of tasks associated with it. The state activates its
task list by calling the TaskManager::SetTaskList() method with its task list 
(typically during the TaskState::Resuming notification). The SetTaskList() method 
will deactive the prior task list (be suspending all tasks in it that task list), 
switch to the new taak list, then activate all tasks in the new task list. If
a state does not require any tasks it should still call SetTaskList() with an
empty task list.

A third function of the TaskManager class is to dispatch events that have
been queued. Events are queued by tasks (or other event sources) to the global
EventQueue object. After the TaskManager has dispatched all active tasks, it 
walks the EventQueue and dispatches all queued events to the currently active
state. A state has an OnEvent() method that can bew overriden to handle any 
events dispatched to it by the TaskManager while the state is active. 

This is only a brief, high-level overview. Some details have been omitted. See
the documentation of each class for more specific information.