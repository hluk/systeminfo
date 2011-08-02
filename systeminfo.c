#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/kernel.h>   
#include <sys/sysinfo.h>
#include <sys/vfs.h> 

#define ONEHOUR  3600
#define ONEMINUTE  60
#define LOADS_SCALE 65536.0

#ifndef DELAY
#define DELAY 1
#endif

int main()
{
	struct sysinfo s;
    struct statfs fs;

	int hours; int minutes;
	long int upminh;
	long int uptimes;

    struct tm *t;
    time_t the_time;

	FILE *f;
    int cpu_user, cpu_nice, cpu_sys, cpu_idle, cpu_iowait, cpu_irq, cpu_softirq,
        cpu_steal, cpu_total, cpu_active, cpu = 0;
    int cpu_last_total = 0;
    int cpu_last_active = 0;

    char buf[BUFSIZ];
    char *eth_name = "", *p = "";

    float downspeed = 0, upspeed = 0;
    long long recv, transmit, last_recv = 0, last_transmit = 0;

    int bat_full = 0;
    int bat;

    int temp;

#ifdef PRINT_BATTERY
    f = fopen("/proc/acpi/battery/BAT0/info", "r");
    if (f) {
        if (fgets(buf, BUFSIZ, f) &&
            fgets(buf, BUFSIZ, f) &&
            fgets(buf, BUFSIZ, f))
        {
            sscanf(buf, "last full capacity:      %d", &bat_full);
        }
        fclose(f);
    }
    if (!bat_full) {
        perror("Cannot read battery info");
        exit(1);
    }
#endif

#ifdef PRINT_NETWORK
    f = fopen("/proc/net/dev", "r");
    if (f) {
        if (fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f))
        {
            p = buf;
            while (*p && *p != ':') {
                p++;
            }
            p++;

            sscanf(p, "%lld  %*d     %*d  %*d  %*d  %*d   %*d        %*d       %lld",
                    &last_recv, &last_transmit);
        }
        fclose(f);
    }
#endif

    for(;;) {
#if defined(PRINT_UPTIME) || defined(PRINT_LOAD) || defined(PRINT_MEMORY)
        sysinfo(&s);
#endif

#ifdef PRINT_UPTIME
        uptimes = s.uptime;
        hours = uptimes / ONEHOUR;
        upminh = uptimes - hours * ONEHOUR;
        minutes = upminh / ONEMINUTE;
#endif

#ifdef PRINT_TIME
        time(&the_time);
        t = localtime(&the_time);
#endif

#ifdef PRINT_DISK
        statfs("/", &fs);
#endif

#ifdef PRINT_CPU
        f = fopen("/proc/stat", "r");
        if (f) {
            if ( fscanf(f, "cpu  %d %d %d %d %d %d %d %d",
                        &cpu_user, &cpu_nice, &cpu_sys, &cpu_idle, &cpu_iowait,
                        &cpu_irq, &cpu_softirq, &cpu_steal ) == 8 ) {
                cpu_total = cpu_user + cpu_nice + cpu_sys + cpu_idle + cpu_iowait +
                        cpu_irq + cpu_softirq + cpu_steal;
                cpu_active = cpu_total - (cpu_idle + cpu_iowait);
                cpu = 100*(cpu_active - cpu_last_active)/(cpu_total-cpu_last_total); 
                cpu_last_active = cpu_active;
                cpu_last_total = cpu_total;
            }
            fclose(f);
        }
#endif

#ifdef PRINT_BATTERY
        f = fopen("/proc/acpi/battery/BAT0/state", "r");
        if (f) {
            if (fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f))
            {
                sscanf(buf, "remaining capacity:      %d", &bat);
            }
            fclose(f);
        }
#endif

#ifdef PRINT_TEMPERATURE
        f = fopen("/sys/class/hwmon/hwmon0/temp1_input", "r");
        if (f) {
            if ( fgets(buf, BUFSIZ, f) ) {
                sscanf(buf, "%d", &temp);
            }
            fclose(f);
        }
#endif

#ifdef PRINT_NETWORK
        f = fopen("/proc/net/dev", "r");
        if (f) {
            if (fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f) &&
                fgets(buf, BUFSIZ, f))
            {
                p = buf;
                while (*p == ' ') {
                    p++;
                }

                eth_name = p;
                while (*p && *p != ':') {
                    p++;
                }
                *p = '\0';
                p++;

                sscanf(p, "%lld  %*d     %*d  %*d  %*d  %*d   %*d        %*d       %lld",
                        &recv, &transmit);
                downspeed = (float)((recv-last_recv)/DELAY)/1024;
                upspeed = (float)((transmit-last_transmit)/DELAY)/1024;
                last_recv = recv;
                last_transmit = transmit;
            }
            fclose(f);
        }
#endif

        STATUS;
        fflush(stdout);

        sleep(DELAY);
    }

	return(0);
}

