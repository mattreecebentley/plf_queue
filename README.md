# plf_queue
A data container replicating std::queue functionality but with better performance than standard library containers in a queue context. C++98/03/11/14/etc-compatible.

Compared to std::queue it is:
* 36% faster for char
* 44% faster for int
* 45% faster for double
* 52% faster for small structs
* 560% faster for large structs
* 147% faster on average across types

(Averaged across total numbers of stored elements ranging between 10 and 1000000, with the number of samples = 126 and the number of elements increasing by 10% per sample. Larger numbers of elements tend to perform better. These benchmarks are performed without calling reserve() on plf::queue, but if reserve() is used, the performance advantage will be even greater)
