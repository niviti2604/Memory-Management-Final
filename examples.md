### Example 1
```bash
help
init memory 4096
set allocator first_fit
malloc 1024
malloc 512
malloc 2048
free 2
dump memory
stats
```
##### Output:
```bash
init memory 4096
Memory initialized with size 4096
set allocator first_fit
Allocator set to first_fit
 malloc 1024
Allocated block id=1 at address=0x24138a1f400
malloc 512
Allocated block id=2 at address=0x24138a1f640
malloc 2048
Allocated block id=3 at address=0x24138a1f0a0
dump memory
[0x0 - 0x3ff] USED (id=1, req=400)
[0x400 - 0x5ff] USED (id=2, req=200)
[0x600 - 0xdff] USED (id=3, req=800)
[0xe00 - 0xfff] FREE
free 2
Block 2 freed and merged
dump memory
[0x0 - 0x3ff] USED (id=1, req=400)
[0x400 - 0x5ff] FREE
[0x600 - 0xdff] USED (id=3, req=800)
[0xe00 - 0xfff] FREE
stats

===== Memory Statistics =====
Total memory: 4096
Used memory: 3072
Free memory: 1024
Memory utilization: 75%
Internal fragmentation: 0 bytes
External fragmentation: 50%
==============================

```
### Example 2
```bash
    help
    init memory 2048
    set allocator best_fit
    malloc 300
    malloc 500
    malloc 200
    free 2
    dump memory
    malloc 400
    set allocator worst_fit
    free 4
    malloc 400
  
```
##### Output:
```bash
init memory 2048
Memory initialized with size 2048
set allocator best_fit
Allocator set to best_fit
malloc 300
Allocated block id=1 at address=0x1598f281ae0
malloc 500
Allocated block id=2 at address=0x1598f281c60
malloc 200
Allocated block id=3 at address=0x1598f281de0
free 2
Block 2 freed and merged
dump memory
[0x0 - 0x12b] USED (id=1, req=12c)
[0x12c - 0x31f] FREE
[0x320 - 0x3e7] USED (id=3, req=c8)
[0x3e8 - 0x7ff] FREE
malloc 400
Allocated block id=4 at address=0x1598f281c60
set allocator worst_fit
Allocator set to worst_fit
free 4
Block 4 freed and merged
malloc 400
Allocated block id=5 at address=0x1598f281c90
```
### Example 3

```bash
init memory 1024
set allocator worst_fit
malloc 100
malloc 200
malloc 150
malloc 250
dump memory
free 2
free 4
dump memory
stats
  
```
##### Output:
```bash
init memory 1024
Memory initialized with size 1024
set allocator worst_fit
Allocator set to worst_fit
malloc 100
Allocated block id=1 at address=0x2ab5ed00fd0
malloc 200
Allocated block id=2 at address=0x2ab5ed00d00
malloc 150
Allocated block id=3 at address=0x2ab5ed00ee0
malloc 250
Allocated block id=4 at address=0x2ab5ed00eb0
dump memory
[0x0 - 0x63] USED (id=1, req=64)
[0x64 - 0x12b] USED (id=2, req=c8)
[0x12c - 0x1c1] USED (id=3, req=96)
[0x1c2 - 0x2bb] USED (id=4, req=fa)
[0x2bc - 0x3ff] FREE
free 2
Block 2 freed and merged
free 4
Block 4 freed and merged
dump memory
[0x0 - 0x63] USED (id=1, req=64)
[0x64 - 0x12b] FREE
[0x12c - 0x1c1] USED (id=3, req=96)
[0x1c2 - 0x3ff] FREE
stats

===== Memory Statistics =====
Total memory: 1024
Used memory: 250
Free memory: 774
Memory utilization: 24.4141%
Internal fragmentation: 0 bytes
External fragmentation: 25.8398%
==============================
```
### Example 4
```bash
help
init memory 8192
init cache 1 512 32 1 FIFO
set allocator first_fit
malloc 100
malloc 200
cache read 0x1000
stats
  
```
##### Output:
```bash
init memory 8192
Memory initialized with size 8192
init cache 1 512 32 1 FIFO
Cache initialized: 16 sets, 1-way, block size: 32 bytes, Policy: FIFO
Cache L1 initialized
set allocator first_fit
Allocator set to first_fit
malloc 100
Allocated block id=1 at address=0x18015cce290
malloc 200
Allocated block id=2 at address=0x18015cce5c0
cache read 0x1000
Cache read at 0x1000
stats

===== Memory Statistics =====
Total memory: 8192
Used memory: 300
Free memory: 7892
Memory utilization: 3.66211%
Internal fragmentation: 0 bytes
External fragmentation: 0%
==============================

===== Cache Statistics =====
L1 Cache Hits: 1
L1 Cache Misses: 15
L1 Hit Rate: 6.25%
==============================

```
