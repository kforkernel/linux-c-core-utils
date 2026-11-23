# linux-c-core-utils

A clean-room implementation of standard Linux core utilities written in C.

> [!NOTE]
> This project is an active work in progress focused on exploring Linux systems programming, POSIX system calls, and low-level C development. While these utilities prioritize core functionality and essential flags over 100% feature parity with GNU Coreutils, they are built from scratch to faithfully replicate standard behavior.

## Implemented Utilities

- [x] `cat` – Concatenate and print files
- [x] `echo` – Display a line of text
- [ ] `ls` – List directory contents
- [x] `cp` – Copy files and directories
- [ ] `rm` – Remove files or directories

## Building & Usage

Each utility is a standalone C file with zero external dependencies. Clone the repository and compile any file directly using `gcc` or `clang`:

```bash
git clone https://github.com/kforkernel/linux-c-core-utils
cd linux-c-core-utils/src

# Compile a utility (using gcc or clang)
gcc echo.c -o echo
```

Run the compiled binary directly from the directory:

```bash
./echo "Hello, world!"
```

Or install them system-wide by moving the binaries to `/usr/local/bin`:

```bash
sudo mv ./echo /usr/local/bin
```

## License

This project is under the kforkernel Use License (KUL) v1.0.
For more details, read [LICENSE.md](LICENSE.md).
