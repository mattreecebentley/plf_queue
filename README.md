# plf::queue
A data container replicating std::queue functionality but with better performance than standard library containers in a queue context. C++98/03/11/14/etc-compatible.

In real-world benchmarking it is on average:

* 20% faster for char
* 10% faster for int
* 15% faster for double
* 40% faster for small structs
* 65% faster for large structs

(averaged across total numbers of stored elements ranging between 10 and 1000000, with the number of samples = 126 and the number of elements increasing by 10% per sample. The test in question is a pump test, where elements are pushed and popped consecutively with the overall number of elements fluctuating over time. Benchmarked on a 3rd gen i5, GCC 9.2, x64. Priority is == plf::speed)

Full benchmarks and methodology are here: https://plflib.org/queue_benchmarks_i5_gcc.htm

Full description of container and functions here: https://plflib.org/queue.htm
