/*
 * udd unix.c file - unix (or system level) functions
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include "defs.h"

#undef OLDTTY
#ifdef OLDTTY

#include <sgtty.h>
static struct sgttyb out, cur;
#define TTY_GET(X) ioctl(0, TIOCGETP, (X))
#define TTY_SET(X) ioctl(0, TIOCSETP, (X))

#else

#include <termios.h>
static struct termios out, cur;
#define TTY_GET(X) tcgetattr(0, (X))
#define TTY_SET(X) tcsetattr(0, TCSANOW, (X))

#endif


void unix_init(void)
{
  TTY_GET(&out);
  TTY_GET(&cur);
  signal(SIGINT, SIG_IGN);
  /*signal(SIGQUIT, SIG_IGN); /*DEBUG XXX CDC CORE DUMP*/
  signal(SIGHUP, sighup);
#ifdef SIGTSTP
  signal(SIGTSTP, sigstop);
#endif
}

void unix_exit
  (
    int n
  )
{
  TTY_SET(&out);
  exit(n);
}

void sigstop
  (
    int signum
  )
{
  TTY_SET(&out);
  printf("[Suspending...]\r\n");
  kill(getpid(), SIGSTOP);
  printf("[Resuming...]\r\n");
  TTY_SET(&cur);
}

void unix_tty_cook(void)
{
#ifdef OLDTTY
  cur.sg_flags |= ECHO|CRMOD;
  cur.sg_flags &= ~CBREAK;
#else
  cur.c_iflag |= ICRNL;
  cur.c_oflag |= ONLCR;
  cur.c_lflag |= ECHO | ICANON;
#endif
  TTY_SET(&cur);
}

void unix_tty_pswd(void)
{
#ifdef OLDTTY
  cur.sg_flags |= CRMOD;
  cur.sg_flags &= ~ECHO & ~CBREAK;
#else
  cur.c_iflag |= ICRNL;
  cur.c_oflag |= ONLCR;
  cur.c_lflag |= ICANON;
  cur.c_lflag &= ~ECHO;
#endif
  TTY_SET(&cur);
}

void unix_tty_dgn(void)
{
#ifdef OLDTTY
  cur.sg_flags |= CBREAK;
  cur.sg_flags &= ~ECHO & ~CRMOD;
#else
  cur.c_iflag &= ~ICRNL;
  cur.c_oflag &= ~ONLCR;
  cur.c_lflag &= ~(ICANON | ECHO);
#endif
  TTY_SET(&cur);
}


char *month[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep",
                  "Oct","Nov","Dec"};

char *unix_date
  (
    long dat
  )
{
  struct tm *t;
  static char buf[12];
  t = localtime(&dat);
  sprintf(buf, "%02d-%s-%02.2d", t->tm_mday, month[t->tm_mon],
          t->tm_year % 100);
  return(buf);
}

void sighup
  (
    int signum
  )
{
  if (autosave)
    {
#ifdef L_STRICT
      chr_save(NOPE);    /* try and save'em - XXX the old ^Z trick */
#else
      chr_save(YEP);
#endif
    } /*if*/
  unix_exit(0);
}

static bool
    got_debug_mode = false,
    in_debug_mode;

bool debug_mode(void)
  {
    if (!got_debug_mode)
      {
        const char * const str = getenv("UDD_DEBUG");
        in_debug_mode = str != NULL && atoi(str) != 0;
        got_debug_mode = true;
      } /*if*/
    return
        u.c[UC_DEBUGCHR] == 1 || in_debug_mode;
  } /*debug_mode*/
