try:

  1$ ./prodfile

or if you don't have a perl handy

  1$ $CC $CFLAGS -c run.c -o run.o
  2$ $CC run.o $LDFLAGS -o run

but, adjusted to the actual list of files, which will be outdated.

Tested with 
  $(CC) -v	
	Using built-in specs.
	COLLECT_GCC=$PATH/x86_64-linux-musl-gcc
	COLLECT_LTO_WRAPPER=$PATH/x86_64-linux-musl/4.9.2/lto-wrapper
	Target: x86_64-linux-musl
	Configured with: ..$PATH/x86_64-linux-musl --enable-languages=c,c++ \
	    --disable-libmudflap --disable-libsanitizer --disable-nls \
	    --disable-multilib --with-multilib-list= --disable-lto-plugin
	Thread model: posix
	gcc version 4.9.2 (GCC) 

yeah. no glibc. drop dead.

\blade
