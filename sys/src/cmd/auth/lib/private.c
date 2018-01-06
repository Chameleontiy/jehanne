/*
 * This file is part of the UCB release of Plan 9. It is subject to the license
 * terms in the LICENSE file found in the top-level directory of this
 * distribution and at http://akaros.cs.berkeley.edu/files/Plan9License. No
 * part of the UCB release of Plan 9, including this file, may be copied,
 * modified, propagated, or distributed except according to the terms contained
 * in the LICENSE file.
 */
/* Portions of this file are Copyright (C) 2015-2018 Giacomo Tesio <giacomo@tesio.it>
 * See /doc/license/gpl-2.0.txt for details about the licensing.
 */
/* Portions of this file are Copyright (C) 9front's team.
 * See /doc/license/9front-mit for details about the licensing.
 * See http://code.9front.org/hg/plan9front/ for a list of authors.
 */
#include <u.h>
#include <lib9.h>
#include <bio.h>
#include <authsrv.h>
#include "authcmdlib.h"

static char *pmsg = "Warning! %s can't protect itself from debugging: %r\n";
static char *smsg = "Warning! %s can't turn off swapping: %r\n";

/* don't allow other processes to debug us and steal keys */
void
private(void)
{
	int fd;
	char buf[64];

	snprint(buf, sizeof(buf), "#p/%d/ctl", getpid());
	fd = open(buf, OWRITE);
	if(fd < 0){
		fprint(2, pmsg, argv0);
		return;
	}
	if(fprint(fd, "private") < 0)
		fprint(2, pmsg, argv0);
	if(fprint(fd, "noswap") < 0)
		fprint(2, smsg, argv0);
	close(fd);
}
