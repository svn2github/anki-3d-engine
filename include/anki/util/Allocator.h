#ifndef ANKI_UTIL_ALLOCATOR_H
#define ANKI_UTIL_ALLOCATOR_H

#include "anki/util/Exception.h"
#include "anki/util/Assert.h"
#include <cstddef> // For ptrdiff_t
#include <cstring> // For memset
#include <atomic>
#include <utility> // For forward

#define ANKI_DEBUG_ALLOCATORS ANKI_DEBUG
#define ANKI_PRINT_ALLOCATOR_MESSAGES 1

#if ANKI_PRINT_ALLOCATOR_MESSAGES
#	include <iostream> // Never include it on release
#endif

namespace anki {

/// @addtogroup util
/// @{
/// @addtogroup memory
/// @{

namespace detail {

/// Internal methods for the #Allocator class
class AllocatorInternal
{
public:
	/// Print a few debugging messages
	static void dump();

protected:
	/// Keep track of the allocated size. Relevant only when debugging
	static PtrSize allocatedSize;

	/// Allocate memory
	static void* gmalloc(PtrSize size);

	/// Free memory
	static void gfree(void* p, PtrSize size);
};

} // end namespace detail

/// The default allocator. It uses malloc and free for 
/// allocations/deallocations. It's STL compatible
template<typename T>
class Allocator: public detail::AllocatorInternal
{
public:
	// Typedefs
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	/// Default constructor
	Allocator() throw()
	{}
	/// Copy constructor
	Allocator(const Allocator&) throw()
	{}
	/// Copy constructor with another type
	template<typename U>
	Allocator(const Allocator<U>&) throw()
	{}

	/// Destructor
	~Allocator()
	{}

	/// Copy
	Allocator<T>& operator=(const Allocator&)
	{
		return *this;
	}
	/// Copy with another type
	template<typename U>
	Allocator& operator=(const Allocator<U>&) 
	{
		return *this;
	}

	/// Get address of reference
	pointer address(reference x) const 
	{
		return &x; 
	}
	/// Get const address of const reference
	const_pointer address(const_reference x) const 
	{
		return &x;
	}

	/// Allocate memory
	pointer allocate(size_type n, const void* = 0)
	{
		size_type size = n * sizeof(value_type);
		return (pointer)detail::AllocatorInternal::gmalloc(size);
	}

	/// Deallocate memory
	void deallocate(void* p, size_type n)
	{
		size_type size = n * sizeof(T);
		detail::AllocatorInternal::gfree(p, size);
	}

	/// Call constructor
	void construct(pointer p, const T& val)
	{
		// Placement new
		new ((T*)p) T(val); 
	}
	/// Call constructor with more arguments
	template<typename U, typename... Args>
	void construct(U* p, Args&&... args)
	{
		// Placement new
		::new((void*)p) U(std::forward<Args>(args)...);
	}

	/// Call the destructor of p
	void destroy(pointer p) 
	{
		p->~T();
	}
	/// Call the destructor of p of type U
	template<typename U>
	void destroy(U* p)
	{
		p->~U();
	}

	/// Get the max allocation size
	size_type max_size() const 
	{
		return size_type(-1); 
	}

	/// A struct to rebind the allocator to another allocator of type U
	template<typename U>
	struct rebind
	{ 
		typedef Allocator<U> other; 
	};
};

/// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2>
inline bool operator==(const Allocator<T1>&, const Allocator<T2>&)
{
	return true;
}

/// Another allocator of the another type cannot deallocate from this one
template<typename T1, typename AnotherAllocator>
inline bool operator==(const Allocator<T1>&, const AnotherAllocator&)
{
	return false;
}

/// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2>
inline bool operator!=(const Allocator<T1>&, const Allocator<T2>&)
{
	return false;
}

/// Another allocator of the another type cannot deallocate from this one
template<typename T1, typename AnotherAllocator>
inline bool operator!=(const Allocator<T1>&, const AnotherAllocator&)
{
	return true;
}

namespace detail {

/// Thread safe memory pool
struct MemoryPool
{
	/// Allocated memory
	U8* memory = nullptr;
	/// Size of the allocated memory
	PtrSize size = 0;
	/// Points to the memory and more specifically to the address of the next
	/// allocation
	std::atomic<U8*> ptr = {nullptr};
	/// Reference counter
	std::atomic<I32> refCounter = {1};

#if ANKI_DEBUG_ALLOCATORS
	std::atomic<PtrSize> wastedSize = {0};
#endif
};

/// Internal members for the @ref StackAllocator. They are separate because we 
/// don't want to pollute the @ref StackAllocator template with specialized 
/// functions that take space
class StackAllocatorInternal
{
protected:
	/// The memory pool
	detail::MemoryPool* mpool = nullptr;

	/// Init the memory pool with the given size
	void init(const PtrSize size);

	/// Deinit the memory pool
	void deinit();

	/// Copy
	void copy(const StackAllocatorInternal& b);
};

} // end namespace detail

/// Stack based allocator
///
/// @tparam T The type
/// @tparam deallocationFlag If true then the allocator will try to deallocate
///                          the memory. This is extremely important to
///                          understand when it should be true. See notes
/// @tparam alignmentBits Set the alighment in bits
///
/// @note The deallocationFlag can brake the allocator when the deallocations
///       are not in the correct order. For example when deallocationFlag==true
///       and the allocator is used in vector it is likely to fail.
///
/// @note Don't ever EVER remove the double copy constructor and the double
///       operator=. The compiler will create defaults
template<typename T, Bool deallocationFlag = false, U32 alignmentBits = 16>
class StackAllocator: public detail::StackAllocatorInternal
{
	template<typename U, Bool deallocationFlag_, U32 alignmentBits_>
	friend class StackAllocator;

public:
	// Typedefs
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	/// Default constructor
	StackAllocator() throw()
	{}
	/// Copy constructor
	StackAllocator(const StackAllocator& b) throw()
	{
		*this = b;
	}
	/// Copy constructor
	template<typename U>
	StackAllocator(
		const StackAllocator<U, deallocationFlag, alignmentBits>& b) throw()
	{
		*this = b;
	}
	/// Constuctor with size
	StackAllocator(size_type size) throw()
	{
		init(size);
	}

	/// Destructor
	~StackAllocator()
	{
		deinit();
	}

	/// Copy
	StackAllocator& operator=(const StackAllocator& b)
	{
		copy(b);
		return *this;
	}
	/// Copy
	template<typename U>
	StackAllocator& operator=(
		const StackAllocator<U, deallocationFlag, alignmentBits>& b)
	{
		copy(b);
		return *this;
	}

	/// Get the address of a reference
	pointer address(reference x) const
	{
		return &x;
	}
	/// Get the const address of a const reference
	const_pointer address(const_reference x) const
	{
		return &x;
	}

	/// Allocate memory
	pointer allocate(size_type n, const void* hint = 0)
	{
		ANKI_ASSERT(mpool != nullptr);
		(void)hint;
		size_type size = n * sizeof(value_type);
		size_type alignedSize = calcAlignSize(size);

		U8* out = mpool->ptr.fetch_add(alignedSize);

#if ANKI_PRINT_ALLOCATOR_MESSAGES
			std::cout << "Allocating: size: " << size
				<< ", size after alignment: " << alignedSize
				<< ", returned address: " << (void*)out
				<< ", hint: " << hint <<std::endl;
#endif

		if(out + alignedSize <= mpool->memory + mpool->size)
		{
			// Everything ok
		}
		else
		{
			throw ANKI_EXCEPTION("Allocation failed. There is not enough room");
		}

		return (pointer)out;
	}

	/// Deallocate memory
	void deallocate(void* p, size_type n)
	{
		ANKI_ASSERT(mpool != nullptr);
		(void)p;
		(void)n;

		if(deallocationFlag)
		{
			size_type alignedSize = calcAlignSize(n * sizeof(value_type));
#if ANKI_PRINT_ALLOCATOR_MESSAGES
			std::cout << "Deallocating: size: " << (n * sizeof(value_type))
				<< " alignedSize: " << alignedSize
				<< " pointer: " << p << std::endl;
#endif
			U8* headPtr = mpool->ptr.fetch_sub(alignedSize);

			if(headPtr - alignedSize != p)
			{
				throw ANKI_EXCEPTION("Freeing wrong pointer. "
					"The deallocations on StackAllocator should be in order");
			}

			ANKI_ASSERT((headPtr - alignedSize) >= mpool->memory);
		}
		else
		{
#if ANKI_DEBUG_ALLOCATORS
			size_type alignedSize = calcAlignSize(n * sizeof(value_type));
			mpool->wastedSize += alignedSize;
#endif
		}
	}

	/// Call constructor
	void construct(pointer p, const T& val)
	{
		// Placement new
		new ((T*)p) T(val);
	}
	/// Call constructor with many arguments
	template<typename U, typename... Args>
	void construct(U* p, Args&&... args)
	{
		// Placement new
		::new((void *)p) U(std::forward<Args>(args)...);
	}

	/// Call destructor
	void destroy(pointer p)
	{
		p->~T();
	}
	/// Call destructor
	template<typename U>
	void destroy(U* p)
	{
		p->~U();
	}

	/// Get the max allocation size
	size_type max_size() const
	{
		ANKI_ASSERT(mpool != nullptr);
		return mpool->size;
	}

	/// A struct to rebind the allocator to another allocator of type U
	template<typename U>
	struct rebind
	{
		typedef StackAllocator<U, deallocationFlag, alignmentBits> other;
	};

	/// Reinit the allocator. All existing allocated memory will be lost
	void reset()
	{
		ANKI_ASSERT(mpool != nullptr);
		mpool->ptr = mpool->memory;
	}

private:
	/// Calculate tha align size
	size_type calcAlignSize(size_type size)
	{
		return size + (size % (alignmentBits / 8));
	}
};

/// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2, Bool deallocationFlag, U32 alignmentBits>
inline bool operator==(
	const StackAllocator<T1, deallocationFlag, alignmentBits>&,
	const StackAllocator<T2, deallocationFlag, alignmentBits>&)
{
	return true;
}

/// Another allocator of the another type cannot deallocate from this one
template<typename T1, typename AnotherAllocator, Bool deallocationFlag,
	U32 alignmentBits>
inline bool operator==(
	const StackAllocator<T1, deallocationFlag, alignmentBits>&,
	const AnotherAllocator&)
{
	return false;
}

/// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2, Bool deallocationFlag, U32 alignmentBits>
inline bool operator!=(
	const StackAllocator<T1, deallocationFlag, alignmentBits>&,
	const StackAllocator<T2, deallocationFlag, alignmentBits>&)
{
	return false;
}

/// Another allocator of the another type cannot deallocate from this one
template<typename T1, typename AnotherAllocator, Bool deallocationFlag,
	U32 alignmentBits>
inline bool operator!=(
	const StackAllocator<T1, deallocationFlag, alignmentBits>&,
	const AnotherAllocator&)
{
	return true;
}

/// @}
/// @}

} // end namespace anki

#endif
