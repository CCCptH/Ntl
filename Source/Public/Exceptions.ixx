module;
#include <exception>
#include <stdexcept>
export module ntl.exceptions;
//import ntl.string.string;

export namespace ne
{
    using Exception = std::exception;

    class LogicError: public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
        /*explicit LogicError(const String& str)
	        : logic_error(str.cstr())
    	{}*/
    };

    class InvalidArgument: public LogicError
    {
    public:
        using LogicError::LogicError;
    };
    class OutOfRange : public LogicError
    {
    public:
        using LogicError::LogicError;
    };
    class LengthError : public LogicError
    {
    public:
        using LogicError::LogicError;
    };
    

    class RuntimeError: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
        /*explicit RuntimeError(const String& str)
	        : runtime_error(str.cstr())
    	{}*/
    };
} // namespace ne
