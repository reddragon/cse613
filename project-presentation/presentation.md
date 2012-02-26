Minute Sort
===========

---


What is Minute Sort?
====================
Minute Sort -- Sort as much as you can in a minute.
Current World Record -- 1.3 TB in a minute, with 
- 47 nodes
- Each having
  - 2 quad-core processors 
  - 24 GB memory
  - 16 disks of 500 GB each

---

Our Architecture
================

- We have 50 nodes on Lonestar and Ranger, each having
TODO -- Fill here

---

Our Architecture
================

- Use OpenMPI for inter-machine parallelism
- Use Cilk for intra-machine parallelism

---

Basic Algorithm - Inter Machine
===============================

- As discussed in class, we use over-sampling
to choose k good pivots, and hence divide the 
input into k roughly equal parts.

- Each machine then works on an independent 
partition.

- When all the machines have independently sorted
their partition, we can stitch the independent
paritions without needing to merge them.


---

Basic Algorithm - Intra Machine
===============================

- We will use a Merge-Sort based algorithm
to sort the relevant parition on each node.

- We will use Cilk for this purpose

- We are also trying various optimizations to
reduce buffer copying in merge-sort.


---

Challenges
===========

- We are practically limited by the disk I/O
bandwidth. If the disk can only read / write
50 MB/s, we cannot sort more than (50*60)/2 = 1.5 GB
in a minute. Potentially use compression before
writing.

- Network Bandwidth might be limited. Potentially use
compression.

- Certain optimizations like ensuring the file is in
the cache might help (by reading the file just before
the program is run).


---

Thank You
=========


