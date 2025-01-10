# AOS_Page-Replacement
Advanced Operating Systems - Page Replacement Algorithms and Evaluation

Objective:
To implement, evaluate, and analyze the performance of page replacement algorithms while proposing and testing a custom-designed algorithm.

Requirements:
  1. Algorithms to Implement:
    ● FIFO (First-In-First-Out) algorithm.
    ● Optimal page replacement algorithm.
    ● Enhanced second-chance algorithm.
    ● A custom-designed algorithm that must outperform FIFO in terms of page-fault rate or cost.
  2. Reference String Details:
    ● Range: 1 to 1,200.
    ● Length: At least 120,000 memory references.
    ● Three types of test reference strings:
      ● Random: Randomly generated reference strings.
      ● Locality-based: Simulates procedure calls with varying lengths.
      ● Custom: A user-defined reference string.
  3. Memory Configuration:
      ● Number of frames in main memory: 10, 20, 30, ..., up to 100.
  4. Performance Metrics:
    ● Page faults versus the number of frames.
    ● Number of interrupts versus the number of frames.
    ● Disk writes (in pages) versus the number of frames.

