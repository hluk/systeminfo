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

#define UP_HOURS (hours)
#define UP_MINUTES (minutes)
#define CPU_TOTAL (cpu_total)
#define CPU_ACTIVE (cpu_active)
#define CPU (((cpu_active-cpu_last_active)<<10)/(cpu_total-cpu_last_total))
#define CPU_PERCENT (100*(cpu_active-cpu_last_active)/(cpu_total-cpu_last_total))
#define LOAD_1 (s.loads[0]/LOADS_SCALE)
#define LOAD_2 (s.loads[1]/LOADS_SCALE)
#define LOAD_3 (s.loads[2]/LOADS_SCALE)
#define ROOT_SIZE ((int)((fs.f_blocks*fs.f_bsize) >> 30))
#define ROOT_FREE ((int)((fs.f_bavail*fs.f_bsize) >> 30))
#define ROOT_USED ((int)(((fs.f_blocks-fs.f_bavail)*fs.f_bsize) >> 30))
#define MEM_SIZE ((int)(s.totalram>>20))
#define MEM_FREE ((int)(s.freeram>>20))
#define MEM_USED ((int)((s.totalram - s.freeram)>>20))
#define BATTERY (100*bat/bat_full)
#define TEMPERATURE (temp/1000)
#define NET_1_NAME (eth_name)
#define NET_1_DOWNSPEED (downspeed)
#define NET_1_UPSPEED (upspeed)
#define DATE_YEAR (t->tm_year)
#define DATE_MONTH (t->tm_mon)
#define DATE_MONTHDAY (t->tm_mday)
#define DATE_WEEKDAY (t->tm_wday)
#define DATE_YEARDAY (t->tm_yday)
#define DATE_HOURS (t->tm_hour)
#define DATE_MINUTES (t->tm_min)
#define DATE_SECONDS (t->tm_sec)

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
        cpu_steal, cpu_total, cpu_active;
    int cpu_last_total;
    int cpu_last_active;

    char buf[BUFSIZ];
    char *eth_name = "", *p = "";

    float downspeed = 0, upspeed = 0;
    long long recv, transmit, last_recv, last_transmit;

    int bat_full = 0;
    int bat;

    int temp;

#ifdef WITH_BATTERY
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

#ifdef WITH_NET
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
                    &recv, &transmit);
        }
        fclose(f);
    }
#endif

    for(;;) {
#if defined(WITH_UP) || defined(WITH_LOAD) || defined(WITH_MEM)
        sysinfo(&s);
#endif

#ifdef WITH_UP
        uptimes = s.uptime;
        hours = uptimes / ONEHOUR;
        upminh = uptimes - hours * ONEHOUR;
        minutes = upminh / ONEMINUTE;
#endif

#ifdef WITH_DATE
        time(&the_time);
        t = localtime(&the_time);
#endif

#ifdef WITH_ROOT
        statfs("/", &fs);
#endif

#ifdef WITH_CPU
        f = fopen("/proc/stat", "r");
        if (f) {
            if ( fscanf(f, "cpu  %d %d %d %d %d %d %d %d",
                        &cpu_user, &cpu_nice, &cpu_sys, &cpu_idle, &cpu_iowait,
                        &cpu_irq, &cpu_softirq, &cpu_steal ) == 8 ) {
                cpu_last_total = cpu_total;
                cpu_last_active = cpu_active;
                cpu_total = cpu_user + cpu_nice + cpu_sys + cpu_idle + cpu_iowait +
                        cpu_irq + cpu_softirq + cpu_steal;
                cpu_active = cpu_total - (cpu_idle + cpu_iowait);
            }
            fclose(f);
        }
#endif

#ifdef WITH_BATTERY
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

#ifdef WITH_TEMPERATURE
        f = fopen("/sys/class/hwmon/hwmon0/temp1_input", "r");
        if (f) {
            if ( fgets(buf, BUFSIZ, f) ) {
                sscanf(buf, "%d", &temp);
            }
            fclose(f);
        }
#endif

#ifdef WITH_NET
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

                last_recv = recv;
                last_transmit = transmit;
                sscanf(p, "%lld  %*d     %*d  %*d  %*d  %*d   %*d        %*d       %lld",
                        &recv, &transmit);
#ifdef WITH_NET_1_DOWNSPEED
                downspeed = (float)((recv-last_recv)/DELAY)/1024;
#endif
#ifdef WITH_NET_1_UPSPEED
                upspeed = (float)((transmit-last_transmit)/DELAY)/1024;
#endif
            }
            fclose(f);
        }
#endif

#define IGNORE(x)
        printf(PRINT_STR, PRINT_ARGS);
        fflush(stdout);

        sleep(DELAY);
    }

	return(0);
}

