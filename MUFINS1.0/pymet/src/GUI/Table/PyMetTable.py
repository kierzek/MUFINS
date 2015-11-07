__author__="Albert Gevorgyan"
__date__ ="$01-Mar-2010 14:41:55$"


from javax import swing
from java import lang

import Util
import Data.Tabular

super = swing.table.AbstractTableModel

#TAB = Data.Tabular.TAB
TAB = Util.TAB

class PyMetTableModel(super):

    fields = Util.OrderedDict.OrderedDict([])
    Types = {
        Util.BooleanType : lang.Boolean,
        #Util.PyMetFloat   : lang.Float,
    }

    def __init__(self):
        super.__init__(self)
        self.lines = []
        self.adjustClasses()

    def adjustClasses(self):
        self.colfields = []
        for field in self.fields.getOrder():
            colclass = self.fields[field]
            colclass = self.Types.get(colclass, str) #w# convert <class 'Util.Stoich.Equation'> to java.lang.Class
            self.colfields.append(colclass)
            #self.fields.replace(field, colclass)
            

    def getRowCount(self):
        return len(self.lines)

    def getColumnCount(self):
        return len(self.fields)

    def getColumnName(self, c):
        return self.fields.getKeyAt(c)

    def isCellEditable(self, r, c):
        return True

    def getColumnClass(self, c):
        #return self.fields.getValueAt(c)
        return self.colfields[c]

    def getRowValueAt(self, r, c):
        return self.lines[r][c]
    
    def getValueAt(self, r, c):
        rv = self.getRowValueAt(r, c)
        return self.getColumnClass(c)(rv)

    def getStringValueAt(self, r, c):
        rv = self.getRowValueAt(r, c)
        strclass = self.fields.getValueAt(c)
        if strclass==Util.Stoich.Equation:#t#
            return str(rv)#t#
#            print rv#t#
        rv = strclass(rv)
        return str(rv)

    def getRowIndex(self, name, c):#w# get row index by name and column index
        rowc=[row[c] for row in self.lines]
        if name in rowc:
            return rowc.index(name)
        else: return -1

    def setValueAt(self, value, r, c):
        theclass = self.lines[r][c].__class__
        try:
            if value == None:
                value = theclass()
            else:
                value = theclass(value)
            self.lines[r][c] = value
            self.fireTableCellUpdated(r, c)#w#
        except:
            pass

    def __str__(self):
        rv = ''
        for r in range(self.getRowCount()):
            toks = self.getStringValuesAt(r)
            rv += TAB.join(toks).strip() + '\n'
        return rv[:-1]

    def getStringValuesAt(self, r):
        return [self.getStringValueAt(r, c) for c in range(self.getColumnCount())]


    def delete(self, rows):
        rows.reverse()
        for row in rows:
            del self.lines[row]
        self.fireTableDataChanged()

    def deleteAll(self):
        del self.lines[:]
        self.fireTableDataChanged()


    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines[:] = self.lines[:start] + newlines + self.lines[start:]
        self.fireTableDataChanged()

    def emptyRow(self):
        return [self.fields.getValueAt(i)() for i in range(self.getColumnCount())]
    
    def save(self, path):
        Data.Tabular.save(self, path)

    def read(self, s):
        s = s.strip().strip('\n')
        lines = s.split('\n')
        for line in lines:
            row = self.emptyRow()
#            line, comment = Data.Tabular.splitLineAndComment(line)
            toks = self.split(line)
            for i in range(len(toks)):
                tok = toks[i]
                if not tok.isspace() and len(tok) > 0:
                    row[i] = self.fields.getValueAt(i)(tok)
#            row[-1] = Util.CommentType(comment)
            self.lines.append(row)
#        self.lines.append(row)
        self.fireTableDataChanged()


    def split(self, line):
        return line.split(TAB)

    def isEmpty(self):
        return len(self.lines) == 0

    def loadData(self, lines):#w#
        self.lines=lines
        self.fireTableDataChanged()

    def getData(self):#w#
        return self.lines

class PyMetTable(swing.JTable):

    def __init__(self, tablemodel):
        swing.JTable.__init__(self, tablemodel)
        #self.setDefaultRenderer(types.BooleanType, BooleanRenderer())
        #self.setColumnSelectionAllowed(True)
        self.setAutoCreateRowSorter(True)
        self.setCellSelectionEnabled(True)
        self.setAutoResizeMode(swing.JTable.AUTO_RESIZE_OFF)
        #self.setSelectionModel(swing.ListSelectionModel.MULTIPLE_INTERVAL_SELECTION) #wtr#
        self.setRowSorter(MyTableRowSorter(tablemodel))#w#
        
    def setColWidths(self, width):
        n = self.getColumnCount()
        width = int(width / n)
        for i in range(0, n):
            column = self.getColumnModel().getColumn(i)
            column.setPreferredWidth(width);


    def setColWidths_gsa(self, width):#w#
        n = self.getColumnCount()
        colwid={}
        sumwid=0
        for i in range(0, n):
            column = self.getColumnModel().getColumn(i)
            colname = self.getModel().getColumnName(i)
#            column.setMinWidth(10)
            if colname in Util.tableColWidth:
                column.setPreferredWidth(Util.tableColWidth[colname])
#               column.setMaxWidth(Util.tableColWidth[colname])
                colwid[column]=Util.tableColWidth[colname]
                sumwid+=Util.tableColWidth[colname]
            else:
                column.setPreferredWidth(70)
                colwid[column]=70
                sumwid+=70
        if sumwid<width:
            for column, wid in colwid.items():
                column.setPreferredWidth(int(width*(float(wid)/sumwid)))

    def getSelectedRows(self):
        inds = swing.JTable.getSelectedRows(self)
        inds = list(inds)
        for i in range(len(inds)):
            inds[i] = self.convertRowIndexToModel(inds[i])
        inds.sort()
        return inds

    def getSelectedLists(self, colinds = None):
        rv = []
        if colinds == None:
            colinds = self.getSelectedColumns()
        if len(colinds) > 0:
            rowinds = self.getSelectedRows()
            model = self.getModel()
            for i in rowinds:
                row = map(lambda c: model.getRowValueAt(i, c), colinds)
                rv.append(row)
        return rv

    def getSelectedValues(self, c):
        inds = self.getSelectedRows()
        model = self.getModel()
        return map(lambda r: model.getRowValueAt(r, c), inds)

    def deleteSelectedRows(self):
        inds = self.getSelectedRows()
        self.getModel().delete(list(inds))

    def deleteAll(self):
        self.getModel().deleteAll()

    def insertRows(self):
        n = swing.JOptionPane.showInputDialog(self, 'Enter the number of rows:')
        if n and not n.isspace():
            n = int(n)
            self.getModel().insert(n, self.getSelectionIndex())
#            print 'insertModel; ',self.getModel().__class__#t#

    def getSelectionIndex(self):
        rv = self.getRowCount()
        inds = self.getSelectedRows()
        if len(inds) > 0:
            rv = inds[0]
        return rv

    def save(self, path):
        self.getModel().save(path)

    def filterByRegEx(self, re):
        sorter = self.getRowSorter()
        inds = self.getSelectedColumns()
        if len(inds) == 0:
            inds = range(self.getColumnCount())
        refilter = swing.RowFilter.regexFilter(re, inds)
        filters = [refilter]
        curfilter = sorter.getRowFilter()
        if curfilter != None:
            filters.append(curfilter)
        andfilter = swing.RowFilter.andFilter(filters)
        sorter.setRowFilter(andfilter);

    def filter(self, IDs, ind):
        rf = ValueFilter(IDs, ind)
        self.getRowSorter().setRowFilter(rf)

    def unFilter(self):
        self.getRowSorter().setRowFilter(None)

    def read(self, s):
        self.getModel().read(s)

    def isEmpty(self):
        return self.getModel().isEmpty()


class MyTableRowSorter(swing.table.TableRowSorter):#w#
    def __init__(self, tm):
        swing.table.TableRowSorter.__init__(self, tm)
#        self.sortKeys = self.getSortKeys()
#        self.sortKeys=[swing.RowSorter.SortKey(0, swing.SortOrder.UNSORTED)]
        self.sortKeys = []
#        sortKeys.append(swing.RowSorter.SortKey(0, swing.SortOrder.ASCENDING))
#        sortKeys.append(swing.RowSorter.SortKey(0, swing.SortOrder.DESCENDING))
#        self.setSortKeys(self.sortKeys)

    def toggleSortOrder(self, col):
        if not self.sortKeys or self.sortKeys[0].getSortOrder()==swing.SortOrder.UNSORTED:
            self.sortKeys=[swing.RowSorter.SortKey(col, swing.SortOrder.ASCENDING)]
            self.setSortKeys(self.sortKeys)
            return
        elif self.sortKeys[0].getSortOrder()==swing.SortOrder.ASCENDING:
            self.sortKeys=[swing.RowSorter.SortKey(col, swing.SortOrder.DESCENDING)]
            self.setSortKeys(self.sortKeys)
            return
        elif self.sortKeys[0].getSortOrder()==swing.SortOrder.DESCENDING:
            self.sortKeys=[swing.RowSorter.SortKey(col, swing.SortOrder.UNSORTED)]
            self.setSortKeys(self.sortKeys)
            return
#        self.toggleSortOrder(col)

class ValueFilter(swing.RowFilter):
    def __init__(self, IDs, ind):
        self.IDs = IDs
        self.ind = ind

    def include(self, entry):
        return entry.getStringValue(self.ind) in self.IDs






