#include "TaskBase.h"
#include "RTL_TaskManager.h"


TaskBase::TaskBase(TaskState startingState)
{
    _taskState = startingState;
}


bool TaskBase::Run()
{
    if (_taskState == Running) return (Poll(), true);
    if (_taskState == Resuming) return (Resume(), true);

    return false;
};


void TaskBase::Suspend()
{
    if (_taskState == Suspended) return;

    StateChanging(Suspending);
    _taskState = Suspended;
}


void TaskBase::Resume()
{
    if (_taskState == Running) return;

    StateChanging(Resuming);
    _taskState = Running;
}
