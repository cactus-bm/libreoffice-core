/*
 * StorageNativeInputStream.java
 *
 * Created on 9. September 2004, 11:52
 */

package com.sun.star.sdbcx.comp.hsqldb;
import com.sun.star.lib.util.NativeLibraryLoader;
import com.sun.star.embed.ElementModes;
/**
 *
 * @author  Ocke
 */
public class StorageNativeInputStream {
    static {
    // preload shared libraries whichs import lips are linked to hsqldb
    if ( System.getProperty( "os.name" ).startsWith( "Windows" ) )
    {
            NativeLibraryLoader.loadLibrary(StorageNativeOutputStream.class.getClassLoader(), "msvcr71");
            NativeLibraryLoader.loadLibrary(StorageNativeOutputStream.class.getClassLoader(), "uwinapi");
            NativeLibraryLoader.loadLibrary(StorageNativeOutputStream.class.getClassLoader(), "sal3");
            NativeLibraryLoader.loadLibrary(StorageNativeOutputStream.class.getClassLoader(), "dbtools680mi");
    }

    // load shared library for JNI code
        NativeLibraryLoader.loadLibrary(StorageNativeOutputStream.class.getClassLoader(), "hsqldb2");
    }
    /** Creates a new instance of StorageNativeInputStream */
    public StorageNativeInputStream(String key,String _file) {
        openStream(key,_file, ElementModes.READ);
    }
    public native void openStream(String key,String name, int mode);
    public native int read(String key,String name) throws java.io.IOException;
    public native int read(String key,String name,byte[] b, int off, int len) throws java.io.IOException;
    public native void close(String key,String name) throws java.io.IOException;
    public native long skip(String key,String name,long n) throws java.io.IOException;
    public native int available(String key,String name) throws java.io.IOException;
    public native int read(String key,String name,byte[] b) throws java.io.IOException;
}
