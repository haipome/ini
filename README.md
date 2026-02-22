# ini

A simple, lightweight, read-only INI configuration file parser written in C.

## Features

- **No size limits** — section names, property names, and values can be of any length (bounded only by available memory)
- **Global section** — properties declared before any section header are placed in an implicit `global` section
- **Duplicate sections merged** — if a section name appears more than once, subsequent declarations are merged into the first
- **Duplicate keys overwritten** — if a property name appears more than once within the same section, the later value overwrites the earlier one
- **Whitespace trimmed** — leading and trailing whitespace around section names, property names, and values is ignored; internal whitespace is preserved
- **Line continuation** — a backslash (`\`) at the end of a line joins it with the next line
- **Comments** — lines beginning with `#` or `;` are treated as comments and ignored
- **Blank lines ignored**
- **Rich type support** — read values as strings, integers (signed/unsigned, 8/16/32/64-bit), floats, doubles, booleans, and IPv4 addresses

## INI File Format

```ini
; This is a comment
# This is also a comment

; Properties before any section go into the implicit "global" section
proc_name = my_server

[section_name]
key = value
another_key = another value

; Long values can be split across multiple lines with a trailing backslash
description = This is a very long \
              value that spans \
              multiple lines

[database]
host = 127.0.0.1
port = 5432

; Sections can be reopened and new properties are merged in
[database]
max_conn = 100
```

## Building

The library consists of two files: `ini.h` and `ini.c`. Simply add them to your project and compile together with your source files.

```sh
gcc -o my_app my_app.c ini.c
```

To enable debug output (prints the parsed INI tree to stdout), compile with `-DDEBUG`:

```sh
gcc -DDEBUG -o my_app my_app.c ini.c
```

## API Reference

### Return Values

All `ini_read_*` functions share the same return value convention:

| Return | Meaning |
|--------|---------|
| `0`    | Key found; `*value` is set to the parsed value |
| `1`    | Key not found; `*value` is set to `default_value` |
| `-1`   | Error (invalid arguments or memory allocation failure) |

---

### Load and Free

```c
ini_t *ini_load(char *path);
```

Parses the INI file at `path` and returns a handle used by all `ini_read_*` functions. Returns `NULL` on failure (e.g., file not found).

```c
void ini_free(ini_t *handler);
```

Releases all memory associated with the INI handle. Always call this when the handle is no longer needed.

---

### Reading Strings

```c
int ini_read_str(ini_t *handler,
        char *section, char *name, char **value, char *default_value);
```

Reads a value as a dynamically allocated string. The returned string is allocated with `malloc` — **you must `free` it after use**.

```c
int ini_read_strn(ini_t *handler,
        char *section, char *name, char *value, size_t n, char *default_value);
```

Reads a value into a fixed-size buffer of `n` bytes. If the value is longer than `n - 1` characters, it is truncated. The buffer is always null-terminated and zero-padded.

---

### Reading Integers

```c
int ini_read_int(ini_t *handler,
        char *section, char *name, int *value, int default_value);

int ini_read_unsigned(ini_t *handler,
        char *section, char *name, unsigned *value, unsigned default_value);

int ini_read_int8(ini_t *handler,
        char *section, char *name, int8_t *value, int8_t default_value);

int ini_read_uint8(ini_t *handler,
        char *section, char *name, uint8_t *value, uint8_t default_value);

int ini_read_int16(ini_t *handler,
        char *section, char *name, int16_t *value, int16_t default_value);

int ini_read_uint16(ini_t *handler,
        char *section, char *name, uint16_t *value, uint16_t default_value);

int ini_read_int32(ini_t *handler,
        char *section, char *name, int32_t *value, int32_t default_value);

int ini_read_uint32(ini_t *handler,
        char *section, char *name, uint32_t *value, uint32_t default_value);

int ini_read_int64(ini_t *handler,
        char *section, char *name, int64_t *value, int64_t default_value);

int ini_read_uint64(ini_t *handler,
        char *section, char *name, uint64_t *value, uint64_t default_value);
```

All integer readers support decimal, octal (prefix `0`), and hexadecimal (prefix `0x` or `0X`) notation.

---

### Reading Floating-Point Numbers

```c
int ini_read_float(ini_t *handler,
        char *section, char *name, float *value, float default_value);

int ini_read_double(ini_t *handler,
        char *section, char *name, double *value, double default_value);
```

---

### Reading Booleans

```c
int ini_read_bool(ini_t *handler,
        char *section, char *name, bool *value, bool default_value);
```

Accepts `true` or `false` (case-insensitive). Any other value leaves `*value` set to `default_value`.

---

### Reading IPv4 Addresses

```c
int ini_read_ipv4_addr(ini_t *handler,
        char *section, char *name, struct sockaddr_in *addr, char *default_value);
```

Parses an IPv4 address and port in the form `ip:port` or `ip port` (e.g., `127.0.0.1:8080`) into a `struct sockaddr_in`. Returns `-1` if the value is present but cannot be parsed.

---

### Section and Name Lookup

- Pass an empty string `""` or `NULL` as `section` to look up a key in the implicit `global` section.
- Section and property names are looked up with exact (case-sensitive) matching.

## Usage Example

Given the following `config.ini`:

```ini
log_level = info
log_file  = /var/log/app.log

[server]
host = 0.0.0.0
port = 8080
max_connections = 1024
enable_tls = true

[database]
host = 127.0.0.1:5432
timeout = 30.5
```

```c
#include <stdio.h>
#include <stdlib.h>
#include "ini.h"

int main(void)
{
    ini_t *conf = ini_load("config.ini");
    if (conf == NULL) {
        fprintf(stderr, "Failed to load config.ini\n");
        return 1;
    }

    /* Read a global string (must be freed after use) */
    char *log_level = NULL;
    ini_read_str(conf, NULL, "log_level", &log_level, "warning");
    printf("log_level: %s\n", log_level);
    free(log_level);

    /* Read a string into a fixed-size buffer */
    char log_file[256];
    ini_read_strn(conf, NULL, "log_file", log_file, sizeof(log_file), "/tmp/app.log");
    printf("log_file: %s\n", log_file);

    /* Read integers */
    unsigned max_conn = 0;
    ini_read_unsigned(conf, "server", "max_connections", &max_conn, 128);
    printf("max_connections: %u\n", max_conn);

    /* Read a boolean */
    bool enable_tls = false;
    ini_read_bool(conf, "server", "enable_tls", &enable_tls, false);
    printf("enable_tls: %s\n", enable_tls ? "true" : "false");

    /* Read a double */
    double timeout = 0.0;
    ini_read_double(conf, "database", "timeout", &timeout, 10.0);
    printf("timeout: %f\n", timeout);

    /* Read an IPv4 address */
    struct sockaddr_in db_addr;
    ini_read_ipv4_addr(conf, "database", "host", &db_addr, "127.0.0.1:5432");
    printf("db host: %s:%u\n", inet_ntoa(db_addr.sin_addr), ntohs(db_addr.sin_port));

    ini_free(conf);
    return 0;
}
```

## License

See [LICENSE](LICENSE).
