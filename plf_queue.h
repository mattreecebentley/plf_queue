// Copyright (c) 2024, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

// zLib license (https://www.zlib.net/zlib_license.html):
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// 	claim that you wrote the original software. If you use this software
// 	in a product, an acknowledgement in the product documentation would be
// 	appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// 	misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


#ifndef PLF_QUEUE_H
#define PLF_QUEUE_H


// Compiler-specific defines:

// Define default cases before possibly redefining:
#define PLF_NOEXCEPT throw()
#define PLF_NOEXCEPT_ALLOCATOR
#define PLF_CONSTEXPR
#define PLF_CONSTFUNC

#define PLF_EXCEPTIONS_SUPPORT

#if ((defined(__clang__) || defined(__GNUC__)) && !defined(__EXCEPTIONS)) || (defined(_MSC_VER) && !defined(_CPPUNWIND))
	#undef PLF_EXCEPTIONS_SUPPORT
	#include <exception> // std::terminate
#endif


#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
	 // Suppress incorrect (unfixed MSVC bug) warnings re: constant expressions in constexpr-if statements
	#pragma warning ( push )
	#pragma warning ( disable : 4127 )

	#if _MSC_VER >= 1600
		#define PLF_MOVE_SEMANTICS_SUPPORT
	#endif
	#if _MSC_VER >= 1700
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
	#endif
	#if _MSC_VER >= 1800
		#define PLF_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
		#define PLF_INITIALIZER_LIST_SUPPORT
	#endif
	#if _MSC_VER >= 1900
		#define PLF_ALIGNMENT_SUPPORT
		#undef PLF_NOEXCEPT
		#undef PLF_NOEXCEPT_ALLOCATOR
		#define PLF_NOEXCEPT noexcept
		#define PLF_NOEXCEPT_ALLOCATOR noexcept(noexcept(allocator_type()))
		#define PLF_IS_ALWAYS_EQUAL_SUPPORT
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
		#undef PLF_CONSTEXPR
		#define PLF_CONSTEXPR constexpr
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && _MSC_VER >= 1929
		#define PLF_CPP20_SUPPORT
		#undef PLF_CONSTFUNC
		#define PLF_CONSTFUNC constexpr
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L // C++11 support, at least
	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4
			#define PLF_MOVE_SEMANTICS_SUPPORT
			#define PLF_VARIADICS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4
			#define PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4
			#undef PLF_NOEXCEPT
			#undef PLF_NOEXCEPT_ALLOCATOR
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_ALLOCATOR noexcept(noexcept(allocator_type()))
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ > 4
			#define PLF_ALIGNMENT_SUPPORT
		#endif
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif
		#if __GNUC__ > 6
			#define PLF_IS_ALWAYS_EQUAL_SUPPORT
		#endif
	#elif defined(__clang__) && !defined(__GLIBCXX__) && !defined(_LIBCPP_CXX03_LANG) && __clang_major__ >= 3
		#define PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_TYPE_TRAITS_SUPPORT

		#if __has_feature(cxx_alignas) && __has_feature(cxx_alignof)
			#define PLF_ALIGNMENT_SUPPORT
		#endif
		#if __has_feature(cxx_noexcept)
			#undef PLF_NOEXCEPT
			#undef PLF_NOEXCEPT_ALLOCATOR
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_ALLOCATOR noexcept(noexcept(allocator_type()))
			#define PLF_IS_ALWAYS_EQUAL_SUPPORT
		#endif
		#if __has_feature(cxx_rvalue_references) && !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)
			#define PLF_MOVE_SEMANTICS_SUPPORT
		#endif
		#if __has_feature(cxx_variadic_templates) && !defined(_LIBCPP_HAS_NO_VARIADICS)
			#define PLF_VARIADICS_SUPPORT
		#endif
		#if (__clang_major__ == 3 && __clang_minor__ >= 1) || __clang_major__ > 3
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
	#elif defined(__GLIBCXX__) // Using another compiler type with libstdc++ - we are assuming full c++11 compliance for compiler - which may not be true
		#define PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT

		#if __GLIBCXX__ >= 20080606
			#define PLF_MOVE_SEMANTICS_SUPPORT
			#define PLF_VARIADICS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20090421
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20120322
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
			#undef PLF_NOEXCEPT
			#undef PLF_NOEXCEPT_ALLOCATOR
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_ALLOCATOR noexcept(noexcept(allocator_type()))
		#endif
		#if __GLIBCXX__ >= 20130322
			#define PLF_ALIGNMENT_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20160111
			#define PLF_IS_ALWAYS_EQUAL_SUPPORT
		#endif
	#elif defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) // Special case for checking C++11 support with libCPP
		#if !defined(_LIBCPP_HAS_NO_VARIADICS)
			#define PLF_VARIADICS_SUPPORT
		#endif
	#else // Assume type traits and initializer support for other compilers and standard library implementations
		#define PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_ALIGNMENT_SUPPORT
		#define PLF_INITIALIZER_LIST_SUPPORT
		#undef PLF_NOEXCEPT
		#undef PLF_NOEXCEPT_ALLOCATOR
		#define PLF_NOEXCEPT noexcept
		#define PLF_NOEXCEPT_ALLOCATOR noexcept(noexcept(allocator_type()))
		#define PLF_IS_ALWAYS_EQUAL_SUPPORT
	#endif

	#if __cplusplus >= 201703L && ((defined(__clang__) && ((__clang_major__ == 3 && __clang_minor__ == 9) || __clang_major__ > 3)) || (defined(__GNUC__) && __GNUC__ >= 7) || (!defined(__clang__) && !defined(__GNUC__))) // assume correct C++17 implementation for non-gcc/clang compilers
		#undef PLF_CONSTEXPR
		#define PLF_CONSTEXPR constexpr
	#endif

	#if __cplusplus > 201704L && ((((defined(__clang__) && !defined(__APPLE_CC__) && __clang_major__ >= 14) || (defined(__GNUC__) && (__GNUC__ > 11 || (__GNUC__ == 11 && __GNUC_MINOR__ > 0)))) && ((defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 14) || (defined(__GLIBCXX__) && __GLIBCXX__ >= 201806L))) || (!defined(__clang__) && !defined(__GNUC__)))
		#define PLF_CPP20_SUPPORT
		#undef PLF_CONSTFUNC
		#define PLF_CONSTFUNC constexpr
	#endif
#endif

#if defined(PLF_IS_ALWAYS_EQUAL_SUPPORT) && defined(PLF_MOVE_SEMANTICS_SUPPORT) && defined(PLF_ALLOCATOR_TRAITS_SUPPORT) && (__cplusplus >= 201703L || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)))
	#define PLF_NOEXCEPT_MOVE_ASSIGN(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<the_allocator>::is_always_equal::value)
	#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value || std::allocator_traits<the_allocator>::is_always_equal::value)
#else
	#define PLF_NOEXCEPT_MOVE_ASSIGN(the_allocator)
	#define PLF_NOEXCEPT_SWAP(the_allocator)
#endif

#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, ...) std::allocator_traits<the_allocator>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, data)	std::allocator_traits<the_allocator>::construct(allocator_instance, location, data)
	#endif

	#define PLF_DESTROY(the_allocator, allocator_instance, location)				std::allocator_traits<the_allocator>::destroy(allocator_instance, location)
	#define PLF_ALLOCATE(the_allocator, allocator_instance, size, hint)			std::allocator_traits<the_allocator>::allocate(allocator_instance, size, hint)
	#define PLF_DEALLOCATE(the_allocator, allocator_instance, location, size)	std::allocator_traits<the_allocator>::deallocate(allocator_instance, location, size)
#else
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, ...) 	(allocator_instance).construct(location, __VA_ARGS__)
	#else
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, data)	(allocator_instance).construct(location, data)
	#endif

	#define PLF_DESTROY(the_allocator, allocator_instance, location)				(allocator_instance).destroy(location)
	#define PLF_ALLOCATE(the_allocator, allocator_instance, size, hint)			(allocator_instance).allocate(size, hint)
	#define PLF_DEALLOCATE(the_allocator, allocator_instance, location, size)	(allocator_instance).deallocate(location, size)
#endif




#include <cstring> // memset, memcpy
#include <cassert> // assert
#include <limits>  // std::numeric_limits
#include <memory> // std::uninitialized_copy, std::allocator
#include <stdexcept> // std::length_error
#include <utility> // std::move, std::swap

#ifdef PLF_TYPE_TRAITS_SUPPORT
	#include <cstddef> // offsetof, used in blank()
	#include <type_traits> // std::is_trivially_destructible
#endif




namespace plf
{
	// std:: tool replacements for C++03/98/11 support:

#ifndef PLF_TOOLS
	#define PLF_TOOLS

	template <bool condition, class T = void>
	struct enable_if
	{
		typedef T type;
	};

	template <class T>
	struct enable_if<false, T>
	{};



	template <bool flag, class is_true, class is_false> struct conditional;

	template <class is_true, class is_false> struct conditional<true, is_true, is_false>
	{
		typedef is_true type;
	};

	template <class is_true, class is_false> struct conditional<false, is_true, is_false>
	{
		typedef is_false type;
	};



	template <class element_type>
	struct less
	{
		bool operator() (const element_type &a, const element_type &b) const PLF_NOEXCEPT
		{
			return a < b;
		}
	};



	template<class element_type>
	struct equal_to
	{
		const element_type &value;

		explicit equal_to(const element_type &store_value) PLF_NOEXCEPT:
			value(store_value)
		{}

		bool operator() (const element_type &compare_value) const PLF_NOEXCEPT
		{
			return value == compare_value;
		}
	};



	// To enable conversion to void * when allocator supplies non-raw pointers:
	template <class source_pointer_type>
	static PLF_CONSTFUNC void * void_cast(const source_pointer_type source_pointer) PLF_NOEXCEPT
	{
		#ifdef PLF_CPP20_SUPPORT
			return static_cast<void *>(std::to_address(source_pointer));
		#else
			return static_cast<void *>(&*source_pointer);
		#endif
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		template <class iterator_type>
		static PLF_CONSTFUNC std::move_iterator<iterator_type> make_move_iterator(iterator_type it)
		{
			return std::move_iterator<iterator_type>(std::move(it));
		}
	#endif



	enum priority { performance = 1, memory_use = 4};

#endif





template <class element_type, plf::priority priority = plf::memory_use, class allocator_type = std::allocator<element_type> > class queue : private allocator_type // Empty base class optimisation - inheriting allocator functions
{
public:
	// Standard container typedefs:
	typedef element_type value_type;

	#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<allocator_type>::size_type 		size_type;
		typedef element_type &														reference;
		typedef const element_type &												const_reference;
		typedef typename std::allocator_traits<allocator_type>::pointer			pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer		const_pointer;
	#else
		typedef typename allocator_type::size_type			size_type;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename allocator_type::pointer			pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
	#endif

private:
	struct group; // Forward declaration for typedefs below

	#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<group>	group_allocator_type;
		typedef typename std::allocator_traits<group_allocator_type>::pointer					group_pointer_type;
		typedef typename std::allocator_traits<allocator_type>::pointer						element_pointer_type;
	#else
		typedef typename allocator_type::template rebind<group>::other group_allocator_type;
		typedef typename group_allocator_type::pointer					group_pointer_type;
		typedef typename allocator_type::pointer						element_pointer_type;
	#endif


	struct group : private allocator_type
	{
		const element_pointer_type 	elements;
		group_pointer_type				next_group, previous_group;
		const element_pointer_type 	end; // One-past the back element


		#ifdef PLF_VARIADICS_SUPPORT
			group(const size_type elements_per_group, const group_pointer_type previous = NULL):
				elements(PLF_ALLOCATE(allocator_type, *this, elements_per_group, (previous == NULL) ? 0 : previous->elements)),
				next_group(NULL),
				previous_group(previous),
				end(elements + elements_per_group)
			{}


		#else
			// This is a hack around the fact that allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in the copy constructor.
			group(const size_type elements_per_group, const group_pointer_type previous = NULL) PLF_NOEXCEPT:
				elements(NULL),
				next_group(reinterpret_cast<group_pointer_type>(elements_per_group)),
				previous_group(previous),
				end(NULL)
			{}


			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source):
				allocator_type(source),
				elements(PLF_ALLOCATE(allocator_type, *this, reinterpret_cast<size_type>(source.next_group), (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL),
				previous_group(source.previous_group),
				end(elements + reinterpret_cast<size_type>(source.next_group))
			{}
		#endif



		~group() PLF_NOEXCEPT
		{
			// Null check not necessary (for empty group and copied group as above) as deallocate will do it's own null check.
			PLF_DEALLOCATE(allocator_type, *this, elements, static_cast<size_type>(end - elements));
		}
	};


	group_pointer_type		current_group, first_group; // current group is location of top pointer, first_group is 'front' group, saves performance for ~queue etc
	element_pointer_type top_element, start_element, end_element; // start_element/end_element cache current_group->end/elements for better performance
	size_type				total_size, total_capacity, min_block_capacity;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with the least-used member variable, for empty-base-class optimization
	{
		size_type max_block_capacity;
		ebco_pair(const size_type max_elements, const allocator_type &alloc) PLF_NOEXCEPT:
			group_allocator_type(alloc),
			max_block_capacity(max_elements)
		{};
	} group_allocator_pair;



	void check_capacities_conformance(const size_type min, const size_type max) const
	{
		if (min < 2 || min > max || max > (std::numeric_limits<size_type>::max() / 2))
		{
			#ifdef PLF_EXCEPTIONS_SUPPORT
				throw std::length_error("Supplied memory block capacities outside of allowable ranges");
			#else
				std::terminate();
			#endif
		}
	}



public:


	static PLF_CONSTFUNC size_type default_min_block_capacity() PLF_NOEXCEPT
	{
		return ((sizeof(element_type) * 8 > (sizeof(queue) + sizeof(group)) * 2) ? 8 : (((sizeof(queue) + sizeof(group)) * 2) / sizeof(element_type)) + 1) / priority;
	}



	static PLF_CONSTFUNC size_type default_max_block_capacity() PLF_NOEXCEPT
	{
		return ((sizeof(element_type) > 128) ? 768 : 12288 / sizeof(element_type)) / priority;
	}





	// Default constructor:
	PLF_CONSTFUNC queue() PLF_NOEXCEPT_ALLOCATOR:
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(default_min_block_capacity()),
		group_allocator_pair(default_max_block_capacity(), *this)
	{}


	// Allocator-extended constructor:
	explicit queue(const allocator_type &alloc):
		allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(default_min_block_capacity()),
		group_allocator_pair(default_max_block_capacity(), alloc)
	{}



	// Constructor with limits:
	queue(const size_type min, const size_type max = default_max_block_capacity()):
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(min),
		group_allocator_pair(max, *this)
	{
		check_capacities_conformance(min, max);
	}



	// Allocator-extended constructor with minimum & maximum group size parameters:
	queue(const size_type min, const size_type max, const allocator_type &alloc):
		allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(min),
		group_allocator_pair(max, alloc)
	{
		check_capacities_conformance(min, max);
	}



private:

	void allocate_new_group(const size_type capacity, const group_pointer_type previous_group)
	{
		previous_group->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, previous_group);

		#ifdef PLF_EXCEPTIONS_SUPPORT
			try
			{
				#ifdef PLF_VARIADICS_SUPPORT
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, previous_group->next_group, capacity, previous_group);
				#else
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, previous_group->next_group, group(capacity, previous_group));
				#endif
			}
			catch (...)
			{
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group->next_group, 1);
				throw;
			}
		#else
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, previous_group->next_group, capacity, previous_group);
			#else
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, previous_group->next_group, group(capacity, previous_group));
			#endif
		#endif

		total_capacity += capacity;
	}



	void deallocate_group(const group_pointer_type the_group) PLF_NOEXCEPT
	{
		PLF_DESTROY(group_allocator_type, group_allocator_pair, the_group);
		PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group, 1);
	}



	void initialize()
	{
		first_group = current_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		#ifdef PLF_EXCEPTIONS_SUPPORT
			try
			{
				#ifdef PLF_VARIADICS_SUPPORT
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, min_block_capacity);
				#else
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(min_block_capacity));
				#endif
			}
			catch (...)
			{
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
				first_group = current_group = NULL;
				throw;
			}
		#else
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, min_block_capacity);
			#else
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(min_block_capacity));
			#endif
		#endif

		start_element = top_element = first_group->elements;
		end_element = first_group->end;
		total_capacity = min_block_capacity;
	}



	void progress_to_next_group() // used by push/emplace
	{
		if (current_group->next_group == NULL) // no reserved groups or groups left over from previous pops, allocate new group
		{
			// Logic: if total_size > current group capacity * 2 or total_size < current group capacity / 2, make new group capacity == total_size, otherwise make it same as current group capacity. The new size is then truncated if necessary to fit within the user-specified min/max block sizes. This means we are more likely to end up with blocks of equal capacity when the total size is not increasing or lowering significantly over time - which in turn means previous blocks can get reused when they become empty.
			const size_type current_group_capacity = static_cast<size_type>(current_group->end - current_group->elements);
			const size_type divided_size = total_size / priority;
			const size_type new_group_capacity = ((divided_size < (current_group_capacity * 2)) & (divided_size > (current_group_capacity / 2))) ? current_group_capacity :
															(divided_size < min_block_capacity) ? min_block_capacity :
															(divided_size > group_allocator_pair.max_block_capacity) ? group_allocator_pair.max_block_capacity : divided_size;
			allocate_new_group(new_group_capacity, current_group);
		}

		current_group = current_group->next_group;
		top_element = current_group->elements;
		end_element = current_group->end;
	}



	void copy_from_source(const queue &source) // Note: this function is only called on an empty un-initialize()'d queue
	{
		assert(&source != this);

		if (source.total_size == 0) return;

		group_pointer_type current_copy_group = source.first_group;
		const group_pointer_type end_copy_group = source.current_group;

		if (source.total_size <= group_allocator_pair.max_block_capacity) // most common case
		{
			const size_type original_min_block_capacity = min_block_capacity;
			min_block_capacity = source.total_size;
			initialize();
			min_block_capacity = original_min_block_capacity;

			element_pointer_type start_pointer = source.start_element;

			// Copy groups to this queue:
			while (current_copy_group != end_copy_group)
			{
				std::uninitialized_copy(start_pointer, current_copy_group->end, top_element);
				top_element += current_copy_group->end - start_pointer;
				current_copy_group = current_copy_group->next_group;
				start_pointer = current_copy_group->elements;
			}

			// Handle special case of last group:
			std::uninitialized_copy(start_pointer, source.top_element + 1, top_element);
			top_element += source.top_element - start_pointer; // This should make top_element == the last "pushed" element, rather than the one past it
			end_element = top_element + 1; // Since we have created a single group where capacity == size, this is correct
			total_size = source.total_size;
		}
		else // uncommon edge case, so not optimising:
		{
			min_block_capacity = group_allocator_pair.max_block_capacity;
			element_pointer_type start_pointer = source.start_element;

			while (current_copy_group != end_copy_group)
			{
				for (element_pointer_type element_to_copy = start_pointer; element_to_copy != current_copy_group->end; ++element_to_copy)
				{
					push(*element_to_copy);
				}

				current_copy_group = current_copy_group->next_group;
				start_pointer = current_copy_group->elements;
			}

			// Handle special case of last group:
			for (element_pointer_type element_to_copy = start_pointer; element_to_copy != source.top_element + 1; ++element_to_copy)
			{
				push(*element_to_copy);
			}

			min_block_capacity = source.min_block_capacity;
		}
	}



	void destroy_all_data() PLF_NOEXCEPT
	{
		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR(!std::is_trivially_destructible<element_type>::value) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with empty destructor.
		#endif // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
		{
			if (total_size != 0)
			{
				while (first_group != current_group)
				{
					const element_pointer_type past_end = first_group->end;

					for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
					{
						PLF_DESTROY(allocator_type, *this, element_pointer);
					}

					const group_pointer_type next_group = first_group->next_group;
					deallocate_group(first_group);
					first_group = next_group;
					start_element = next_group->elements;
				}

				// Special case for current group:
				const element_pointer_type past_end = top_element + 1;

				for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
				{
					PLF_DESTROY(allocator_type, *this, element_pointer);
				}

				first_group = first_group->next_group; // To further process reserved groups in the following loop
				deallocate_group(current_group);
			}
		}

		total_size = 0;

		while (first_group != NULL)
		{
			current_group = first_group;
			first_group = first_group->next_group;
			deallocate_group(current_group);
		}
	}



	void blank() PLF_NOEXCEPT
	{
		#ifdef PLF_IS_ALWAYS_EQUAL_SUPPORT // allocator_traits and type_traits always available when is_always_equal is available
			if PLF_CONSTEXPR (std::is_standard_layout<queue>::value && std::allocator_traits<allocator_type>::is_always_equal::value && std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value) // if all pointer types are trivial, we can just nuke it from orbit with memset (NULL is always 0 in C++):
			{
				std::memset(static_cast<void *>(this), 0, offsetof(queue, min_block_capacity));
			}
			else
		#endif
		{
			current_group = NULL;
			first_group = NULL;
			top_element = NULL;
			start_element = NULL;
			end_element = NULL;
			total_size = 0;
			total_capacity = 0;
		}
	}



public:

	// Copy constructor:
	queue(const queue &source):
		#if (defined(__cplusplus) && __cplusplus >= 201103L) || _MSC_VER >= 1700
			allocator_type(std::allocator_traits<allocator_type>::select_on_container_copy_construction(source)),
		#else
			allocator_type(source),
		#endif
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(source.min_block_capacity),
		group_allocator_pair(source.group_allocator_pair.max_block_capacity, *this)
	{
		copy_from_source(source);
	}



	// Allocator-extended copy constructor:
	queue(const queue &source, const allocator_type &alloc):
		allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(source.min_block_capacity),
		group_allocator_pair(source.group_allocator_pair.max_block_capacity, alloc)
	{
		copy_from_source(source);
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// move constructor
		queue(queue &&source) PLF_NOEXCEPT:
			allocator_type(std::move(static_cast<allocator_type &>(source))),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			top_element(std::move(source.top_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_size(source.total_size),
			total_capacity(source.total_capacity),
			min_block_capacity(source.min_block_capacity),
			group_allocator_pair(source.group_allocator_pair.max_block_capacity, source)
		{
			source.blank();
		}


		// allocator-extended move constructor
		queue(queue &&source, const allocator_type &alloc):
			allocator_type(alloc),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			top_element(std::move(source.top_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_size(source.total_size),
			total_capacity(source.total_capacity),
			min_block_capacity(source.min_block_capacity),
			group_allocator_pair(source.group_allocator_pair.max_block_capacity, alloc)
		{
			source.blank();
		}
	#endif



	~queue() PLF_NOEXCEPT
	{
		destroy_all_data();
	}



	void push(const element_type &element)
	{
		if (top_element == NULL)
		{
			initialize();
		}
		else if (++top_element == end_element) // ie. out of capacity for current element memory block
		{
			progress_to_next_group();
		}

		// Create element:
		#ifdef PLF_EXCEPTIONS_SUPPORT
			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
				{
					PLF_CONSTRUCT(allocator_type, *this, top_element, element);
				}
				else
			#endif
			{
				try
				{
					PLF_CONSTRUCT(allocator_type, *this, top_element, element);
				}
				catch (...)
				{
					if (top_element == start_element && total_size != 0) // for post-initialize push
					{
						current_group = current_group->previous_group;
						start_element = current_group->elements;
						top_element = current_group->end - 1;
					}
					else
					{
						--top_element;
					}

					throw;
				}
			}
		#else
			PLF_CONSTRUCT(allocator_type, *this, top_element, element);
		#endif

		++total_size;
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// Identical to non-move push asides from type traits checks
		void push(element_type &&element)
		{
			if (top_element == NULL)
			{
				initialize();
			}
			else if (++top_element == end_element)
			{
				progress_to_next_group();
			}


			#ifdef PLF_EXCEPTIONS_SUPPORT
				#ifdef PLF_TYPE_TRAITS_SUPPORT
					if PLF_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
					{
						PLF_CONSTRUCT(allocator_type, *this, top_element, std::move(element));
					}
					else
				#endif
				{
					try
					{
						PLF_CONSTRUCT(allocator_type, *this, top_element, std::move(element));
					}
					catch (...)
					{
						if (top_element == start_element && total_size != 0)
						{
							current_group = current_group->previous_group;
							start_element = current_group->elements;
							top_element = current_group->end - 1;
						}
						else
						{
							--top_element;
						}

						throw;
					}
				}
			#else
				PLF_CONSTRUCT(allocator_type, *this, top_element, std::move(element));
			#endif


			++total_size;
		}
	#endif




	#ifdef PLF_VARIADICS_SUPPORT
		template<typename... arguments>
		void emplace(arguments &&... parameters)
		{
			if (top_element == NULL)
			{
				initialize();
			}
			else if (++top_element == end_element)
			{
				progress_to_next_group();
			}


			#ifdef PLF_EXCEPTIONS_SUPPORT
				#ifdef PLF_TYPE_TRAITS_SUPPORT
					if PLF_CONSTEXPR (std::is_nothrow_constructible<element_type>::value)
					{
						PLF_CONSTRUCT(allocator_type, *this, top_element, std::forward<arguments>(parameters)...);
					}
					else
				#endif
				{
					try
					{
						PLF_CONSTRUCT(allocator_type, *this, top_element, std::forward<arguments>(parameters)...);
					}
					catch (...)
					{
						if (top_element == start_element && total_size != 0)
						{
							current_group = current_group->previous_group;
							start_element = current_group->elements;
							top_element = current_group->end - 1;
						}
						else
						{
							--top_element;
						}

						throw;
					}
				}
			#else
				PLF_CONSTRUCT(allocator_type, *this, top_element, std::forward<arguments>(parameters)...);
			#endif

			++total_size;
		}
	#endif



	reference front() const // Exception may occur if queue is empty in release mode
	{
		assert(total_size != 0);
		return *start_element;
	}



	reference back() const // Exception may occur if queue is empty in release mode
	{
		assert(total_size != 0);
		return *top_element;
	}



	void pop() // Exception may occur if queue is empty
	{
		assert(total_size != 0);

		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
		#endif
		{
			PLF_DESTROY(allocator_type, *this, start_element);
		}

		if (--total_size == 0)
		{
			start_element = first_group->elements;
			end_element = first_group->end;
			top_element = start_element - 1;
		}
		else if (++start_element == first_group->end)
		{ // ie. is start element, but not first group in queue
			const group_pointer_type next_group = first_group->next_group;

			if (current_group->next_group == NULL && ((first_group->end - first_group->elements) == (current_group->end - current_group->elements)))
			{
				current_group->next_group = first_group;
				first_group->next_group = NULL;
			}
			else
			{
				total_capacity -= static_cast<size_type>(first_group->end - first_group->elements);
				deallocate_group(first_group);
			}

			first_group = next_group;
			start_element = next_group->elements;
		}
	}



	queue & operator = (const queue &source)
	{
		assert(&source != this);

		destroy_all_data();
		queue temp(source);

		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	private:

		void move_assign(queue &&source) PLF_NOEXCEPT_MOVE_ASSIGN(allocator_type)
		{
			#ifdef PLF_IS_ALWAYS_EQUAL_SUPPORT
				if PLF_CONSTEXPR ((std::is_trivially_copyable<allocator_type>::value || std::allocator_traits<allocator_type>::is_always_equal::value) &&
					std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value)
				{
					std::memcpy(static_cast<void *>(this), &source, sizeof(queue));
				}
				else
			#endif
			{
				current_group = std::move(source.current_group);
				first_group = std::move(source.first_group);
				top_element = std::move(source.top_element);
				start_element = std::move(source.start_element);
				end_element = std::move(source.end_element);
				total_size = source.total_size;
				total_capacity = source.total_capacity;
				min_block_capacity = source.min_block_capacity;
				group_allocator_pair.max_block_capacity = source.group_allocator_pair.max_block_capacity;

				#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
					if PLF_CONSTEXPR (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value)
				#endif
				{
					static_cast<allocator_type &>(*this) = std::move(static_cast<allocator_type &>(source));
					// Reconstruct rebinds:
					static_cast<group_allocator_type &>(group_allocator_pair) = group_allocator_type(*this);
				}
			}
		}



	public:

		// Move assignment
		queue & operator = (queue &&source) PLF_NOEXCEPT_MOVE_ASSIGN(allocator_type)
		{
			assert (&source != this);

			destroy_all_data();

			#ifdef PLF_IS_ALWAYS_EQUAL_SUPPORT
				if PLF_CONSTEXPR (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value || std::allocator_traits<allocator_type>::is_always_equal::value)
				{
					move_assign(std::move(source));
				}
				else
			#endif
			if (static_cast<allocator_type &>(*this) == static_cast<allocator_type &>(source))
			{
				move_assign(std::move(source));
			}
			else // Allocator isn't movable so copy elements from source and deallocate the source's blocks:
			{
				queue temp(source);
				swap(temp);
			}

			source.blank();
			return *this;
		}
	#endif



	#ifdef PLF_CPP20_SUPPORT
		[[nodiscard]]
	#endif
	bool empty() const PLF_NOEXCEPT
	{
		return total_size == 0;
	}



	size_type size() const PLF_NOEXCEPT
	{
		return total_size;
	}



	size_type max_size() const PLF_NOEXCEPT
	{
		#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
			return std::allocator_traits<allocator_type>::max_size(*this);
		#else
			return allocator_type::max_size();
		#endif
	}



	size_type capacity() const PLF_NOEXCEPT
	{
		return total_capacity;
	}



	size_type memory() const PLF_NOEXCEPT
	{
		size_type memory_use = sizeof(*this) + (sizeof(value_type) * total_capacity);

		for (group_pointer_type current = first_group; current != NULL; current = current->next_group)
		{
			memory_use += sizeof(group);
		}

		return memory_use;
	}



private:

	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		void move_from_source(queue &source) // This function is a mirror copy of copy_from_source, with move instead of copy
		{
			assert(&source != this);

			if (source.total_size == 0) return;

			group_pointer_type current_copy_group = source.first_group;
			const group_pointer_type end_copy_group = source.current_group;

			if (source.total_size <= group_allocator_pair.max_block_capacity) // most common case
			{
				const size_type original_min_block_capacity = min_block_capacity;
				min_block_capacity = source.total_size;
				initialize();
				min_block_capacity = original_min_block_capacity;

				element_pointer_type start_pointer = source.start_element;

				// Copy groups to this queue:
				while (current_copy_group != end_copy_group)
				{
					std::uninitialized_copy(plf::make_move_iterator(start_pointer), plf::make_move_iterator(current_copy_group->end), top_element);
					top_element += current_copy_group->end - start_pointer;
					current_copy_group = current_copy_group->next_group;
					start_pointer = current_copy_group->elements;
				}

				// Handle special case of last group:
				std::uninitialized_copy(plf::make_move_iterator(start_pointer), plf::make_move_iterator(source.top_element + 1), top_element);
				top_element += source.top_element - start_pointer; // This should make top_element == the last "pushed" element, rather than the one past it
				end_element = top_element + 1; // Since we have created a single group where capacity == size, this is correct
				total_size = source.total_size;
			}
			else // uncommon edge case, so not optimising:
			{
				min_block_capacity = group_allocator_pair.max_block_capacity;
				element_pointer_type start_pointer = source.start_element;

				while (current_copy_group != end_copy_group)
				{
					for (element_pointer_type element_to_copy = start_pointer; element_to_copy != current_copy_group->end; ++element_to_copy)
					{
						push(std::move(*element_to_copy));
					}

					current_copy_group = current_copy_group->next_group;
					start_pointer = current_copy_group->elements;
				}

				// Handle special case of last group:
				for (element_pointer_type element_to_copy = start_pointer; element_to_copy != source.top_element + 1; ++element_to_copy)
				{
					push(std::move(*element_to_copy));
				}

				min_block_capacity = source.min_block_capacity;
			}
		}
	#endif



	void consolidate()
	{
		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			queue temp((min_block_capacity > total_size) ? min_block_capacity : ((total_size > group_allocator_pair.max_block_capacity) ? group_allocator_pair.max_block_capacity : total_size), group_allocator_pair.max_block_capacity); // Make first allocated block as large as size(), where possible

			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_move_assignable<element_type>::value && std::is_move_constructible<element_type>::value)
				{
					temp.move_from_source(*this);
				}
				else
			#endif
			{
				temp.copy_from_source(*this);
			}

			temp.min_block_capacity = min_block_capacity; // reset to correct value for future clear() or erasures
			*this = std::move(temp);
		#else
			queue temp(*this);
			swap(temp);
		#endif
	}



public:


	void reshape(const size_type min, const size_type max)
	{
		check_capacities_conformance(min, max);

		min_block_capacity = min;
		group_allocator_pair.max_block_capacity = max;

		// Need to check all group sizes, because append might append smaller blocks to the end of a larger block:
		for (group_pointer_type current = first_group; current != NULL; current = current->next_group)
		{
			if (static_cast<size_type>(current->end - current->elements) < min || static_cast<size_type>(current->end - current->elements) > max)
			{
				#ifdef PLF_TYPE_TRAITS_SUPPORT // If type is non-copyable/movable, cannot be consolidated, throw exception:
					if PLF_CONSTEXPR (!((std::is_copy_constructible<element_type>::value && std::is_copy_assignable<element_type>::value) || (std::is_move_constructible<element_type>::value && std::is_move_assignable<element_type>::value)))
					{
						#ifdef PLF_EXCEPTIONS_SUPPORT
							throw;
						#else
							std::terminate();
						#endif
					}
					else
				#endif
				{
					consolidate();
				}

				return;
			}
		}
	}



	void clear() PLF_NOEXCEPT
	{
		destroy_all_data();
		blank();
	}



	friend bool operator == (const queue &lh, const queue &rh) PLF_NOEXCEPT
	{
		if (lh.total_size != rh.total_size) return false;

		for (const_iterator lh_iterator = lh.cbegin(), rh_iterator = rh.cbegin(); lh_iterator != lh.cend(); ++lh_iterator, ++rh_iterator)
		{
			if (*lh_iterator != *rh_iterator) return false;
		}

		return true;
	}



	friend bool operator != (const queue &lh, const queue &rh) PLF_NOEXCEPT
	{
		return !(lh == rh);
	}



	// Remove trailing groups (as may be created by reserve or pop)
	void trim() PLF_NOEXCEPT
	{
		if (current_group == NULL) return; // ie. queue is empty

		group_pointer_type temp_group = current_group->next_group;
		current_group->next_group = NULL; // Set to NULL regardless of whether it is already NULL (avoids branching). Cuts off rest of groups from this group.

		while (temp_group != NULL)
		{
			const group_pointer_type next_group = temp_group->next_group;
			total_capacity -= static_cast<size_type>(temp_group->end - temp_group->elements);
			deallocate_group(temp_group);
			temp_group = next_group;
		}
	}



	void shrink_to_fit()
	{
		if (first_group == NULL || total_size == capacity())
		{
			return;
		}
		else if (total_size == 0) // Edge case
		{
			clear();
			return;
		}

		consolidate();
	}



	void reserve(size_type reserve_amount)
	{
		if (reserve_amount == 0 || reserve_amount <= total_capacity) return;

		reserve_amount -= total_capacity;

		if (reserve_amount < min_block_capacity)
		{
			reserve_amount = min_block_capacity;
		}
		else if (reserve_amount > max_size())
		{
			reserve_amount = max_size();
		}


		size_type number_of_max_capacity_groups = reserve_amount / group_allocator_pair.max_block_capacity,
					remainder = reserve_amount - (number_of_max_capacity_groups * group_allocator_pair.max_block_capacity);

		if (remainder < min_block_capacity) remainder = min_block_capacity;

		if (first_group == NULL) // ie. uninitialized queue
		{
			const size_type original_min_elements = min_block_capacity;

			if (remainder != 0)
			{
				min_block_capacity = remainder;
				remainder = 0;
			}
			else
			{
				min_block_capacity = group_allocator_pair.max_block_capacity;
				--number_of_max_capacity_groups;
			}

			initialize();
			--top_element;
			min_block_capacity = original_min_elements;
		}


		group_pointer_type temp_group = current_group;

		// navigate to end of all current reserved groups (if they exist):
		while (temp_group->next_group != NULL)
		{
			temp_group = temp_group->next_group;
		}


		if (remainder != 0)
		{
			allocate_new_group(remainder, temp_group);
			temp_group = temp_group->next_group;
		}


		while(number_of_max_capacity_groups != 0)
		{
			allocate_new_group(group_allocator_pair.max_block_capacity, temp_group);
			temp_group = temp_group->next_group;
			--number_of_max_capacity_groups;
		}
	}



	allocator_type get_allocator() const PLF_NOEXCEPT
	{
		return allocator_type();
	}



	void swap(queue &source) PLF_NOEXCEPT_SWAP(allocator_type)
	{
		#ifdef PLF_IS_ALWAYS_EQUAL_SUPPORT
			if PLF_CONSTEXPR (std::allocator_traits<allocator_type>::is_always_equal::value && std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value) // if all pointer types are trivial we can just copy using memcpy - avoids constructors/destructors etc and is faster
			{
				char temp[sizeof(queue)];
				std::memcpy(&temp, static_cast<void *>(this), sizeof(queue));
				std::memcpy(static_cast<void *>(this), static_cast<void *>(&source), sizeof(queue));
				std::memcpy(static_cast<void *>(&source), &temp, sizeof(queue));
			}
			#ifdef PLF_MOVE_SEMANTICS_SUPPORT // If pointer types are not trivial, moving them is probably going to be more efficient than copying them below
				else if PLF_CONSTEXPR (std::is_move_assignable<group_pointer_type>::value && std::is_move_assignable<element_pointer_type>::value && std::is_move_constructible<group_pointer_type>::value && std::is_move_constructible<element_pointer_type>::value)
				{
					queue temp(std::move(source));
					source = std::move(*this);
					*this = std::move(temp);
				}
			#endif
			else
		#endif
		{
			// Otherwise, make the reads/writes as contiguous in memory as-possible (yes, it is faster than using std::swap with the individual variables):
			const group_pointer_type	swap_current_group = current_group, swap_first_group = first_group;
			const element_pointer_type swap_top_element = top_element, swap_start_element = start_element, swap_end_element = end_element;
			const size_type				swap_total_size = total_size, swap_total_capacity = total_capacity, swap_min_block_capacity = min_block_capacity, swap_max_block_capacity = group_allocator_pair.max_block_capacity;

			current_group = source.current_group;
			first_group = source.first_group;
			top_element = source.top_element;
			start_element = source.start_element;
			end_element = source.end_element;
			total_size = source.total_size;
			total_capacity = source.total_capacity;
			min_block_capacity = source.min_block_capacity;
			group_allocator_pair.max_block_capacity = source.group_allocator_pair.max_block_capacity;

			source.current_group = swap_current_group;
			source.first_group = swap_first_group;
			source.top_element = swap_top_element;
			source.start_element = swap_start_element;
			source.end_element = swap_end_element;
			source.total_size = swap_total_size;
			source.total_capacity = swap_total_capacity;
			source.min_block_capacity = swap_min_block_capacity;
			source.group_allocator_pair.max_block_capacity = swap_max_block_capacity;

			#ifdef PLF_IS_ALWAYS_EQUAL_SUPPORT
				if PLF_CONSTEXPR (std::allocator_traits<allocator_type>::propagate_on_container_swap::value && !std::allocator_traits<allocator_type>::is_always_equal::value)
			#endif
			{
				std::swap(static_cast<allocator_type &>(source), static_cast<allocator_type &>(*this));

				// Reconstruct rebinds for swapped allocators:
				static_cast<group_allocator_type &>(group_allocator_pair) = group_allocator_type(*this);
			} // else: undefined behaviour, as per standard
		}
	}




	// Iterators:

	// Iterator forward declarations:
	template <bool is_const> class			queue_iterator;
	typedef queue_iterator<false> 		iterator;
	typedef queue_iterator<true>			const_iterator;
	friend class queue_iterator<false>;
	friend class queue_iterator<true>;

	template <bool is_const_r> class 	queue_reverse_iterator;
	typedef queue_reverse_iterator<false>	reverse_iterator;
	typedef queue_reverse_iterator<true>	const_reverse_iterator;
	friend class queue_reverse_iterator<false>;
	friend class queue_reverse_iterator<true>;



	iterator begin() PLF_NOEXCEPT
	{
		return iterator(first_group, start_element);
	}



	const_iterator begin() const PLF_NOEXCEPT
	{
		return const_iterator(first_group, start_element);
	}



	iterator end() PLF_NOEXCEPT
	{
		return iterator(current_group, top_element + 1);
	}



	const_iterator end() const PLF_NOEXCEPT
	{
		return const_iterator(current_group, top_element + 1);
	}



	const_iterator cbegin() const PLF_NOEXCEPT
	{
		return const_iterator(first_group, start_element);
	}



	const_iterator cend() const PLF_NOEXCEPT
	{
		return const_iterator(current_group, top_element + 1);
	}



	reverse_iterator rbegin() PLF_NOEXCEPT
	{
		return reverse_iterator(current_group, top_element);
	}



	const_reverse_iterator rbegin() const PLF_NOEXCEPT
	{
		return const_reverse_iterator(current_group, top_element);
	}



	reverse_iterator rend() PLF_NOEXCEPT
	{
		return reverse_iterator(first_group, start_element - 1);
	}



	const_reverse_iterator rend() const PLF_NOEXCEPT
	{
		return const_reverse_iterator(first_group, start_element - 1);
	}



	const_reverse_iterator crbegin() const PLF_NOEXCEPT
	{
		return const_reverse_iterator(current_group, top_element);
	}



	const_reverse_iterator crend() const PLF_NOEXCEPT
	{
		return const_reverse_iterator(first_group, start_element - 1);
	}



	template <bool is_const>
	class queue_iterator
	{
	private:
		typedef typename queue::group_pointer_type		group_pointer_type;
		typedef typename queue::pointer					pointer_type;

		group_pointer_type		group_pointer;
		pointer_type			element_pointer;

	public:
		struct queue_iterator_tag {};
		typedef std::bidirectional_iterator_tag	iterator_category;
		typedef std::bidirectional_iterator_tag	iterator_concept;
		typedef typename queue::value_type			value_type;
		typedef typename queue::difference_type		difference_type;
		typedef queue_reverse_iterator<is_const>	reverse_type;
		typedef typename plf::conditional<is_const, typename queue::const_pointer, typename queue::pointer>::type		pointer;
		typedef typename plf::conditional<is_const, typename queue::const_reference, typename queue::reference>::type	reference;

		friend class queue;
		friend class queue_reverse_iterator<false>;
		friend class queue_reverse_iterator<true>;


		queue_iterator() PLF_NOEXCEPT:
			group_pointer(NULL),
			element_pointer(NULL)
		{}



		queue_iterator (const queue_iterator &source) PLF_NOEXCEPT:
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer)
		{}



		#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			template <bool is_const_it = is_const, class = typename plf::enable_if<is_const_it>::type >
			queue_iterator(const queue_iterator<false> &source) PLF_NOEXCEPT:
		#else
			queue_iterator(const queue_iterator<!is_const> &source) PLF_NOEXCEPT:
		#endif
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer)
		{}



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			queue_iterator(queue_iterator &&source) PLF_NOEXCEPT:
				group_pointer(std::move(source.group_pointer)),
				element_pointer(std::move(source.element_pointer))
			{}



			#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
				template <bool is_const_it = is_const, class = typename plf::enable_if<is_const_it>::type >
				queue_iterator(queue_iterator<false> &&source) PLF_NOEXCEPT:
			#else
				queue_iterator(const queue_iterator<!is_const> &&source) PLF_NOEXCEPT:
			#endif
				group_pointer(std::move(source.group_pointer)),
				element_pointer(std::move(source.element_pointer))
			{}
		#endif



		queue_iterator & operator = (const queue_iterator &source) PLF_NOEXCEPT
		{
			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			return *this;
		}



		#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			template <bool is_const_it = is_const, class = typename plf::enable_if<is_const_it>::type >
			queue_iterator & operator = (const queue_iterator<false> &source) PLF_NOEXCEPT
		#else
			queue_iterator & operator = (const queue_iterator<!is_const> &source) PLF_NOEXCEPT
		#endif
		{
			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			return *this;
		}



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			queue_iterator & operator = (queue_iterator &&source) PLF_NOEXCEPT
			{
				assert(&source != this);
				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				return *this;
			}



			#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
				template <bool is_const_it = is_const, class = typename plf::enable_if<is_const_it>::type >
				queue_iterator & operator = (const queue_iterator<false> &&source) PLF_NOEXCEPT
			#else
				queue_iterator & operator = (const queue_iterator<!is_const> &&source) PLF_NOEXCEPT
			#endif
			{
				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				return *this;
			}
		#endif



		bool operator == (const queue_iterator &rh) const PLF_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}



		bool operator == (const queue_iterator<!is_const> &rh) const PLF_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}



		bool operator != (const queue_iterator &rh) const PLF_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}



		bool operator != (const queue_iterator<!is_const> &rh) const PLF_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}



		reference operator * () const // may cause exception with uninitialized iterator
		{
			return *element_pointer;
		}



		pointer operator -> () const
		{
			return element_pointer;
		}



		queue_iterator & operator ++ ()
		{
			assert(group_pointer != NULL); // covers uninitialised queue_iterator

			if (++element_pointer == group_pointer->end && group_pointer->next_group != NULL)
			{
				group_pointer = group_pointer->next_group;
				element_pointer = group_pointer->elements;
			}

			return *this;
		}



		queue_iterator operator ++(int)
		{
			const queue_iterator copy(*this);
			++*this;
			return copy;
		}



		queue_iterator & operator -- ()
		{
			assert(group_pointer != NULL);

			if (element_pointer == group_pointer->elements && group_pointer->previous_group != NULL) // second condition necessary for matching to rend() in reverse_iterator
			{
				group_pointer = group_pointer->previous_group;
				element_pointer = group_pointer->end;
			}

			--element_pointer;
			return *this;
		}



		queue_iterator operator -- (int)
		{
			const queue_iterator copy(*this);
			--*this;
			return copy;
		}



	private:
		// Used by cend(), erase() etc:
		queue_iterator(const group_pointer_type group_p, const pointer_type element_p) PLF_NOEXCEPT:
			group_pointer(group_p),
			element_pointer(element_p)
		{}


	}; // queue_iterator




	template <bool is_const_r>
	class queue_reverse_iterator
	{
	private:
		typedef typename queue::group_pointer_type		group_pointer_type;
		typedef typename queue::pointer					pointer_type;

	protected:
		iterator current;

	public:
		struct queue_iterator_tag {};
		typedef std::bidirectional_iterator_tag	iterator_category;
		typedef std::bidirectional_iterator_tag	iterator_concept;
		typedef iterator							iterator_type;
		typedef typename queue::value_type		value_type;
		typedef typename queue::difference_type	difference_type;
		typedef typename plf::conditional<is_const_r, typename queue::const_pointer, typename queue::pointer>::type 	pointer;
		typedef typename plf::conditional<is_const_r, typename queue::const_reference, typename queue::reference>::type	reference;

		friend class queue;


		queue_reverse_iterator () PLF_NOEXCEPT
		{}



		queue_reverse_iterator (const queue_reverse_iterator &source) PLF_NOEXCEPT:
			current(source.current)
		{}


		#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			template <bool is_const_rit = is_const_r, class = typename plf::enable_if<is_const_rit>::type >
			queue_reverse_iterator (const queue_reverse_iterator<false> &source) PLF_NOEXCEPT:
		#else
			queue_reverse_iterator (const queue_reverse_iterator<!is_const_r> &source) PLF_NOEXCEPT:
		#endif
			current(source.current)
		{}


		queue_reverse_iterator (const queue_iterator<is_const_r> &source) PLF_NOEXCEPT:
			current(source)
		{
			++(*this);
		}


		#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			template <bool is_const_rit = is_const_r, class = typename plf::enable_if<is_const_rit>::type >
			queue_reverse_iterator (const queue_iterator<false> &source) PLF_NOEXCEPT:
		#else
			queue_reverse_iterator (const queue_iterator<!is_const_r> &source) PLF_NOEXCEPT:
		#endif
			current(source)
		{
			++(*this);
		}


		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			queue_reverse_iterator (queue_reverse_iterator &&source) PLF_NOEXCEPT:
				current(std::move(source.current))
			{}


			#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
				template <bool is_const_rit = is_const_r, class = typename plf::enable_if<is_const_rit>::type >
				queue_reverse_iterator (queue_reverse_iterator<false> &&source) PLF_NOEXCEPT:
			#else
				queue_reverse_iterator (queue_iterator<!is_const_r> &&source) PLF_NOEXCEPT:
			#endif
				current(std::move(source.current))
			{}
		#endif


		queue_reverse_iterator& operator = (const queue_iterator<is_const_r> &source) PLF_NOEXCEPT
		{
			current = source;
			++current;
			return *this;
		}


		#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			template <bool is_const_rit = is_const_r, class = typename plf::enable_if<is_const_rit>::type >
			queue_reverse_iterator& operator = (const queue_iterator<false> &source) PLF_NOEXCEPT
		#else
			queue_reverse_iterator& operator = (const queue_iterator<!is_const_r> &source) PLF_NOEXCEPT
		#endif
		{
			current = source;
			++current;
			return *this;
		}


		queue_reverse_iterator& operator = (const queue_reverse_iterator &source) PLF_NOEXCEPT
		{
			current = source.current;
			return *this;
		}


		#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
			template <bool is_const_rit = is_const_r, class = typename plf::enable_if<is_const_rit>::type >
			queue_reverse_iterator& operator = (const queue_reverse_iterator<false> &source) PLF_NOEXCEPT
		#else
			queue_reverse_iterator& operator = (const queue_reverse_iterator<!is_const_r> &source) PLF_NOEXCEPT
		#endif
		{
			current = source.current;
			return *this;
		}


		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			queue_reverse_iterator& operator = (queue_reverse_iterator &&source) PLF_NOEXCEPT
			{
				assert(&source != this);
				current = std::move(source.current);
				return *this;
			}


			#ifdef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
				template <bool is_const_rit = is_const_r, class = typename plf::enable_if<is_const_rit>::type >
				queue_reverse_iterator& operator = (queue_reverse_iterator<false> &&source) PLF_NOEXCEPT
			#else
				queue_reverse_iterator& operator = (queue_reverse_iterator<!is_const_r> &&source) PLF_NOEXCEPT
			#endif
			{
				assert(&source != this);
				current = std::move(source.current);
				return *this;
			}
		#endif



		bool operator == (const queue_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (current == rh.current);
		}



		bool operator == (const queue_reverse_iterator<!is_const_r> &rh) const PLF_NOEXCEPT
		{
			return (current == rh.current);
		}



		bool operator != (const queue_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (current != rh.current);
		}



		bool operator != (const queue_reverse_iterator<!is_const_r> &rh) const PLF_NOEXCEPT
		{
			return (current != rh.current);
		}



		reference operator * () const PLF_NOEXCEPT
		{
			return *current.element_pointer;
		}



		pointer operator -> () const PLF_NOEXCEPT
		{
			return current.element_pointer;
		}



		queue_reverse_iterator & operator ++ ()
		{
			--current;
			return *this;
		}



		queue_reverse_iterator operator ++ (int)
		{
			const queue_reverse_iterator copy(*this);
			++*this;
			return copy;
		}



		queue_reverse_iterator & operator -- ()
		{
			++current;
			return *this;
		}



		queue_reverse_iterator operator -- (int)
		{
			const queue_reverse_iterator copy(*this);
			--*this;
			return copy;
		}



		queue_iterator<is_const_r> base() const PLF_NOEXCEPT
		{
			return (current.group_pointer != NULL) ? ++(queue_iterator<is_const_r>(current)) : queue_iterator<is_const_r>(NULL, NULL, NULL);
		}



	private:
		// Used by rend(), etc:
		queue_reverse_iterator(const group_pointer_type group_p, const pointer_type element_p) PLF_NOEXCEPT: current(group_p, element_p) {}

	}; // queue_reverse_iterator


}; // queue


#ifdef PLF_CPP20_SUPPORT
	template <class T>
	concept queue_iterator_concept = requires { typename T::queue_iterator_tag; };
#endif

} // plf namespace



namespace std
{

template <class element_type, plf::priority q_priority, class allocator_type>
void swap (plf::queue<element_type, q_priority, allocator_type> &a, plf::queue<element_type, q_priority, allocator_type> &b) PLF_NOEXCEPT_SWAP(allocator_type)
{
	a.swap(b);
}



#ifdef PLF_CPP20_SUPPORT
	// std::reverse_iterator overload, to allow use of queue with ranges and make_reverse_iterator primarily:
	template <plf::queue_iterator_concept it_type>
	class reverse_iterator<it_type> : public it_type::reverse_type
	{
	public:
		typedef typename it_type::reverse_type rit;
		using rit::rit;
	};
#endif

}


#undef PLF_EXCEPTIONS_SUPPORT
#undef PLF_DEFAULT_TEMPLATE_ARGUMENT_SUPPORT
#undef PLF_ALIGNMENT_SUPPORT
#undef PLF_INITIALIZER_LIST_SUPPORT
#undef PLF_IS_ALWAYS_EQUAL_SUPPORT
#undef PLF_TYPE_TRAITS_SUPPORT
#undef PLF_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_VARIADICS_SUPPORT
#undef PLF_MOVE_SEMANTICS_SUPPORT
#undef PLF_NOEXCEPT
#undef PLF_NOEXCEPT_ALLOCATOR
#undef PLF_NOEXCEPT_SWAP
#undef PLF_NOEXCEPT_MOVE_ASSIGN
#undef PLF_CONSTEXPR
#undef PLF_CONSTFUNC
#undef PLF_CPP20_SUPPORT

#undef PLF_CONSTRUCT
#undef PLF_DESTROY
#undef PLF_ALLOCATE
#undef PLF_DEALLOCATE

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
	#pragma warning ( pop )
#endif

#endif // PLF_QUEUE_H
