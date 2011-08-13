#include "features.h"
#include "systeminfo.h"
#include "config.current.h"

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

#ifdef XSETROOT
static int printfX(const char *fmt, ...)
{
    va_list ap;
    int res;

    va_start(ap, fmt);
    res = vsprintf(pname, fmt, ap);
    pname += res;
    va_end(ap);

    return res;
}

static int fputsX(const char *s, FILE *stream)
{
    int l = strlen(s);
    strncpy(pname, s, l);
    pname += l;

    return 1;
}
#endif

int main()
{
#if defined(WITH_CPU) || defined(WITH_BATTERY) || defined(WITH_NET) || defined(WITH_TEMPERATURE)
    FILE *f;
#endif

#if defined(WITH_BATTERY) || defined(WITH_NET) || defined(WITH_TEMPERATURE)
    char buf[BUFSIZ];
#endif

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

#if DELAY >= 0
    for(;;)
#endif
    {
#if defined(WITH_UP) || defined(WITH_LOAD) || defined(WITH_MEM) || defined(WITH_SWAP)
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
                cpu_active = cpu_user + cpu_nice + cpu_sys +
                        cpu_irq + cpu_softirq + cpu_steal;
                cpu_total = cpu_active + cpu_idle + cpu_iowait;
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

#ifdef XSETROOT
    Display *dpy;
    if( !(dpy = XOpenDisplay(0)) ) {
        perror("Cannot open X11 display!");
        exit(1);
    }
    name[0] = 0;
    pname = name;
#endif

#define A        fputs(
#define B        "",stdout)
#define IF(x)    B;if(x){A
#define ELIF(x)  B;}else if(x){A
#define ELSE     B;}else{A
#define ENDIF    B;};A
#define WHILE(x) B;while(x){A
#define FOR(x)   B;for(x){A
#define END      B;};A
#define DO(x)    B;(x);A
#define C(t,x)   B;printf("%"#t,(x));A
#define NUM(x)   C(d,(int)(x))
#define FLOAT(x) C(.2f,(double)(x))
#define STR(x)   C(s,(char *)(x))
#define CHAR(x)  C(c,(char)(x))
    A STATUS B;
    fflush(stdout);

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

