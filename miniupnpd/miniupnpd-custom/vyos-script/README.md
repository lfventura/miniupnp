# VyOS MiniUPnPd Integration Scripts

## Overview

This directory contains scripts to integrate MiniUPnPd with VyOS firewall management using VyOS's native `script-template` system.

This is not intended to be used by Enterprises or Mission Critical Environments. But enjoy at your Home Lab.

Enhancements are Welcome!

Made with love by dream team (Me + AI)

## Architecture

- **`miniupnp-firewall.sh`**: Root script called by MiniUPnPd (configured via `external_script_path`)
- **`vyos_*.sh`**: VyOS-specific scripts that perform actual firewall operations using `script-template`

## Configuration

### Firewall Name

Make sure `FIREWALL_NAME` is properly set in the scripts with the firewall name where the filter rules should be added.

Edit `miniupnp-firewall.sh`:
```bash
FIREWALL_NAME="your-firewall-name"
```

### MiniUPnPd Configuration

Point MiniUPnPd to the root script in your `miniupnpd.conf`:
```ini
use_external_script=yes
external_script_path=/path/to/miniupnp-firewall.sh
```

## Testing

### Using upnpc Client

List current port mappings:
```bash
upnpc -l
```

Add a port mapping:
```bash
upnpc -a <client_ip> <external_port> <internal_port> <PROTOCOL>
```
Example:
```bash
upnpc -a 192.168.1.220 5000 5001 UDP
```

Delete a port mapping:
```bash
upnpc -d <external_port> <PROTOCOL>
```
Example:
```bash
upnpc -d 5000 UDP
```

### Check VyOS Firewall Rules

View UPnP-created rules in VyOS configuration:
```bash
show configuration commands | grep -i upn
```

### Debug Logs

Enable verbose logging in MiniUPnPd by adding `-v` or `-vv` flags:

**Systemd:**
Edit the service unit or create an override:
```bash
sudo systemctl edit miniupnpd
```
Add:
```ini
[Service]
ExecStart=
ExecStart=/usr/sbin/miniupnpd -v -f /etc/miniupnpd/miniupnpd.conf
```

**Direct execution:**
```bash
miniupnpd -vv -f /etc/miniupnpd/miniupnpd.conf
```

Then monitor logs:
```bash
sudo journalctl -u miniupnpd -f
```

## Script Flow

1. MiniUPnPd receives UPnP request
2. Calls `miniupnp-firewall.sh` with operation and parameters
3. Script delegates to appropriate `vyos_*.sh` script
4. VyOS script uses `script-template` to modify firewall
5. Exit code returned to MiniUPnPd (0 = success)

## VyOS Integration

The `vyos_*.sh` scripts use VyOS's `script-template` command which:
- Maintains VyOS configuration consistency
- Allows proper commit and save operations
- Ensures firewall changes are persistent
- Follows VyOS's configuration management best practices
