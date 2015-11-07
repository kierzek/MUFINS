# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$01-Mar-2010 14:41:55$"



import Util
import Data.Problem
import PyMetTable


TAB = Util.TAB
LB = Util.LB
UB = Util.UB
COMMENT = Util.COMMENTt
EXPRESSION = Util.EXPRESSIONp
EMPTYFLOAT = Util.PyMetFloat(Util.NaN)


super = PyMetTable.PyMetTableModel

def isTagged(line):
    return line.startswith(Util.OBJTAG) or\
             line.startswith(Util.DOLTAG) or\
             line.startswith(Util.ARGTAG) or\
             line.startswith(Util.ENDTAG) or\
             line.startswith(Util.EXTTAG)

class ProblemTableModel(super):

    fields = Util.OrderedDict.OrderedDict([
        (EXPRESSION, Util.StringType),
        (LB, Util.PyMetFloat),
        (UB, Util.PyMetFloat),
        (COMMENT, Util.CommentType),
    ])

    def setValueAt(self, value, r, c):
        if c in [1, 2] and value == None:
            value = EMPTYFLOAT
        super.setValueAt(self, value, r, c)

    def emptyRow(self):
        row = super.emptyRow(self)
        row[1] = EMPTYFLOAT
        row[2] = EMPTYFLOAT
        return row


    def addProblem(self, problem, program):#w/ program
        for line in problem.toLines(program):#w/
            row = self.emptyRow()
            toks = line.split(TAB)
            for i in range(len(toks)):
                row[i] = toks[i]
            self.lines.append(row)
        self.fireTableDataChanged()

    def addRecords(self, recs):
        ncols = self.getColumnCount()
        for rec in recs:
            row = self.emptyRow()
            for c in range(ncols):
                value = rec[self.getColumnName(c)]
                if value != None:
                    row[c] = value
            self.lines.append(row)
        self.fireTableDataChanged()


    def isCellEditable(self, r, c):
        rv = True
        if c > 0 and self.isRowTagged(r):
            rv = False
        return rv

    def getStringValuesAt(self, r):
        rv = super.getStringValuesAt(self, r)
        if self.isRowTagged(r):
            rv = [rv[0]]
        return rv

    def isRowTagged(self, r):
        return isTagged(self.getRowValueAt(r, 0))


class ProblemTable(PyMetTable.PyMetTable):
    def __init__(self):
        PyMetTable.PyMetTable.__init__(self, ProblemTableModel())
#        column = self.getColumnModel().getColumn(1)
#        width = column.getWidth()
#        expwidth = int(width / 4)
#        column.setPreferredWidth(expwidth)
#        self.getColumnModel().getColumn(2).setPreferredWidth(expwidth)
#        self.doLayout()

    def addProblem(self, problem, program):#w/
        self.getModel().addProblem(problem, program)#w/

    def getScript(self):
        return str(self.getModel())

    def getObjArg(self):#w/ get obj(!) argument for problem SGNI
        arg=self.getModel().getValueAt(0,0)
        arg=arg.strip().lstrip('!').split(TAB)
        return arg