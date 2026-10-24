#include "plf_tools.h"

#include <cstdio> // log redirection
#include <cstdlib> // abort

#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#include <algorithm> // std::sort
#include <limits> // std::numeric_limits
#include <map>
#include <vector>

#ifdef PLF_CPP20_SUPPORT
	#include <compare> // std::strong_ordering, std::weak_ordering, std::partial_ordering, std::three_way_comparable
	#include <type_traits> // std::is_same_v
#endif

#include "plf_queue.h"

#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
	#include <map> // stateful allocator testing
#endif




// Element types for the comparison operator tests:
struct less_only_type // Has < but not <=>
{
	int value;

	less_only_type(const int v) : value(v) {}
	bool operator < (const less_only_type &rh) const { return value < rh.value; }
	bool operator != (const less_only_type &rh) const { return value != rh.value; }
};


struct unordered_type // Has != (for ==) but no ordering
{
	int value;

	unordered_type(const int v) : value(v) {}
	bool operator != (const unordered_type &rh) const { return value != rh.value; }
};


#ifdef PLF_CPP20_SUPPORT
	struct non_bool_result // Result of non_bool_less_type's <, cannot be used as a bool
	{
		explicit operator bool() const = delete;
	};


	struct non_bool_less_type // Has < but its result is not convertible to bool
	{
		int value;

		non_bool_less_type(const int v) : value(v) {}
		non_bool_result operator < (const non_bool_less_type &) const { return non_bool_result(); }
		bool operator != (const non_bool_less_type &rh) const { return value != rh.value; }
	};
#endif


template <class container_type, class value_type>
void push_values(container_type &container, const value_type *values, const unsigned int count)
{
	for (unsigned int index = 0; index != count; ++index)
	{
		container.push(values[index]);
	}
}




void title1(const char *title_text)
{
	printf("\n\n\n*** %s ***\n", title_text);
	printf("===========================================\n\n\n");
}

void title2(const char *title_text)
{
	printf("\n\n--- %s ---\n\n", title_text);
}


void failpass(const char *test_type, bool condition)
{
	printf("%s: ", test_type);

	if (condition)
	{
		printf("Pass\n");
	}
	else
	{
		printf("Fail\n");
		getchar();
		abort();
	}
}




#ifdef PLF_VARIADICS_SUPPORT
	struct perfect_forwarding_test
	{
		const bool success;

		perfect_forwarding_test(int && /*perfect1*/, int& perfect2)
			: success(true)
		{
			perfect2 = 1;
		}

		template <typename T, typename U>
		perfect_forwarding_test(T&& /*imperfect1*/, U&& /*imperfect2*/)
			: success(false)
		{}
	};
#endif


#if defined(PLF_TYPE_TRAITS_SUPPORT) && (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
	struct non_copyable_non_movable
	{
		int value;

		non_copyable_non_movable(const int v) : value(v) {}
		non_copyable_non_movable(const non_copyable_non_movable &) = delete;
		non_copyable_non_movable & operator = (const non_copyable_non_movable &) = delete;
	};
#endif



#if defined(PLF_ALLOCATOR_TRAITS_SUPPORT) && defined(PLF_MOVE_SEMANTICS_SUPPORT)
	// Stateful allocator: every block must be allocated through an allocator that was not default-constructed, and freed through one equal to the allocator that allocated it
	std::map<void *, int> allocation_owners;
	int allocator_mismatches = 0;

	template <class value_type_, bool propagate = true>
	struct id_allocator
	{
		typedef value_type_ value_type;
		typedef std::false_type is_always_equal;
		typedef typename plf::conditional<propagate, std::true_type, std::false_type>::type propagate_on_container_copy_assignment;
		typedef propagate_on_container_copy_assignment propagate_on_container_move_assignment;
		typedef std::true_type propagate_on_container_swap;

		template <class other_type> struct rebind { typedef id_allocator<other_type, propagate> other; };

		int id;

		id_allocator() PLF_NOEXCEPT: id(0) {} // Nothing may be allocated through a default-constructed allocator
		explicit id_allocator(const int new_id) PLF_NOEXCEPT: id(new_id) {}
		id_allocator(const id_allocator &source) PLF_NOEXCEPT: id(source.id) {}
		template <class other_type> id_allocator(const id_allocator<other_type, propagate> &source) PLF_NOEXCEPT: id(source.id) {}
		id_allocator & operator = (const id_allocator &source) PLF_NOEXCEPT { id = source.id; return *this; }

		value_type * allocate(const std::size_t size)
		{
			value_type * const allocation = static_cast<value_type *>(::operator new(size * sizeof(value_type)));
			if (id == 0) ++allocator_mismatches;
			allocation_owners[allocation] = id;
			return allocation;
		}

		void deallocate(value_type * const allocation, const std::size_t) PLF_NOEXCEPT
		{
			const std::map<void *, int>::iterator owner = allocation_owners.find(allocation);
			if (owner == allocation_owners.end() || owner->second != id) ++allocator_mismatches;
			if (owner != allocation_owners.end()) allocation_owners.erase(owner);
			::operator delete(allocation);
		}

		template <class other_type> bool operator == (const id_allocator<other_type, propagate> &source) const PLF_NOEXCEPT { return id == source.id; }
		template <class other_type> bool operator != (const id_allocator<other_type, propagate> &source) const PLF_NOEXCEPT { return id != source.id; }
	};
#endif



int main()
{
	freopen("error.log","w", stderr);

	using namespace std;
	using namespace plf;


	unsigned int looper = 0;


	while (++looper != 50)
	{
		{
			title1("Test basics");

			queue<unsigned int> i_queue(50, queue<unsigned int>::default_max_block_capacity());

			for (unsigned int temp = 0; temp != 250000; ++temp)
			{
				i_queue.push(10);
			}

 			failpass("Multipush test", i_queue.size() == 250000);


			queue<unsigned int> i_queue2;
 			i_queue2 = i_queue;

 			queue<unsigned int> i_queue3(i_queue);

 			failpass("Copy constructor test", i_queue3.size() == 250000);

 			queue<unsigned int> i_queue6(i_queue, i_queue3.get_allocator());

 			failpass("Allocator-extended copy constructor test", i_queue6.size() == 250000);

 			i_queue3.reserve(400000);

 			failpass("Reserve test", i_queue3.size() == 250000);


 			queue<unsigned int> i_queue7(50, 50);

 			for (unsigned int temp = 0; temp != 449; ++temp)
 			{
 				i_queue7.push(10);
 			}

 			failpass("Max limit test", i_queue7.capacity() == 450);

 			i_queue7.reshape(100, 100);

 			failpass("Reshape test", i_queue7.capacity() == 500);


 			#ifdef PLF_MOVE_SEMANTICS_SUPPORT
 				queue<unsigned int> i_queue4;
 				i_queue4 = std::move(i_queue3);
 				failpass("Move equality operator test", i_queue2 == i_queue4);
 				queue<unsigned int> i_queue5(std::move(i_queue4), i_queue3.get_allocator());

 				failpass("Allocator-extended move-construct test", i_queue5.size() == 250000);

 				i_queue3 = std::move(i_queue5);

 			#else
 				failpass("Equality operator test", i_queue2 == i_queue3);
 			#endif

 			failpass("Copy test", i_queue2.size() == 250000);
 			failpass("Equality operator test 2", i_queue == i_queue2);

 			i_queue2.push(5);

 			failpass("Inequality operator test", i_queue != i_queue2);

 			i_queue2.swap(i_queue3);

 			failpass("Swap test", i_queue2.size() == i_queue3.size() - 1);

 			swap(i_queue2, i_queue3);

 			failpass("Swap test 2", i_queue3.size() == i_queue2.size() - 1);

 			failpass("max_size() test", i_queue2.max_size() > i_queue2.size());


 			unsigned int total = 0;

 			const unsigned int temp_capacity = static_cast<unsigned int>(i_queue.capacity());

 			for (unsigned int temp = 0; temp != 200000; ++temp)
 			{
 				total += i_queue.back();
 				i_queue.pop();
 			}

 			failpass("Multipop test", i_queue.size() == 50000);
 			failpass("back() test", total == 2000000);

 			i_queue.shrink_to_fit();

 			failpass("shrink_to_fit() test", temp_capacity != i_queue.capacity());


 			do
 			{
 				if ((rand() & 3) == 0)
 				{
 					i_queue.push(10);
 				}
 				else
 				{
 					i_queue.pop();
 				}
 			} while (!i_queue.empty());;

 			failpass("Randomly pop/push till empty test", i_queue.empty());

 			#ifdef PLF_VARIADICS_SUPPORT
 				i_queue.emplace(20);
 				failpass("Emplace test", i_queue.size() == 1);
 			#endif
 		}


		{
 			title2("queue expansion test");

 			queue<int> i_queue;
			int push_total = 0, pop_total = 0;

			for (int counter = 0; counter != 500; ++counter)
			{
				i_queue.push(counter);
				push_total += counter;
			}


 			do
 			{
				pop_total += i_queue.front();
				i_queue.pop();
 			} while (!i_queue.empty());


			failpass("queue expansion test 1", pop_total == push_total);


			push_total = 0;
			pop_total = 0;


			for (int counter = 0; counter != 50; ++counter)
			{
				i_queue.push(counter);
				push_total += counter;
			}


 			do
 			{
 				if ((rand() & 3) == 0)
 				{
 					pop_total += i_queue.front();
					i_queue.pop();
 				}
 				else
 				{
 					i_queue.push(10);
					push_total += 10;
 				}
 			} while (!i_queue.empty() && i_queue.capacity() < 5000);



 			do
 			{
 				if ((rand() & 3) == 0)
 				{
 					i_queue.push(10);
					push_total += 10;
 				}
 				else
 				{
 					pop_total += i_queue.front();
					i_queue.pop();
 				}
 			} while (!i_queue.empty());


 			failpass("random queue expansion test", pop_total == push_total);
		}


 		{
 			title2("queue Special Case Tests");

 			queue<int> i_queue(50, 100);

 			for (int temp = 0; temp != 256; ++temp)
 			{
 				i_queue.push(10);
 			}

 			queue<int> i_queue_copy(i_queue);

 			int temp2 = 0;

 			for (int temp = 0; temp != 256; ++temp)
 			{
 				temp2 += i_queue_copy.back();
 				i_queue_copy.pop();
 			}

 			failpass("queue copy special case test", temp2 == 2560);
 		}


 		#ifdef PLF_VARIADICS_SUPPORT
 		{
 			title2("Perfect Forwarding tests");

 			queue<perfect_forwarding_test> pf_queue;

 			int lvalue = 0;
 			int &lvalueref = lvalue;

 			pf_queue.emplace(7, lvalueref);

 			failpass("Perfect forwarding test", pf_queue.back().success);
 			failpass("Perfect forwarding test 2", lvalueref == 1);
 		}
 		#endif


		#if defined(PLF_TYPE_TRAITS_SUPPORT) && defined(PLF_VARIADICS_SUPPORT) && defined(PLF_EXCEPTIONS_SUPPORT) && (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)) // before C++17, reshape() cannot compile for this type because consolidate() is instantiated without if constexpr
		{
			title2("Reshape non-copyable non-movable type test");

			queue<non_copyable_non_movable> ncm_queue;
			ncm_queue.emplace(1);

			const queue<non_copyable_non_movable>::size_type capacity_before = ncm_queue.capacity();
			bool threw_length_error = false;

			try
			{
				ncm_queue.reshape(2, 2); // the existing block is outside these limits and cannot be consolidated
			}
			catch (const std::length_error &)
			{
				threw_length_error = true;
			}

			failpass("Reshape non-copyable non-movable type throws length_error", threw_length_error);
			failpass("Reshape non-copyable non-movable type leaves queue unchanged", ncm_queue.size() == 1 && ncm_queue.back().value == 1 && ncm_queue.capacity() == capacity_before);

			// The rejected limits must not be kept: the next block should not be capped at 2 elements
			while (ncm_queue.size() != capacity_before)
			{
				ncm_queue.emplace(1);
			}

			ncm_queue.emplace(1);
			failpass("Reshape non-copyable non-movable type keeps the old block capacity limits", ncm_queue.capacity() - capacity_before > 2);
		}
		#endif


		{
			title1("Iterator tests");

			queue<int> iqueue;
			int total = 0;

			for (int temp = 0; temp != 1000; ++temp)
			{
				iqueue.push(1);
				++total;
			}

			int number_of_elements = iqueue.size();

			for (queue<int>::iterator current = iqueue.begin(); current != iqueue.end(); ++current)
			{
				total -= *current;
				--number_of_elements;
			}

			failpass("Iterator test 1", total == 0 && number_of_elements == 0);

			iqueue.pop();
			iqueue.pop();
			iqueue.pop();
			iqueue.pop();
			iqueue.pop();

			number_of_elements = iqueue.size();

			for (queue<int>::iterator current = iqueue.begin(); current != iqueue.end(); ++current)
			{
				--number_of_elements;
			}

			failpass("Iterator test 2", number_of_elements == 0);

			iqueue.push(1);
			iqueue.push(1);

			number_of_elements = iqueue.size();

			for (queue<int>::reverse_iterator current = iqueue.rbegin(); current != iqueue.rend(); ++current)
			{
				--number_of_elements;
			}

			failpass("Reverse Iterator test 1", number_of_elements == 0);

			{
				queue<int> freed_queue(3, 8); // The first block holds 3 elements, the second 6

				for (int counter = 0; counter != 6; ++counter)
				{
					freed_queue.push(counter);
				}

				freed_queue.pop();
				freed_queue.pop();
				freed_queue.pop(); // The first block is now empty, and is deallocated

				int expected = 5;
				bool correct = true;
				number_of_elements = 0;

				for (queue<int>::reverse_iterator current = freed_queue.rbegin(); current != freed_queue.rend() && number_of_elements != 10; ++current, ++number_of_elements)
				{
					correct = correct && *current == expected--;
				}

				failpass("Reverse Iterator after deallocating the front block test", correct && number_of_elements == 3);
			}

			{
				queue<int> reused_queue(3, 3);

				for (int counter = 0; counter != 6; ++counter)
				{
					reused_queue.push(counter);
				}

				reused_queue.pop();
				reused_queue.pop();
				reused_queue.pop(); // The first block is now empty, and is moved to the back for reuse

				for (int counter = 6; counter != 9; ++counter)
				{
					reused_queue.push(counter); // Fills the reused block
				}

				int expected = 8;
				bool correct = true;
				number_of_elements = 0;

				for (queue<int>::reverse_iterator current = reused_queue.rbegin(); current != reused_queue.rend() && number_of_elements != 10; ++current, ++number_of_elements)
				{
					correct = correct && *current == expected--;
				}

				failpass("Reverse Iterator after reusing the front block test", correct && number_of_elements == 6);
			}

			{ // The back element is the last in its block, and the emptied front block was moved after it for reuse
				queue<int> recycled_queue(3, 3);

				for (int counter = 0; counter != 6; ++counter)
				{
					recycled_queue.push(counter);
				}

				recycled_queue.pop();
				recycled_queue.pop();
				recycled_queue.pop(); // The first block is now empty, and is moved to the back for reuse

				int expected = 3;
				bool correct = true;
				number_of_elements = 0;

				for (queue<int>::iterator current = recycled_queue.begin(); current != recycled_queue.end() && number_of_elements != 10; ++current, ++number_of_elements)
				{
					correct = correct && *current == expected++;
				}

				failpass("Iterator with a reused block after the back block test", correct && number_of_elements == 3 && *--recycled_queue.end() == 5);

				queue<int> compare_queue(3, 3);

				for (int counter = 3; counter != 6; ++counter)
				{
					compare_queue.push(counter);
				}

				failpass("Equality operator with a reused block after the back block test", recycled_queue == compare_queue);
			}

			{ // The back element is the last in its block, and reserve() has added a block after it
				queue<int> reserved_queue(3, 3);

				for (int counter = 0; counter != 3; ++counter)
				{
					reserved_queue.push(counter);
				}

				reserved_queue.reserve(6);

				number_of_elements = 0;

				for (queue<int>::iterator current = reserved_queue.begin(); current != reserved_queue.end() && number_of_elements != 10; ++current)
				{
					++number_of_elements;
				}

				failpass("Iterator with a reserved block after the back block test", number_of_elements == 3);
			}

			{
				queue<int> empty_queue; // No block allocated yet
				const queue<int> &const_empty_queue = empty_queue;

				failpass("Reverse Iterator on a queue with no blocks test", empty_queue.rbegin() == empty_queue.rend() && empty_queue.crbegin() == empty_queue.crend() && const_empty_queue.rbegin() == const_empty_queue.rend());

				number_of_elements = 0;

				for (queue<int>::reverse_iterator current = empty_queue.rbegin(); current != empty_queue.rend(); ++current)
				{
					++number_of_elements;
				}

				failpass("Reverse Iterator loop on a queue with no blocks test", number_of_elements == 0);
			}

		}


		{
			title1("Comparison operator tests");

			// Block capacity 3, so containers with more than 3 elements span several blocks
			queue<int> empty1(3, 3), empty2(3, 3), one(3, 3), one_two(3, 3), one_three(3, 3), one_two_zero(3, 3), two_one(3, 3), long1(3, 3), long2(3, 3), long_differs(3, 3);

			const int one_values[] = {1}, one_two_values[] = {1, 2}, one_three_values[] = {1, 3}, one_two_zero_values[] = {1, 2, 0}, two_one_values[] = {2, 1};
			const int long_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, long_differs_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};

			push_values(one, one_values, 1);
			push_values(one_two, one_two_values, 2);
			push_values(one_three, one_three_values, 2);
			push_values(one_two_zero, one_two_zero_values, 3);
			push_values(two_one, two_one_values, 2);
			push_values(long1, long_values, 10);
			push_values(long2, long_values, 10);
			push_values(long_differs, long_differs_values, 10);

			failpass("Comparison of two empty queues", !(empty1 < empty2) && !(empty1 > empty2) && empty1 <= empty2 && empty1 >= empty2);
			failpass("Empty queue is less than non-empty queue", empty1 < one && one > empty1 && empty1 <= one && !(empty1 >= one));
			failpass("Comparison of queues differing in the last element", one_two < one_three && one_three > one_two && !(one_three < one_two));
			failpass("Prefix queue is less than longer queue", one_two < one_two_zero && !(one_two_zero < one_two) && one_two <= one_two_zero);
			failpass("Elements compared front to back", one_two < two_one && !(two_one < one_two));
			failpass("Comparison of equal multi-block queues", !(long1 < long2) && !(long1 > long2) && long1 <= long2 && long1 >= long2);
			failpass("Comparison of multi-block queues differing in the last block", long1 < long_differs && long_differs > long1 && !(long_differs <= long1));

			{ // The first has its front part-way through a block after pops, the second has a different block capacity
				queue<int> popped_queue(3, 3), other_queue(4, 100);

				for (int counter = 0; counter != 20; ++counter)
				{
					popped_queue.push(counter);
				}

				for (int counter = 0; counter != 16; ++counter)
				{
					popped_queue.pop();
				}

				for (int counter = 16; counter != 20; ++counter)
				{
					other_queue.push(counter);
				}

				failpass("Comparison of equal queues with different block layouts", !(popped_queue < other_queue) && !(other_queue < popped_queue) && popped_queue <= other_queue && popped_queue >= other_queue);

				#ifdef PLF_CPP20_SUPPORT
					failpass("Three-way comparison of equal queues with different block layouts", (popped_queue <=> other_queue) == 0);
				#endif

				other_queue.push(20);

				failpass("Comparison of queues with different block layouts differing in length", popped_queue < other_queue && !(other_queue <= popped_queue));
			}

			std::map<queue<int>, int> queue_map;
			queue_map[one_two] = 1;
			queue_map[empty1] = 2;
			queue_map[one_three] = 3;
			queue_map[one_two] = 4; // Existing key

			failpass("queue as std::map key test", queue_map.size() == 3 && queue_map.begin()->second == 2 && queue_map[one_two] == 4);

			std::vector<queue<int> > queue_vector;
			queue_vector.push_back(one_three);
			queue_vector.push_back(empty1);
			queue_vector.push_back(one_two);
			std::sort(queue_vector.begin(), queue_vector.end());

			failpass("std::sort on queues test", queue_vector[0] == empty1 && queue_vector[1] == one_two && queue_vector[2] == one_three);

			queue<less_only_type> less_only1, less_only2;
			less_only1.push(less_only_type(1));
			less_only2.push(less_only_type(2));

			failpass("Comparison of queues of an element type with only <", less_only1 < less_only2 && !(less_only2 <= less_only1));

			queue<unordered_type> unordered1, unordered2; // Comparison operators must not be instantiated for this type
			unordered1.push(unordered_type(1));
			unordered2.push(unordered_type(1));

			failpass("Equality of queues of an element type with no ordering", unordered1 == unordered2);

			#ifdef PLF_CPP20_SUPPORT
				static_assert(std::is_same_v<decltype(one_two <=> one_three), std::strong_ordering>);
				failpass("Three-way comparison test", (one_two <=> one_three) < 0 && (two_one <=> one_two) > 0 && (long1 <=> long2) == 0 && (empty1 <=> one) < 0);

				static_assert(std::is_same_v<decltype(less_only1 <=> less_only2), std::weak_ordering>);
				failpass("Three-way comparison of an element type with only <", (less_only1 <=> less_only2) == std::weak_ordering::less);

				queue<double> nan_queue, double_queue;
				nan_queue.push(std::numeric_limits<double>::quiet_NaN());
				double_queue.push(1.0);

				static_assert(std::is_same_v<decltype(nan_queue <=> double_queue), std::partial_ordering>);
				failpass("Three-way comparison of an element type with partial ordering", (nan_queue <=> double_queue) == std::partial_ordering::unordered);

				static_assert(std::three_way_comparable<queue<int>, std::strong_ordering>);
				static_assert(!std::three_way_comparable<queue<unordered_type> >);
				static_assert(!std::three_way_comparable<queue<non_bool_less_type> >);
			#endif
		}
	}


	#if defined(PLF_ALLOCATOR_TRAITS_SUPPORT) && defined(PLF_MOVE_SEMANTICS_SUPPORT)
	{
		title2("Stateful allocator tests");

		{
			typedef queue<int, plf::memory_use, id_allocator<int> > id_container;

			id_container container1(id_allocator<int>(1));

			for (int counter = 0; counter != 1000; ++counter)
			{
				container1.push(counter);
			}

			for (int counter = 0; counter != 500; ++counter)
			{
				container1.pop();
			}

			container1.reshape(10, 50);
			container1.shrink_to_fit();
			container1.trim();

			id_container container2(container1);
			id_container container3(container1, id_allocator<int>(3));
			id_container container4(std::move(container2));
			id_container container5(std::move(container3), id_allocator<int>(5)); // unequal allocators, so elements are moved individually

			id_container container6(id_allocator<int>(6));
			container6.push(1);
			container6 = container1; // propagates allocator 1
			id_container container7(id_allocator<int>(7));
			container7.push(1);
			container7 = std::move(container4); // propagates allocator 1
			container6.swap(container5); // swaps allocators 1 and 5

			for (int counter = 0; counter != 100; ++counter)
			{
				container5.push(counter);
				container6.push(counter);
				container7.push(counter);
			}

			failpass("Stateful allocator get_allocator test", container1.get_allocator().id == 1 && container5.get_allocator().id == 1 && container6.get_allocator().id == 5 && container7.get_allocator().id == 1);

			typedef queue<int, plf::memory_use, id_allocator<int, false> > id_container_no_propagation;

			id_container_no_propagation container8(id_allocator<int, false>(8));

			for (int counter = 0; counter != 1000; ++counter)
			{
				container8.push(counter);
			}

			container8.reshape(10, 50);
			container8.shrink_to_fit();

			failpass("Stateful allocator reshape test", container8.get_allocator().id == 8 && container8.size() == 1000);

			id_container_no_propagation container9(id_allocator<int, false>(9));
			container9.push(1);
			container9 = container8; // keeps allocator 9
			id_container_no_propagation container10(id_allocator<int, false>(10));
			container10.push(1);
			container10 = std::move(container8); // keeps allocator 10, so the elements are copied
			container9.swap(container10); // swaps allocators 9 and 10

			failpass("Stateful allocator no-propagation test", container9.get_allocator().id == 10 && container10.get_allocator().id == 9 && container9.size() == 1000 && container10.size() == 1000 && container8.empty());
		}

		failpass("Stateful allocator test", allocator_mismatches == 0 && allocation_owners.empty());
	}
	#endif

	title1("Test Suite PASS - Press ENTER to Exit");
	getchar();

	return 0;
}
