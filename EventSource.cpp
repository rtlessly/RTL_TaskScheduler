/*******************************************************************************
A base class for an object that sources events.
*******************************************************************************/
#define DEBUG 0

#include <Arduino.h>
#include "Event.h"
#include "EventQueue.h"
#include "EventBinding.h"
#include "EventSource.h"


DEFINE_CLASSNAME(EventSource);


//******************************************************************************
// Add an event binding to this EventSource's list of bindings
//******************************************************************************
void EventSource::Attach(IEventBinding& binding)
{
    // Insert the new binding at the head of the linked list
    binding._nextLink = (_firstBinding != nullptr) ? _firstBinding : nullptr;
    _firstBinding = &binding;
    TRACE(Logger(_classname_, this) << F("Attach: binding=") << _HEX(PTR(&binding)) << endl);
}


//******************************************************************************
// Add an event listener to this EventSource's list of listeners
//******************************************************************************
IEventBinding* EventSource::Attach(IEventListener& listener, EventBinding* pBinding)
{
    if (pBinding == nullptr)
    {
        for (auto pBinding = (EventBinding*)_firstBinding; pBinding != nullptr; pBinding = (EventBinding*)pBinding->_nextLink)
        {
            if (pBinding->_pListener == &listener) return pBinding; 
        }

        pBinding = new EventBinding(listener);
    }

    Attach(*pBinding);

    return pBinding;
}


IEventBinding* EventSource::Attach(EVENT_LISTENER pfListener, StaticEventBinding* pBinding)
{
    if (pBinding == nullptr)
    {
        for (auto pBinding = (StaticEventBinding*)_firstBinding; pBinding != nullptr; pBinding = (StaticEventBinding*)pBinding->_nextLink)
        {
            if (pBinding->_pfEventListener == pfListener) return pBinding; 
        }

        pBinding = new StaticEventBinding(pfListener);
    }

    Attach(*pBinding);

    return pBinding;
}


//******************************************************************************
// Removes an event binding from this EventSource's list of bindings
//******************************************************************************
void EventSource::Detach(IEventBinding& binding)
{
    IEventBinding*& nextLink = _firstBinding;

    do 
    {
        if (nextLink == &binding)
        {
            binding.Unlink(nextLink);
            break;
        }

        nextLink = nextLink->_nextLink;
    } 
    while (nextLink != nullptr);
}


//******************************************************************************
// Queues an event with the given event ID and data.
//******************************************************************************
void EventSource::QueueEvent(EVENT_ID eventID, variant_t eventData)
{
    TRACE(Logger(_classname_, this) << F("QueueEvent: eventID=") << _HEX(eventID) << endl);

    Event event(eventID, eventData); { event.Source = this; }

    EventQueue::Queue(event);
}


//******************************************************************************
// Queues an event with the given event ID and data.
//******************************************************************************
void EventSource::QueueEvent(Event& event)
{
    TRACE(Logger(_classname_, this) << F("QueueEvent: eventID=") << _HEX(event.EventID) << endl);

    event.Source = this;

    EventQueue::Queue(event);
}


//******************************************************************************
// Creates and dispatches an event with the given event ID and data to the
// attached listeners.
//******************************************************************************
void EventSource::DispatchEvent(EVENT_ID eventID, variant_t eventData)
{
    Event event(eventID, eventData);  { event.Source = this; }

    DispatchEvent(event);
}


//******************************************************************************
// Dispatches an event to all listeners of this EventSource
//******************************************************************************
void EventSource::DispatchEvent(Event& event)
{
    TRACE(Logger(_classname_, this) << F("DispatchEvent: eventID=") << _HEX(event.EventID) << endl);

    for (IEventBinding* pBinding = _firstBinding; pBinding != nullptr; pBinding = pBinding->_nextLink)
    {
        pBinding->DispatchEvent(event);
    }
}

