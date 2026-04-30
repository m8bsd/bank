# DigitalOcean Droplet Snapshot Backup
# Converted from PHP to Makefile by Claude
# Original by: Abdulmogeeb AlHumaid - abdulmogeeb@gmail.com
# Licence: nothing, just use it and enjoy it.
#
# Usage:
#   make backup          - Run full backup (prune old snapshots + snapshot all tagged droplets)
#   make list-droplets   - List all droplets with the configured tag
#   make list-snapshots  - List all droplet snapshots
#   make prune           - Delete old snapshots (non-Friday older than 1 day, Friday older than 7 days)
#   make snap            - Snapshot all tagged droplets
#
# Configuration:
#   Edit TOKEN and TAG_NAME below, or override on the CLI:
#   make backup TOKEN=your_token TAG_NAME=your_tag

TOKEN    ?= TOKEN_HERE
TAG_NAME ?= TAG_NAME_HERE

DO_API   := https://api.digitalocean.com/v2
CURL     := curl --silent --fail
AUTH     := Authorization: Bearer $(TOKEN)
JSON     := Content-Type: application/json

.PHONY: backup list-droplets list-snapshots prune snap help

## Default target: full backup cycle
backup: prune snap

## List all droplets with TAG_NAME
list-droplets:
	@echo "==> Listing droplets tagged '$(TAG_NAME)'..."
	@$(CURL) -H "$(AUTH)" -H "$(JSON)" \
		"$(DO_API)/droplets?tag_name=$(TAG_NAME)" | \
		python3 -c "
import sys, json
data = json.load(sys.stdin)
droplets = data.get('droplets', [])
if not droplets:
    print('No droplets found.')
for d in droplets:
    print(f\"  id={d['id']}  name={d['name']}  status={d['status']}\")
"

## List all droplet snapshots
list-snapshots:
	@echo "==> Listing all droplet snapshots..."
	@$(CURL) -H "$(AUTH)" -H "$(JSON)" \
		"$(DO_API)/snapshots?resource_type=droplet" | \
		python3 -c "
import sys, json
data = json.load(sys.stdin)
snaps = data.get('snapshots', [])
if not snaps:
    print('No snapshots found.')
for s in snaps:
    print(f\"  id={s['id']}  name={s['name']}  created={s['created_at']}\")
"

## Prune old snapshots:
##   - Non-Friday snapshots: deleted if >= 1 day old
##   - Friday snapshots:     deleted if >= 7 days old
prune:
	@echo "==> Pruning old snapshots..."
	@$(CURL) -H "$(AUTH)" -H "$(JSON)" \
		"$(DO_API)/snapshots?resource_type=droplet" | \
		python3 -c "
import sys, json
from datetime import datetime, timezone

TOKEN    = '$(TOKEN)'
DO_API   = '$(DO_API)'
data     = json.load(sys.stdin)
snaps    = data.get('snapshots', [])
today    = datetime.now(timezone.utc)

import urllib.request

for s in snaps:
    created   = datetime.fromisoformat(s['created_at'].replace('Z', '+00:00'))
    day_name  = created.strftime('%a')   # e.g. 'Fri'
    age_days  = (today - created).days

    is_friday = (day_name == 'Fri')
    should_delete = (not is_friday) or (is_friday and age_days >= 7)

    if should_delete:
        print(f\"Deleting snapshot: name={s['name']}  id={s['id']}  created={s['created_at']}  age={age_days}d  day={day_name}\")
        req = urllib.request.Request(
            f\"{DO_API}/snapshots/{s['id']}\",
            method='DELETE',
            headers={'Authorization': f'Bearer {TOKEN}', 'Content-Type': 'application/json'}
        )
        try:
            urllib.request.urlopen(req)
            print(f\"  -> Deleted.\")
        except Exception as e:
            print(f\"  -> ERROR deleting: {e}\")
    else:
        print(f\"Keeping  snapshot: name={s['name']}  day={day_name}  age={age_days}d\")
"

## Snapshot all droplets with TAG_NAME
snap:
	@echo "==> Snapshotting all droplets tagged '$(TAG_NAME)'..."
	@$(CURL) -H "$(AUTH)" -H "$(JSON)" \
		"$(DO_API)/droplets?tag_name=$(TAG_NAME)" | \
		python3 -c "
import sys, json, urllib.request

TOKEN   = '$(TOKEN)'
DO_API  = '$(DO_API)'
data    = json.load(sys.stdin)
droplets = data.get('droplets', [])

if not droplets:
    print('No droplets found with the given tag.')
    sys.exit(0)

for d in droplets:
    print(f\"Snapshotting droplet: {d['name']} (id={d['id']})\")
    payload = json.dumps({'type': 'snapshot'}).encode()
    req = urllib.request.Request(
        f\"{DO_API}/droplets/{d['id']}/actions\",
        data=payload,
        method='POST',
        headers={'Authorization': f'Bearer {TOKEN}', 'Content-Type': 'application/json'}
    )
    try:
        with urllib.request.urlopen(req) as resp:
            result = json.load(resp)
            action = result.get('action', {})
            print(f\"  -> Action id={action.get('id')}  status={action.get('status')}\")
    except Exception as e:
        print(f\"  -> ERROR: {e}\")
"

## Show this help
help:
	@echo ""
	@echo "DigitalOcean Droplet Snapshot Backup — Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make backup          Full cycle: prune old snapshots, then snapshot all tagged droplets"
	@echo "  make prune           Delete old snapshots (non-Fri after 1d, Fri after 7d)"
	@echo "  make snap            Snapshot all droplets matching TAG_NAME"
	@echo "  make list-droplets   List droplets with TAG_NAME"
	@echo "  make list-snapshots  List all droplet snapshots"
	@echo ""
	@echo "Config (set in Makefile or pass on CLI):"
	@echo "  TOKEN    = $(TOKEN)"
	@echo "  TAG_NAME = $(TAG_NAME)"
	@echo ""
	@echo "Example crontab (daily at 2am):"
	@echo "  0 2 * * * cd /path/to/Makefile && make backup TOKEN=xxx TAG_NAME=yyy >> /var/log/do-backup.log 2>&1"
	@echo ""
