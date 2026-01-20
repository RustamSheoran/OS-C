# OS-C Kernel Test Framework

## Overview
Comprehensive test suite for kernel validation, including boot tests, subsystem tests, and stress tests. Tests run in kernel self-test mode or via external scripts.

## Test Categories

### Boot Tests
- EFI memory map parsing validation
- Higher-half mapping verification
- Serial console initialization
- Basic kernel entry sanity

### Memory Management Tests
- Page frame allocator integrity (alloc/free cycles)
- Virtual memory mapping correctness
- Heap allocator leak detection
- Page fault handling

### Scheduler Tests
- Task creation/destruction
- Context switching validation
- Preemption timing accuracy
- Fairness under load (100 tasks)

### Syscall Tests
- ABI compliance (register passing)
- Error handling (invalid params)
- Performance (syscall overhead)
- Concurrency safety

### SMP Tests
- AP boot verification
- Lock contention testing
- Per-CPU data isolation
- Cross-core communication

### I/O Tests
- Serial I/O reliability
- ATA sector read/write integrity
- Interrupt handling correctness
- Driver hotplug simulation

### Filesystem Tests
- File create/read/write/delete
- Directory operations
- Concurrent access safety
- Corruption recovery

## Implementation
- Tests in `tests/` directory
- `test_runner.c` orchestrates all tests
- Kernel boot flag enables test mode
- Results logged to serial console
- Assertions with detailed failure info

## Running Tests
```bash
make test  # Builds test kernel
./scripts/run_test.sh  # Boots and runs tests
```

## Test Results
- PASS/FAIL status per test
- Timing information
- Memory usage stats
- Failure diagnostics