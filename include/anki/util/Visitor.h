/// @file
/// The file contains visitor concepts

#ifndef ANKI_UTIL_VISITOR_H
#define ANKI_UTIL_VISITOR_H

namespace anki {

/// @addtogroup util
/// @{

namespace detail {

//==============================================================================
// ConstVisitor                                                                =
//==============================================================================

// Forward declaration
template<typename... Types>
struct ConstVisitor;

// Specialized for one and many
template<typename First, typename... Types>
struct ConstVisitor<First, Types...>: ConstVisitor<Types...>
{
	using ConstVisitor<Types...>::visit;
	virtual void visit(const First&) = 0;
};

// Specialized for one
template<typename First>
struct ConstVisitor<First>
{
	virtual void visit(const First&) = 0;
};

//==============================================================================
// MutableVisitor                                                              =
//==============================================================================

// Forward declaration
template<typename... Types>
struct MutableVisitor;

// Specialized for one and many
template<typename First, typename... Types>
struct MutableVisitor<First, Types...>: MutableVisitor<Types...>
{
	using MutableVisitor<Types...>::visit;
	virtual void visit(First&) = 0;
};

// Specialized for one
template<typename First>
struct MutableVisitor<First>
{
	virtual void visit(First&) = 0;
};

//==============================================================================
// GetTypeIdVisitor                                                            =
//==============================================================================

/// Visitor for getting the type id
template<typename... Types>
struct GetTypeIdVisitor
{
	// Forward
	template<typename... Types_>
	struct Helper;

	// Specialized for one and many
	template<typename First, typename... Types_>
	struct Helper<First, Types_...>: Helper<Types_...>
	{
		using Helper<Types_...>::visit;

		void visit(const First& x)
		{
			Helper<Types_...>::id = sizeof...(Types_);
		}
	};

	// Specialized for one
	template<typename First>
	struct Helper<First>: ConstVisitor<Types...>
	{
		int id;

		void visit(const First&)
		{
			id = 0;
		}
	};

	typedef Helper<Types...> Type;
};

//==============================================================================
// DummyVisitor                                                                =
//==============================================================================

/// Implements the visit() function. The new visit() does nothing
template<typename... Types>
struct DummyVisitor
{
	// Forward
	template<typename... Types_>
	struct Helper;

	// Declare
	template<typename First, typename... Types_>
	struct Helper<First, Types_...>: Helper<Types_...>
	{
		void visit(const First&)
		{}
	};

	// Specialized for one
	template<typename First>
	struct Helper<First>: ConstVisitor<Types...>
	{
		void visit(const First&)
		{}
	};

	typedef Helper<Types...> Type;
};

//==============================================================================
// GetVisitableId                                                              =
//==============================================================================

// Forward
template<typename Type, typename... Types>
struct GetVisitableId;

/// A smart struct that given a @a Type and a list of types defines a const
/// integer indicating the @a Type's position from the back of the list. The
/// integer is named ID
/// @code
/// GetVisitableId<float, int, float, std::string>::ID == 1
/// GetVisitableId<int, int, float, std::string>::ID == 2
/// GetVisitableId<std::string, int, float, std::string>::ID == 0
/// @endcode
template<typename Type, typename First, typename... Types>
struct GetVisitableId<Type, First, Types...>: GetVisitableId<Type, Types...>
{};

// Specialized
template<typename Type, typename... Types>
struct GetVisitableId<Type, Type, Types...>
{
	static const int ID = sizeof...(Types);
};

} // end namespace detail

//==============================================================================
// Visitable                                                                   =
//==============================================================================

/// Visitable class
template<typename... Types>
struct Visitable
{
	using MutableVisitor = detail::MutableVisitor<Types...>;
	using ConstVisitor = detail::ConstVisitor<Types...>;
	using GetTypeIdVisitor = typename detail::GetTypeIdVisitor<Types...>::Type;
	using DummyVisitor = typename detail::DummyVisitor<Types...>::Type;

	/// Visitor accept
	virtual void accept(MutableVisitor& v) = 0;
	/// Visitor accept
	virtual void accept(ConstVisitor& v) const = 0;

	/// Using the GetVisitableId get the id of the @a T
	template<typename T>
	static constexpr int getTypeId()
	{
		return sizeof...(Types) - detail::GetVisitableId<T, Types...>::ID - 1;
	}
};
/// @}

} // end namespace

#endif
