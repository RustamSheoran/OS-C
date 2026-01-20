# OS-C Kernel Test Framework

## Overview
Comprehensive test suite for kernel validation, including boot tests, subsystem tests, and stress tests. Tests run in kernel self-test mode or via external scripts.

## Test Categories

### Boot Tests (2 implemented)
- EFI memory map parsing and validation
- Higher-half kernel mapping verification
- Serial console initialization
- Basic kernel entry sanity checks

### Memory Management Tests (5 implemented)
- Page frame allocator integrity (alloc/free cycles)
- PFA stress testing (100 page allocations)
- Virtual memory mapping correctness
- Heap allocator leak detection
- Heap stress testing (50 allocations)

### Scheduler Tests (3 implemented)
- Task creation and destruction validation
- Context switching correctness
- Preemption timing and fairness
- SMP task distribution (basic)

### Syscall Tests (1 implemented)
- x86_64 syscall ABI compliance
- Register passing validation
- Write/read operations testing
- Error handling for invalid parameters

### Filesystem Tests (1 implemented)
- In-memory file create/read/write/delete
- NTFS boot sector parsing validation
- VFS layer functionality

### SMP Tests (0 active - requires multi-core)
- AP boot verification
- Per-CPU data isolation
- SMP scheduler load balancing

## Implementation Details

### Test Structure
```
tests/
├── test_runner.c    # Main test orchestration (12 tests)
├── boot_test.c      # EFI/serial tests
├── memory_test.c    # PFA/heap/paging tests
└── scheduler_test.c # Task/context switch tests
```

### Test Runner
- `run_tests()` function executes all suites
- Results logged to serial console with PASS/FAIL
- Integrated into kernel shell (`test` command)
- No external dependencies

### Test Execution
```bash
# Build test kernel (same as normal)
make

# Run tests in QEMU
./scripts/run_test.sh

# Manual shell testing
./scripts/run.sh
# Then type 'test' in kernel prompt
```

## CI Integration
- Automated toolchain build (binutils + GCC)
- Kernel compilation verification
- QEMU boot test (checks "Kernel loaded")
- Self-test execution (validates test output)
- Result logging and failure reporting

## Current Test Status
- **Total Tests**: 12 implemented and functional
- **Coverage**: Boot, memory, scheduler, syscalls, filesystem
- **Execution**: Kernel self-test mode
- **CI Status**: Fully automated with GitHub Actions

## Adding New Tests
1. Create test file in `tests/` directory
2. Implement test functions returning 0 (pass) or non-zero (fail)
3. Add extern declarations in `test_runner.c`
4. Call from `run_tests()` function
5. Update this documentation

## Test Results Format
```
Running kernel tests...
Boot test: PASS
EFI init: PASS
Serial output: PASS
PFA alloc/free: PASS
PFA stress: PASS
...
Tests completed: 12/12
```

## Future Expansions
- SMP multi-core testing (requires QEMU -smp 2+)
- Network packet I/O validation
- Graphics framebuffer tests
- USB/PCI driver hotplug tests
- Performance regression testing
- Real hardware compatibility tests