A multithreaded, doubly linked list with infrastructure for measuring lock contention for a workload.

July 4th, 2018

From the root:

```sh
$ cmake .
$ make
$ ./linked_list_test
```

Perf numbers are dumped to:

* contention_measurements.txt
* no_contention_measurements.txt

They are in the following format:

```sh
<name> <total calls> <total time> <time per call>
```



