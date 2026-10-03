# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- ATmega328P as the first supported device.
- A portability layer of device-specific pin, register and immediate-argument macros, later extended with an ATmega32 port and a global-interrupt enable macro.
- A central configuration header defining the system tick rate and the LCD bus width.
- A UART driver with a callback-driven receive path: register, deregister, enable and disable callbacks, character and string peek/get, and separate receive/transmit flush functions.
- UART sequence matching for framing messages received from a host.
- A non-blocking `uart_get_char2()` for streaming reads.
- Configurable UART settings in a dedicated `uart_config.h`.
- `screen` as an alternative serial monitor alongside `putty` and `minicom`.
- An HD44780-compatible LCD driver with 4-bit/8-bit bus selection, DDRAM addressing, a screen buffer, and `lcd_set_brightness()`, by [@notweerdmonk].
- LCD helpers for flash-resident strings and for unsigned integers.
- An ADC driver plus an auto-trigger demonstration application.
- A serial debugger with task inspection and control, driven by single-character commands and toggled with `Escape` and `F6`, by [@notweerdmonk].
- Binary search over the debugger command table, with `achar_sort.gawk` and `achar_sort_cmts.gawk` keeping the command list and dispatch table in matching order.
- Debugger support for suspending, resuming and restarting tasks, and for halting other tasks.
- Link-time stubs enabling peripheral modules to be excluded conditionally from a build.
- Coroutine-based cooperative tasks, with `COROUTINE_RETURN()` for early exits, by [@notweerdmonk].
- Software timers with callback handlers and a repeat mode, plus `SET_TIMER()` and `RESET_TIMER()` macros, by [@notweerdmonk].
- Resettable timers and a single scheduler cycle per task.
- Hardware timer drivers for Timer/Counter 0, 1 and 2.
- A system tick on Timer/Counter 1, configurable through `F_SYS_TICK`, with millisecond ticks derived from it.
- An event registry supporting registration, deregistration, edge and level triggers, and asynchronous event delivery, by [@notweerdmonk].
- A task state machine covering suspend, resume and restart, with a `TASK_SIG_RESTART` state-change signal.
- Task resume events, and automatic resource release when a task times out.
- Watchdog-based task supervision: a watchdog interrupt, task timeout detection, `setjmp`/`longjmp`-based task reset, and the reset reason logged at boot, by [@notweerdmonk].
- The ability for tasks to reset the watchdog voluntarily.
- `crash()` for fault handling, persisting error codes and the faulting program counter in EEPROM.
- `get_atboot_mcusc()` to read the reset-cause mirror captured at boot.
- A trap and `raise()` mechanism for synchronous kernel services, served by a naked INT1 trap interrupt.
- Trap-based resource owner checks, which stay atomic once context switches and preemption are implemented.
- A resource management subsystem providing `acquire_resource()`, `acquire_resource_mask()`, `release_resource()`, `release_all_resources()` and `check_resource_owner()`, by [@notweerdmonk].
- Resource control for the ADC.
- A build flag to compile resource management in or out.
- Multi-channel software PWM with registration, value and deregistration APIs plus a `REGISTER_SOFTWARE_PWM()` macro, and a second PWM channel.
- Sleep mode selection and query macros, with idle sleep skipped while the ADC is enabled.
- A version string reported in the boot banner.
- A boot logo and verbose boot output.
- Demonstration applications for LED blinking, ADC reading, and application states with sleep.
- Standalone and non-standalone build modes: the non-standalone mode produces a `libmain.a` archive for third-party use, while standalone links the kernel against a separate tasks archive to produce a full firmware image, by [@notweerdmonk].
- A separate `libtasks.a` archive linked ahead of `libmain.a`, with a weak kernel default for `tasks_main()` so that user code overrides it.
- A private interface framework that registers functions in a per-subsystem flash section via `__start_`/`__stop_` symbols, removing the need for header files or extern declarations.
- Automatic `--wrap` symbol wrappers generated from annotated header declarations, injecting resource owner checks into peripheral APIs.
- Compile-time optional kernel and peripheral modules in `enable_modules.mk`, covering `UART`, `LCD`, `ADC`, `RESMGMT`, `DEBUGGER`, `SIM` and `SA`.
- Out-of-source builds with per-target artifact directories, `avr-size` reporting, and stack usage summaries produced by `avstack.pl`.
- Doxygen configuration and generated API documentation.
- Developer helper scripts: `avstack.pl`, `tabulate.awk`, `achar_sort.gawk`, `achar_sort_cmts.gawk`, `make_gdb_script.bash` and `marichi.bashrc`, plus an action-item generation target.
- External dependencies vendored as git submodules: `coroutine` for coroutines on both target and host, and `loge` for logging.
- Worked examples: a host-side length-encoding string encoder paired with a coroutine-based parser running on the target.
- GPL-3 license headers across all sources.

### Changed

- Peripheral initialisation takes explicit pin arguments, and functions were renamed for consistency.
- Application entry points are invoked through callbacks, with the kernel registering a callback with the UART for the application.
- ASCII character codes were moved into `common.h`, replacing magic numbers throughout.
- The directory structure was reorganised, with a dedicated configuration header and applications under `apps/`.
- User applications were renamed to tasks and moved to `tasks/`.
- Optional kernel components moved under `kernel/opt/<component>/`, and peripheral sources under `peripheral/<name>/`, with stubs in `peripheral/<name>/stub/`.
- Peripheral public APIs are declared weak, and stubs moved out of inline preprocessor guards into separate translation units; stub compilation no longer depends on feature flags.
- The kernel is compiled as `gnu11` with `-Werror` and `-fshort-enums`.
- Kernel initialisation and the scheduler now run from `main()` constructor and destructor hooks.
- Debugger and `setjmp`/`longjmp` internals moved into private headers.
- Naked functions moved to a dedicated translation unit that is excluded from stack usage analysis.
- Resource owner checks moved onto the trap interrupt, avoiding duplication of the implementation at every call site.
- "Exception" terminology was replaced by "trap" throughout, with asynchronous event delivery routed through it.
- The ADC now uses portability macros directly, removing redundant resource owner checks.
- The current task mask is used instead of recomputing a bitmask from the current task id.
- Resource management was optimised after its initial implementation.
- `acquire_resource()` now returns status values, and `change_task_state()` returns void.
- `suspend_task()`, `run_task()`, `resume_task()` and `restart_task()` return the resulting task state, and `change_task_state()` was refactored.
- Timer and event handler callbacks receive the event id and timer id, and a dedicated `timer_handler` type was introduced.
- Global kernel variables were grouped into structures.
- The event trigger mechanism was reworked to support asynchronous delivery.
- Timers moved to a macro-only implementation, and timer 2 is rejected for use as the system tick.
- Coroutines are delegated into a helper function.
- `CAT` was renamed to `CONCAT`, and `BYTE_TO_MASK` was renamed to `BITMASK` using `__tmp_reg__`.
- A macro was added to obtain a flash ROM address with an optional offset.
- LCD configuration moved from hardcoded ports to absolute pin numbers.
- The public API header was reorganised and the jump buffer renamed.
- Kernel sizing limits were consolidated into `kernel/kernel_config.h`.
- Debugger output honours the verbosity setting, and the debugger no longer drives the LCD.
- The UART character read return type changed, with all callers updated.
- The UART transmit buffer was enlarged.
- UART public functions now delegate to internal ones only while the debugger is inactive, and the debugger takes exclusive control of the UART while running.
- `crash()` uses `r0` for the error code.
- Strings moved from SRAM to flash across the debugger, boot message and tasks, then were made `static` and moved to function scope.
- Boot messages were revised.
- Verbose boot output and improved application LCD text.
- The kernel is pinged from the display application to confirm liveness.
- The project and tasks Makefiles were reworked for out-of-source builds, sharing a tasks-specific fragment and helper functions.
- The standalone firmware filename is driven by a Make variable.
- Build output and cscope artifacts are excluded from version control, as is Doxygen output.
- The tags file is excluded from version control, and `avstack.pl` warnings are suppressed while stack usage is saved to a file.
- Doxygen comments were added across the public headers, with Javadoc syntax corrected.
- Code formatting and comment consistency improved throughout the kernel and Makefiles.

### Deprecated

- Macros for marking deprecated functions and variables.
- Some ADC functions, in favour of the equivalent portability macros.

### Fixed

- `coroutine.h` include paths corrected across the top-level Makefile, the LCD header and the task sources, and the external dependency include path added to the top-level Makefile.
- Optional kernel subsystem inclusion in `enable_modules.mk` now invokes the `add_module_sources` helper correctly.
- The debugger enable variable in `Makefile.config` corrected, and `-Werror` applied in the top-level Makefile.
- The action-item generation target in the top-level Makefile fixed.
- `tasks/Makefile` include paths fixed and `CPPFLAGS` clobbering removed.
- Implicit fallthrough warnings fixed, and an error condition macro added to `coroutine.h`.
- Stray whitespace removed from the `set_serial_prog_cmd` invocation.
- Task resume and restart transitions corrected: the event callback now deregisters its handler when the target task is already running.
- Duplicate event handler registration rejected, events cleared when a task is deregistered, and repeated `sleep()` and `wait_on_event()` calls handled.
- Software PWM now clears the global interrupt flag when values change, clears the port only on deregistration, and checks for overflow.
- The watchdog is reset in `alive()` and while handling task timeout.
- A `print_boot_msg()` stub added for builds with the UART disabled, with stub arguments marked unused.
- The ADC application fixed, and the debugger's ADC read now handles interrupts and auto-trigger results.
- LCD DDRAM addressing corrected.
- Application initialisation callbacks fixed.
- Compiler warnings across the kernel addressed.
- Windows line endings converted to Unix.

[@notweerdmonk]: https://github.com/notweerdmonk
