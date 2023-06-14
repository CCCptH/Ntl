export module ntl.string.character;

import ntl.utils;
import ntl.exceptions;
import ntl.compare;

export namespace ne
{
    /**
     * @class Character
     * @brief Unicode code point.
    */
    class Character
    {
    private:
        struct
        {
            uint32 len:			7;
            utf32  code_point:  21;
        };
        char utf8s[5];
    public:
        constexpr Character() noexcept 
            : code_point()
            , utf8s{}
			, len(0)
        {}
        constexpr Character(utf32 code_point)
            : code_point(code_point)
			, utf8s{'\0'}
        {
	        if (code_point<0x80)
	        {
                utf8s[0] = code_point & 0x7f;
                len = 1;
	        }
            else if (code_point<0x800)
            {
                utf8s[0] = 0xC0 | (code_point >> 6);
                utf8s[1] = 0x80 | (code_point & 0x3F);
                len = 2;
            }
            else if (code_point<0x8000)
            {
                utf8s[0] = 0xE0 | (code_point >> 12);           
                utf8s[1] = 0x80 | ((code_point >> 6) & 0x3F);
                utf8s[2] = 0x80 | (code_point & 0x3F);
                len = 3;
            }
            else if (code_point <= 0x10FFFF)
            {
                utf8s[0] = 0xF0 | (code_point >> 18);
                utf8s[1] = 0x80 | ((code_point >> 12) & 0x3F);
                utf8s[2] = 0x80 | ((code_point >> 6) & 0x3F);
                utf8s[3] = 0x80 | (code_point & 0x3F);
                len = 4;
            }
            else
            {
                throw OutOfRange{ "[ntl.string.character] Code point is out of range." };
            }
        }

        constexpr Character(char ch) noexcept
            : code_point(ch)
            , utf8s{ch, '\0', '\0', '\0', '\0'}
			, len(1)
        {}

        constexpr Character(const Character&) noexcept = default;
        constexpr Character(Character&&) noexcept = default;

        constexpr Character& operator=(const Character&) noexcept = default;
        constexpr Character& operator=(Character&&) noexcept = default;

        constexpr Character& operator=(char ch) noexcept
        {
            code_point = ch;
            utf8s[0] = ch;
            utf8s[1] = '\0';
            utf8s[2] = '\0';
            utf8s[3] = '\0';
            len = 1;
            return *this;
        }

        constexpr Character& operator=(utf32 code_point)
        {
            this->code_point = code_point;
            if (code_point < 0x80)
            {
                utf8s[0] = code_point & 0x7f;
                utf8s[1] = '\0';
                utf8s[2] = '\0';
                utf8s[3] = '\0';
                len = 1;
            }
            else if (code_point < 0x800)
            {
                utf8s[0] = 0xC0 | (code_point >> 6);
                utf8s[1] = 0x80 | (code_point & 0x3F);
                utf8s[2] = '\0';
                utf8s[3] = '\0';
                len = 2;
            }
            else if (code_point < 0x8000)
            {
                utf8s[0] = 0xE0 | (code_point >> 12);
                utf8s[1] = 0x80 | ((code_point >> 6) & 0x3F);
                utf8s[2] = 0x80 | (code_point & 0x3F);
                utf8s[3] = '\0';
                len = 3;
            }
            else if (code_point <= 0x10FFFF)
            {
                utf8s[0] = 0xF0 | (code_point >> 18);
                utf8s[1] = 0x80 | ((code_point >> 12) & 0x3F);
                utf8s[2] = 0x80 | ((code_point >> 6) & 0x3F);
                utf8s[3] = 0x80 | (code_point & 0x3F);
                len = 4;
            }
            else
            {
                throw OutOfRange{ "[ntl.string.character] Code point is out of range." };
            }
            return *this;
        }

        constexpr const char* cstr() const
        {
            return utf8s;
        }

        constexpr utf32 codepoint() const noexcept
        {
            return code_point;
        }

        constexpr uint32 charcode() const noexcept
        {
            return code_point;
        }

        constexpr int64 byteLength() const noexcept
        {
            return len;
        }

        friend constexpr StrongOrdering operator<=> (const Character& lhs, const Character& rhs)
        {
            return lhs.code_point <=> rhs.code_point;
        }
    	friend constexpr StrongOrdering operator<=> (const Character& lhs, utf32 rhs)
        {
            return lhs.code_point <=> rhs;
        }
        friend constexpr bool operator==(const Character& lhs, const Character& rhs)
        {
            return lhs.code_point == rhs.code_point;
        }
        friend constexpr bool operator== (const Character& lhs, utf32 rhs)
        {
            return lhs.code_point == rhs;
        }
    };
}