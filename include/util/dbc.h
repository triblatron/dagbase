#ifndef __DBC_H__
#define __DBC_H__

#include <stdexcept>
#include <cassert>

// Make sure there are no prior definitions.
#undef SIM_REQUIRE
#undef SIM_ENSURE
#undef SIM_INVARIANT

namespace sim
{
	//! Exception to represent a broken pre-condition.
	struct broken_precondition : public std::runtime_error
	{
		broken_precondition( const char * what )
		:
		std::runtime_error(what)
		{
		}
	};
	
	//! Exception to represent a broken post-condition.
	struct broken_postcondition : public std::runtime_error
	{
		broken_postcondition( const char * what )
		:
		std::runtime_error(what)
		{
		}
	};
	
	//! Exception to represent a broken class or loop invariant.
	struct broken_invariant : public std::runtime_error
	{
		broken_invariant( const char * what )
		:
		std::runtime_error(what)
		{
		}
	};
}

//! Macro that uses the pre-processor's stringification operator to get the text for an expression.
#define SIM_TOSTRING_(expr) #expr

//! Top-level macro to convert an expression to its string equivalent.  Useful for debugging.
//! @note This needs to call an auxilliary macro to do the work otherwise the result will not be as expected.
//! e.g. __FILE__ would become "__FILE__" rather than the name of the source file.
//! @see SIM_TOSTRING_
#define SIM_TOSTRING(expr) SIM_TOSTRING_(expr)

//! Macro to assert that a pre-condition is true or throw an exception with useful debugging information.
#define SIM_REQUIRE( desc, expr ) \
assert(expr);

//! Macro to assert that a post-condition is true or throw an exception with useful debugging information.
//! @note Currently does not support the old syntax since that would typically entail copying objects.
#define SIM_ENSURE( desc, expr ) \
assert(expr);

//! Macro to assert that a class or loop invariant is true or throw an exception with useful debugging information.
#define SIM_INVARIANT( desc, expr )\
assert(expr);
#endif // !__DBC_H__
