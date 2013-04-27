#!/usr/bin/env python
import sys

init_script = '''#!/bin/env sh
### BEGIN INIT INFO
# Provides:          ping monitor
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Should-Start:      udev network-manager
# Should-Stop:       udev network-manager
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start the ping monitor
# Description:       Start script for the ping monitor
### END INIT INFO

DAEMON={daemon}
DAEMONPATH={daemon_path}
PIDDIR=/var/run/ping_monitor
PIDFILE=$PIDDIR/pid
DAEMONUSER={daemon_user}
PATH=/sbin:/bin:/usr/sbin:/usr/bin

test -x $DAEMONPATH || exit 0

. /lib/lsb/init-functions


ping_monitor_start () {
    log_daemon_msg "Starting system ping monitor Daemon"
    if [ ! -d $PIDDIR ]; then
        mkdir -p $PIDDIR
        chown $DAEMONUSER:$DAEMONUSER $PIDDIR
    fi
    start-stop-daemon -x $DAEMONPATH -p $PIDFILE -c $DAEMONUSER --start
    status=$?
    log_end_msg ${status}
}

ping_monitor_stop () {
    log_daemon_msg "Stopping system ping monitor Daemon"
    start-stop-daemon -x $DAEMONPATH -p $PIDFILE --stop --retry 5 || echo -n "...which is not running"
    log_end_msg $?
}

case "$1" in
    start|stop)
        ping_monitor_${1}
        ;;
    restart|reload|force-reload)
        if [ -s $PIDFILE ] && kill -0 $(cat $PIDFILE) >/dev/null 2>&1; then
            ping_monitor_stop
            ping_monitor_start
        fi
        ;;
    force-stop)
        ping_monitor_stop
        killall $DEAMON || true
        sleep 2
        killall -9 $DAEMON || true
        ;;
    status)
        status_of_proc -p $PIDFILE "$DAEMONPATH" "ping monitor" && exit 0 || exit $?
        ;;
    *)
        echo "Usage: {init_file} {start|stop|force-stop|restart|reload|force-reload|status}"
        exit 1
        ;;
esac

exit 0'''

def main(argv):
    if len(argv) > 1:
        install_bin = argv[1]
    else:
        install_bin = '/usr/local/bin'
    
    config = {
        'daemon': 'ping-monitor',
        'daemon_path': install_bin + '/' + daemon,
        'daemon_user': 'arnaud',
        'init_file': '/etc/init.d/ping-monitor'
    }
    
    f = open(init_file, 'w+')
    f.write(init_script.format(**config))
    f.close()

if __name__ == '__main__':
    main(sys.argv[:])

# update-rc.d ping-monitor remove|defaults