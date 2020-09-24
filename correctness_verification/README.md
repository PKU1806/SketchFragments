# SuMax Verification

- In this section, we emulated and tested correctness performance of SuMax with CAIDA2018 by C++ code.

- File structure

  ```
  .
  ├── [ 384]  delay_distribution
  │   ├── [ 11K]  BOBHash32.h
  │   ├── [7.4K]  EMFSD.h
  │   ├── [7.1K]  Sketch.h
  │   ├── [ 320]  compute_wmre.sh
  │   ├── [6.3K]  delay_chi2_wmre_vs_memory.cpp
  │   ├── [6.4K]  delay_distribution_wmre_with_topk.cpp
  │   ├── [6.4K]  delay_mixed_wmre_vs_memory.cpp
  │   ├── [6.6K]  delay_union_wmre_vs_memory.cpp
  │   ├── [ 534]  distribution.h
  │   └── [ 994]  param.h
  ├── [ 320]  flowsize
  │   ├── [ 11K]  BOBHash32.h
  │   ├── [7.4K]  EMFSD.h
  │   ├── [7.1K]  Sketch.h
  │   ├── [4.8K]  card_dist_ent_vs_memory.cpp
  │   ├── [ 534]  distribution.h
  │   ├── [7.4K]  flowsize_ARE_vs_memory.cpp
  │   ├── [ 151]  flowsize_estimation.sh
  │   └── [ 994]  param.h
  ├── [ 256]  last_arrival-time
  │   ├── [ 11K]  BOBHash32.h
  │   ├── [3.7K]  Interval.h
  │   ├── [  76]  compute_last_arrival-time.sh
  │   ├── [3.6K]  last_arrival.cpp
  │   ├── [ 458]  packet.h
  │   └── [ 984]  param.h
  └── [ 352]  max_inter-arrival
      ├── [ 11K]  BOBHash32.h
      ├── [3.5K]  Interval.h
      ├── [2.5K]  Interval_old.h
      ├── [ 175]  compute_inter-arrival.sh
      ├── [3.8K]  max_interval_vs_memory.cpp
      ├── [3.9K]  max_interval_vs_topk.cpp
      ├── [ 458]  packet.h
      └── [ 971]  param.h
  ```

  - ``delay_distribution`` is to calculate WMRE of delay_distribution with SuMax, CM and CU. ``delay_chi2_wmre_vs_memory.cpp`` is to emulate WMRE of Sumax, CM and CU with different memory and the ground truth obeys a Chi-Square distribution.
Moreover, ``delay_union_wmre_vs_memory.cpp`` and ``delay_mixed_wmre_vs_memory.cpp`` are able to accomplish the task above with uniform distributed and mixed distributed ground truth.
Moreover, ``delay_distribution_wmre_with_topk.cpp`` is able to accomplish that task of different top-k flows.

  - ``flowsize`` is to calculate RE of flowsize, RE of cardinality, RE of entropy and WMRE of flowsize distribution. ``flowsize_ARE_vs_memory.cpp`` is able to calculate ARE of flowsize of SuMax, CM and CU with different memory. Moreover,
``card_dist_ent_vs_memory.cpp`` is to calculate cardinality RE, flowsize distribution WMRE and entropy RE of SuMax, CM and CU with different memory.

  - ``last_arrival-time`` is to calculate correctness rate and ARE of last arrival-time of SuMax. ``last_arrival.cpp`` is the main file.

  - ``max_inter-arrival`` is to calculate ARE of maximum inter-arrival time of SuMax. ``max_interval_vs_memory.cpp`` is able to calculate ARE of maximum inter-arrival time with different memory. 
``max_interval_vs_topk.cpp`` is able to calculate ARE of maximum inter-arrival time with different top-k flows.

- Run the code
  - ``cd \task\path``
  - change macro ``datapath`` as path of your CAIDA2018 dataset. 
    
    ``#define datapath "\your\path"``
  - run the ``.sh`` file in the folder. 
  
    ``/bin/sh *.sh``
