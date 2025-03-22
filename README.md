# cscript - macOS Compatible Version

This is a macOS-compatible port of [cscript](https://git.zx2c4.com/cscript/about/) originally created by Jason A. Donenfeld.

## Overview

`cscript` is a utility that allows you to run C code directly without a separate compilation step. This version has been enhanced to work on macOS and includes support for external libraries through environment variables.

## Changes from the original

The original Linux version of cscript relies on several Linux-specific features that aren't available on macOS:

- Replaced `memfd_create()` with `mkstemp()` to create temporary files
- Replaced `splice()` with standard read/write operations
- Replaced `fexecve()` with standard `execve()`
- Added `CSCRIPT_FLAGS` environment variable support for external libraries
- Default compiler changed from gcc to clang (macOS standard)
- Removed `-march=native` flag for better Apple hardware compatibility
- Added appropriate error handling for macOS

## Building and Installing

```bash
make
sudo make install
```

You can specify an installation prefix:

```bash
sudo make install PREFIX=/usr
```

The default installation prefix is `/usr/local`.

## Usage

Run C files directly:

```bash
cscript hello.c [arg1 arg2 ...]
```

Command-line arguments are passed to your C program and accessible through the standard `argc` and `argv` parameters in your `main()` function.

### Linking External Libraries

To link external libraries with your cscript programs, use the `CSCRIPT_FLAGS` environment variable:

```bash
# Link with curl and enable all warnings
CSCRIPT_FLAGS="-lcurl -Wall" cscript example_curl.c
```

### Example: Using OpenGL

Run with:

If you're using Apple Silicon (M1/M2/M3):
```bash
CSCRIPT_FLAGS="-I/opt/homebrew/include -L/opt/homebrew/lib -framework OpenGL -lglfw -DGL_SILENCE_DEPRECATION" cscript example_opengl.c
```
Or if you're using Apple Intel

```bash
CSCRIPT_FLAGS="-I/usr/local/include -L/usr/local/lib -framework OpenGL -lglfw -DGL_SILENCE_DEPRECATION" cscript example_opengl.c
```

### Example: Using libcurl

Run with:
```bash
CSCRIPT_FLAGS="-lcurl" cscript example_curl.c
```

I'll provide you with step-by-step instructions for running a memsuo example with cscript, starting with cloning the repository.

### Example: Using memsuo

Clone the memsuo repository
```bash
git clone https://github.com/tetsuo-ai/memsuo
```

Install dependencies (if using the full functionality)

On macOS with Homebrew:
```bash
brew install jemalloc libsodium
```
Run with (for full functionality with jemalloc and libsodium):

```bash
CSCRIPT_FLAGS="-I. -I/opt/homebrew/opt/jemalloc/include -I/opt/homebrew/opt/libsodium/include -L/opt/homebrew/opt/jemalloc/lib -L/opt/homebrew/opt/libsodium/lib -DUSE_JEMALLOC -DUSE_SODIUM -ljemalloc -lsodium -pthread" cscript example_memsuo.c
```

#### Troubleshooting

1. If you get compiler errors about missing headers:
   - Make sure the memsuo directory is in the correct location relative to your example file
   - Try using an absolute path in the CSCRIPT_FLAGS: `-I/full/path/to/directory`

2. If you get library linking errors:
   - Make sure jemalloc and libsodium are installed
   - Check if you need to specify the library path: `-L/usr/local/lib` or `-L/opt/homebrew/lib` for Apple Silicon

3. If you get runtime errors:
   - Try running without optional features first
   - Check if library paths are correct in your environment

For a more comprehensive example that also uses the arena allocator, simply add the arena example code to your file and include the appropriate header.

### Example: Using with watchexec

For development, you can use watchexec to automatically rerun your script when files change. First install watchexec:

On macOS with Homebrew:
```bash
brew install watchexec
```

Then you can use it to automatically run your C script whenever it changes:

```bash
watchexec -c -r -- "cscript your_script.c"
```

For scripts using external libraries, include your CSCRIPT_FLAGS:

```bash
watchexec -c -r -- "CSCRIPT_FLAGS='-lcurl -Wall' cscript your_script.c"
```

The flags used above:
- `-c`: Clear the screen between runs
- `-r`: Restart the command if it's still running when changes occur

This is particularly useful during development as it allows you to see your changes in real-time without manually rerunning the script.

## License

This program is licensed under the GNU General Public License v2.0.
See the [COPYING](COPYING) file for the full license text.

## Credits

Original implementation by Jason A. Donenfeld.
macOS port with enhancements for external library support.
