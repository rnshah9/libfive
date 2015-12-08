#pragma once

#include <libguile.h>

extern "C" {
    /*
     *  Helper functions to manipulate store pointers
     */
    void store_delete(void* ptr);
    SCM store_new();
    SCM store_describe(SCM store);

    /*
     *  Return variables wrapped as SCM pointers
     */
    SCM token_x(SCM store);
    SCM token_y(SCM store);
    SCM token_z(SCM store);

    /*
     *  Make a token for a constant
     */
    SCM token_const(SCM store, SCM n);

    /*
     *  Make a token for an operation
     */
    SCM token_op(SCM store, SCM op, SCM args);

    /*
     *  Initialize the shared library functions
     */
    void fab_init();
}
