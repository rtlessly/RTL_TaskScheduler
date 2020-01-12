#ifndef _EventQueue_h_
#define _EventQueue_h_

#include <RTL_StdLib.h>
#include "Event.h"


/*******************************************************************************
Event queue manager.

EventQueue is a global static singleton that queues and dispatches events in a
program.
*******************************************************************************/
class EventQueue
{
    DECLARE_CLASSNAME;

    private: static const int QUEUE_SIZE = 8;

    // The singleton instance
    //public: static EventQueue SoleInstance;

    /*--------------------------------------------------------------------------
    Base class overrides
    --------------------------------------------------------------------------*/
    //public: void Poll();

    /*--------------------------------------------------------------------------
    Public interface
    --------------------------------------------------------------------------*/
    public: static bool Queue(Event& event) { return Queue(*event.Source, event.EventID, event.Data);  };

    public: static bool Queue(EventSource& source, EVENT_ID eventID, variant_t eventData = 0L);

    public: static bool Dequeue(Event& event);

    public: static void Dispatch();

    public: static int8_t Length() { return _queueCount; };

    /*--------------------------------------------------------------------------
    Internal implementation
    --------------------------------------------------------------------------*/
    /// The event queue 
    private: static Event _queue[QUEUE_SIZE];   // size = sizeof(Event)*QUEUE_SIZE = 8*8 = 64 bytes

    /// The event queue head and tail pointers
    private: static uint8_t _queueHead;         // size = 1
    private: static uint8_t _queueTail;         // size = 1
    private: static int8_t  _queueCount;        // size = 1
};

#endif
