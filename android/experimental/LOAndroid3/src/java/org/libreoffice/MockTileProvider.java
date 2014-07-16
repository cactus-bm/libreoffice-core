package org.libreoffice;

import android.graphics.Bitmap;

import org.mozilla.gecko.gfx.BufferedCairoImage;
import org.mozilla.gecko.gfx.CairoImage;
import org.mozilla.gecko.gfx.LayerController;
import org.mozilla.gecko.gfx.SubTile;

public class MockTileProvider implements TileProvider {
    private final LayerController layerController;
    private static final int TILE_SIZE = 256;

    public MockTileProvider(LayerController layerController) {
        this.layerController = layerController;
    }

    @Override
    public int getPageWidth() {
        return 549;
    }

    @Override
    public int getPageHeight() {
        return 630*5;
    }

    @Override
    public boolean isReady() {
        return true;
    }

    @Override
    public SubTile createTile(int x, int y) {
        int tiles = (getPageWidth() / TILE_SIZE) + 1;
        int tileNumber = (y / TILE_SIZE) * tiles + (x / TILE_SIZE);
        tileNumber %= 9;
        tileNumber += 1; // 0 to 1 based numbering

        String imageName = "d" + tileNumber;
        Bitmap bitmap = layerController.getDrawable(imageName);

        CairoImage image = new BufferedCairoImage(bitmap);
        SubTile tile = new SubTile(image, x, y);
        tile.beginTransaction();

        return tile;
    }
}
