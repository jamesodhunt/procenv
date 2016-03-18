Building on FreeBSD
-------------------

FreeBSD is awkward. This worked for me on FreeBSD 10.2::

  $ gmake CC=clang-devel LD=ld.gold LDFLAGS='-v -fuse-ld=gold'

Building on Minix
------------------

Try this::

  $ ./configure CC=clang CFLAGS='-I/usr/pkg/include' LDFLAGS='-L/usr/pkg/lib'
