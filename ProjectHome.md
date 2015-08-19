`hsm-statechart` is a fast, lightweight, state machine implementation written in C, based on UML statecharts.

## Motivation ##

There are [other](Resources#Other_Implementations.md) statechart implementations out there, so what's so good about hsm-statechart?

Here are some of the advantages hsm-statechart offers:
  1. A liberal license for games and indie development.
  1. The creation of state machines directly in code.
  1. An easy to understand hierarchical engine, with low policy requirements on user code.
  1. It's written in C and ports easily between OSX, Win32, and mobile platforms.

In addition hsm-statechart supports:
  * Zero dynamic memory allocation in its core API.
  * Optional per state instance data in the extended API.
  * A compact [declarative interface](HsmBuilder.md).
  * [Lua](Lua.md)(!)

## Status of the code ##

[Version 0.5](http://code.google.com/p/hsm-statechart/downloads/list) is here! Source includes: [doxygen](http://hsm-statechart.googlecode.com/hg/docs/html/index.html) html reference, some [unit tests](http://code.google.com/p/hsm-statechart/source/browse/#hg%2Ftest), and several short [samples](http://code.google.com/p/hsm-statechart/source/browse/samples). Both C and Lua are supported; you can find the Lua rock on [LuaRocks.org](http://LuaRocks.org).

A good place to get started is to read through the [Stop watch tutorial](StopWatch.md), and look at the optional [declarative](HsmBuilder.md) way for defining state machines. Questions and feedback are more than welcome.

[Future releases](FutureWork.md) are planned to increase [UML statechart compatibility](UMLSupport.md), and improve the re-usability of states in different charts.