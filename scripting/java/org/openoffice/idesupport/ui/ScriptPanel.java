package org.openoffice.idesupport.ui;

import java.io.File;
import java.util.Vector;
import java.util.Enumeration;

import java.awt.BorderLayout;
import java.awt.event.FocusEvent;
import java.awt.event.FocusAdapter;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JTable;
import javax.swing.DefaultCellEditor;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableColumn;
import javax.swing.table.AbstractTableModel;

import org.openoffice.idesupport.ScriptEntry;

public class ScriptPanel extends JPanel {
    private ScriptTableModel model;
    private JTable table;
    private File descriptor;

    public ScriptPanel(ScriptEntry[] scripts) {
        model = new ScriptTableModel(scripts);
        initUI();
    }

    public void addScriptEntries(ScriptEntry[] entries) {
        for (int i = 0; i < entries.length; i++) {
            ScriptEntry entry = new ScriptEntry(entries[i].getLanguageName(),
                                                entries[i].getLocation());
            entry.setLogicalName(entries[i].getLanguageName());
            model.add(entry);
        }
    }

    public void removeSelectedRows() {
        int[] selections = table.getSelectedRows();

        for (int i = selections.length - 1; i >= 0; i--) {
            model.remove(selections[i]);
        }
    }

    public void removeAllRows() {
        model.removeAll();
    }

    public Enumeration getScriptEntries() {
        return model.getScriptEntries();
    }

    private void initUI() {
        table = new JTable(model);
        TableColumn column = table.getColumnModel().getColumn(1);
        column.setCellEditor(new DefaultCellEditor(new JTextField()));

        table.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent evt) {
                tableFocusLost(evt);
            }
        });

        JScrollPane pane = new JScrollPane(table);
        JLabel label = new JLabel("Scripts:");
        label.setLabelFor(pane);

        BorderLayout layout = new BorderLayout();
        setLayout(layout);
        layout.setVgap(5);
        add(label, BorderLayout.NORTH);
        add(pane, BorderLayout.CENTER);
    }

    private void tableFocusLost(FocusEvent evt) {
        TableCellEditor editor = table.getCellEditor();
        if (editor != null) {
            Object value = editor.getCellEditorValue();
            if (value != null)
                model.setValueAt(value,
                                 table.getEditingRow(), table.getEditingColumn());
        }
    }

    private class ScriptTableModel extends AbstractTableModel {
        final String[] columnNames = {"Exported Method",
                                      "Script Name"};

        private Vector scripts;
        private int nextRow;

        public ScriptTableModel(ScriptEntry[] entries) {
            scripts = new Vector(entries.length + 11);
            for (int i = 0; i < entries.length; i++) {
                scripts.addElement(entries[i]);
            }
            nextRow = entries.length;
        }

        public int getColumnCount() {
            return columnNames.length;
        }

        public int getRowCount() {
            return scripts.size();
        }

        public String getColumnName(int col) {
            return columnNames[col];
        }

        public void add(ScriptEntry entry) {
            scripts.addElement(entry);
            fireTableRowsInserted(nextRow, nextRow);
            nextRow++;
        }

        public void remove(int row) {
            scripts.removeElementAt(row);
            fireTableRowsDeleted(row, row);
            nextRow--;
        }

        public void removeAll() {
            scripts.removeAllElements();
            fireTableRowsDeleted(0, nextRow);
            nextRow = 0;
        }

        public Enumeration getScriptEntries() {
            return scripts.elements();
        }

        public Object getValueAt(int row, int col) {
            String result = "";
            ScriptEntry entry;

            entry = (ScriptEntry)scripts.elementAt(row);

            if (col == 0)
                result = entry.getLanguageName();
            else if (col == 1)
                result = entry.getLogicalName();

            return result;
        }

        public boolean isCellEditable(int row, int col) {
            if (col == 0)
                return false;
            else
                return true;
        }

        public void setValueAt(Object value, int row, int col) {
            ScriptEntry entry = (ScriptEntry)scripts.elementAt(row);
            entry.setLogicalName((String)value);
            fireTableCellUpdated(row, col);
        }
    }
}
