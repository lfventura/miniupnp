# External Script Support for MiniUPnPd (Linux only)

## Overview

This feature allows you to completely disable miniupnpd's direct manipulation of iptables/nftables and instead have it call an external script to handle firewall operations. This is useful when:

- You have a custom firewall management system
- You want full control over firewall rule implementation
- You need to integrate with third-party firewall solutions
- You want to add custom logging or validation logic
- This customization was made to be able to actually have UPNP on Vyos (Tested on Vyos 1.5Q2)

## Architecture

The external script support is **completely isolated** from the standard miniupnpd codebase:

- **Separate build system**: Uses `Makefile.linux_script` instead of `Makefile.linux_nft`
- **Separate compilation flag**: `USE_EXTERNAL_SCRIPT` isolates all script-related code
- **No nftables dependencies**: Script mode doesn't link against libnftnl or libmnl
- **Easy upstream merging**: Original code remains unchanged, minimizing merge conflicts

## Building and Installation

### Build Options

miniupnpd now supports a new separate firewall backends on Linux:

1. **External script mode** (new): `./configure --firewall=script`

### Prerequisites
- Linux system
- Build tools: gcc, make
- For packaging: dpkg-deb (Debian) or appropriate tools for your distro
- Check build.sh for a real world example on it

### Build Instructions

#### Option 1: External Script Mode (Custom Firewall)

```bash
./configure --firewall=script
make clean
make -j$(nproc)
```

This will:
- Use `Makefile.linux_script`
- Define `USE_EXTERNAL_SCRIPT` 
- Link with `netfilter/extscriptrdr.o`, `netfilter/rdr_desc.o`, `netfilter/noop_rdr.o`
- **NOT** link with nftables libraries

### Installation

```bash
sudo make install
```

Or create a package for your distribution.
Check the draft at build.sh file.

## Configuration

Add the following options to your `miniupnpd.conf`:

```ini
# Enable external script mode (REQUIRED)
use_external_script=yes

# Path to your firewall management script (REQUIRED)
external_script_path=/usr/local/bin/miniupnpd-firewall.sh
```

**Important:** Both options must be present in the configuration file for the external script feature to work. If `use_external_script=yes` is set but `external_script_path` is missing or empty, miniupnpd will log an error and fail to process port mapping requests.

After modifying the configuration, restart the service:
```bash
sudo systemctl restart miniupnpd
```

Verify the configuration was loaded correctly:
```bash
sudo journalctl -u miniupnpd | grep -i "external script"
```

You should see messages like:
```
miniupnpd: external script mode: enabled
miniupnpd: external script path: /usr/local/bin/miniupnpd-firewall.sh
```

## Script Interface

Your script will be called with different operations and arguments:

### Operations

#### 1. add_redirect
Called when a port forwarding rule needs to be added.

**Arguments:**
```
add_redirect <ifname> <rhost> <eport> <iaddr> <iport> <proto> <desc> <timestamp>
```

- `ifname`: External network interface name (e.g., "eth0")
- `rhost`: Remote host IP address or "*" for any host
- `eport`: External port number
- `iaddr`: Internal IP address to forward to
- `iport`: Internal port number
- `proto`: Protocol - "TCP", "UDP", or "UDPLITE"
- `desc`: Description of the port mapping
- `timestamp`: Lease expiration timestamp (0 for permanent mapping)

**Example:**
```bash
add_redirect eth0 * 8080 192.168.1.100 80 TCP "Web Server" 1734480000
```

#### 2. add_filter
Called when a filter rule needs to be added to allow forwarded traffic.

**Arguments:**
```
add_filter <ifname> <rhost> <iaddr> <eport> <iport> <proto> <desc>
```

**Example:**
```bash
add_filter eth0 * 192.168.1.100 8080 80 TCP "Web Server"
```

#### 3. delete_redirect
Called when a port forwarding rule needs to be removed.

**Arguments:**
```
delete_redirect <ifname> <eport> <proto>
```

**Example:**
```bash
delete_redirect eth0 8080 TCP
```

#### 4. delete_filter
Called when a filter rule needs to be removed.

**Arguments:**
```
delete_filter <ifname> <eport> <proto>
```

**Example:**
```bash
delete_filter eth0 8080 TCP
```

#### 5. delete_redirect_and_filter
Called to remove both redirect and filter rules at once (Linux optimization).

**Arguments:**
```
delete_redirect_and_filter <eport> <proto>
```

**Example:**
```bash
delete_redirect_and_filter 8080 TCP
```

## Script Requirements

Your script MUST:

1. **Be executable** (`chmod +x your-script.sh`)
2. **Exit with status 0** on success
3. **Exit with non-zero status** on failure
4. **Handle all five operations** listed above
5. **Be idempotent** where possible (handle cases where rules already exist or don't exist)
6. **Avoid using `logger` command** - The `logger` command may hang when called from systemd context. Use file logging or echo to stdout/stderr instead:
   ```bash
   # Good - log to file
   log() {
       echo "[$(date)] $@" >> /var/log/miniupnpd-script.log
   }
   
   # Bad - may hang under systemd
   log() {
       logger -t miniupnpd "$@"
   }
   ```

## Example Implementation

See `miniupnpd-firewall-script-example.sh` for a complete example script template.

### Basic iptables Example

```bash
#!/bin/bash
set -e

OPERATION="$1"
shift

case "$OPERATION" in
    add_redirect)
        ifname="$1"
        rhost="$2"
        eport="$3"
        iaddr="$4"
        iport="$5"
        proto="$6"
        
        iptables -t nat -A MINIUPNPD -i "$ifname" -p "${proto,,}" --dport "$eport" \
            -j DNAT --to-destination "$iaddr:$iport"
        ;;
        
    add_filter)
        ifname="$1"
        rhost="$2"
        iaddr="$3"
        eport="$4"
        iport="$5"
        proto="$6"
        
        iptables -A MINIUPNPD -i "$ifname" -p "${proto,,}" -d "$iaddr" \
            --dport "$iport" -j ACCEPT
        ;;
        
    delete_redirect_and_filter)
        eport="$1"
        proto="$2"
        
        # Delete NAT rule
        iptables -t nat -D MINIUPNPD -p "${proto,,}" --dport "$eport" -j DNAT 2>/dev/null || true
        
        # Delete filter rule
        iptables -D MINIUPNPD -p "${proto,,}" --dport "$eport" -j ACCEPT 2>/dev/null || true
        ;;
esac

exit 0
```

## Testing

1. Enable the feature in `miniupnpd.conf`
2. Create your script and make it executable
3. Start miniupnpd with your configuration
4. Test by requesting a port mapping from a UPnP client
5. Check system logs for script execution messages:
   ```bash
   journalctl -u miniupnpd -f
   ```

## Security Considerations

- **Validate input** in your script before executing firewall commands
- **Use absolute paths** for firewall commands (iptables, nft, etc.)
- **Run with minimal privileges** if possible
- **Log operations** for audit purposes
- **Handle errors gracefully** and return appropriate exit codes

## Debugging

Enable debug logging in miniupnpd to see when the script is called:

```bash
miniupnpd -d -f /etc/miniupnpd.conf
```

Check syslog for script execution details:
```bash
tail -f /var/log/syslog | grep miniupnpd
```

## Limitations

- This feature is **Linux-only**
- The script is called **synchronously**, so it should execute quickly
- **Query operations use internal tracking**: Port mappings are tracked in memory (rdr_desc_list)
- The interface is **forward-compatible** but may be extended in future versions

## How It Works Internally

When `USE_EXTERNAL_SCRIPT` is defined:

1. **Rule creation**: Calls external script AND stores mapping in internal list (`rdr_desc_list`)
2. **Rule queries**: Returns data from internal list (no script calls)
3. **Rule deletion**: Calls external script AND removes from internal list
4. **Stub functions**: Unused firewall functions in `netfilter/noop_rdr.c` return errors

This hybrid approach allows UPnP queries to work efficiently without calling the script for every read operation.

## Performance

Each port mapping operation requires executing an external process. For high-volume scenarios:

- Use efficient firewall commands
- Consider batching operations in your script
- Minimize external dependencies
- Use compiled tools instead of interpreted scripts if needed

## Example Use Cases

1. **Custom firewall with API**: Script calls REST API of custom firewall appliance
2. **Database logging**: Record all port mapping operations to a database
3. **Advanced validation**: Implement business-specific port mapping policies
4. **Multi-firewall management**: Update multiple firewall systems simultaneously
5. **Cloud integration**: Update cloud provider security groups via API
6. **VyOS integration**: Use VyOS script-template for firewall management. If interested, check the folder ../vyos-script

## Code Structure (For Developers)

### New Files (100% custom)
- `netfilter/extscriptrdr.c/h`: External script execution via posix_spawn()
- `netfilter/rdr_desc.c/h`: Internal tracking list (add/get/delete operations)
- `netfilter/noop_rdr.c`: Stub implementations for unused firewall functions
- `Makefile.linux_script`: Separate build configuration for script mode
- `EXTERNAL_SCRIPT_README.md`: This documentation
- `miniupnpd-firewall-script-example.sh`: Example script template

### Modified Files (with #ifdef USE_EXTERNAL_SCRIPT)
- `upnpredirect.c`: Add/get/delete operations wrapped in conditionals
- `upnpglobalvars.h/c`: Script configuration variables
- `options.h/c`: Script configuration parsing
- `configure`: Added `--firewall=script` option

### Unchanged Files (pristine upstream)
- `Makefile.linux_nft`: Original nftables build (unmodified)
- All other core miniupnpd files

### Internal Tracking Structure

```c
struct rdr_desc {
    unsigned short eport;
    unsigned short iport;
    char iaddr[16];  // IPv4 or IPv6
    int proto;       // IPPROTO_TCP or IPPROTO_UDP
    char desc[64];
    unsigned int timestamp;
    struct rdr_desc *next;
};
```

Operations:
- `add_redirect_desc()`: Called after successful external script execution
- `get_redirect_rule_from_desc_list()`: Query by eport/proto
- `get_redirect_rule_by_index_from_desc_list()`: Query by index (for listing)
- `del_redirect_desc()`: Called after successful deletion

This ensures UPnP clients can query port mappings efficiently without calling the external script.
