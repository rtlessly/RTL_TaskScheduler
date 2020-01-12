#ifndef _EventCodesX_H_
#define _EventCodesX_H_

class EventSourceID
{
    public: enum
    {
        Timer                  = 0x0100,
        Switch                 = 0x0200,
        IRSensor               = 0x0300,
        IRProximity            = 0x0400,
        IRProximityArray       = 0x0500,
        SonarSensor            = 0x0600,
        SonarPlatform          = 0x0700,
        DCMotorController      = 0x0800,
        StepperMotorController = 0x0900,
        MovementController     = 0x0A00,    /* deprecated */
        Movement               = 0x0A00,
        InertialReference      = 0x0B00,
        Navigation             = 0x0C00,
        Task                   = 0x0D00,
        Keypad                 = 0x0E00,
        CustomEvent            = 0xF000
    };
};

class EventCode
{
    public: enum 
    {
        DefaultEvent   = 0x0000,    // Default event for an event source
        DebugInfo      = 0x0001,    // Posted some debug information
        Detect         = 0x0002,    // Detected something
        Trigger        = 0x0003,    // CurrReading (such a timer firing)
        Toggle         = 0x0004,    // Toggled binary state from off to on or vis-versa
        Update         = 0x0005,    // Updated something
        Command        = 0x0006,    // Sent a command
        CommandRepeat  = 0x0007,    // Repeated a previous command
        CommandEnd     = 0x0008,    // End of repeated command
        Obstacle       = 0x0009,    // Detected an obstacle
        StartMotion    = 0x000A,    // Started moving
        StopMotion     = 0x000B,    // Stopped moving
        TurnBegin      = 0x000C,    // Began a turn
        TurnEnd        = 0x000D,    // Ended a turn
        Moved          = 0x000E,    // Moved by a certain amount
        Turned         = 0x000F,    // Turned by a certain amount
        Started        = 0x0010,    // Something has started
        Complete       = 0x0011,    // Something has completed
        Aborted        = 0x0012,    // Something has aborted
        Response       = 0x0013,    // A response event
        Notify         = 0x0014,    // A notification event
        SpinBegin      = 0x0015,    // Began a spin movement
        SpinEnd        = 0x0016,    // Ended a spin movement
        BackupBegin    = 0x0017,    // Began a backup movement
        BackupEnd      = 0x0018,    // Ended a backup movement
        KeyPressed     = 0x0019,    // A key was pressed on a keypad
        SpinAbort      = 0x001A,    // Aborted a spin movement
        TurnAbort      = 0x001B,    // Aborted a turn movement
    };
};


enum CommonEvents_enum
{
    TimerFiredEvent   = EventSourceID::Timer      | EventCode::DefaultEvent,
    NavigationEvent   = EventSourceID::Navigation | EventCode::DefaultEvent,
    TaskStartedEvent  = EventSourceID::Task       | EventCode::Started,
    TaskCompleteEvent = EventSourceID::Task       | EventCode::Complete,
    TaskAbortedEvent  = EventSourceID::Task       | EventCode::Aborted,
    TaskResponseEvent = EventSourceID::Task       | EventCode::Response,
};

#endif
