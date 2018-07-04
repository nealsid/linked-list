A multithreaded, doubly linked list with some infrastructure for measuring lock contention for a workload.

July 4th, 2018

From the root:

```sh
$ cmake .
$ make
$ ./linked_list_test
```

Some perf numbers are dumped to:

```sh
* contention_measurements.txt
* no_contention_measurements.txt
```

They are in the following format:

```sh
<name> <total calls> <total time> <time per call>
```



