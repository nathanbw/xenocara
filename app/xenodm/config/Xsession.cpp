XCOMM!SHELL_CMD
XCOMM
XCOMM $OpenBSD: Xsession.cpp,v 1.2 2016/11/11 15:58:35 matthieu Exp $

XCOMM redirect errors to a file in user's home directory if we can

errfile="$HOME/.xsession-errors"
if ( umask 077 && cp /dev/null "$errfile" 2> /dev/null )
then
	exec > "$errfile" 2>&1
else
	for errfile in "${TMPDIR-/tmp}/xses-$USER" "/tmp/xses-$USER"
	do
		if ef="$( umask 077 && mktemp "$errfile.XXXXXX" 2> /dev/null)"
		then
			exec > "$ef" 2>&1
			mv "$ef" "$errfile" 2> /dev/null
			break
		fi
	done
fi

XCOMM if we have private ssh key(s), start ssh-agent and add the key(s)
id1=$HOME/.ssh/identity
id2=$HOME/.ssh/id_dsa
id3=$HOME/.ssh/id_rsa
id4=$HOME/.ssh/id_ecdsa
id5=$HOME/.ssh/id_ed25519
if [ -z "$SSH_AGENT_PID" ];
then
	if [ -x /usr/bin/ssh-agent ] && [ -f $id1 -o -f $id2 -o -f $id3 -o -f $id4 -o -f $id5 ];
	then
		eval `ssh-agent -s`
		ssh-add < /dev/null
	fi
fi

do_exit() {
	if [ "$SSH_AGENT_PID" ]; then
		ssh-add -D < /dev/null
		eval `ssh-agent -s -k`
	fi
	exit
}

case $# in
1)
	case $1 in
	failsafe)
		BINDIR/xterm -geometry 80x24-0-0
		do_exit
		;;
	esac
esac

XCOMM The startup script is not intended to have arguments.

startup=$HOME/.xsession
resources=$HOME/.Xresources

if [ -s "$startup" ]; then
	if [ -x "$startup" ]; then
		"$startup"
	else
		/bin/sh "$startup"
	fi
else
	if [ -f "$resources" ]; then
		BINDIR/xrdb -load "$resources"
	fi
	BINDIR/xterm &
	BINDIR/fvwm
fi
do_exit
