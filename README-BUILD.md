# Build procenv

## Build from source

1. Install dependencies

   | Platform | Usage | Required? | Dependency | Rationale |
   |-|-|-|-|-|
   | common | build | yes | GCC or Clang compiler | For building the code |
   | common | build | yes | GNU Autoconf | For configuring the source package |
   | common | build | yes | GNU Autoconf Archive | For configuring the source package |
   | common | build | yes | GNU Automake | For generating makefiles |
   | common | build | yes | GNU Make | For building the code |
   | common | build | yes | `pkgconf` / `pkg-config` | For configuring build dependencies |
   | common | test | optional | Check | For running unit tests |
   | common | test | optional | Expat | For validating XML output |
   | common | test | optional | GNU Groff | For checking man page documentation |
   | Linux | build | optional | `libapparmor` development package | For AppArmor details |
   | Linux | build | optional | `libcap` development package | For capabilities details |
   | Linux | build | optional | `libnuma` development package | For NUMA memory details |
   | Linux | build | optional | `libselinux` development package | For SELinux details |
   | BSD | build | optional | `libsysinfo` package or port | For general memory details |

   > **Note:**
   >
   > The definitive list of dependenciese can always be seen by looking at the GitHub Actions workflow file here:
   >
   > - [`.github/workflows/build.yaml`](.github/workflows/build.yaml)

1. Checkout the source code:

   ```bash
   $ git clone https://github.com/jamesodhunt/procenv
   $ cd procenv
   ```

1. Configure and build:

   ```bash
   $ autoreconf -fi && ./configure
   $ make && make check && sudo make install
   ```

   > **Note:**
   >
   > For BSD systems, replace `make` with `gmake` above to ensure you run using
   > GNU Make (BSD make will hang at the test stage!)

## Build snap package

```bash
$ git clone https://github.com/jamesodhunt/procenv
$ cd procenv
$ snapcraft
```

### Build on FreeBSD

FreeBSD is awkward. This worked for me on FreeBSD 10.2:

```bash
$ gmake CC=clang-devel LD=ld.gold LDFLAGS='-v -fuse-ld=gold'
```

### Build on Minix

Try this:

```bash
$ ./configure CC=clang CFLAGS='-I/usr/pkg/include' LDFLAGS='-L/usr/pkg/lib'
```
