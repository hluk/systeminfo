#include "systeminfo.h"

#ifdef LUA
#include <lua.h>
#include <lauxlib.h>
#include <string.h>

lua_State *L;
void luaL_openlibs (lua_State *L);

typedef struct LoadF {
    FILE *f;
    char buff[BUFSIZ];
    int init;
} LoadF;

static const char *getF (lua_State *L, void *ud, size_t *size) {
    LoadF *lf = (LoadF *)ud;
    if (lf->init) {
        lf->init = 0;
        char *init_str =
            "C = function(fmt,x) return string.format('%'..fmt,x) end;"
            "NUM = function(x)   return string.format('%d',x) end;"
            "FLOAT = function(x) return string.format('%.2f',x) end;";
        *size = strlen(init_str);
        return init_str;
    }
    if (feof(lf->f)) return NULL;
    *size = fread(lf->buff, 1, sizeof(lf->buff), lf->f);  /* read block */
    return lf->buff;
}

static void loadlua() {
    LoadF lf;
    int status, readstatus;

    lf.f = fopen(LUA, "r");
    if (lf.f == NULL) {
        perror(LUA);
        exit(1);
    }

    lf.init = 1;
    status = lua_load(L, getF, &lf, lua_tostring(L, -1));
    readstatus = ferror(lf.f);
    fclose(lf.f);  /* close file (even in case of errors) */

    if (readstatus) {
        perror(LUA);
        exit(1);
    }

    if (status) {
        lua_pushstring(L, LUA);
        lua_error(L);
    }
}

static int l_status(lua_State *L)
{
    int i, j, n;

    for ( i=1, n=lua_gettop(L); i<=n; ++i ) {
        if ( lua_isstring(L, i) ) {
            status_puts( lua_tostring(L, i) );
        } else if ( lua_istable(L, i) ) {
            /* unpack table */
            j = 0;
            do {
                lua_pushinteger(L, ++j);
                lua_gettable(L, i);
                lua_insert(L, i+j);
            } while ( !lua_isnil(L, i+j) );
            lua_remove(L, i+j);
            n += j-1;
        }
    }

    return 0;
}

#define SET_LUA_VAR(x,y) \
    lua_push##y(L, x); \
    lua_setglobal(L, #x);

static void status()
{
#ifdef WITH_CPU
    SET_LUA_VAR(CPU, number);
    SET_LUA_VAR(CPU_TOTAL, number);
    SET_LUA_VAR(CPU_ACTIVE, number);
    SET_LUA_VAR(CPU_PERCENT, number);
#endif
#ifdef WITH_UP
    SET_LUA_VAR(UP_MINUTES, number);
    SET_LUA_VAR(UP_HOURS, number);
#endif
#ifdef WITH_LOAD
    SET_LUA_VAR(LOAD_1, number);
    SET_LUA_VAR(LOAD_2, number);
    SET_LUA_VAR(LOAD_3, number);
#endif
#ifdef WITH_ROOT
    SET_LUA_VAR(ROOT_SIZE, number);
    SET_LUA_VAR(ROOT_FREE, number);
    SET_LUA_VAR(ROOT_USED, number);
#endif
#ifdef WITH_MEM
    SET_LUA_VAR(MEM_SIZE, number);
    SET_LUA_VAR(MEM_FREE, number);
    SET_LUA_VAR(MEM_USED, number);
#endif
#ifdef WITH_SWAP
    SET_LUA_VAR(SWAP_SIZE, number);
    SET_LUA_VAR(SWAP_FREE, number);
    SET_LUA_VAR(SWAP_USED, number);
#endif
#ifdef WITH_BATTERY
    SET_LUA_VAR(BATTERY, number);
#endif
#ifdef WITH_TEMPERATURE
    SET_LUA_VAR(TEMPERATURE, number);
#endif
#ifdef WITH_UP
    SET_LUA_VAR(UP_MINUTES, number);
    SET_LUA_VAR(UP_HOURS, number);
#endif
#ifdef WITH_NET
    SET_LUA_VAR(NET_1_NAME, string);
    SET_LUA_VAR(NET_1_UPSPEED, number);
    SET_LUA_VAR(NET_1_DOWNSPEED, number);
#endif
#ifdef WITH_DATE
    SET_LUA_VAR(DATE_YEAR, number);
    SET_LUA_VAR(DATE_MONTH, number);
    SET_LUA_VAR(DATE_MONTHDAY, number);
    SET_LUA_VAR(DATE_WEEKDAY, number);
    SET_LUA_VAR(DATE_YEARDAY, number);
    SET_LUA_VAR(DATE_HOURS, number);
    SET_LUA_VAR(DATE_MINUTES, number);
    SET_LUA_VAR(DATE_SECONDS, number);
#endif

    lua_pushvalue(L, 1);
    if( lua_pcall(L, 0, 0, 0) ) {
        lua_pushstring(L, LUA);
        lua_error(L);
    }
    status_flush();
}
#else /* !LUA */
#define A        status_puts(""
#define B        )
#define IF(x)    B;if(x){A
#define ELIF(x)  B;}else if(x){A
#define ELSE     B;}else{A
#define ENDIF    B;};A
#define WHILE(x) B;while(x){A
#define FOR(x)   B;for(x){A
#define END      B;};A
#define DO(x)    B;(x);A
#define C(t,x)   B;status_printf("%"#t,(x));A
#define NUM(x)   C(d,(int)(x))
#define FLOAT(x) C(.2f,(double)(x))
#define STR(x)   C(s,(char *)(x))
#define CHAR(x)  C(c,(char)(x))

#ifdef XSETROOT
#define BEGIN static void status(){ pname=name;*pname=0;A
#else
#define BEGIN static void status(){A
#endif

#ifdef STATUS
BEGIN
STATUS
#else
#define STATUS BEGIN
#ifdef CONFIG
#include CONFIG
#else
#include "config.h"
#endif
#endif
B;
status_flush();
};
#endif

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
static void status_puts(const char *s)
{
    strcpy(pname, s);
    pname += strlen(s);
}
static void status_flush()
{
    Display *dpy;
    if( !(dpy = XOpenDisplay(0)) ) {
        perror("Cannot open X11 display");
        exit(1);
    }
    *pname=0;
    XStoreName(dpy, RootWindow(dpy, DefaultScreen(dpy)), name);
    XCloseDisplay(dpy);
}
#else
static void status_puts(const char *s)
{
    fputs(s,stdout);
}
#endif

int main()
{
#ifdef LUA
    L = luaL_newstate();
    luaL_openlibs(L);

	loadlua();
    lua_register(L, "status", l_status);
#endif

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
                downspeed = (float)((recv-last_recv)/DELAY)/1024;
                upspeed = (float)((transmit-last_transmit)/DELAY)/1024;
            }
            fclose(f);
        }
#endif

    status();

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

