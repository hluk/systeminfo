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

User adjustable C macros in `config.h` file:

* `STATUS`

    Format of output string.

* `DELAY`

    Status is printed every `DELAY` seconds.
    If `DELAY` is -1 then application prints status and exits (network upload and download speed in this case are alway zero).

* `USLEEP`

    If `USLEEP` is 1 then `DELAY` is in microseconds.

* `ALARM

    If `ALARM` is set then status is updated every time the application recieves SIGALRM signal.

Examples
--------
* CPU and memory usage (prints single line)

        make STATUS='"cpu: "NUM(CPU_PERCENT)" %   memory: "NUM(MEM_USED)" MiB\r"' rebuild
        ./systeminfo

* Network statistics (uses `.2f` to print floating point number with two decimal places -- same as printf format string `%.2f`)

        make STATUS='STR(NET_1_NAME)": download "C(.2f,NET_1_DOWNSPEED)" upload "C(.2f,NET_1_UPSPEED)"\n"' rebuild
        ./systeminfo

* Simple counter (increased every millisecond)

        make STATUS='C(8d,{static i;i++;})" ms\r"' DELAY=1000 USLEEP=1 rebuild
        ./systeminfo

