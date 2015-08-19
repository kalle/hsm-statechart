# Introduction #

The original version of `hsm-statechart` was written for C++, but it had(has) some quirks, and the C version is more fully featured.  That said, the original C++ version **looks** much more like a statechart by virtue of its ability to use nested classes.

# C++ sample #

The working snippet below is fairly readable. Its main features are partial transition tables, with automatic detection of enter and exit actions, and automatic detection of hierarchy ( by using nested classes and some C++ compiler tricks ).

Something like this would be a good goal for a C++ version of hsm-statechart.

```
struct Shop {
    HSM_XTABLE( Shop, 
        trans::on<evtInit>() >> state<Buy>(),
        trans::on<evtBuyItems>() >> state<Buy>(),
        trans::on<evtSellItems>() >> state<Sell>(),
    );
    static context_ptr_t hsmEnter( context_ptr_t, event_ptr_t );
    static void          hsmExit ( context_ptr_t, event_ptr_t );
    static void       resortItems( evtContext*, const evtResort*);

    struct Buy {
        HSM_XTABLE( Buy, 
            trans::on<evtInit>() >> state<ListItems>(),
            trans::on<evtBuyItem>() [ evtBuyItem::canAfford ] / buyItem >> state<ListItems>(), 
            trans::on<evtBuyItem>() / cantBuyItem >> state<DescribeItem>(),
        );


```