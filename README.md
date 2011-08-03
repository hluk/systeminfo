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
Configuration can be changed by editing `config.mk` file and setting Makefile variables. See default configuration in `config_default.mk` file.

After changing configuration the application needs to be rebuilt with `` `make rebuild` `` command.

Makefile variables:

* `STATUS`

    Format of output string.

* `DELAY`

    Print status after DELAY seconds.

Examples
--------
* CPU and memory usage (prints single line)

        make STATUS='cpu: $(CPU_PERCENT) %   memory: $(MEM_USED) MiB\r' rebuild
        ./systeminfo

* Network statistics (uses `$(call XXX,.2)` to print floating point number with two decimal places -- same as printf format string `%.2f`)

        make STATUS='$(ETH1_NAME): download $(call ETH1_DOWNSPEED,.2) upload $(call ETH1_UPSPEED,.2)\n' rebuild
        ./systeminfo

