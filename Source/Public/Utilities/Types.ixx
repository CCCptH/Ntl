export module ntl.utils.nint;

namespace ne
{
    using intp = int*;
}

export namespace ne
{
    using int8 = signed char;

    using uint8 = unsigned char;

    using int16 = signed short;

    using uint16 = unsigned short;

    using int32 = signed int;

    using uint32 = unsigned int;

    using int64 = signed long long;

    using uint64 = unsigned long long;

    using size_t = decltype(sizeof(int));

    using utf8 = char8_t;
    using utf16 = char16_t;
    using utf32 = char32_t;

    using ptrdiff_t = decltype(intp{}-intp{});

    using byte = unsigned char;
}
namespace ne {
    static_assert(sizeof(int8)==1);
    static_assert(sizeof(uint8)==1);
    static_assert(sizeof(int16)==2);
    static_assert(sizeof(uint16)==2);
    static_assert(sizeof(int32)==4);
    static_assert(sizeof(uint32)==4);
    static_assert(sizeof(int64)==8);
    static_assert(sizeof(uint64)==8);
}