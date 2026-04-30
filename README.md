# How to use:

# Edit TOKEN and TAG_NAME in the Makefile, then:
make backup

# Or pass them on the CLI:
make backup TOKEN=your_do_token TAG_NAME=your_tag

# Daily cron at 2am:
0 2 * * * cd /path/to/ && make backup TOKEN=xxx TAG_NAME=yyy >> /var/log/do-backup.log 2>&1
