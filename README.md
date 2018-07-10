# RTL_TaskScheduler

An Arduino library that performs simple task management and scheduling.

The TaskScheduler class maintains a queue of tasks to execute and implements 
a simple dispatch loop to execute those tasks. The ATask class is an abstract
base class for a task, while the Task class is a concrete subclass that wraps
a function which embodies the actual task functionality. The task function 
takes no arguments and returns a boolean value. 

If a task returns true it signals the scheduler to restart its task execution
loop, whereas returning false tells the scheduler to continue on to the next
task in the queue. This simple mechanism permits the prioritized execution of
tasks, where tasks higher in the queue can preempt later tasks by returning 
true. However, it also expects each task to "play nice" so that no task hogs
the CPU by returning true all the time and preventing any other task from
executing.

Task functions are typically designed so that they execute a small "unit of 
work" on each iteration.

This simple design works well for controllers that need to monitor multiple
sensors and processes that execute in response to those sensors. For example,
a task may check the value of a sensor. If the sensor has triggered it initiates
some process and return true. Otherwise, the task just returns false. Note that
the initiated process itself may be another task that is dynamically inserted
into the queue.

Internally, the task queue is implemented as a singly-linked list. The TaskScheduler
class maintains a pointer to the first task in the queue, while the ATask class
provides a pointer to the next task in the queue. The last task in the queue has
its next pointer set to NULL. The TaskScheduler includes methods to link and unlink
tasks in the queue. This mechanism avoids the need to use a preallocated fixed-
sized queue, or to use new to dynamically allocate queue entries - both of which
are frowned upon in an Arduino environment (preallocation can waste precious RAM
memory by allocating a queue that is larger than what is actually used, while new
carries overhead, is constrained by limited available memory, and also is less 
memory efficient).
