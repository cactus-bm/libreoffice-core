package org.libreoffice;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.widget.DrawerLayout;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.RelativeLayout;

import org.mozilla.gecko.gfx.GeckoLayerClient;
import org.mozilla.gecko.gfx.LayerController;

import java.util.ArrayList;
import java.util.List;

public class LibreOfficeMainActivity extends Activity {

    private static final String LOGTAG = "LibreOfficeMainActivity";
    private static final String DEFAULT_DOC_PATH = "/assets/test1.odt";

    public static LibreOfficeMainActivity mAppContext;

    private static LayerController mLayerController;
    private static GeckoLayerClient mLayerClient;
    private static LOKitThread sLOKitThread;

    public Handler mMainHandler;

    private DrawerLayout mDrawerLayout;
    private RelativeLayout mGeckoLayout;
    private ListView mDrawerList;
    private List<DocumentPartView> mDocumentPartView = new ArrayList<DocumentPartView>();
    private DocumentPartViewListAdpater mDocumentPartViewListAdpater;

    public static GeckoLayerClient getLayerClient() {
        return mLayerClient;
    }

    public static LayerController getLayerController() {
        return mLayerController;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        // If the nav drawer is open, hide action items related to the content view
        boolean drawerOpen = mDrawerLayout.isDrawerOpen(mDrawerList);
        menu.findItem(R.id.action_list).setVisible(!drawerOpen);
        return super.onPrepareOptionsMenu(menu);
    }

    public DisplayMetrics getDisplayMetrics() {
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        return metrics;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        mAppContext = this;
        super.onCreate(savedInstanceState);

        mMainHandler = new Handler();

        String inputFile;

        if (getIntent().getData() != null) {
            inputFile = getIntent().getData().getEncodedPath();
        } else {
            inputFile = DEFAULT_DOC_PATH;
        }

        setContentView(R.layout.activity_main);

        getActionBar().setDisplayHomeAsUpEnabled(false);
        getActionBar().setHomeButtonEnabled(false);

        mGeckoLayout = (RelativeLayout) findViewById(R.id.gecko_layout);
        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        if (mDocumentPartViewListAdpater == null) {
            mDrawerList = (ListView) findViewById(R.id.left_drawer);

            mDocumentPartViewListAdpater = new DocumentPartViewListAdpater(this, R.layout.document_part_list_layout, mDocumentPartView);
            mDrawerList.setAdapter(mDocumentPartViewListAdpater);
            mDrawerList.setOnItemClickListener(new DocumentPartClickListener());
        }

        if (sLOKitThread == null) {
            sLOKitThread = new LOKitThread();
            sLOKitThread.start();
        } else {
            sLOKitThread.clearQueue();
        }

        mLayerController = new LayerController(this);
        mLayerClient = new GeckoLayerClient(this);
        mLayerController.setLayerClient(mLayerClient);
        mGeckoLayout.addView(mLayerController.getView(), 0);

        LOKitShell.sendEvent(LOEvent.load(inputFile));

        Log.w(LOGTAG, "UI almost up");
    }

    @Override
    protected void onResume() {
        Log.i(LOGTAG, "Resume..");
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.i(LOGTAG, "Pause..");
        super.onPause();
    }

    public LOKitThread getLOKitThread() {
        return sLOKitThread;
    }

    public List<DocumentPartView> getDocumentPartView() {
        return mDocumentPartView;
    }

    public DocumentPartViewListAdpater getDocumentPartViewListAdpater() {
        return mDocumentPartViewListAdpater;
    }

    private class DocumentPartClickListener implements android.widget.AdapterView.OnItemClickListener {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            DocumentPartView partView = mDocumentPartViewListAdpater.getItem(position);
            LOKitShell.sendEvent(LOEvent.changePart(partView.getPartIndex()));
            mDrawerLayout.closeDrawer(mDrawerList);
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
