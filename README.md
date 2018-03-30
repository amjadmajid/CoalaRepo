# Coala - OOPSLA '18

Temporary private repository hosting Coala's code.

### Compiling an application

Coala's library and the available applications support the MSP430-GCC compiler. Before compiling an application, edit `maker/envCom.mk` to have the variable `GCC_DIR` point to the path where the compiler is installed. Then,

```bash
cd apps/anyapp
make
```

This also compiles Coala and all the dependencies, if needed.

### Writing an application

Coala's API is provided in `coala/include/coala.h`. This is the only header that has to be included in the application, and it contains the programming interface:
- `COALA_TASK`, used to declare a new task;
- `COALA_PV`, used to declare a new protected variable;
- `coala_next_task`, used to mark the next task to be scheduled after the currently executing one completes;
- `RP`, used to mark a protected variable read;
- `WP`, used to mark a protected variable write;
- `coala_init`, passing the global initialization task to be scheduled upon the first boot as argument;
- `coala_run`, which hands the control to Coala’s scheduler;
- `COALA_SM`, used for typedef'ed structures' members, to ensure proper memory alignment required by Coala’s page handler.