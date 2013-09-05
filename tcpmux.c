/*
   TCPMUX.C
   by Mark K. Lottor
   November 1988

  This program implements RFC 1078.
  The program runs under inetd, on TCP port 1 (TCPMUX).
  Don't forget to make the necessary mods to '/etc/services'.
  When a connection is made from a foreign host, the service 
  requested is sent to us, we look it up in the config file,
  and exec the proper server for the service.  This program
  returns a negative reply if the server doesn't exist, otherwise
  the invoked server is expected to return the positive reply (see
  note below).

  The format of the config file is a seperate line for each service,
  or a line starting with "#" for comments.  The service lines are
  the name of the service, some whitspace, and the filename to exec
  for the service.  The program is passed the tcp connection as
  file descriptors 0 and 1.

  If the service name in the config file is immediately preceded
  by a '+', the server will return the positive reply for the
  process; this is for compatability with older server code,
  and also allows you to invoke programs that use stdin/stdout
  without putting any special server code in them.

*/

#include <stdio.h>
#include <ctype.h>

#define CONFIG_FILE "/usr/local/etc/tcpmux.cf"
#define MAX_LINE 120
#define MAXNAMLEN 64

main()
{
  FILE *cfd;
  char service[MAX_LINE];
  char linbuf[MAX_LINE];
  char sname[64], sfile[MAXNAMLEN];
  char *p;

/*
 inetd passes us the tcp connection as fd[0].
*/

  dup2(0,1);      /* make tcp connection be stdin and stdout */

/* get service name requested */
  if (fgets(service, MAX_LINE, stdin) == NULL)
  {
    puts("-Error reading service name.\r");
    fflush(stdout);
    exit(0);
  }
/* kill trailing newline */
  p = service;
  while ((*p != '\0') && (*p != '\r') && (*p != '\n')) p++;
  *p = '\0';   

/* open config file */
  if (!(cfd = fopen(CONFIG_FILE,"r")))
  {
    puts("-No services available\r");
    fflush(stdout);
    exit(0);
  }

/* help is a required command, and lists available services one per line */
  if (!strCMP(service,"help"))
  {
    while (fgets(linbuf, MAX_LINE, cfd) != NULL)
    {
      if (*linbuf != '#')              /* if its not a comment */
        if (sscanf(linbuf,"%s %s",sname,sfile) == 2)
        {
          p = sname; if (*p == '+') p++;
          printf("%s\r\n",p);          /* then display service name */
        }
    }
    fflush(stdout);
    fclose(cfd);
    exit(0);
  }

/* try matching a line of config file with service requested */
  while (fgets(linbuf, MAX_LINE, cfd) != NULL)
  {
    if (*linbuf != '#')              /* if its not a comment */
      if (sscanf(linbuf,"%s %s",sname,sfile) == 2)
      {
        p = sname; if (*p == '+') p++;
        if (!strCMP(service,p))
        {
          fclose(cfd);
          if (*sname == '+') puts("+Go\r"); fflush(stdout);
          execl(sfile,p,0)    ;      /* found it -- so start it */
        }
      }
  }
  puts("-Service not available\r");
  fflush(stdout);
  exit(0);
}

/* a proper string compare */
strCMP(s1,s2)
  char *s1, *s2;
{
  register int c1, c2;

  for (;;) {
    c1 = (isupper(*s1) ? tolower(*s1) : *s1);
    c2 = (isupper(*s2) ? tolower(*s2) : *s2);
    if (c1 != c2)
      return c1 - c2;
    if (c1 == '\0')
      return 0;
    s1++;
    s2++;
  }
}
