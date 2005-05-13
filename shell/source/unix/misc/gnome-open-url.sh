#!/bin/sh

# restore search path for dynamic loader to system defaults to
# avoid version clashes of mozilla libraries shipped with OOo
case `uname -s` in
  AIX)
    LIBPATH=$SYSTEM_LIBPATH
    if [ -z "$LIBPATH" ]; then
        unset LIBPATH SYSTEM_LIBPATH
    else
        export LIBPATH; unset SYSTEM_LIBPATH
    fi
    ;;

  Darwin)
    DYLD_LIBRARY_PATH=$SYSTEM_DYLD_LIBRARY_PATH
    if [ -z "$DYLD_LIBRARY_PATH" ]; then
        unset DYLD_LIBRARY_PATH SYSTEM_DYLD_LIBRARY_PATH
    else
        export DYLD_LIBRARY_PATH; unset SYSTEM_DYLD_LIBRARY_PATH
    fi
    ;;

  HP-UX)
    SHLIB_PATH=$SYSTEM_SHLIB_PATH
    if [ -z "$SHLIB_PATH" ]; then
        unset SHLIB_PATH SYSTEM_SHLIB_PATH
    else
        export SHLIB_PATH; unset SYSTEM_SHLIB_PATH
    fi
    ;;

  IRIX*)
    LD_LIBRARYN32_PATH=$SYSTEM_LD_LIBRARYN32_PATH
    if [ -z "$LD_LIBRARYN32_PATH" ]; then
        unset LD_LIBRARYN32_PATH SYSTEM_LD_LIBRARYN32_PATH
    else
        export LD_LIBRARYN32_PATH; unset $SYSTEM_LD_LIBRARYN32_PATH
    fi
    ;;

  *)
    LD_LIBRARY_PATH=$SYSTEM_LD_LIBRARY_PATH
    if [ -z "$LD_LIBRARY_PATH" ]; then
        unset LD_LIBRARY_PATH SYSTEM_LD_LIBRARY_PATH
    else
        export LD_LIBRARY_PATH; unset SYSTEM_LD_LIBRARY_PATH
    fi
    ;;
esac

# use gnome-open utility coming with libgnome if available
gnome-open $1 2>/dev/null || "$0.bin" $1

exit 0
