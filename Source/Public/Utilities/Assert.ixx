module;
#include <assert.h>
#include <stdio.h>
#include <source_location>
export module ntl.utils.assert;

export namespace ne
{
    inline constexpr void Assert(bool expr, const char* info="") {
        if (!expr) {                
            if (info) {
                fprintf(stderr, "%s\n", info);
            }
            assert(expr);
        }
    }

    inline void DAssert(bool expr, const char* info = nullptr) {
    #ifdef NE_DEBUG
        Assert(expr, info);
    #endif
    }
}