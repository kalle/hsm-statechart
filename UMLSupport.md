What follows is a list of features from UML statecharts<sup>[1]</sup> and how they are implemented (or not) in `hsm-statechart`.



# Simple states #
**Yes**. A simple state is a state with no sub-states.

The [HSM\_STATE](http://code.google.com/p/hsm-statechart/source/browse/hsm/hsm_state.h) macro is currently the easiest way to designate states.

The `HSM_STATE*` macros take define the states and their relationships take three parameters:
  1. The state being defined.
  1. The parent state.
  1. The initial state.

```
  HSM_STATE( ExampleState, HsmTopState,0 ); 
```

`HsmTopState` is a system defined state that is recommended for use as the outermost state of every hierarchy.

## Entry and exit actions ##
**Yes**. Entry and exit are modeled as function callbacks. You can run any actions you need in those callbacks.

Some systems, notably QEP, model entry and exit as events. However, under UML, entry and exit cannot have guards, nor can they have transitions, so it makes more sense to designate them as functions.

The HSM\_STATE\_ENTER and HSM\_STATE\_ENTERX can declare states with enter ( or  enter+exit ) actions.
```
  // declare just enter
  HSM_STATE_ENTER( ExampleState, HsmTopState,0 );
  
  // declare enter and exit
  HSM_STATE_ENTERX( ExampleState, HsmTopState,0 );

  // the enter function:
  hsm_context ExampleStateEnter( hsm_status status ) {
  }
  
  // the exit function:
  void ExampleStateExit( hsm_status status ) {
  }
```

## 'Do' actions ##
**Indirectly.** 'Do' actions represent ongoing updates for a state. Under UML, whenever a state is active 'do' actions are processed.

`hsm-statechart` doesn't model these directly -- in fact most statechart implementations don't -- but, you can either:
  * Create a "Tick Event", manually send the event to the machine on a regular basis, and handle the event in the states requiring 'do'.
  * Spawn an activity in an `enter` callback, and destroy the activity in an `exit` callback. The ContextStack can help manage the memory associated with the creation of activities ( including automatically cleaning up activities when they go out of scope ).

### Determining the cause of entry and exit ###
**Yes!** The latest UML spec dropped the requirement to pass the event which caused entry, but `hsm-statechart` does support this.

Many implementations lack this ability, but I believe it's of vital importance for managing state. More on this issue can be found here on my [development blog](http://dev.ionous.net/2012/05/statechart-data-2-events-and-entry.html).

# Compound states #
**Yes**. Compound states are states with one or more child states, where exactly one child is active at a time.

`hsm-statechart` models compound states via a `parent` pointer in the StateDescriptor. Any state, in effect, automatically becomes a composite whenever it's designated as some other state's parent.

Note: Under UML, the 'current' state of a statemachine cannot be a compound state. `hsm-statechart` does some link time validation of initial and parent state relationships to try to verify this, but it's not foolproof.

## Initial pseudo-state ##
**Partially**. Under UML, every composite state targeted by a transition must have an initial "pseudo-state" designating the 'default' sub-state that the machine should immediately enter. Transitions out of the pseudo-state into that default state, can have actions. Some systems, notably QEP, therefore have an "init event", and actions can be run just as with any other event.

As of Version 0.1 the StateDescriptor for a given state simply has an reference to the desired initial state. This is similar to how [MSM works](http://redboltz.wikidot.com/initial-transition-s-action).

From the watch sample, the initial state of `ActiveState` is `StoppedState`.

```
HSM_STATE( ActiveState, HsmTopState, StoppedState );
    HSM_STATE( StoppedState, ActiveState, 0 ); 
    HSM_STATE( RunningState, ActiveState, 0 );
```

In some rare case where you need an action to run on the init transition from 'ActiveState' to 'StoppedState', you can manually add an extra helper state, and then use the `enter` callback of the helper state to run the action you need. The code might look something like this:

```
HSM_STATE( ActiveState, HsmTopState, HelperState );
    HSM_STATE_ENTER( HelperState, ActiveState, 0 ); 
    HSM_STATE( StoppedState, ActiveState, 0 ); 
    HSM_STATE( RunningState, ActiveState, 0 );

hsm_context HelperStateEnter( hsm_status status )
{
    run_the_init_action_here();
    return status->ctx;
}
```

## Final State ##
**Yes** A statemachine or region that has terminated stays in this pseduo-state from that point out until the machine or region has been reset. Modeled in `hsm-statechart` via [HsmStateTerminated](http://code.google.com/p/hsm-statechart/source/browse/hsm/hsm_machine.h).

```
  hsm_state ExampleStateEvent( hsm_status status )
  {
    hsm_state next=0;
    if (status->evt== et_tu_brute) {
       next= HsmStateFinal();
    }
    return next;
  }
```

# Events #
**Yes**. Events are signals which cause a statemachine to transition from one state to the next.

EventHandlers in `hsm-statechart` are modeled with function callbacks. All events for a given state flow through the same callback function. The various [HSM\_STATE](http://code.google.com/p/hsm-statechart/source/browse/hsm/hsm_state.h) macros define a StateDescriptor and designate the callback function.

See EventHandlers for more specific info, but an example handler is:
```
  hsm_state CargoStateEvent( hsm_status status )
  {
    if (status->event == et_tu_brute) {
       return HsmStateFinal();
    }
    // ...
  }
```

## Event deferral ##
**Indirectly** There's nothing to stop you from writing your own event deferral system. `hsm-statechart` doesn't attempt to do this for you.


## Actions ##
**Yes** Actions are behaviors that a state can take in response to events.

Since `hsm-statechart` models EventHandlers as functions, it's easy to write ( or call ) actions using procedural code.

Note: In order to tell the statemachine that the event has been taken care of, the event handler needs to return a special token state: `HsmStateHandled`.  This token is particular to the `hsm-statechart' implementation. Other statemachine systems have their own ways of doing this.

```
  hsm_state CombatStateEvent( hsm_status status )
  {
    hsm_state ret=0;
    if (status->event == trigger_pulled) {
       fire_gun();
       ret= HsmStateHandled();
    }
    return ret;
  }
```

## Guards ##
**Yes** Guards determine whether an event should trigger the actions and transitions for some particular state.

Note: if a guard stops the state from handling the event, then you **should not** return `HsmStateHandled`.

```
  hsm_state CombatStateEvent( hsm_status status )
  {
    // by default return 0
    hsm_state ret=0;

    // guard: reload if you're out of bullets
    // but keep returning 0 if you've still got bullets
    if (status->evt == gun_fired && ((gun_event*)status->evt)->gun->bullets==0) {
       reload();
       ret= HsmStateHandled();
    }

    return ret;
  }
```

# Transitions #
**Mostly** Transitions in UML can happen as the result of an event, but they can also happen automatically as part of “flow-through” states that live only briefly. ( How fleeting sad life. )

In `hsm-statechart` transitions can only happen as the result of events. In fact, for safety's sake, there is no API that you can access to cause a transition outside of an event callback.

## Order of transition, enter, exit ##
**Non-standard**.  In UML the sequence of operations is supposed to be ( leaving all complications aside ):
  1. find the appropriate transition
  1. exit appropriate active state(s) running their exit actions,
  1. run the transition actions,
  1. enter the target state(s) running their their enter actions.

Since `hsm-statechart` uses a returned `next_state` from the [event handler](EventHandlers.md), but it's the EventHandler that runs the transition, the net result is:
  1. run the event handler
  1. user code runs the transition actions
  1. user code returns the desired next state
  1. the machine exits all appropriate active states running their exit callbacks
  1. the machine enters all target states running their enter callbacks

## Conflicting transitions ##
**Implementation defined**.  So far as I understand, the UML spec leaves the handling of conflicting transitions up to the implementation.

With `hsm-statechart` the transition taken is to the state returned from the [event handler](EventHandlers.md).

## Internal vs. External ##
**Partial.** All transitions are internal, except self-transitions which  exit and re-enter the state.

External transitions only affect what happens when the source of a transition is the ancestor of the target of the transitions. Internal transitions do not cause an exit of the source, external transitions do. Currently, in 'hsm-statechart' there is no way to configure which of the two styles any given transition should use: so they all use the internal style.

## Completion transitions ##
**No**

# History #

## Shallow history ##
**No**.

## Deep history ##
**No**. There are no plans to implement deep history.

# Parallel States ( [Regions](Regions.md) ) #

**Not Yet**. A parallel state is similar to a compound states in that they are both collections of child states, however, parallel states can have more than one child active at a time.

Compound states, in affect, say: activate this child **OR** that child; Parallel states say: active this child **AND** that child.

See [Regions](Regions.md) for complete details on what `hsm-statechart` supports, and a comparison with other systems.

# Miscellaneous Pseudo-States #
**Partially** Pseudo-states are pre-defined notational helpers. I've tried to cover them in the parts above that make the most sense. Here's a few I haven't mentioned.

## Choice Point ##
**Indirectly**. Choice points are a notational convention in UML diagrams which allow transitions from multiple places to meet, and then branch out again. They are ways of sharing decision making logic.

Choice Points can be modeled with `hsm-statechart` by creating event processing functions shared by multiple states.

```
  hsm_state MyChoicePoint( data some_data ) 
  {
    hsm_state choice= ExampleState3(); 
    if (some_data == bad_data_tm) {
       choice= HsmStateFinal();
    }
    return choice;
  }

  hsm_state ExampleState1Event( hsm_status status )
  {
     hsm_state next_state=0;
     if (status->evt == interesting) {
       some_data= run_a_function();
       next_state= MyChoicePoint( some_data );
     }
     return next_state;
  }

  hsm_state ExampleState2Event( hsm_status status )
  {
     hsm_state next_state=0;
     if (status->evt == also_interesting) {
       some_data= run_other_function();
       next_state= MyChoicePoint( some_data );
     }
     return next_state;
  }
```

# Footnotes #

(1) Okay. I'll admit it. I've almost completely given up trying to understand the [UML 2.4.1 superstructure documentation](http://www.omg.org/technology/documents/modeling_spec_catalog.htm#UML). I find it confusing and frequently ambiguous. The `hsm-statechart` implementation instead relies on the definitions from [UML v1.3](http://code.google.com/p/hsm-statechart/downloads/detail?name=uml-section3.73-94.pdf), and [common knowledge](Resources.md) pulled from other implementations of Harel's and UML statecharts.