#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
	#if _MSC_VER >= 1600
		#define PLF_TEST_MOVE_SEMANTICS_SUPPORT
	#endif
	#if _MSC_VER >= 1700
		#define PLF_TEST_TYPE_TRAITS_SUPPORT
	#endif
	#if _MSC_VER >= 1800
		#define PLF_TEST_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_TEST_INITIALIZER_LIST_SUPPORT
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && _MSC_VER >= 1929
		#define PLF_TEST_CPP20_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L // C++11 support, at least
	#define PLF_TEST_MOVE_SEMANTICS_SUPPORT

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4 // 4.2 and below do not support variadic templates
			#define PLF_TEST_MOVE_SEMANTICS_SUPPORT
			#define PLF_TEST_VARIADICS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4 // 4.3 and below do not support initializer lists
			#define PLF_TEST_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TEST_TYPE_TRAITS_SUPPORT
		#endif
	#elif defined(__clang__) && !defined(__GLIBCXX__) && !defined(_LIBCPP_CXX03_LANG)
		#if __clang_major__ >= 3 // clang versions < 3 don't support __has_feature() or traits
			#define PLF_TEST_TYPE_TRAITS_SUPPORT

			#if __has_feature(cxx_rvalue_references) && !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)
				#define PLF_TEST_MOVE_SEMANTICS_SUPPORT
			#endif
			#if __has_feature(cxx_variadic_templates) && !defined(_LIBCPP_HAS_NO_VARIADICS)
				#define PLF_TEST_VARIADICS_SUPPORT
			#endif
			#if (__clang_major__ == 3 && __clang_minor__ >= 1) || __clang_major__ > 3
				#define PLF_TEST_INITIALIZER_LIST_SUPPORT
			#endif
		#endif
	#elif defined(__GLIBCXX__)
		#if __GLIBCXX__ >= 20080606
			#define PLF_TEST_MOVE_SEMANTICS_SUPPORT
			#define PLF_TEST_VARIADICS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20090421
			#define PLF_TEST_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20150422
			#define PLF_TEST_TYPE_TRAITS_SUPPORT
		#endif
	#elif !(defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) || defined(_LIBCPP_HAS_NO_VARIADICS))
		// Assume full support for other compilers and standard libraries
		#define PLF_TEST_VARIADICS_SUPPORT
		#define PLF_TEST_TYPE_TRAITS_SUPPORT
		#define PLF_TEST_MOVE_SEMANTICS_SUPPORT
		#define PLF_TEST_INITIALIZER_LIST_SUPPORT
	#endif

	#if __cplusplus > 201704L && ((defined(__clang__) && (__clang_major__ >= 13)) || (defined(__GNUC__) && __GNUC__ >= 10) || (!defined(__clang__) && !defined(__GNUC__))) // assume correct C++20 implementation for other compilers
		#define PLF_TEST_CPP20_SUPPORT
	#endif
#endif



#include <cstdio> // log redirection
#include <cstdlib> // abort

#ifdef PLF_TEST_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#include "plf_queue.h"




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




#ifdef PLF_TEST_VARIADICS_SUPPORT
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

			queue<unsigned int> i_queue(50);

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


 			#ifdef PLF_TEST_MOVE_SEMANTICS_SUPPORT
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

 			#ifdef PLF_TEST_VARIADICS_SUPPORT
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


 		#ifdef PLF_TEST_VARIADICS_SUPPORT
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

	}

	title1("Test Suite PASS - Press ENTER to Exit");
	getchar();

	return 0;
}
