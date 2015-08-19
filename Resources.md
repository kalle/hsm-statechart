# Useful Documentation #

  * [A crash course in UML state machines](http://eetimes.com/design/embedded/4008247/A-crash-course-in-UML-state-machines-Part-1) ( [pdf](http://classes.soe.ucsc.edu/cmpe013/Spring11/LectureNotes/A_Crash_Course_in_UML_State_Machines.pdf) version of same ).
  * Boost's [State Machine tutorial](http://www.boost.org/doc/libs/1_49_0/libs/msm/doc/HTML/ch02s02.html">Meta)
  * The statemachine part of [UML 1.3 docs](http://code.google.com/p/hsm-statechart/downloads/detail?name=uml-section3.73-94.pdf).
  * Official [UML 2.4.1 docs](http://www.omg.org/spec/UML/2.4/).
  * [Unofficial](http://www.uml-diagrams.org/state-machine-diagrams.html#behavioral-state-machine)  UML 2.4 docs.
  * Wikipedia:
    * [State diagram](http://en.wikipedia.org/wiki/State_diagram)
    * [Finite state machine](http://en.wikipedia.org/wiki/Finite-state_machine)
    * [Uml state machine](http://en.wikipedia.org/wiki/UML_state_machine)
  * [Sproutcore](http://blog.sproutcore.com/statecharts-in-sproutcore/) is a web ui development framework. Their blog has an interesting look at UI and statecharts.
  * [scxml](http://www.w3.org/TR/scxml/): statechart xml documentation. Has decent descriptions of behavior.


# Books #

  * [Modeling Reactive Systems with Statecharts](http://www.wisdom.weizmann.ac.il/%7Eharel/reactive_systems.html) by [David Harel](http://en.wikipedia.org/wiki/David_Harel) ( free )
  * [Practical UML Statecharts](http://www.amazon.com/Practical-UML-Statecharts-Second-Edition/dp/0750687061) by Miro Samek
  * [Constructing the User Interface with Statecharts](http://www.amazon.com/Constructing-User-Interface-Statecharts-Horrocks/dp/0201342782) by [Ian Horrocks](http://en.wikipedia.org/wiki/Ian_Horrocks) ( out of print )

# Other Implementations #
I've mainly been trying to track down fully functional ( if not necessarily fully featured ) C and C++ hierarchical statemachines. There are a bunch of projects which have been started, then stopped; with little to no documentation. It doesn't seem there are many meant to be used as standalone libraries. Here are a few that are.

#### [QP](http://state-machine.com/) ####
This is the implementation described in Miro Samek's Practical Statechart book. It's C based, with licenses both commercial and GNU. The company also provides a free [graphical modeling tool](http://www.state-machine.com/qm/shot0.jpg): you model in the tool, add your code in the tool, and then it generates the QP implementation incorporating the code you input.

#### [Boost MSM](http://www.boost.org/libs/msm/doc/HTML/index.html) ####
Boost MSM is a close to a full UML spec implementation.

What I see as benefits are: Statecharts implemented with the MSM are **amazing** readable. It generates the whole machine at compile time, so I suspect its smaller and faster at run-time than almost any other system.

What I see as drawbacks are: Its template magic makes it **incredibly** slow to compile. More importantly it seems to require the use of a concrete event class -- not a pointer or a reference -- for event dispatch which makes event queuing, and the separation of code into different modules difficult.

There's an [msm guide](http://redboltz.wikidot.com/boost-msm-guide) on reboltz as well.

#### [Boost Statechart](http://www.boost.org/libs/statechart/doc/index.html) ####
That's right! Boost has more than one statechart implementation.

States are modeled as instances of classes. They are created on state enter, and destroyed on state exit; RTTI, and C++ exceptions are a key part of its implementation. Like MSM, it has compile time checking of machine correctness.

The author has a list of [advantages](http://www.boost.org/doc/libs/1_49_0/libs/statechart/doc/rationale.html#Introduction) boost statechart provides.

#### [YAHSM](http://accu.org/index.php/journals/252) ####
A simple, easy to understand templated statechart. It's not meant to be a complete statechart implementation, more a starting point.

## Lua ##

  * [Miros](http://www.bellfelljar.org/tractwo/wiki/Lua%20Projects) A straight port of QP from C to pure lua.
  * [rFSM](http://people.mech.kuleuven.be/~mklotzbucher/rfsm/README.html)  An implementation of UML statecharts from scratch in pure lua. Its most distinguishing features are its ability to generate graphical charts of the code based charts(!), and support for uml `do` events.

## Code Generators ##
There are bunches of professional grade statemachine code generator tools. Mainly, they seem to be targeted for Java.

[SinelaboreRT](http://www.sinelabore.com) - is a visual editor and simulation tool for hierarchical state machines that targets, C, C++, Java and C#. I like that you edit the hierarchy with a tree view control, and it generates the pretty pictures for you. It has a relatively inexpensive license fee, and seems worth checking out.

[SMC](http://smc.sourceforge.net/) - the statemachine compiler is not meant to be a statechart implementation so it does not support hierarchical states, but it does target both C and C++, along with many other languages as well.

## Domain Specific Languages ##
Martin Fowler wrote about using DSL to describe simple non-hierarchical statemachines. There are various implementations of his idea around the web. These are a few:

  * [http://blog.efftinge.de](http://blog.efftinge.de/2012/05/implementing-fowlers-state-machine-dsl.html)
  * http://simplestatemachine.codeplex.com/
  * [stateless](http://code.google.com/p/stateless/) is a C# based version of this idea.