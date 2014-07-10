package org.libreoffice;

import android.graphics.Rect;
import android.graphics.RectF;
import android.util.Log;

import org.mozilla.gecko.gfx.FloatSize;
import org.mozilla.gecko.gfx.SubTile;
import org.mozilla.gecko.gfx.ViewportMetrics;

import java.util.ArrayList;
import java.util.concurrent.ConcurrentLinkedQueue;

public class LOKitThread extends Thread {
    private static final String LOGTAG = LOKitThread.class.getSimpleName();

    private static final int TILE_SIZE = 256;
    public ConcurrentLinkedQueue<LOEvent> mEvents = new ConcurrentLinkedQueue<LOEvent>();
    private LibreOfficeMainActivity mApplication;
    private TileProvider mTileProvider;
    private ViewportMetrics mViewportMetrics;

    LOKitThread() {
    }

    private boolean draw() throws InterruptedException {
        int pageWidth = mTileProvider.getPageWidth();
        int pageHeight = mTileProvider.getPageHeight();

        mViewportMetrics = new ViewportMetrics();
        mViewportMetrics.setPageSize(new FloatSize(pageWidth, pageHeight));

        RectF viewport = mApplication.getLayerController().getViewportMetrics().getClampedViewport();
        float zoomFactor = mApplication.getLayerController().getViewportMetrics().getZoomFactor();

        boolean shouldContinue = mApplication.getLayerClient().beginDrawing(mViewportMetrics);

        if (!shouldContinue) {
            return false;
        }

        int minX = ((int) viewport.left / TILE_SIZE) * TILE_SIZE;
        int minY = ((int) viewport.top / TILE_SIZE) * TILE_SIZE;
        int maxX = (((int) viewport.right / TILE_SIZE) + 1) * TILE_SIZE;
        int maxY = (((int) viewport.bottom / TILE_SIZE) + 1) * TILE_SIZE;

        Rect rect = new Rect(
                Math.round(minX / zoomFactor),
                Math.round(minY / zoomFactor),
                Math.round(maxX / zoomFactor),
                Math.round(maxY / zoomFactor));

        ArrayList<SubTile> removeTiles = new ArrayList<SubTile>();
        for (SubTile tile : mApplication.getLayerClient().getTiles()) {
            if (!rect.intersects(tile.x, tile.y, tile.x + TILE_SIZE, tile.y + TILE_SIZE)) {
                removeTiles.add(tile);
            }
        }
        mApplication.getLayerClient().getTiles().removeAll(removeTiles);

        for (int y = minY; y <= maxY; y+=TILE_SIZE) {
            for (int x = minX; x <= maxX; x+=TILE_SIZE) {
                if (x > pageWidth) {
                    continue;
                }
                if (y > pageHeight) {
                    continue;
                }
                boolean contains = false;
                for (SubTile tile : mApplication.getLayerClient().getTiles()) {
                    if (tile.x == x && tile.y == y) {
                        contains = true;
                    }
                }
                if (!contains) {
                    SubTile tile = mTileProvider.createTile(x, y);
                    mApplication.getLayerClient().addTile(tile);
                }
            }
        }

        mApplication.getLayerClient().endDrawing();

        return true;
    }

    private void initialize() {
        mApplication = LibreOfficeMainActivity.mAppContext;
        mTileProvider = new LOKitTileProvider(mApplication.getLayerController());
    }

    public void run() {
        initialize();
        try {
            boolean drawn = false;
            while (true) {
                if (!mEvents.isEmpty()) {
                    processEvent(mEvents.poll());
                }
                Thread.sleep(100L);
            }
        } catch (InterruptedException ex) {
        }
    }

    private void processEvent(LOEvent event) throws InterruptedException {
        switch (event.mType) {
            case LOEvent.VIEWPORT:
                mViewportMetrics = event.getViewport();
                draw();
                break;
            case LOEvent.DRAW:
                draw();
                break;
            case LOEvent.SIZE_CHANGED:
                break;
        }
    }

    public void queueEvent(LOEvent event) {
        Log.i(LOGTAG, "Event: " + event.getTypeString());
        mEvents.add(event);
    }
}
