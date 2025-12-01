### a demon spirit took over me and through my hands created the worst optimisations known to man

this centers around 2 implementations of reading config data from a config file (`config.pws`) and storing its data into a struct.
The rules for the config file are:
- the valid config fields are known
- the config fields are 16 bytes, the values are arbitrary length
- the rule for valid kv pairs is defined as `field[ ]?=[ ]*value`
- the whole file can't be larger than 5KiB

I added some redundant lines just to make the test take longer

here are the results after running `perf stat -r 99 <binary>` on both binaries with the same config file:

here are the results without gcc optimisation:

| metric          | naive solution | ruthlessly optimised | diff   |
|-----------------|----------------|----------------------|--------|
| task-clock      | 0.22msec       | 0.22msec             | 0%     |
| page-faults     | 59             | 59                   | 0%     |
| cycles          | 936,329        | 944,089              | +0.8%  |
| instructions    | 825,360        | 805,468              | -2.4%  |
| branches        | 186,793        | 183,577              | -1.75% |
| branch-misses   | 6,357          | 5,447                | -16.7% |
| branch-misses   | 3.44%          | 2.95%                | -16.6% |
| seconds elapsed | 0.00044566     | 0.00042505           | -4.84% |

here they are with -O3:

| metric          | naive solution | ruthlessly optimised | diff   |
|-----------------|----------------|----------------------|--------|
| task-clock      | 0.22           | 0.22                 | 0%     |
| page-faults     | 60             | 62                   | +3.3%  |
| cycles          | 920,641        | 897,751              | -2.50% |
| instructions    | 809,245        | 814,633              | +0.66% |
| branches        | 185,796        | 188,243              | +1.31% |
| branch-misses   | 6,295          | 5,733                | -9.80% |
| branch-misses   | 3.41%          | 3.12%                | -9.29% |
| seconds elapsed | 0.00043912     | 0.00043517           | -0.9%  |


There is a clear winner, my optimised to hell program is undeniably faster! 16% less branch misses on the unoptimised version! 4% speed gain!! clearly this is worth the unmaintainable code I have created and im sure its worth it to load a config file (for a networking project). 

## RAW DATA
### no optimisation flag:

Performance counter stats for './read_config' (99 runs):

              0.22 msec task-clock                       #    0.491 CPUs utilized            ( +-  1.08% )
                 0      context-switches                 #    0.000 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                59      page-faults                      #  270.939 K/sec                    ( +-  0.21% )
           936,329      cycles                           #    4.300 GHz                      ( +-  0.52% )
           825,360      instructions                     #    0.90  insn per cycle           ( +-  0.13% )
           186,793      branches                         #  857.787 M/sec                    ( +-  0.12% )
             6,357      branch-misses                    #    3.44% of all branches          ( +-  0.67% )

        0.00044566 +- 0.00000709 seconds time elapsed  ( +-  1.59% )

Performance counter stats for './read_config_cursed' (99 runs):

              0.22 msec task-clock                       #    0.520 CPUs utilized            ( +-  0.71% )
                 0      context-switches                 #    0.000 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                59      page-faults                      #  273.966 K/sec                    ( +-  0.24% )
           944,089      cycles                           #    4.384 GHz                      ( +-  0.55% )
           805,468      instructions                     #    0.88  insn per cycle           ( +-  0.18% )
           183,577      branches                         #  852.438 M/sec                    ( +-  0.17% )
             5,447      branch-misses                    #    2.95% of all branches          ( +-  0.74% )

        0.00042505 +- 0.00000377 seconds time elapsed  ( +-  0.89% )

### -O3:

Performance counter stats for './read_config' (99 runs):

              0.22 msec task-clock                       #    0.491 CPUs utilized            ( +-  1.02% )
                 0      context-switches                 #    0.000 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                60      page-faults                      #  281.736 K/sec                    ( +-  0.19% )
           920,641      cycles                           #    4.323 GHz                      ( +-  0.52% )
           809,245      instructions                     #    0.90  insn per cycle           ( +-  0.16% )
           185,796      branches                         #  872.424 M/sec                    ( +-  0.16% )
             6,295      branch-misses                    #    3.41% of all branches          ( +-  0.71% )

        0.00043912 +- 0.00000639 seconds time elapsed  ( +-  1.46% )

 Performance counter stats for './read_config_cursed' (99 runs):

              0.22 msec task-clock                       #    0.502 CPUs utilized            ( +-  0.75% )
                 0      context-switches                 #    0.000 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                62      page-faults                      #  292.992 K/sec                    ( +-  0.20% )
           897,751      cycles                           #    4.242 GHz                      ( +-  0.50% )
           814,633      instructions                     #    0.91  insn per cycle           ( +-  0.17% )
           188,243      branches                         #  889.575 M/sec                    ( +-  0.16% )
             5,733      branch-misses                    #    3.12% of all branches          ( +-  0.70% )

        0.00043517 +- 0.00000480 seconds time elapsed  ( +-  1.10% )

