# Instruction to check PRISM memory usage

## Testing memory usage in fit

```
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app

# First check if a fit can be run interactively
PRISM_4Flavour_dChi2Scan /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/fcl/PRISM/PRISMOscScan_Grid.fcl > fit_output_novalgrind.log &

# If success then use valgrind
# the machine has itself valgrind
which valgrind
# DUNE also has valgrind setup in ups
setup valgrind
which valgrind
```

### Inspect memory leak

```
valgrind --leak-check=yes --suppressions=$ROOTSYS/etc/valgrind-root.supp --log-file="fit_dmsq32_stat_only_FourFlavor_mem_leak.log" PRISM_4Flavour_dChi2Scan /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/fcl/PRISM/PRISMOscScan_Grid.fcl > fit_output_with_valgrind.log &
# The suppressions option suppresses messages about things that ROOT does routinely that are not bugs
# The log-file option to direct memory check output
```

### Heap memory usage

```
# Time unit is set to the number of bytes allocated/deallocated on the heap and stack(s)
# --time-unit=MB
valgrind --tool=massif PRISM_4Flavour_dChi2Scan /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/fcl/PRISM/PRISMOscScan_Grid.fcl > fit_heap_memory.log &
```


### Stack/all memory usage

```
valgrind --tool=massif --pages-as-heap=yes PRISM_4Flavour_dChi2Scan /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/fcl/PRISM/PRISMOscScan_Grid.fcl > fit_all_memory.log &

ms_print massif.out.<PID> > memory_graph.log
```

## Testing time usage in fit (less critical)

Tools:

```
gprof
valgrind --tool=callgrind
Debugger
```

```
valgrind --tool=callgrind --fn-skip=_dl_runtime_resolve_avx   yourprogram
```

## Useful resources

Some references:
https://valgrind.org/docs/manual/quick-start.html
https://valgrind.org/docs/manual/ms-manual.html
https://dune.github.io/computing-training-basics/10-code-makeover-day2/index.html
https://root-forum.cern.ch/t/valgrind-and-root/28506
