#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_DATE
#include <time.h>
#endif

#if defined(WITH_UP) || defined(WITH_LOAD) || defined(WITH_MEM) || defined(WITH_SWAP)
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
#define CPU ((double)(cpu_active-cpu_last_active)/(cpu_total-cpu_last_total+1))
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
#define SWAP_SIZE ((int)(s.totalswap>>20))
#define SWAP_FREE ((int)(s.freeswap>>20))
#define SWAP_USED ((int)((s.totalswap - s.freeswap)>>20))
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

#if defined(WITH_UP) || defined(WITH_LOAD) || defined(WITH_MEM) || defined(WITH_SWAP)
	static struct sysinfo s;
#endif

#ifdef WITH_ROOT
    static struct statfs fs;
#endif

#ifdef WITH_UP
	static int hours, minutes;
	static long int upminh, uptimes;
#endif

#ifdef WITH_DATE
    static struct tm *t;
    static time_t the_time;
#endif

#ifdef WITH_CPU
    static int cpu_user, cpu_nice, cpu_sys, cpu_idle, cpu_iowait, cpu_irq,
        cpu_softirq, cpu_steal, cpu_total = 0, cpu_active = 0,
        cpu_last_total = 0, cpu_last_active = 0;
#endif

#ifdef WITH_TEMPERATURE
    static int temperature;
#endif

#ifdef WITH_BATTERY
    static int bat_full = 0, bat;
#endif

#ifdef WITH_NET
    static char *eth_name = "", *p = "";
    static float downspeed = 0, upspeed = 0;
    static long long recv, transmit, last_recv, last_transmit;
#endif

static void status_puts(const char *s);

#ifdef XSETROOT
#include <stdarg.h>
#include <string.h>
#include <X11/Xlib.h>
static char name[1024];
static char *pname;
static void status_flush();
#define status_printf(fmt,x) pname += sprintf(pname,fmt,x)
#define fputc(c,_) *(pname++)=c
#define fwrite(x,y,len,z) strncpy(pname,x,len);pname+=len
#else
#define status_printf(fmt,x) printf(fmt,x)
#define status_flush() fflush(stdout)
#endif

#endif

