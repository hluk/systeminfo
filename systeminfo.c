#include "features.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_DATE
#include <time.h>
#endif

#if defined(WITH_UP) || defined(WITH_LOAD) || defined(WITH_MEM)
#include <sys/sysinfo.h>
#endif

#ifdef WITH_ROOT
#include <sys/vfs.h> 
#endif

#define ONEHOUR  3600
#define ONEMINUTE  60
#define LOADS_SCALE 65536.0

#define UP_HOURS (hours)
#define UP_MINUTES (minutes)
#define CPU_TOTAL (cpu_total)
#define CPU_ACTIVE (cpu_active)
#define CPU (((cpu_active-cpu_last_active)<<10)/(cpu_total-cpu_last_total+1))
#define CPU_PERCENT (100*(cpu_active-cpu_last_active)/(cpu_total-cpu_last_total+1))
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
#define TEMPERATURE (temperature/1000)
#define NET_1_NAME (eth_name)
#define NET_1_DOWNSPEED (downspeed)
#define NET_1_UPSPEED (upspeed)
#define NET_DOWNSPEED(x)
#define NET_UPSPEED(x)
#define DATE_YEAR (t->tm_year)
#define DATE_MONTH (t->tm_mon)
#define DATE_MONTHDAY (t->tm_mday)
#define DATE_WEEKDAY (t->tm_wday)
#define DATE_YEARDAY (t->tm_yday)
#define DATE_HOURS (t->tm_hour)
#define DATE_MINUTES (t->tm_min)
#define DATE_SECONDS (t->tm_sec)

#include <stdarg.h>

char strbuf[4096];
int last = 0;
const char *str(const char *fmt, ...)
{
    int i = ++last;
    va_list vl;

    va_start(vl, fmt);
    last += vsprintf(strbuf+last, fmt, vl);
    va_end(vl);

    return strbuf+i;
}

/*#define C(t,x)   ,({int i=++last; last+=sprintf(strbuf+last,"%"#t,(x)); strbuf+i;}),*/
#define C(t,x)   ,str("%"#t, (x)),
#define NUM(x)   C(d,(int)(x))
#define FLOAT(x) C(.2f,(double)(x))
#define STR(x)   C(s,(char *)(x))
#define CHAR(x)  C(c,(char)(x))

#ifndef DELAY
#define DELAY 1
#undef USLEEP
#endif

#if DELAY > 0
#include <unistd.h>
#endif

#ifdef ALARM
#include <signal.h>
#endif

#ifdef ALARM
static void sig_alrm(int sig) {}
#endif

void print(const char **status) {
    const char **p = status-1;
    last = 0;
    while(*++p) fputs(*p, stdout);
    fflush(stdout);
}

int main()
{
#if defined(WITH_UP) || defined(WITH_LOAD) || defined(WITH_MEM)
	struct sysinfo s;
#endif

#ifdef WITH_ROOT
    struct statfs fs;
#endif

#ifdef WITH_UP
	int hours; int minutes;
	long int upminh;
	long int uptimes;
#endif

#ifdef WITH_DATE
    struct tm *t;
    time_t the_time;
#endif

#ifdef WITH_CPU
    int cpu_user, cpu_nice, cpu_sys, cpu_idle, cpu_iowait, cpu_irq, cpu_softirq,
        cpu_steal, cpu_total = 0, cpu_active = 0;
    int cpu_last_total = 0;
    int cpu_last_active = 0;
#endif

#ifdef WITH_TEMPERATURE
    int temperature;
#endif

#if defined(WITH_CPU) || defined(WITH_BATTERY) || defined(WITH_NET) || defined(WITH_TEMPERATURE)
    FILE *f;
#endif

#if defined(WITH_BATTERY) || defined(WITH_NET) || defined(WITH_TEMPERATURE)
    char buf[BUFSIZ];
#endif

#ifdef WITH_BATTERY
    int bat_full = 0;
    int bat;

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
    char *eth_name = "", *p = "";
    float downspeed = 0, upspeed = 0;
    long long recv, transmit, last_recv, last_transmit;

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

#if DELAY >= 0
    for(;;)
#endif
    {
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
                sscanf(buf, "%d", &temperature);
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

        const char *status[] = {"" STATUS "",0};
        print(status);

#ifdef USLEEP
#  ifdef ALARM
        signal(SIGALRM, sig_alrm);
#    if DELAY > 0
        ualarm(DELAY, 0);
#    endif
        pause();
#  elif DELAY > 0
        usleep(DELAY);
#  endif
#else /* !USLEEP */
#  ifdef ALARM
        signal(SIGALRM, sig_alrm);
#    if DELAY > 0
        alarm(DELAY);
#    endif
        pause();
#  elif DELAY > 0
        sleep(DELAY);
#  endif
#endif
    }

	return(0);
}

