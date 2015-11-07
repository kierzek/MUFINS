# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$25-Feb-2010 14:47:33$"

#from java import lang
#from javax import swing
#
#import Util
#
#import Model.Model as Model

from javax import swing

import PyMetTable
super = PyMetTable.PyMetTableModel

class LevelTableModel(super):


    def __init__(self, model, myclass, listener, number,  fields = None):
        self.fields = fields or myclass.fields
        super.__init__(self)
        self.model = model
        self.myclass = myclass
        self.lines = model.getLevelList(self.myclass)
        self.listener = listener
        self.number = number

    def isCellEditable(self, r, c):
        return self.fields.getKeyAt(c) not in self.myclass.UneditableFields

        
    def getRowValueAt(self, r, c):
        field = self.getColumnName(c)
        ID = self.lines[r]
        return self.model.getElementValue(ID, field)

    def setValueAt(self, value, r, c):
#        if c != 0:
        if value == None:
            value = ''
        field = self.getColumnName(c)
        ID = self.lines[r]
        self.model.editElement(ID, field, value)
        self.fireTableDataChanged()


#    def insert(self, ID):
#        self.model[ID] = self.myclass()
#        self.fireTableDataChanged()

    def insert(self, IDs, start):
        self.model.insert(IDs, self.myclass, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        self.model.delete(rows, self.myclass)
        self.fireTableDataChanged()

#    def updateOrder(self, IDs):
#        #self.rowlist[:] = IDs
#        print IDs, self.rowlist


    def fireTableDataChanged(self):
        super.fireTableDataChanged(self)
        self.listener.setLevelTitle(self.number, self.getRowCount())
        



class LevelTable(PyMetTable.PyMetTable):


    def deleteSelectedRows(self):
        inds = self.getSelectedRows()
        IDs = [self.getValueAt(i, 0) for i in inds]
        self.getModel().delete(IDs)


    def insertRows(self):
        classname = self.getModel().myclass.__name__.lower()
        IDstr = swing.JOptionPane.showInputDialog(self, 'Enter a space-separated list of %s names:' % classname)
        if IDstr and not IDstr.isspace():
            IDs = IDstr.split(' ')
            self.getModel().insert(IDs, self.getSelectionIndex())








        

#    def sorterChanged(self, e):
#        swing.JTable.sorterChanged(self, e)
#        if e.getType() == swing.event.RowSorterEvent.Type.SORT_ORDER_CHANGED:
#            IDs = [self.getValueAt(i, 0) for i in range(self.getRowCount())]
#            self.getModel().updateOrder(IDs)
        


#        self.setCellSelectionEnabled(True)

        #self.setFillsViewportHeight(True);

#for (int i = 0; i < 5; i++) {
#    column = table.getColumnModel().getColumn(i);
#    if (i == 2) {
#        column.setPreferredWidth(100); //third column is bigger
#    } else {
#        column.setPreferredWidth(50);
#    }
#}
#
#    def tableChanged(self, e):
#        swing.JTable.tableChanged(self, e)






#class BooleanRenderer (swing.table.DefaultTableCellRenderer):
#    def __init__(self):
#        self.comp = swing.JCheckBox()
#        self.comp.setBackground(awt.Color.WHITE)
#
#    def getTableCellRendererComponent(self, table, value, isSelected, hasFocus, r, c):
#        rv = self.comp
#        rv.setSelected(value)
#        return rv



#class FloatRenderer(swing.table.DefaultTableCellRenderer):
#    def __init__(self):
#        self.comp = swing.JTextField()
#        self.comp.setBackground(awt.Color.WHITE)
#        self.comp.setHorizontalAlignment(self.comp.RIGHT)
#        self.comp.setEditable(True)
#
#    def getTableCellRendererComponent(self, table, value, isSelected, hasFocus, r, c):
#        rv = self.comp
#        rv.setText(str(value))
#        return rv



