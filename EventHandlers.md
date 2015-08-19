At their core, state machines are all about by events. `hsm-statechart` remains agnostic about how events are defined so you can decide for yourself.



# Event Definition #

The various [HSM\_STATE](http://code.google.com/p/hsm-statechart/source/browse/hsm/hsm_state.h) macros define a StateDescriptor, and designate an event callback. For example:
```
  // this declaration:
  HSM_STATE( ExampleState, HsmTopState,0 );

  // expects this event handler:
  hsm_state ExampleStateEvent( hsm_status )
  {
    
  }
```

The `hsm_event` structure itself however still remains undefined, and in fact, in the sample code, different samples define events in different ways.

For example, [watch1\_named\_events](http://code.google.com/p/hsm-statechart/source/browse/samples/watch1_named_events.c) defines events with shared strings:
```
  static const char * WATCH_TICK= "Tick";
  static const char * WATCH_RESET_PRESSED= "Reset";
  static const char * WATCH_TOGGLE_PRESSED= "Toggle";

  typedef struct hsm_event WatchEvent;
  struct hsm_event {
    const char * name;  // <---- a string
  };  
  
  // later... send an event...
 
  WatchEvent evt= { WATCH_RESET_PRESSED };
  HsmProcessEvent( &hsm, &evt );

```

While [watch1\_enum\_events](http://code.google.com/p/hsm-statechart/source/browse/samples/watch1_enum_events.c) uses enums:

```
  typedef enum watch_events WatchEvents;
  enum watch_events {
    WATCH_RESET_PRESSED,
    WATCH_TOGGLE_PRESSED,
    WATCH_TICK,
  };

  typedef struct hsm_event WatchEvent;
  struct hsm_event {
    WatchEvents evt;  // <---- an enum
  };
  
  // sending looks the same as before,
  // so for completeness, 
  // handle the event...

  HSM_STATE( ActiveState,  HsmTopState, StoppedState );
    HSM_STATE( StoppedState, ActiveState, 0 );
    HSM_STATE( RunningState, ActiveState, 0 );

  hsm_state StoppedStateEvent( hsm_status status )
  {
    hsm_state ret=NULL;
    if (status->evt->name == WATCH_TOGGLE_PRESSED) {
      ret= RunningState();
    }            
    return ret;
  }
```

This should make it fairly straight-forward to implement events however you want, even reusing existing the events of exiting frameworks.  I'm pretty sure it's also a unique way of handling this issue.

# Events With Data #

Just most statechart implementations, `hsm-statechart` allows you to have multiple types of events, including events that have their own unique data structures, provided they all "derive" from the initial event structure you define.

Both examples in watch1, for instance, have a `WATCH_TICK` event. The tick passes the delta time ( really, it's always `1`, but that's example code for you ) by extending the base `WatchEvent`.

```
  typedef struct tick_event TickEvent;
  struct tick_event {
                     // in C you can't extend structs through class inheritance, but 
    WatchEvent evt;  // <---- this 
                     // works pretty much the same way.
    int time;
  };
  
  // later... send an event...
  
  TickEvent tick= { WATCH_TICK, 1 };
  HsmProcessEvent( &hsm, &tick.core );
```

The event handler simply looks at the event name `WATCH_TICK` to know that the `TickEvent` structure has been sent along.

```
hsm_state RunningStateEvent( hsm_status status )
{
  // ...
  if (status->evt->name == WATCH_TICK) {
    Watch* watch=((WatchContext*)status->ctx)->watch;
    TickEvent* tick= (TickEvent*)status->evt;
    TickTime ( watch, tick->time );
    printf("%d,", watch->elapsed_time );
    // ...
```

# Unhandled Events, Transitions, and Guards #

In order to tell the `hsm_machine` some particular event has been taken care of by its handler, the function **must** either return a new state for the machine to transition into, or -- if a transition isn't desired -- a special token: `HsmStateHandled`.

```
hsm_state RunningStateEvent( hsm_status status )
{
  hsm_state next_state=0;
  if (status->evt->name == WATCH_TICK) {
    Watch* watch=((WatchContext*)status->ctx)->watch;

    //... update the watch timer as before...
    
    // but make sure to tell the machine we've handled this event! 
    next_state= HsmStateHandled();

    // or, if there's a transition that needs to take place,
    // return that new state instead:
    if (watch->elapsed_time == 2112) {
      next_state= SecretMayanExplosionState();
    }
  }
  return next_state;
}
```

If your state listens for an event but decides not to handle it this time around, you should return NULL (0).

```
hsm_state GuardSleepingState( hsm_status status  )
{
  // by default: always return 0 from your handlers to designate the event has not been handled
  hsm_state next_state=0;
  switch (status->evt->id) {
    case FOOTSTEP: {
       if (status->evt->volume < deafening) {
           // still sleeping, so keep returning 0.
       }
       else {
           // only now move to a new state
           next_state= GuardDrowsyButAwakeState();  
       }
    }
    break;
  };
  return next_state;
}
```


# Related Work #

Stefan Heinzmann has an [even more decoupled way](http://accu.org/index.php/journals/252) of doing this -- he doesn't define events at all(!) Instead, he recommends users put a "current event" pointer in a derived state machine class. His method provides a measure of type-safety ( though only for the base event type ), but it comes at the expense of boiler plate code in each and every end user machine. ( It also slightly inflates the size of a machine, but only by one pointer. )