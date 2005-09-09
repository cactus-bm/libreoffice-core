/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OfficeDocumentChildren.java,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:18:25 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

package org.openoffice.netbeans.modules.office.nodes;

import java.io.IOException;
import java.util.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.openide.nodes.*;
import org.openide.actions.*;
import org.openide.util.actions.SystemAction;
import org.openide.util.HelpCtx;

import org.openoffice.netbeans.modules.office.options.OfficeSettings;
import org.openoffice.netbeans.modules.office.utils.NagDialog;
import org.openoffice.netbeans.modules.office.actions.OfficeDocumentCookie;

public class OfficeDocumentChildren extends Children.Keys
    implements ChangeListener {

    private OfficeDocumentCookie document = null;

    public OfficeDocumentChildren(OfficeDocumentCookie cookie) {
        document = cookie;
    }

    private void refreshKeys() {
        if (document == null) {
            setKeys(Collections.EMPTY_SET);
            return;
        }

        Enumeration parcels = document.getParcels();
        if (parcels.hasMoreElements() != true) {
            setKeys(Collections.EMPTY_SET);
            return;
        }

        ArrayList keys = new ArrayList();
        while (parcels.hasMoreElements()) {
            String parcel = (String)parcels.nextElement();
            keys.add(parcel);
        }
        setKeys(keys);
    }

    protected void addNotify() {
        super.addNotify();
        document.addChangeListener(this);
        refreshKeys();
    }

    protected void removeNotify() {
        super.removeNotify();
        document.removeChangeListener(this);
        setKeys(Collections.EMPTY_SET);
    }

    protected Node[] createNodes(Object key) {
        String name = (String)key;
        return new Node[] {new ParcelNode(name)};
    }

    public void stateChanged(ChangeEvent e) {
        refreshKeys();
    }

    private class ParcelNode extends AbstractNode {
        private String name;

        public ParcelNode(String name) {
            super(Children.LEAF);
            this.name = name;
            init();
        }

        private void init() {
            setIconBase("/org/openoffice/netbeans/modules/office/resources/ParcelIcon");

            setName(name);
            setDisplayName(name.substring(name.lastIndexOf("/") + 1));
            setShortDescription(name);
        }

        protected SystemAction[] createActions() {
            return new SystemAction[] {
                SystemAction.get(DeleteAction.class),
            };
        }

        public HelpCtx getHelpCtx() {
            return HelpCtx.DEFAULT_HELP;
        }

        public boolean canDestroy() {
            return true;
        }

        public void destroy() throws IOException {
            OfficeSettings settings = OfficeSettings.getDefault();
            String message = "If you already have this document open in " +
                "Office, please close it before continuing. Click OK to " +
                "delete this parcel.";

            if (settings.getWarnBeforeParcelDelete() == true) {
                NagDialog warning = NagDialog.createConfirmationDialog(
                    message, "Show this message in future", true);

                boolean result = warning.show();

                if (warning.getState() == false)
                    settings.setWarnBeforeParcelDelete(false);

                if (result == false)
                    return;
            }
            super.destroy();
            document.removeParcel(name);
        }
    }
}
