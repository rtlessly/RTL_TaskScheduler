#define DEBUG 0

#include <Arduino.h>
#include "EventSource.h"
#include "EventQueue.h"


/*******************************************************************************
Global event queue.

EventQueue is a global static singleton that queues and dispatches events in a
program. 

The EventQueue::Dispatch() method should be invoked on every iteration of the 
sketch's loop() method. The Dispatch() will dispatch all events that had been 
queued up to the point it was called. It does not dispatch any new events triggered
as a result of processing disptached events. Those events will be dispatched on
the next go-around.
*******************************************************************************/

DEFINE_CLASSNAME(EventQueue);

//EventQueue EventQueue::SoleInstance;

Event   EventQueue::_queue[QUEUE_SIZE];
uint8_t EventQueue::_queueHead = 0;
uint8_t EventQueue::_queueTail = 0;
int8_t  EventQueue::_queueCount = 0;


//******************************************************************************
// Queues an event to the event queue
//******************************************************************************
bool EventQueue::Queue(EventSource& eventSource, EVENT_ID eventID, variant_t eventData)
{
    /*
    Interrupts MUST be disabled while an event is being queued to ensure stability
    while the queue is being manipulated. But, disabling interrupts MUST come
    BEFORE the queue-full check is done.

    If the queue-full check is FALSE and an asynchronous interrupt queues an
    event while that check is being made, thus making the queue full, the current
    insert operation will then corrupt the queue because it will add an event to
    an already full queue. So the entire operation, from the queue-full check to
    completing the insert must be atomic.

    Note that this race condition can only happen when an event is queued by normal
    (non-interrupt) code and simultaneously an interrupt also tries to queue an event.
    In the case where only normal code is queueing events, this can't happen because
    all normal code executes synchronously on a single thread. Conversely, if only
    interrupts are queueing events, it can't happen either because further interrupts
    are blocked while the current interrupt is being serviced. In the general case,
    however, we have to assume that events could enter the queue both ways. So,
    to protect against this race condition we have to disable interrupts.

    Contrast this with the logic in the Dequeue() method.
    */

    auto isQueued = false;

    noInterrupts(); // ATOMIC BLOCK BEGIN

    if (_queueCount <= QUEUE_SIZE)
    {
        _queue[_queueTail].Source  = &eventSource;
        _queue[_queueTail].EventID = eventID;
        _queue[_queueTail].Data    = eventData;
        _queueTail = (_queueTail + 1) % QUEUE_SIZE;
        _queueCount++;
        isQueued = true;
    }

    interrupts(); // ATOMIC BLOCK END

    return isQueued;
}


bool EventQueue::Dequeue(Event& event)
{
    TRACE(Logger(_classname_) << F("Dequeue") << endl);

    /*
    Interrupts MUST be disabled while an event is being de-queued to ensure
    stability while the queue is being manipulated. HOWEVER, disabling interrupts
    MUST come AFTER the queue-empty check.

    There is no harm if the queue-empty check (_queueHead == _queueTail) produces 
    an "incorrect" TRUE response while an asynchronous interrupt queues. It will 
    just pick up that event the next time Dequeue() is called.

    However, If interrupts are suppressed before the queue-empty check, we pretty
    much lock-up the system. This is because Dequeue() is normally called inside
    loop(), which means it is called VERY OFTEN.  Most of the time (>99%), the
    event queue will be empty. But that means that we'll have interrupts turned
    off for a significant fraction of time and we don't want to do that. Instead,
    interrupts should only be turned off when we actually have an event to de-queue.

    Contrast this with the logic in the Queue() method.
    */

    // Check for empty queue
    if (_queueHead == _queueTail) return false;

    noInterrupts(); // ATOMIC BLOCK BEGIN

    event = _queue[_queueHead];
    _queueHead = (_queueHead + 1) % QUEUE_SIZE;
    _queueCount--;

    interrupts();   // ATOMIC BLOCK END

    return true;
}


void EventQueue::Dispatch()
{
    // Dispatch all events that were queued up to this point.
    // NOTE: This loop is specifically constructed to only go around the event queue
    // at most one time. It does NOT dispatch any new events added as a result of
    // processing a dispatched event. Those will get processed on the next go-around. 
    // Otherwise, we could create an endless loop where object A posts an event 
    // that object B receives who, in turn, posts an event that object A receives, 
    // etc... In such a scenario the event queue would never empty and the dispatch 
    // loop would go on forever.
    for (auto i = _queueCount; i > 0; i--)
    {
        Event event;

        if (Dequeue(event))
        {
            if (event.Source != nullptr) event.Source->DispatchEvent(event);
        }
    }
}
