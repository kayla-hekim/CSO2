(Plots also uploaded)

When I plotted the original measurements on a log-scale histogram, most durations were clustered below about 100 ns. There was a clear gap before much larger spikes in the thousands of nanoseconds and one spike in the millions of nanoseconds. I chose a threshold of 300 ns because it is above the normal loop variation but below the interrupt-level spikes.

After re-running with THRESHOLD=300 on a single core, the results contained only larger delays. Comparing /proc/interrupts before and after showed thousands of interrupts occurred during execution. The millisecond-scale spike suggests the program was likely context switched. This gives confidence that the threshold separates normal timing from interrupt and scheduling delays.