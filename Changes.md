# Changes #

## Version 0.5 ##
| In progress |
|:------------|

  * [Lua](Lua.md) support is coming, changes to the builder as it gets exercised more.
  * builder code has moved under the `hsm` directory

## Version 0.4 ##

  * changed/unified the interface of enter, exit, etc. callbacks using a new struct: [hsm\_status](http://code.google.com/p/hsm-statechart/source/browse/hsm/hsm_status.h) ( see below )
  * simplifies the declaration of machines with context data  ( see below )
  * makes [info callbacks](http://code.google.com/p/hsm-statechart/source/browse/hsm/hsm_info.h) global ( instead of per-statemachine ), and adds more things you can listen for.
  * shrinks HSM\_MAX\_DEPTH from 32 to 16
  * separates "test" code into actual unit tests ( in "/test" ) and separate "/samples"
  * add the HsmBuilder interface
  * greatly improved the doxygen based documentation


The depth changes and the callbacks going global were to support the simplified context machine decl. and still keep the size of the core statemachine object small. The core hsm\_machine\_t had been 24 bytes, now it's 16 bytes. Adding a context stack ( by, using hsm\_context\_machine\_t instead of hsm\_machine\_t ) brings it up to 32 bytes.

```
  // instead of callbacks looking like this:
  hsm_state MyStateEvent( hsm_machine_t*hsm, hsm_context_t*ctx,   WatchEvent* evt )
  {
  }

  // they now look like this:
  hsm_state MyStateEvent( hsm_status status )
  {
     // everything is still there, and a little more:
     // status->hsm, status->ctx, status->evt
     //
     // and also:
     // status->state
  }
```

```
// was:
    hsm_machine_t hsm;
    hsm_context_stack_t stack;
    HsmMachine( &hsm, &stack, NULL );
    HsmStart( &hsm, &context, first_state );

// now:
    hsm_context_machine_t hsm;
    HsmMachineWithContext( &hsm, &context );   
    HsmStart( &hsm, first_state );
```

## Version 0.1 ##
  * original distribution