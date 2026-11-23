# linux-c-core-utils

My implementation of the Linux core utilities written in C.

> [!NOTE]
> This project is a work in progress built for learning C and Linux systems programming.

## Implemented Utilities

- [x] `cat` – Concatenate and print files
- [x] `echo` – Display a line of text
- [ ] `ls` – List directory contents
- [x] `cp` – Copy files and directories
- [ ] `rm` – Remove files or directories

## Building & Usage

Each utility is a standalone C file. Clone the repository and compile any file directly using `gcc` or `clang`:

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

Or move them into the `/usr/local/bin` directory:

```bash
sudo mv ./echo /usr/local/bin
```

## License

This project is under the kforkernel Use License (KUL) v1.0.
For more details, read [LICENSE.md](LICENSE.md).
