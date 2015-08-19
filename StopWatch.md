

# Introduction #

The stopwatch samples included in the source are modeled after the [boost statechart](http://www.boost.org/doc/libs/1_49_0/libs/statechart/doc/tutorial.html#BasicTopicsAStopWatch) tutorial. A similar tutorial exits for [Apache's scxml](http://commons.apache.org/scxml/usecases/scxml-stopwatch.html).

This tutorial is meant to demonstrate:
  * basic state declaration
  * simple event handling
  * simple hierarchical charts
  * different ways of using events and context data

It shows how all this is done in both C and Lua. I'll assume you maybe know a little bit about state charts, but you don't have to be any sort of statemachine expert. ( Is there such a person? )

# Defining the Problem #

Conceptually, this simple stopwatch has just two buttons and a dial. One button resets the timer, and the other is a toggle to start and stop the watch. The dial simply shows the elapsed time.

A simple mockup of the watch ( built in [wxLua](http://wxlua.sourceforge.net/index.php) ) looks like this:

![http://hsm-statechart.googlecode.com/hg/docs/images/watch1.jpg](http://hsm-statechart.googlecode.com/hg/docs/images/watch1.jpg)

# The Stop Watch #

In general practice, it's good to separate your models -- the logical objects of your application -- from their behavior. This makes both sides more
flexible and reusable. In this example, the following snippets are all we really need for this simple watch.

<table>
<tr><th>C++</th>
<th></th>
<th>Lua</th></tr>
<tr><td><pre>
class Watch<br>
{<br>
void tick( float delta );<br>
void reset_timer();<br>
float get_elapsed();<br>
private:<br>
float time;<br>
};</pre>
</td>
<td></td>
<td>
watch= { time =0 }<br>
</td>
</tr>
<table>

It's hopefully easy to imagine how you'd display the above model in the pictured stopwatch dialog. The big questions are: how do you handle those buttons, and how do you handle updates? And that of course, is what the statechart is for. Keeping the logic of the watch separate from its display, <b>and</b> from its model.<br>
<br>
<h1>The Statechart</h1>

Here's where you might pull out a piece of paper and begin to sketch a statechart  to support all this. Or, you could just look at this handy UML diagram instead:<br>
<br>
<img src='http://hsm-statechart.googlecode.com/hg/docs/images/watchchart.jpg' />

( In truth, especially with the lua version of <code>hsm-statechart</code>, I probably would skip the drawing stage, and go straight to the code. I think you'll see why in a moment. )<br>
<br>
<h1>Understanding the Chart</h1>

Let's look at the watch chart one state, and one button at a time, and use that as a guide towards the code.<br>
<br>
<h2>Watch startup</h2>

The top-most state here is called the <code>Active</code> state. In this case, when the chart enters the <code>Active</code> state, the <code>entry</code> event resets the watch time. This puts the watch in a good state to start with.<br>
<br>
In UML statecharts, after <code>entry</code> has occurred, a state then enters its initial child state (if any). The initial state is marked always marked with an empty circle and an arrow.<br>
<br>
Since we don't want our batteries to be <code>Running</code> down before we even get the watch home, this chart starts out <code>Stopped</code>.<br>
<br>
<h2>Watch events</h2>

The <code>entry</code> event is one of the predefined UML statechart events, and although it doesn't appear here, <code>exit</code> is another. For this chart, the remaining three events are all custom events: created specifically to handle the watch logic.  They are:<i>evt_toggle</i>, <i>evt_reset</i>, and <i>evt_tick</i>.<br>
<br>
<code>evt_toggle</code> will get triggered whenever the user presses the watch's toggle button, and <code>evt_reset</code> when they press the watch's reset button. <code>evt_tick</code>, on the other hand, we'll just send continuously to the watch. This continuous signalling may not be how a real quartz watch would work, but we do it here to show that the outside world -- the code systems that this watch is embedded in -- don't need to know which state the watch is in. The outside code acts generically with respect to the watch, and the watch logic makes up its own mind what to do.<br>
<br>
Do these events and their behavior as described in the machine make sense?<br>
<br>
Well, you can see in the middle of the diagram that <i>evt_toggle</i> moves the machine to <code>Running</code> while in <code>Stopped</code>, and to <code>Stopped</code> while in <code>Running</code>. That's the behavior we wanted from the outset. Also, notice, that only the body of <code>Running</code> handles the Tick. So, even though Tick is firing madly, the watch time is only updated while the machine is <code>Running</code>.<br>
<br>
That seems about right then. Let's hold off on <i>evt_reset</i> for a second, and look at how to implement what we've got so far in code.<br>
<br>
<h1>The State Machine</h1>

In all flavors of <code>hsm-statechart</code> you need three things:<br>
<ol><li>A chart<br>
</li><li>A machine to run the chart.<br>
</li><li>The events to send to the machine.</li></ol>

Let's start with the middle one, then loop around to end with the chart.<br>
<br>
In the C implementation there are actually two different machine "classes" -- the core <code>hsm_machine_t</code>, and a slightly weightier ( by 8 bytes! ) extension <code>hsm_context_machine_t</code>. The context machine allows states to have optional per state data, and provides an easy way to give the whole chart access to external data. While we don't <b>absolutely</b> need the context machine for this simple example, it does make accessing the watch object easier, so we'll go ahead and use it.<br>
<br>
<pre><code>    Watch watch;                        // an instance of our watch model.<br>
    hsm_context_machine_t machine;      // our statemachine.<br>
    WatchContext ctx= { 0, 0, &amp;watch }; // access to the watch by the machine;<br>
                                        // 0,0 are system defaults.<br>
    <br>
    // initialize the statemachine, pass our watch context data.<br>
    hsm_machine hsm= HsmMachineWithContext( &amp;machine, &amp;ctx.ctx );<br>
<br>
    // start the machine, handing it the topmost state.<br>
    // we'll see the state definitions in just a minute....<br>
    HsmStart( hsm, ActiveState() );<br>
</code></pre>

Watch context in the above is just a small struct wrapping the watch data, it looks like:<br>
<br>
<pre><code>    typedef struct watch_context WatchContext;<br>
    struct watch_context {<br>
        hsm_context_t ctx;  // a required system defined structure ( 8 bytes )<br>
        Watch * watch;<br>
    };<br>
</code></pre>

The lua interface is just as easy, except every machine is automatically a context machine, and you pass the chart instead of the top state.<br>
<pre><code>  require 'hsm_statechart'<br>
<br>
  -- create a simple representation of a watch<br>
  local watch= { time = 0 }<br>
<br>
  -- create a state machine with the chart, and the watch as context<br>
  local hsm= hsm_statechart.new{ stop_watch_chart, context= watch }<br>
</code></pre>

<h1>Sending Events</h1>

The <code>hsm-statechart</code> code is very flexible. At it's core it has a C based implementation and other layers are built up around that.  Applications can define events pretty much however they want.  In the following examples, I'll use enums for the C side, but note: the lua side uses strings. Really, anything goes.<br>
<br>
<table>
<tr><th>C++</th>
<th></th>
<th>Lua</th></tr>
<tr><td><pre>
enum watch_events {<br>
WATCH_RESET_PRESSED,<br>
WATCH_TOGGLE_PRESSED,<br>
WATCH_TICK,<br>
};<br>
typedef struct hsm_event_rec WatchEvent;<br>
struct hsm_event_rec {<br>
enum watch_events type;<br>
};<br>
</pre>
</td>
<td></td>
<td><pre>
-- in lua you don't have to predeclare events<br>
-- they're simply strings: "evt_reset", "evt_toggle", "evt_tick"<br>
</pre></td>
</tr>
<table>
To create and send an event to the machine is a snap, in C it looks like:<br>
<br>
<pre><code>  while ( HsmIsRunning( hsm ) ) {<br>
    // get a key from the keyboard<br>
    const int ch= PlatformGetKey();<br>
<br>
    // turn a '1' into the reset button,<br>
    // turn a '2' into the toggle button<br>
    const WatchEvents events[]= { WATCH_RESET_PRESSED, WATCH_TOGGLE_PRESSED };<br>
    const int index= ch-'1';<br>
    if ((index &gt;=0) &amp;&amp; (index &lt; sizeof(events)/sizeof(WatchEvents))) {<br>
      const WatchEvent evt= { events[index] };<br>
      HsmSignalEvent( hsm, &amp;evt );<br>
    }<br>
</code></pre>

Lua is again very much the same as the C-implementation ( the chart, on the other hand, as you will shortly see, is radically simpler ):<br>
<br>
<pre><code>  -- lookup table for mapping keyboard to events<br>
  local key_to_event = { ["1"]='evt_reset',<br>
                         ["2"]='evt_toggle' }<br>
<br>
  -- keep going until the watch breaks<br>
  while hsm:is_running() do<br>
    local key= string.char( platform.get_key() )<br>
<br>
    -- change it into an event<br>
    local event= key_to_event[key]<br>
<br>
    -- send it to the statemachine<br>
    if event then<br>
      hsm:signal( event )<br>
    end<br>
  end<br>
</code></pre>


The tick event is just an extension of the basic watch event which adds the elapsed time.<br>
<br>
<pre><code>typedef struct tick_event TickEvent;<br>
struct tick_event {<br>
    WatchEvent core;<br>
    int time;<br>
};<br>
<br>
// once per loop, or after some accumulated time, issue a tick<br>
const int elapsed_time= 1; // vanish the coin in the usual fashion.<br>
const WatchEvent watchevt= { WATCH_TICK, elapsed_time };<br>
HsmSignalEvent( hsm, &amp;watchevt );<br>
</code></pre>

You can have as many different kinds of events and associated event data structures as your machine needs.<br>
<br>
In Lua, you can use tables to pass event structures, but you don't need to if you don't want. Just pass one or more additional parameters to the signal function:<br>
<br>
<pre><code>local elapsed_time= 2; -- how fast is coding in lua? twice as fast.<br>
hsm:signal( "evt_tick", elapsed_time )<br>
<br>
-- an example of multiple parameters, *not* how this chart works.<br>
hsm:signal( "evt_tick", seconds, milliseconds, microseconds )<br>
<br>
-- an example of table, also *not* how this chart works.<br>
hsm:signal( "evt_tick", { seconds=5, days=1, years=23 } )<br>
</code></pre>


<h1>States and Event Handling</h1>

States handle the events sent to the machine. The "active" state gets the first crack at handling every event. If it doesn't respond to the event, then the parent gets a chance.<br>
<br>
<h2>C Event Handling</h2>

On the C-side ( is that commonly known as the shore? ) the most basic way of implementing a state's event handler is to define a function. That one function will receive all events for that state, and with enum based events, you simply write a switch with case statements for each event.<br>
<br>
Stopped is the simplest state, so lets start there:<br>
<pre><code>// The naming convention can be changed by redefining some macros<br>
// by default, it's &lt;name of state&gt;Event<br>
hsm_state StoppedStateEvent( hsm_status status )<br>
{<br>
    // return NULL by default to indicate unhandled events<br>
    hsm_state ret=NULL;<br>
    switch (status-&gt;evt-&gt;type) {<br>
        // but, hen the 'toggle' button gets pressed...<br>
        case WATCH_TOGGLE_PRESSED:<br>
            // transition over to the running state<br>
            ret= RunningState();<br>
        break;<br>
    }<br>
    return ret;<br>
}<br>
</code></pre>

The <code>hsm_status</code> object is a <code>const *</code> with three members:<br>
<ol><li>a pointer to the machine: <code>hsm</code>
</li><li>the currently processing event: <code>evt</code>, which is the same event structure we declared above.<br>
</li><li>the context data for the state: <code>ctx</code>, which in this chart is the watch data.</li></ol>

It's those last two items that make it possible to implement the running state:<br>
<br>
<br>
<pre><code>hsm_state RunningStateEvent( hsm_status status )<br>
{<br>
    // by default this function does nothing....<br>
    hsm_state ret=NULL;<br>
    switch (status-&gt;evt-&gt;type) {<br>
        // but, when the 'toggle' button gets pressed....<br>
        case WATCH_TOGGLE_PRESSED:<br>
            // transition back to the stopped state<br>
            ret= StoppedState();<br>
        break;<br>
        // also, when a 'tick' is sent, update our timer<br>
        case WATCH_TICK:<br>
        {<br>
            // our event is the tick event<br>
            TickEvent* tick= (TickEvent*)status-&gt;evt;<br>
            // our context is the watch object<br>
            Watch* watch=((WatchContext*)status-&gt;ctx)-&gt;watch;<br>
            // tick by this much time<br>
            TickTime ( watch, tick-&gt;time );<br>
            // indicate to the statemachine that we've take care of this event<br>
            // this stops the event from being sent to our parent<br>
            ret= HsmStateHandled();<br>
        }<br>
        break;<br>
    }<br>
    return ret;<br>
}<br>
</code></pre>

Notice that in the WATCH_TICK case, the code not only updated the watch, it also returned a special value: <code>HsmStateHandled</code> in order to stop the event from moving up the chart. You should return <code>handled</code> anytime you've done something as a result of the event, but don't want a transition to a new state to occur.<br>
<br>
As a side note: if you're ever tempted to write a little bit of logic for a child state event, and return NULL to allow the parent state to <b>also</b> handle the event -- you probably want to refactor the statechart. It works in code, but it's not how statecharts are intended to function.<br>
<br>
<h2>Lua Event Handling</h2>

In Lua, things are a little different. States aren't represented as functions, they are represented as <b>tables</b>. The event handlers are simply keys in those tables.  Let's see how it works.<br>
<br>
For simple event handlers like <code>Stopped</code>, where all you need to do is simply transition to a new state, you simply provide an entry in the state table mapping the event name to the name of the next state.<br>
<br>
Stopped, in it's entirety, looks like this:<br>
<pre><code>    -- while stopped: <br>
    stopped = {<br>
      -- toggle starts the watch running.<br>
      evt_toggle = 'running'<br>
    }<br>
<br>
</code></pre>

Short and sweet, no?<br>
<br>
In Lua, of course, you can store anything in a table, and that includes functions. For Running, therefore, all we need to do is map the toggle event to the stopped state, and then map the tick event to a function which updates the time.<br>
<br>
<pre><code>     -- while the watch is running:<br>
    running = {<br>
      -- the toggle button stops the watch<br>
      evt_toggle = 'stopped',<br>
<br>
      -- the tick of time updates the watch<br>
      evt_tick   =<br>
        function(watch, time)<br>
          watch.time= watch.time + time<br>
        end,<br>
    }<br>
</code></pre>

<code>hsm-statechart</code> does all the behind the scenes magic necessary to make declaring states in Lua as easy as possible.<br>
<br>
<h2>Handling Reset</h2>

The one event we held off on was <code>evt_reset</code>. For this watch: we want the reset button to both clear the timer, and stop the timer.<br>
<br>
As you've seen in the previous code, no state has code handling <code>evt_reset</code>. That means the parent state of both 'Running<code> and </code>Stopped` will get a chance to handle it.<br>
<br>
In the chart, <code>Active</code> is that parent state. It contains both <code>Running</code> and <code>Stopped</code>. So, let's see its event handler:<br>
<br>
<pre><code>hsm_state ActiveStateEvent( hsm_status status )<br>
{<br>
    // by default this function does nothing....<br>
    hsm_state ret=NULL;<br>
    switch ( status-&gt;evt-&gt;type ) {<br>
        // but, whenever the reset button is pressed...<br>
        case WATCH_RESET_PRESSED:<br>
            // it transitions to itself.<br>
            ret= ActiveState();<br>
        break;<br>
    }<br>
    return ret;<br>
}<br>
</code></pre>

That self-transition, the little curved arrow in the statechart diagram at the start of the tutorial, triggers one very specific behavior. The state exits, and then re-enters itself.  This re-entry is completely indistinguishable from when the watch was started.  The Active state's entry function is called, and then the machine moves into the Stopped state. This behavior, therefore, provides exactly what we want.<br>
<br>
For completeness, sake here is Active's entry function:<br>
<br>
<pre><code>hsm_context ActiveStateEnter( hsm_status status )<br>
{<br>
    Watch* watch=((WatchContext*)status-&gt;ctx)-&gt;watch;<br>
    watch-&gt;time=0; // or maybe, ResetTime( watch ) to be a bit cleaner.<br>
    return status-&gt;ctx;<br>
}<br>
</code></pre>

So, that's all nice and everything, but how do these functions actually get called at the right time?<br>
<br>
<br>
<h1>Implementing the Chart</h1>

The last and final step is to declare the chart as a whole: to show how the states interrelate, and define how the hierarchy works.<br>
<br>
<h2>Chart of the C</h2>

In the core hsm-statechart API, hierarchy is defined via macros. These macros also connect up the logic to call all the functions we've defined above. The chart in it's entirety looks like this:<br>
<pre><code>// all of the HSM_STATE* macros take: <br>
//    1. the state being declared;<br>
//    2. the state's parent state;<br>
//    3. an initial child state to move into first, or zero.<br>
//<br>
// the only difference b/t HSM_STATE_ENTER and HSM_STATE is that HSM_ENTER <br>
// declares a callback that gets called whenever the state gets entered...<br>
// <br>
HSM_STATE_ENTER( ActiveState, HsmTopState, StoppedState );<br>
    HSM_STATE( StoppedState, ActiveState, 0 );<br>
    HSM_STATE( RunningState, ActiveState, 0 );<br>
<br>
</code></pre>

Believe it or not, we are finally done. We've seen the machine that uses this chart, we've defined all of our events, and triggered them at appropriate times, and we've also implemented all the handlers which get called for each state and each event.<br>
<br>
If you want to see the complete source for this example, just take a look at the watch1_enum_events.c sample code, included in the source, and also: <a href='http://code.google.com/p/hsm-statechart/source/browse/samples/watch1_enum_events.c'>here</a>.<br>
<br>
<h2>Lovely Lua</h2>

In Lua, the chart is -- just like the states themselves -- a table. When you define the states, you are actually simultaneously defining the chart.<br>
<br>
There's really no way to break it down into parts, so here it is, finally, all at once:<br>
<br>
<pre><code>local stop_watch_chart= {<br>
  -- each state is represented as a table<br>
  -- active is the top-most state<br>
  active= {<br>
    -- entry to the active state clears the watch timer<br>
    -- the watch is provided by machines using this chart<br>
    entry=<br>
      function(watch) <br>
        watch.time=0<br>
        return watch<br>
      end,<br>
<br>
    -- reset causes a self transition which re-enters active<br>
    -- and clears the time no matter which state the machine is in<br>
    evt_reset = 'active',  <br>
<br>
    -- the active state is stopped by default:<br>
    init = 'stopped',<br>
<br>
    -- while the watch is stopped: <br>
    stopped = {<br>
      -- the toggle button starts the watch running<br>
      evt_toggle = 'running',<br>
    },<br>
<br>
    -- while the watch is running:<br>
    running = {<br>
      -- the toggle button stops the watch<br>
      evt_toggle = 'stopped',<br>
<br>
      -- the tick of time updates the watch<br>
      evt_tick   =<br>
        function(watch, time)<br>
          watch.time= watch.time + time<br>
        end,<br>
    }<br>
  }<br>
}<br>
</code></pre>

Like the C version, tthe complete sample is included in the source, and it's <a href='http://code.google.com/p/hsm-statechart/source/browse/samples/hula/watch.lua'>also here</a>.<br>
<br>
The <a href='http://code.google.com/p/hsm-statechart/source/browse/samples/hula/watch.wx.wlua'>wxLua version</a> has the exact same statechart, and the same watch model, but uses a dialog to gather input from the user, and display the watch's time.