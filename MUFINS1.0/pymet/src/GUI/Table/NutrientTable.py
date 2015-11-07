__author__="John"
__date__ ="$12-Nov-2011 13:54:56$"

#from javax import swing
#from java import lang
from java import awt

import Util
import PyMetTable



class NutrientSet(list):
    def __init__(self, externals=[]):
        self.nuts=externals
        self.nutstate=[[nut,0,0,1] for nut in externals]

    def getNutSetting(self):
        nuts_test=[]
        nuts_pre=[]
        for i in range(len(self.nutstate)):
            if self.nutstate[i][1]: nuts_test.append(self.nutstate[i][0]) #for growth media test
            elif self.nutstate[i][2]: nuts_pre.append(self.nutstate[i][0]) #for present nuts
        return nuts_test, nuts_pre





super = PyMetTable.PyMetTableModel

#TAB = Data.Tabular.TAB

class NutsTableModel(super):
    def __init__(self, nutset):
        super.__init__(self)
        self.lines=nutset.nutstate
#        self.addTableModelListener(NutsTableModelListener())

    fields = Util.OrderedDict.OrderedDict([
        ('Nutrient', Util.StringType),
        ('Test', bool),
        ('Present', bool),
        ('Absent', bool),
    ])


    def isCellEditable(self, r, c):
        if c==0: return 0
        else: return 1

    def setValueAt(self, value, r, c):
        theclass = self.lines[r][c].__class__
        if value == None:
            value = theclass()
        else:
            value = theclass(value)
        self.lines[r][c] = value
        if value:
            if c==1: self.lines[r][2]=theclass(0); self.lines[r][3]=theclass(0)
            elif c==2: self.lines[r][1]=theclass(0); self.lines[r][3]=theclass(0)
            elif c==3: self.lines[r][1]=theclass(0); self.lines[r][2]=theclass(0)
        else: self.lines[r][c]=theclass(1)
#        self.fireTableCellUpdated(r,c)
        self.fireTableDataChanged()





class NutsTable(PyMetTable.PyMetTable):
    def __init__(self, tablemodel):
        PyMetTable.PyMetTable.__init__(self, tablemodel)
        self.setSelectionBackground(awt.Color.white)

#        column = self.getColumnModel().getColumn(1)
#        width = column.getWidth()
#        expwidth = int(width / 4)
#        column.setPreferredWidth(expwidth)
#        self.getColumnModel().getColumn(2).setPreferredWidth(expwidth)
#        self.doLayout()




#class NutsTableModelListener(swing.event.TableModelListener):
#    def tableChanged(self, e):
#        r = e.getFirstRow()
#        c = e.getColumn()
#        model = e.getSource()
#        print [r,c]#t#
#        print model.getValueAt(r, c)#t#

#        model.fireTableDataChanged()






