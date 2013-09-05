tcpmux
======

The reference implementation for RFC-1078 (TCPMUX) from 1988

TCPMUX is described in RFC-1078 (written some 20 years ago). A reference
implementation by Network Wizards can be found at

ftp://ftp.nw.com/nw/software/tcpmux.c

It is also implemented in DragonFlyBSD’s inetd, NetBSD’s inetd and FreeBSD’s
inetd. OpenBSD does not support for it.

The Protocol

A TCP client connects to a foreign host on TCP port 1. It sends the service
name followed by a carriage-return line-feed . The service name is never case
sensitive. The server replies with a single character indicating positive
(“+”) or negative (“-”) acknowledgment, immediately followed by an optional
message of explanation, terminated with a . If the reply was positive, the
selected protocol begins; otherwise the connection is closed.

The 15+ years I have been a sysadmin I have never seen anyone making a use of
it, which is a pity: Most of the time I see fellow sysadmins who want to write
a custom daemon, either write it as a standalone server (usually starting with
passivesock.c or passiveTCP.c from Comer’s Internetworking with TCP/IP vol.3),
or writing is as a simple stdin/stdout application that is started via inetd.
The most trivial problem is sometimes more than trivial:

- What port will this application run on?

It seems that 65535 ports is a lot of freedom to choose from and most people
want to use “interesting” port numbers (for any definition of interesting).
Add firewall policies and router access lists in the picture, you can have a
non-technical deadlock in no time!

TCPMUX might be a choice to help simplify / avoid such situations. Any service
that supports TCPMUX listens on port 1/tcp and can be forked by inetd(8)
(either internally or externally with the help of a tiny server). After all,
it can be considered as an “inetd inside inetd” (the classic inetd responding
to requests on a port, TCPMUX responding to requests based on the name of the
service) and even if you do not want to use TCPMUX, a similar (homegrown)
solution might be the answer to keeping your packet filters lean and less
complex. It does not have to be less complex than it has to be though. The
Wikipedia article on tcpmux clearly identifies risks that come with deploying
it. Personally, I view tcpmux as an old and simple TCP RPC mechanism.

http://blog.postmaster.gr/2010/12/20/tcpmux-a-mostly-overlooked-tcp-service/
