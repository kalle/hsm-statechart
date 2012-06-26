/**
 * @file hsm_builder.h
 *
 * Declarative interface for defining states.
 *
 * The builder is a layer on top of the core hsm statemachine code.
 * It is not necessary to use, or even include, the builder to use hsm-statechart.
 *
 * \internal
 * Copyright (c) 2012, everMany, LLC.
 * All rights reserved.
 * 
 * Code licensed under the "New BSD" (BSD 3-Clause) License
 * See License.txt for complete information.
 */
/*
 * proper reuse of builder is to rebuild not share states
 * ie. function() { hsmState(...): }
 * call it twice to embed the state machine twice, 
 * dont try to reuse the name it returns in multiple places
 */
#pragma once
#ifndef __HSM_BUILDER_H__
#define __HSM_BUILDER_H__

/**
 * Enter callback w/ user data.
 *
 * @see hsm_callback_enter, hsmOnEnterUD
 */
typedef hsm_context (*hsm_callback_enter_ud)( hsm_status status, void * enter_data );

/**
 * Action callback w/ user data.
 *
 * @param status Current state of the machine. 
 * @param action_data The userdata passed to hsmOnActionUD()
 *
 * @see hsm_callback_action, hsmOnActionUD, hsmOnExitUD
 */
typedef void(*hsm_callback_action_ud)( hsm_status status, void * action_data );

/**
 * Guard callback w/ user data
 *
 * @param status Current state of the machine. 
 * @param guard_data The userdata passed 
 * @return Return #HSM_TRUE if the guard passes and the transition,actions should be handled; #HSM_FALSE if the guard filters the transition,actions.
 * 
 * @see hsm_callback_guard, hsmOnEventUD, hsmTestUD
 */
typedef hsm_bool(*hsm_callback_guard_ud)( hsm_status status, void *guard_data );

/**
 * Exit callback w/ user data.
 *
 * @see hsm_callback_enter, hsmOnEnterUD
 */
typedef hsm_callback_action_ud hsm_callback_exit_ud;


/**
 * Builder initialization.
 * <b>Must</b> be called before the very first.
 * You can call hsmStartup() multiple times, but every new call requires a corresponding hsmShutdown()
 */
void hsmStartup();

/**
 * Builder shutdown.
 * Free all internally allocated memory.
 * All states are freed.
 */
void hsmShutdown();

/**
 * Start the passed machine using the passed named state
 * @param hsm Machine to initialize
 * @param name Name of state to start
 */
hsm_bool hsmStart( hsm_machine hsm, const char * name );

/**
 * @see hsmStart
 */
hsm_bool hsmStartId( hsm_machine, int );

/**
 * Declare a new state.
 *
 * @param name Name relative to enclosing state; if no enclosing state, assumed to be the top state of a machine.
 *
 * @return int opaque handle to an internal name
 *
 * This function is idempotent(!) 
 *
 * If you call it with the same name, in the same begin/end scope, in the same run of the app, you will get the same value back out. 
 * The value is *not* gaurenteed to be the same across different launches of the same application.
 * 
 * Names are combined using "::" as per the statechart spec. ex.: Parent::Child::Leaf
 *
 * What this all means is you cannot have two top level states both called "Foo" in your application.
 * You can however reuse states by within a statemachine provided they have different parents: 
 * ( ie. { Outer: { InnerA: Foo }  { InnerB: Foo } } is okay )
 */
int hsmState( const char * name );

/**
 * Define a named state.
 *
 * Every hsmBegin() must, eventually, be paired with a matching hsmEnd(),
 * Until then, all operations, including calls including hsmState() are considered owned by this state.
 *
 * @param state A state id returned by hsmState() or hsmRef().
 * @return The same state id that was passed in.
 */
int hsmBegin( const char * name, int len );

/**
 * Define an already declared state.
 *
 * @see hsmState, hsmEnd
 */
int hsmBeginId( int state );

/**
 * Specify a callback for state entry
 *
 * @param entry Callback triggered on state enter
 * @param user_data Data passed to callback
 *
 * @note user_data lifetime must be longer than the state descriptions
 */
void hsmOnEnterUD( hsm_callback_enter_ud entry, void * user_data );

/**
 * Specify a callback for state exit
 *
 * @param exit Callback triggered on state exit
 * @param user_data Data passed to callback
 *
 * @note user_data lifetime must be longer than the state descriptions
 */
void hsmOnExitUD( hsm_callback_action_ud exit, void * user_data );

/**
 * Event handler initialization.
 * Call the passed guard function, 
 * only if the guard returns true #HSM_TRUE will the rest of the event trigger
 * 
 * @param guard Boolean function to call.
 * @param guard_data Data passed to callback.
 */
void hsmOnEventUD( hsm_callback_guard_ud guard, void* guard_data );

/**
 * Add a guard to the current event handler.
 * Works the same as hsmOnEvent except it doesn't create a new handler, only adds a new guard on to the existing one(s)
 * 
 * @param guard Boolean function to call.
 * @param guard_data Data passed to callback.
 *
 * @see hsmOnEvent.
 */
void hsmTestUD( hsm_callback_guard_ud guard, void* guard_data );


/**
 * The event handler being declared should transition to another state.
 * same as hsmGotoId().
 */
void hsmGoto( const char * name );

/**
 * The event handler being declared should transition to another state.
 * 
 * @param state The id of a state returned by hsmState() or hsmRef() to transition to. 
 *
 * @see hsmState, hsmEnd
 */
void hsmGotoId( int state );

/**
 * The event handler being declared should run the passed action.
 * @param action The action to run.
 */
void hsmRunUD( hsm_callback_action_ud action, void * action_data );

/**
 * Pairs with hsmBegin()
 * @see hsmBegin()
 */
void hsmEnd();

/**
 * Return an hsm_state from a builder state.
 * @param name String name of state
 * @return The #hsm_state; NULL if the named state hasn't been built.
 * @note Requires that hsmEnd() has been called for the state in question.
 */
hsm_state hsmResolve( const char * name );

/**
 * Return a core hsm_state from an id.
 * @param id A state id returned by hsmState() or hsmRef().
 * @return The #hsm_state; NULL if the named state hasn't been built.
 * @note Requires that hsmEnd() has been called for the state in question.
 */
hsm_state hsmResolveId( int id );

/**
 * 
 */
#define HSM_HASH32(x) hsmStringHash( x, 0x811c9dc5 )

/**
 * 
 */
#define HSM_HASH32_CAT(x,v) hsmStringHash( x, v )

/**
 * compute strlwr'd fnva hash
 * @param string string to hash
 * @param seed seed value for hash
 */
hsm_uint32 hsmStringHash(const char *string, hsm_uint32 seed );


#endif // #ifndef __HSM_BUILDER_H__