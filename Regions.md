# Introduction #
This is a companion to the [UMLSupport](UMLSupport.md) doc. Regions and all the ways they can transition are complicated enough to need their own page.

In order to broaden the discussion a tad, I've made a feature comparison chart with other systems that support regions.

**A note on terminology:**
In UML statecharts, a composite state is any state with child states. Such a state can **either** contain two or more concurrently running regions, or it can contain one or more mutual exclusive substates.

The UML documentation has no convention for differentiating between those two kinds of composite states, other than to refer to their contents over and over again. For simplicity, the [scxml docs](http://www.w3.org/TR/scxml) refers to a state that is split into concurrent regions as a [Parallel State](http://www.w3.org/TR/scxml/#N10193), while they refer to the more normal version as a [Compound State](http://www.w3.org/TR/scxml/#N10153). I'll do the same here.

Note also, that some features of a parallel state ( ex. "explicit exit", below. ) also don't have names in the UML docs. [MSM](Resources#Boost_MSM.md) does, so I've used theirs.

# Overview #

  * [QP](Resources#QP.md) does not support regions.
  * [MSM](Resources#Boost_MSM.md): Boost.MSM has possibly the most complete support I've seen.
  * [BSC](Resources#Boost_Statechart.md): Boost Statechart has partial support of regions.
  * `hsm-statechart` will soon have partial support of regions.

# Comparision Table #

| Feature             | **HSM** | **MSM** | **BSC** |
|:--------------------|:--------|:--------|:--------|
| [Initial Pseudostate](Regions#Initial_Pseudostate.md)  |   Y'    |   Y'    |   Y'    |
| [Final State](Regions#Final_State.md)         |   N     |   N'    |   N'    |
| [Parallel Entry](Regions#Parallel_Entry.md)      |   p     |   Y     |   Y     |
| [Parallel Exit](Regions#Parallel_Exit.md)       |   p     |   Y     |   Y     |
| [Fork](Regions#Fork.md)                |   n     |   Y     |   n     |
| [Join](Regions#Join.md)               |   n     |   n     |   n     |
| [Explicit Entry](Regions#Explicit_Entry.md)      |   n     |   Y'    |   n     |
| [Explicit Exit](Regions#Explicit_Exit.md)      |   n     |   n     |   n     |
| [Entry Pseudo Point](Regions#Entry_Pseudo_Point.md)  |   n     |   Y     |   n     |
| [Exit  Pseudo Point](Regions#Exit_Pseudo_Point.md)  |   n     |   Y     |   n     |
| [Synchronization](Regions#Synchronization.md)    |   n     |   n     |   n     |

key:
  * Y : fully.
  * Y': with some caveats .
  * N': the system has a specific work around.
  * n : no support.
  * p : pending.

# An example chart #
This chart uses the same structure as one from the [MSM](http://www.boost.org/doc/libs/1_49_0/libs/msm/doc/HTML/ch03s02.html#d0e852) tutorial.

![http://hsm-statechart.googlecode.com/files/parallel_state_diagram_sm.png](http://hsm-statechart.googlecode.com/files/parallel_state_diagram_sm.png)

I've reproduced it in graphics and text to help with feature descriptions.
```
  FSM:
    S1:
      -e1 >> S2.                         # Parallel Entry
      -e2 >> S2::SS2.                    # Explicit Entry
      -e3 >> ( S2::SS2, S2::SS2B )       # Fork
      -e4 >> S2::PseudoEntry.            
    S2:                                  # Parallel State w/ regions
      -e1 >> S1.                         # Region Exit 
      PseudoEntry:                       # Pseudo Entry Point
        -auto / entry_action >> SS2.     # 'auto' used to indicate an immediate transition.
      ------                             # Marks a region
      A:
        SS1:
        SS2:
          -e6 >> SS1.                    # a normal transition
        SS3:
          -e5 >> PseudoExit.             
        PseudoExit:                      # Pseudo Exit Point
          -auto >> S3.
      ------                             # Marks another region
      B:
        SS1b:
        SS2b:
    S3: 
```

# Region Features #

#### Initial Pseudostate ####
**Yes** supported by `hsm-statechart`.

Techinically, the transition from the initial pseudostate to the first state should allow an action. HSM, BSC, MSM all jump straight to the default state. The [MSM guide](Resources#MSM.md) shows how to simulate a transition with action if needed, which would work for all three systems.

> UML 1.3 (pg.3-130):
> > A newly-created object takes its top most default transition, originating from the topmost initial pseudostate.... A transition to the enclosing state represents a transition to the initial pseudostate.
> > An initial pseudostate is shown as a small solid filled circle... The initial transition may have an action.

#### Final State ####
**No** not supported by `hsm-statechart` for regions, b/c it does not currently have a way to specify what the completion event would be, nor where it would go.


> UML 1.3 (pg.3-131)
> > A final state is shown as a circle surrounding a small solid filled circle (a bull’s eye)....
> > A transition to a final state represents the completion of activity in the enclosing region `[`usually represented as an unlabled transition from the region's container to some outside state`]`.
> > Completion of activity in all concurrent regions represents completion of activity by the enclosing
> > state and triggers a completion event on the enclosing state.
> > An object that transitions to its outermost final state is terminated.

#### Parallel Entry ####
**Soon** to be supported by `hsm-statechart`.

Describes a transition targeting a parallel state directly, not any of its contained children. The initial transitions in each of state's regions run.

In the diagram above it would be `S1: -e1 >> S2.`

UML 1.3 (pg.3-137)

> A transition drawn to the boundary of a composite state is equivalent to a transition to
> its initial point (or to a complex transition to the initial point of each of its concurrent
> regions, if it is concurrent). The entry action is always performed when a state is
> entered from outside.

#### Parallel Exit ####
**Soon** to be supported by `hsm-statechart`.

Describe a transition originating from a parallel state directly, not any of its contained children.

In the diagram above it would be `S2: -e1 >> S1.`

UML 1.3 (pg.3-137)
> A transition from a composite state indicates a transition that applies to each of the states within the state region (at any depth). It is “inherited” by the nested states. Inherited transitions can be masked by the presence of nested transitions with the same trigger.

That's a super confusing statement. I interpret it to mean Parallel Exit causes all contained states to exit. But, if a state inside one of the regions handles an event, then the parallel itself doesn't hear about the event, and therefore Parallel Exit doesn't occur.

#### Fork ####
**No** not supported by `hsm-statechart`.

> UML 1.3 (pg.3-137)
> > A bar with multiple transition arrows leaving it maps into a fork pseudostate

#### Join ####
**No** not supported by `hsm-statechart`.


> UML 1.3 (pg.3-137)
> > A bar with multiple transition arrows entering it maps into a join pseudostate.

#### Explicit Entry ####
**No** not supported by `hsm-statechart`.
#### Explicit Exit ####
**No** not supported by `hsm-statechart`.
#### Entry Pseudo Point ####
**No** not supported by `hsm-statechart`.
#### Exit Pseudo Point ####
**No** not supported by `hsm-statechart`.
#### Synchronization ####
**No** not supported by `hsm-statechart`.