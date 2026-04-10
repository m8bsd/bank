# ==============================
# Config
# ==============================

TOKEN := TOKEN HERE
TAG   := TAG NAME HERE

API  := https://api.digitalocean.com/v2
AUTH := -H "Authorization: Bearer $(TOKEN)" -H "Content-Type: application/json"

# ==============================
# Targets
# ==============================

all: cleanup snapshots

# ------------------------------
# List droplets by tag
# ------------------------------
droplets:
	curl -s $(AUTH) "$(API)/droplets?tag_name=$(TAG)" | jq .

# ------------------------------
# List snapshots
# ------------------------------
snapshots-list:
	curl -s $(AUTH) "$(API)/snapshots?resource_type=droplet" | jq .

# ------------------------------
# Delete old snapshots
# FreeBSD: uses date -j -f instead of GNU date -d
# ------------------------------
cleanup:
	@echo "Checking snapshots..."
	@curl -s $(AUTH) "$(API)/snapshots?resource_type=droplet" | \
	jq -c '.snapshots[]' | while read snap; do \
		ID=$$(echo $$snap | jq -r '.id'); \
		NAME=$$(echo $$snap | jq -r '.name'); \
		CREATED=$$(echo $$snap | jq -r '.created_at'); \
		CREATED_TRIM=$$(echo $$CREATED | sed 's/T.*//' | tr -d '-'); \
		DAY=$$(date -j -f "%Y%m%d" "$$CREATED_TRIM" +%a); \
		SNAP_TS=$$(date -j -f "%Y%m%d" "$$CREATED_TRIM" +%s); \
		NOW_TS=$$(date +%s); \
		DIFF=$$(( ($$NOW_TS - $$SNAP_TS) / 86400 )); \
		if [ "$$DAY" != "Fri" ] || { [ "$$DAY" = "Fri" ] && [ $$DIFF -ge 7 ]; }; then \
			echo "Deleting snapshot $$NAME ($$ID) created $$CREATED"; \
			curl -s -X DELETE $(AUTH) "$(API)/snapshots/$$ID" > /dev/null; \
		fi; \
	done

# ------------------------------
# Snapshot all droplets
# ------------------------------
snapshots:
	@echo "Backing up droplets..."
	@curl -s $(AUTH) "$(API)/droplets?tag_name=$(TAG)" | \
	jq -c '.droplets[]' | while read droplet; do \
		ID=$$(echo $$droplet | jq -r '.id'); \
		NAME=$$(echo $$droplet | jq -r '.name'); \
		echo "Creating snapshot for $$NAME ($$ID)"; \
		curl -s -X POST $(AUTH) \
			-d '{"type":"snapshot"}' \
			"$(API)/droplets/$$ID/actions" > /dev/null; \
	done
