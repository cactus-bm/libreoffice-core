package complex.storages;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;

import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

import com.sun.star.container.XNameAccess;
import com.sun.star.io.XStream;

import com.sun.star.embed.*;

import share.LogWriter;
import complex.storages.TestHelper;
import complex.storages.StorageTest;

public class Test10 implements StorageTest {

    XMultiServiceFactory m_xMSF;
    XSingleServiceFactory m_xStorageFactory;
    TestHelper m_aTestHelper;

    public Test10( XMultiServiceFactory xMSF, XSingleServiceFactory xStorageFactory, LogWriter aLogWriter )
    {
        m_xMSF = xMSF;
        m_xStorageFactory = xStorageFactory;
        m_aTestHelper = new TestHelper( aLogWriter, "Test10: " );
    }

    public boolean test()
    {
        try
        {
            // create temporary storage based on arbitrary medium
            // after such a storage is closed it is lost
            Object oTempStorage = m_xStorageFactory.createInstance();
            XStorage xTempStorage = (XStorage) UnoRuntime.queryInterface( XStorage.class, oTempStorage );
            if ( xTempStorage == null )
            {
                m_aTestHelper.Error( "Can't create temporary storage representation!" );
                return false;
            }

            byte pBytes1[] = { 1, 1, 1, 1, 1 };

            // open a new substream, set "MediaType" and "Compressed" properties to it and write some bytes
            if ( !m_aTestHelper.WriteBytesToSubstream( xTempStorage, "SubStream1", "MediaType1", true, pBytes1 ) )
                return false;


            // open a new substorage
            XStorage xTempSubStorage = m_aTestHelper.openSubStorage( xTempStorage,
                                                                        "SubStorage1",
                                                                        ElementModes.ELEMENT_WRITE );
            if ( xTempSubStorage == null )
            {
                m_aTestHelper.Error( "Can't create substorage!" );
                return false;
            }

            byte pBytes2[] = { 2, 2, 2, 2, 2 };

            // open a new substream, set "MediaType" and "Compressed" properties to it and write some bytes
            if ( !m_aTestHelper.WriteBytesToSubstream( xTempSubStorage, "SubStream2", "MediaType2", true, pBytes2 ) )
                return false;

            // set "MediaType" property for storages and check that "IsRoot" and "OpenMode" properties are set correctly
            if ( !m_aTestHelper.setStorageTypeAndCheckProps( xTempStorage,
                                                            "MediaType3",
                                                            true,
                                                            ElementModes.ELEMENT_WRITE ) )
                return false;

            if ( !m_aTestHelper.setStorageTypeAndCheckProps( xTempSubStorage,
                                                            "MediaType4",
                                                            false,
                                                            ElementModes.ELEMENT_WRITE ) )
                return false;

            // ==============================
            // check cloning at current state
            // ==============================

            // the new storage still was not commited so the clone must be empty
            XStorage xClonedSubStorage = m_aTestHelper.cloneSubStorage( xTempStorage, "SubStorage1" );

            if ( xClonedSubStorage == null )
            {
                m_aTestHelper.Error( "The result of clone is empty!" );
                return false;
            }

            XNameAccess xClonedNameAccess = (XNameAccess) UnoRuntime.queryInterface( XNameAccess.class, xClonedSubStorage );
            if ( xClonedNameAccess == null )
            {
                m_aTestHelper.Error( "XNameAccess is not implemented by the clone!" );
                return false;
            }

            if ( !m_aTestHelper.checkStorageProperties( xClonedSubStorage, "", true, ElementModes.ELEMENT_READ ) )
                return false;

            if ( xClonedNameAccess.hasElements() )
            {
                m_aTestHelper.Error( "The new substorage still was not commited so it must be empty!" );
                return false;
            }

            if ( !m_aTestHelper.disposeStorage( xClonedSubStorage ) )
                return false;

            xClonedSubStorage = null;
            xClonedNameAccess = null;

            // the new stream was opened, written and closed, that means flashed
            // so the clone must contain all the information
            XStream xClonedSubStream = m_aTestHelper.cloneSubStream( xTempStorage, "SubStream1" );
            if ( !m_aTestHelper.InternalCheckStream( xClonedSubStream, "SubStream1", "MediaType1", pBytes1 ) )
                return false;

            if ( !m_aTestHelper.disposeStream( xClonedSubStream, "SubStream1" ) )
                return false;

            // ==============================
            // commit substorage and check cloning
            // ==============================

            if ( !m_aTestHelper.commitStorage( xTempSubStorage ) )
                return false;

            xClonedSubStorage = m_aTestHelper.cloneSubStorage( xTempStorage, "SubStorage1" );
            if ( xClonedSubStorage == null )
            {
                m_aTestHelper.Error( "The result of clone is empty!" );
                return false;
            }

            if ( !m_aTestHelper.checkStorageProperties( xClonedSubStorage, "MediaType4", true, ElementModes.ELEMENT_READ ) )
                return false;

            if ( !m_aTestHelper.checkStream( xClonedSubStorage, "SubStream2", "MediaType2", pBytes2 ) )
                return false;

            return true;
        }
        catch( Exception e )
        {
            m_aTestHelper.Error( "Exception: " + e );
            return false;
        }
    }
}

