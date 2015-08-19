A page to keep track of non-bug feature work. See also: the [issues list](http://code.google.com/p/hsm-statechart/issues/list).



# **Core** #
Core statechart functionality.
| OnEnter error handling | OnEnter needs a mechanism for user code to communicate critical errors ( ex. can't allocate context data ). Options include: the lua technique of long jumping; ~~returning NULL from OnEnter to indicate error~~ ( doesn't work for non-context machines); setting an `hsm_status` flag. ex. see BuildingStateEnter |
|:-----------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Make names optional    | Preconditional compiler macro for disabling the storage of names.                                                                                                                                                                                                                                                    |

# **Builder** #
Mechanisms to make definitions look state-like. 
| Scope | Currently, names must be globally unique. Consider an hsmScope() or hsmContext() that starts a new namespace block. |
|:------|:--------------------------------------------------------------------------------------------------------------------|
| Region support | The builder API needs support for regions.                                                                          |
| Reusable substates | Because names are global, not local to their branch, there's currently no way to reuse substates.                   |                                                                                                                     |
| Better error handling. | Take a look at opengl. ex. hsmResolveId(), what if the finished building state is false, hsmGotoState(), what if registered failed? |
| Register external states | Need a way to mix builder and core states. ex. void hsmGotoState( hsm\_state state void hsmSubState( hsm\_state parent ) hsmEnd() or hsmSelectState( parent ); hsmBegin();  |
| User generated state ids | HsmBuilder currently requires named states; it might be nice instead, if it could use user generated ids. Would need an HsmBeginId(). |
| Collision detection | Currently, in the extremely unlikely case that two state names generate the same crc, there's no notification to the user. |
| Memory configuration | Allow user code to specify c/malloc and free                                                                        |
| Validation | Write a function to do optional evaluation of a built statemachine to ensure all states are properly defined.       |
| Chain hsmRun | Provide way to specify multiple action functions per transition.                                                    |
| If and And | Determine whether hsmIf, and hsmAnd, could share userdata in order to limit,reduce the hsmBuilder API variants.     |


# **Lua** #

| Improve error handling | TBD: someway to provide context for charts that are improperly defined; better error reporting for those that cause errors during execution. Consider passing a file handle to machine start and log via the handle to whatever level of verbosity is desired. | |
|:-----------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:|
| hsm machine info callbacks | just like c-side, provide callbacks for listening to machine internals                                                                                                                                                                                         |
| Reusable substates     |                                                                                                                                                                                                                                                                |
| Hula interface         | expose the hula statechart creator interface into lua, and port the HulaBuildState to lua. The lua-c api makes the code much larger  than it needs to be.                                                                                                      | |
| Scope one builder to each lua\_State | Currently all lua states share the same c-side structures; this seems bad.                                                                                                                                                                                     | |
| Graphing               | Integration, translation to, rHsm to support graphing.                                                                                                                                                                                                         | |


# **C++ support** #
helpers to make definitions look state-like.

| c++ wrappers for the hsm machine |
|:---------------------------------|
| nested classes declaration helpers |
| static asserts for core hierarchy definition |
| templates and builders for transitions  |


# **Documentation** #
| An extended sample | Infocom style game with a few randomly generated rooms and monsters. |
|:-------------------|:---------------------------------------------------------------------|
| Metrics            | Document the size of core states and statemachines; builder; and lua ||


# **Unit tests** #
| Null pointers |
|:--------------|
| Invalid state charts |
| Some lua tests including cloning machines |
| Standalone tests of the context stack - no charts |

# Done stuff #

| Asserts | Assert configuration via precompiler macros | 0.5 |
|:--------|:--------------------------------------------|:----|
| ~~per-state transition tables~~ | i no longer feel the work for macro based actions and guards is valuable enough to warrant the effort. the builder seems to work well enough. |
| Hsm Userdata | Provide an object API to allow lua to create and manipulate statemachines, not just declare them. | 0.5 |
| Watch diagram | Change HsmBuilder "documentation" over to the stopwatch example; make a watch diagram; show comparisons, side-by-side if possible, of different implementations: core only, builder, lua. |
| Lua usage | Lua documentation needs finishing.          |
| HulaUserIsEvent | Re-evaluate this function. It's needed when events are defined in lua, but signaled in c. But, what if the events are signaled in lua? Consider adapter/wrapper states as a possible solution to translation. | 0.5  |
| Lualib  | Provide whatever packaging is needed to use hsm-statechart in an interpreter based environment. eg. [LuaRocks](http://luarocks.org/) | 0.5 |