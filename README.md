systeminfo
==========
Application `systeminfo` is simple console system monitoring tool.

Basic invocation
----------------
To build the application and run it run following commands.

    make
    ./systeminfo

Configuration
-------------
Configuration can be changed by editing `config.h` file and setting Makefile variables. See default configuration in `config.def.h` file.

After changing configuration the application needs to be rebuilt with `` `make rebuild` `` command.

User adjustable C macros (in `config.h` file):

* `STATUS`

    Format of output string. If this macro is used in `config.h` everything after it is considered status string (see `config.def.h`).

* `DELAY`

    Status is printed every `DELAY` seconds.
    If `DELAY` is -1 then application prints status and exits (network upload and download speed in this case are alway zero).

* `USLEEP`

    If `USLEEP` is 1 then `DELAY` is in microseconds.

* `ALARM`

    If `ALARM` is set then status is updated every time the application recieves `SIGALRM` signal.

Makefile variables:

* `CONFIG`

    Filename of header file with configuration.

* `OUTFILE`

    Output binary filename.

* `XSETROOT`

    Instead of printing status on standard output, set name of root window in X11 (this sets status bar massage in some X11 window managers).

* `LUA`

    Path to configuration file in Lua programming language (see `systeminforc.lua` file). This file is reloaded every time the application is executed.

* `FEATURES`

    List of features (default is all features, e.g. `FEATURES="BATTERY CPU DATE LOAD MEM NET ROOT SWAP TEMPERATURE UP"`).

Examples
--------
* CPU and memory usage (prints single line)

        make FEATURES="CPU MEM" STATUS='"cpu: "NUM(CPU_PERCENT)" %   memory: "NUM(MEM_USED)" MiB        \r"' rebuild
        ./systeminfo

* Network statistics (uses `.2f` to print floating point number with two decimal places -- same as printf format string `%.2f`)

        make FEATURES=NET STATUS='STR(NET_1_NAME)": download "C(.2f,NET_1_DOWNSPEED)" upload "C(.2f,NET_1_UPSPEED)"\n"' rebuild
        ./systeminfo

* Simple counter (increased every millisecond)

        make FEATURES= STATUS='C(8d,{static i;i++;})" ms\r"' DELAY=1000 USLEEP=1 rebuild
        ./systeminfo

* Use Lua configuration file

        make LUA="$PWD/systeminforc.lua" rebuild
        ./systeminfo

