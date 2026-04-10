# droplets — DigitalOcean Snapshot Automation (FreeBSD)

A **Makefile-based** tool for automating DigitalOcean droplet snapshots using `curl` and `jq`. This is a FreeBSD-compatible port of [m8bsd/droplets](https://github.com/m8bsd/droplets).

---

## Requirements

- `curl`
- `jq`
- FreeBSD `date` (built-in — no GNU coreutils needed)

Install dependencies:

```sh
pkg install curl jq
```

---

## Configuration

Edit the top of the `Makefile`:

```makefile
TOKEN := YOUR_DIGITALOCEAN_API_TOKEN
TAG   := YOUR_DROPLET_TAG
```

- **TOKEN** — Your DigitalOcean personal access token
- **TAG** — Tag used to filter which droplets to snapshot

---

## Usage

### Run everything (cleanup + snapshot):

```sh
make
```

### Individual targets:

| Command | Description |
|---|---|
| `make droplets` | List droplets filtered by tag |
| `make snapshots-list` | List all existing droplet snapshots |
| `make cleanup` | Delete old snapshots based on retention rules |
| `make snapshots` | Create a new snapshot for each tagged droplet |

---

## Retention Logic

Snapshots are deleted if **either** condition is true:

- The snapshot was **not created on a Friday**, OR
- The snapshot was created on a Friday but is **7 or more days old**

This keeps weekly (Friday) snapshots for one week and discards all others.

---

## Automation (cron)

To run daily at 2 AM, add to your crontab (`crontab -e`):

```
0 2 * * * make -C /path/to/droplets
```

---

## FreeBSD Compatibility Notes

The original project targets Linux and uses GNU `date -d` for date parsing. This port replaces that with FreeBSD's native `date -j -f` syntax:

| | Linux (original) | FreeBSD (this port) |
|---|---|---|
| Parse date string | `date -d "$DATE" +%s` | `date -j -f "%Y%m%d" "$DATE" +%s` |
| Get weekday | `date -d "$DATE" +%a` | `date -j -f "%Y%m%d" "$DATE" +%a` |

No additional packages are required — FreeBSD's built-in `date` handles everything.

---

## API Token Permissions

Your DigitalOcean token must have:

- Read droplets
- Read snapshots
- Create snapshots
- Delete snapshots

---

## License

MIT — based on [m8bsd/droplets](https://github.com/m8bsd/droplets).
