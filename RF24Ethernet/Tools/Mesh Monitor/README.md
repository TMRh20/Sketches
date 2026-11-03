# mesh_monitor.py

A small, dependency-free web dashboard that shows a live radial map of an
**RF24Mesh** network, built from the master node's `dhcplist.txt` address list.

Run it on the machine hosting the mesh master (e.g. a Raspberry Pi), open the
page in a browser, and watch nodes join, move, and release addresses in real time.

## Features

- **Radial tree view.** The master (`00`) sits in the centre and each ring is one more hop
  out. Lines connect every node to its parent.
- **Colour by hop depth:** green for 1 hop, yellow for 2, cyan for 3, pink for 4 and red for 5.
- **Live updates.** The page polls every 2 seconds and only redraws when the node list
  actually changes. A brief flash marks each redraw.
- **Side panel:**
  - Nodes per hop count
  - Active node table (ID, octal address, hops), sorted by depth
  - Released node IDs (entries whose address is `0`)
- **Problem highlighting:**
  - **Duplicate addresses.** When two node IDs hold the same address, the node gets a red
    outline and its rows in the table turn red.
  - **Placeholder parents.** When a node's parent address isn't assigned to any node, a
    dashed "ghost" circle is drawn so the tree still connects.
- Hover any node for a tooltip with its ID, address and hop count.
- Follows the system light/dark theme.

## Requirements

- Python 3.7+ (standard library only: `http.server`, `struct`, `json`, `argparse`)
- Read access to the RF24Mesh `dhcplist.txt` file written by the mesh master

## Usage

```bash
python3 mesh_monitor.py [--file PATH] [--port PORT] [--host HOST]
```

| Option   | Default            | Description                          |
|----------|--------------------|--------------------------------------|
| `--file` | `~/dhcplist.txt`   | Path to the RF24Mesh address list    |
| `--port` | `8080`             | HTTP port to listen on               |
| `--host` | `0.0.0.0`          | Interface to bind (all by default)   |

Example:

```bash
python3 mesh_monitor.py --file /home/pi/rf24/dhcplist.txt --port 8080
```

Then browse to `http://<pi-ip>:8080/`.

To limit access to the local machine only, use `--host 127.0.0.1`.

### HTTP endpoints

| Path                   | Returns                                                    |
|------------------------|------------------------------------------------------------|
| `/` or `/index.html`   | The dashboard page                                         |
| `/data.json`           | `{"nodes":[{"id":N,"addr":A},...], "mtime":..., "size":...}` |

`/data.json` returns HTTP 500 with an `error` field if the file can't be read (for
example, before the master has created it). The page shows the error in its status line
and keeps polling.

## How it works

### File format

`dhcplist.txt` is a binary file of repeated 4-byte entries:

| Bytes | Type     | Meaning                          |
|-------|----------|----------------------------------|
| 0     | `uint8`  | Node ID                          |
| 1     | `uint8`  | Padding                          |
| 2–3   | `uint16` | RF24Network address (little-endian) |

Trailing bytes that don't make up a complete entry are ignored. An address of `0` means
the node ID has been released.

### Address → hop depth

RF24Network addresses are octal, and **each octal digit is one hop** from the master:

| Address (octal) | Hops | Parent |
|-----------------|------|--------|
| `00`            | 0    | n/a (master) |
| `01`–`05`       | 1    | `00`   |
| `015`           | 2    | `05`   |
| `0215`          | 3    | `015`  |

The parent is found by dropping the most significant octal digit. Each node's angular
slice of the chart is proportional to the number of leaves below it, so busy branches
get more room.

## Running as a service (optional)

`/etc/systemd/system/mesh-monitor.service`:

```ini
[Unit]
Description=RF24Mesh web monitor
After=network.target

[Service]
User=pi
ExecStart=/usr/bin/python3 /home/pi/mesh_monitor.py --file /home/pi/dhcplist.txt
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now mesh-monitor
```

Adjust `User` and the paths to match your setup.

## Notes

- The server has no authentication and binds to all interfaces by default. Keep it on a
  trusted network or bind it to `127.0.0.1`.
- It only reads the file and never writes to it or talks to the radio.
- Request logging is turned off to keep the console quiet.
