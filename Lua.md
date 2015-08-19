# Overview #

Lua's a great language for providing glue between systems, and iterating quickly during development. Statecharts, in a sense, provide the glue between actors and activities in an application, so the combination of lua + statecharts proves powerful.

Using lua also provides an opportunity to show alternate ways of using hsm-statechart as an "engine" to implement stateful systems.

# How to get started #

The recommend approach is to first install [Lua Rocks](http://luarocks.org). Lua Rocks is a package manager for lua, and makes building and installing lua modules easy. ( Make sure to check out the [other great rocks](http://luarocks.org/repositories/rocks/) while you are there. )

Once you've installed Lua Rocks, you can install the `hsm_statechart` rock by typing:
```
luarocks install hsm_statechart
```

You can also pull straight from googlecode using:
```
luarocks build http://hsm-statechart.googlecode.com/hg/hsm_statechart-0.5-1.rockspec
```

Both methods will fetch the code, and build the module for you. ( You will need a c-compiler of some sort for this to work. Both both the visual studio compiler, and gcc are supported. )

If you want to run the wxLua stop watch sample, you will also need install [wxLua](http://wxlua.sourceforge.net/index.php).

# What the lua code looks like #

```
local stop_watch_chart= {
  -- each state is represented as a table
  -- active is the top-most state
  active= {
    -- entry to the active state clears the watchs timer
    -- the watch is provided by machines using this chart
    entry=
      function(watch) 
        watch.time=0
        return watch
      end,

    -- reset causes a self transition which re-enters active
    -- and clears the time no matter which state the machine is in
    evt_reset = 'active',  

    -- the active state is stopped by default:
    init = 'stopped',

    -- while the watch is stopped: 
    stopped = {
      -- the toggle button starts the watch running
      evt_toggle = 'running',
    },

    -- while the watch is running:
    running = {
      -- the toggle button stops the watch
      evt_toggle = 'stopped',

      -- the tick of time updates the watch
      evt_tick   =
        function(watch, time)
          watch.time= watch.time + time
        end,
    }
  }
}

-- our simple watch representation
local watch= { time = 0 }

-- our statemachine to run the above chart with our simple watch
require "hsm_statechart"
local hsm= hsm_statechart.new{ stop_watch_chart, context= watch }

-- keep going until the watch breaks
while hsm:is_running() do
  -- send an event to the machine
  hsm:signal( 'evt_tick', 1 )
  -- ....
end
```

# For more information #
Check out the [StopWatch](StopWatch.md) tutorial, and the [sample code](http://code.google.com/p/hsm-statechart/source/browse/#hg%2Fsamples%2Fhula).