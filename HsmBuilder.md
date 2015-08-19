# Motivation #

Creating readable code-based statecharts will make for fewer bugs, and more easily modifiable machines. Ideally, a statemachine library could provide an interface for defining states that makes the code look similar to a statechart drawn on paper or in a tool.

# Introduction #

The `hsm_builder` interface is inspired by OpenGL, and like OpenGL, it uses a state machine under the hood to dynamically construct structured data. Which means, for `hsm-statechart` a statemachine is helping to construct a statemachine!

# Details #

The builder is a completely _optional_ way to define states using `hsm-statechart`.  It sits above the core interface, and the core interface knows nothing about it. ( As a side note: the [Lua](Lua.md) code uses the builder to construct charts out of the users' tables, but the builder, in turn, knows nothing about _it_. )

What follows is an implementation of the stop watch chart from [the tutorial](StopWatch.md). I've left out the event definitions, machine initialization, and main loop.  That part is all the same as the tutorial.
```
// user implemented function required to filter events
static hsm_bool MatchEvent( hsm_status status, WatchEvents evt ) 
{
    return status->evt->type == evt;
}

// recommend macro to improve chart readability
#define IfEvent( val ) hsmIfUD( (hsm_callback_guard_ud) MatchEvent, (void*) val )

hsm_state buildWatchChart() 
{
    int id= hsmBegin( "Active",0 );
    {
        hsmOnEnter( ActiveStateEnter );   // active state enter resets the timer
        // if the user presses "reset" 
        // no matter which state we're in, transition to self, that means: 
        // reset the time ( via enter ), and enter initial state ( stopped )
        IfEvent( WATCH_RESET_PRESSED ); 
        hsmGoto( "Active" );
            
        // the first sub-state entered is the first state listed
        // in this case the first thing active does is enter 'stopped'
        hsmBegin( "Stopped",0 );
        {
            IfEvent( WATCH_TOGGLE_PRESSED );
            hsmGoto( "Running" );
        }
        hsmEnd();
        hsmBegin( "Running",0 );
        {
            IfEvent( WATCH_TOGGLE_PRESSED );
            hsmGoto( "Stopped" );

            // on tick events, update the watch timer
            IfEvent( WATCH_TICK );
            hsmRunUD( RunTickTime, 0 );
        }
        hsmEnd();
    }
    hsmEnd();
    return hsmResolveId(id);
}

```

It's approximately 20 fewer lines of code for the same chart defined with the core interface. Not a huge difference, but still I think this version almost documents itself, where the other takes a little head scratching.

The builder **does** allocate memory to construct the chart. The machines that use the chart, however, do not dynamically allocate memory.

This is the complete source: [watch\_builder.c](http://code.google.com/p/hsm-statechart/source/browse/samples/watch_builder.c)