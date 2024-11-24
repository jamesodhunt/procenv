# procenv developer details

> **Note:**
>
> This document assumes you have already read the
> [build document](README-BUILD.md).

## Debug with gdb or lldb

To disable optimisations when building `procenv`, configure by specifying
the `--disable-compiler-optimisations` configure option (which sets the
`-O0` compiler option):

```bash
$ /configure --disable-compiler-optimisations
$ make
$ gdb src/procenv
```
