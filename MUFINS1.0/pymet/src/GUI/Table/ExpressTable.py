__author__="John"
__date__ ="$07-Nov-2011 14:07:18$"


import Util
import PyMetTable



class Express(dict):#data type
    def __init__(self):
        self.fields=[] #expression names list

    def getValue(self, ID, field):
        return self[ID][field]#w# double dict, gene ID->field



#TAB = Data.Tabular.TAB
TAB = PyMetTable.TAB
EMPTYFLOAT = Util.PyMetFloat(Util.NaN)

super = PyMetTable.PyMetTableModel

class ExpressTableModel(super):


    def __init__(self, expr):        
        self.expr=expr
        self.fid=expr.fields
        self.id=expr.keys()
        self.id.sort()
        ff=[]
        ff.append((self.fid[0], Util.StringType))
        ff.extend([(f, Util.StringType) for f in self.fid[1:]])#PyMetFloat->StringType
        self.fields = Util.OrderedDict.OrderedDict(ff)
        super.__init__(self)


    def getRowCount(self):
        return len(self.expr)

    def getColumnCount(self):
        return len(self.fid)

    def getColumnName(self, c):
#        return self.fields.getKeyAt(c)
        return self.fid[c]

    def getRowValueAt(self, r, c):
        row=self.id[r]
        col=self.fid[c]
        return self.expr[row][col]

    def getValueAt(self, r, c):
        rv = self.getRowValueAt(r, c)
        return self.getColumnClass(c)(rv)

    def setValueAt(self, value, r, c):
        row=self.id[r]
        col=self.fid[c]
        self.expr[row][col]=value
        self.fireTableCellUpdated(r, c)




class ExpressTable(PyMetTable.PyMetTable):
    def __init__(self, tablemodel):
        PyMetTable.PyMetTable.__init__(self, tablemodel)
        self.fid=tablemodel.fid
#        column = self.getColumnModel().getColumn(1)
#        width = column.getWidth()
#        expwidth = int(width / 4)
#        column.setPreferredWidth(expwidth)
#        self.getColumnModel().getColumn(2).setPreferredWidth(expwidth)
#        self.doLayout()

#    def addProblem(self, problem):
#        self.getModel().addProblem(problem)
#
    def getScript(self):#w#
        return '\t'.join(self.fid)+'\n'+str(self.getModel())