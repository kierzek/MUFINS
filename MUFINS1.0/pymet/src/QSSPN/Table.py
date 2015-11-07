import Util.Tool
import QSSPN.Model
# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="John"
__date__ ="$21-Feb-2014 10:33:04$"

from javax import swing #w#
from javax.swing.JOptionPane import showMessageDialog
import Util
from GUI.Table import PyMetTable
from GUI.Table import OutputTable
import re
#**************************************************************************************************
#Get table data

def getDataNode(model):
    data=[]
    for x in model.nodes:
        data.append([model.Node[x].ID, model.Node[x].ini, model.Node[x].max, model.Node[x].type, model.Node[x].moni])
    return data

def getDataInteract(model):
    data=[]
    for x in model.interactions:
        data.append([model.Int[x].ID, model.Int[x].rate, model.Int[x].type, model.Int[x].delay])
    return data

def getDataSubstrate(model, ID): #interaction ID
    data=[]
    if ID:
        interact = model.Int[ID]
        for ID2 in interact.subs:
            if ID2:
                sub = interact[ID2]
                data.append([sub.ID, sub.prod, sub.cons, sub.sto])
    return data

def getDataActivitySub(model, ID1, ID2): #interaction ID1, substrate ID2
    data=[]
    if ID1 and ID2:
        sub=model.Int[ID1][ID2]
        for act in sub.act:
            data.append([act.t, act.a])
    return data

def getDataActivityMet(model, ID): #metabolite ID
    data=[]
    if ID:
        met=model.Met[ID]
        for act in met.act:
            data.append([act.t, act.a])
    return data

def getDataActivityEnz(model, ID): #enzyme ID
    data=[]
    if ID:
        enz=model.Enz[ID]
        for act in enz.act:
            data.append([act.t, act.lb, act.ub])
    return data

def getDataMetabolite(model):
    data=[]
    for x in model.metabolites:
        data.append([model.Met[x].ID, model.Met[x].obj])
    return data

def getDataEnzyme(model):
    data=[]
    for x in model.enzymes:
        data.append([model.Enz[x].ID, model.Enz[x].rxn])
    return data

def getDataRxnList(model, ID): #enzyme ID
    data=[]
    if ID:
        enz=model.Enz[ID]
        for rxn in enz.list:
            data.append([rxn.ID, rxn.wei])
    return data
#***************************************************************************************************
# table model

super = PyMetTable.PyMetTableModel
class QSSPNTableModel(super):
    model=QSSPN.Model.Model()
    def __init__(self, model=None):
        super.__init__(self)
        self.model=model
        self.addTableModelListener(TableModelListenerBase())

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
    ])


class NodeTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None: self.lines = getDataNode(model)

    fields = Util.OrderedDict.OrderedDict([
        (Util.PNNODE, Util.StringType),
        (Util.TOKINI, Util.StringType),
        (Util.TOKMAX, Util.StringType),
        (Util.NODETYPE, Util.StringType),
        (Util.MONITOR, bool),
    ])

    def updateNode(self):
        self.deleteAll()
        self.lines = getDataNode(self.model)
        self.fireTableDataChanged()

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.nodes = Util.Tool.ArrayInsert(self.model.nodes, ['']*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            ID = self.model.nodes[r]
            del self.model.nodes[r]
            if ID: del self.model.Node[ID]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        ID  = self.model.nodes[r]
        if ID: node = self.model.Node[ID]
        else: node = QSSPN.Model.Node() #for insert new
        if c == 0:
            node.ID = change
            self.model.nodes[r] = change
            if ID: self.model.Node[change] = self.model.Node.pop(ID)
            else: self.model.Node[change] = node #for insert new
        elif c == 1: node.ini = change
        elif c == 2: node.max = change
        elif c == 3: node.type = change
        elif c == 4: node.moni = change

class InteractTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None: self.lines = getDataInteract(model)

    fields = Util.OrderedDict.OrderedDict([
        (Util.INTERACT, Util.StringType),
        (Util.RATE, Util.StringType),
        (Util.TRANTYPE, Util.StringType),
        (Util.DELAY, Util.StringType),
    ])

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.interactions = Util.Tool.ArrayInsert(self.model.interactions, ['']*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            ID = self.model.interactions[r]
            del self.model.interactions[r]
            if ID: del self.model.Int[ID]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        ID  = self.model.interactions[r]
        if ID: intact = self.model.Int[ID]
        else: intact = QSSPN.Model.Interaction() #for insert new
        if c == 0:
            intact.ID = change
            self.model.interactions[r] = change
            if ID: self.model.Int[change] = self.model.Int.pop(ID)
            else: self.model.Int[change] = intact #for insert new
        elif c == 1: intact.rate = change
        elif c == 2: intact.type = change
        elif c == 3: intact.delay = change

class SubstrateTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        self.int=model.interactions[0] #current selected interact ID
        if model!= None: self.lines = getDataSubstrate(model, self.int)

    fields = Util.OrderedDict.OrderedDict([
        (Util.SUBSTRATE, Util.StringType),
        (Util.PRODUCT, bool),
        (Util.CONSUMED, bool),
        (Util.RESET, Util.StringType),
    ])

    def showIntSub(self, ID): #interaction ID
        self.deleteAll()
        self.int = ID
        self.lines = getDataSubstrate(self.model, ID)
        if not self.lines and ID: self.insert(1,0)
        self.fireTableDataChanged()

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.Int[self.int].subs = Util.Tool.ArrayInsert(self.model.Int[self.int].subs, ['']*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            ID = self.model.Int[self.int].subs[r]
            del self.model.Int[self.int].subs[r]
            if ID: del self.model.Int[self.int][ID]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        ID  = self.model.Int[self.int].subs[r]
        if ID: sub = self.model.Int[self.int][ID]
        else: sub = QSSPN.Model.Substrate() #for insert new
        if c == 0:
            sub.ID = change
            self.model.Int[self.int].subs[r] = change
            if ID: self.model.Int[self.int][change] = self.model.Int[self.int].pop(ID)
            else: self.model.Int[self.int][change] = sub #for insert new
        elif c == 1: sub.prod = change
        elif c == 2: sub.cons = change
        elif c == 3: sub.sto = change

class ActivitySubTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        self.int=model.interactions[0] #interaction ID
        self.sub=model.Int[self.int].keys()[0] #substrate ID
        if model!= None: self.lines = getDataActivitySub(model, self.int, self.sub)

    fields = Util.OrderedDict.OrderedDict([
        (Util.THRESHOLDa, Util.StringType),
        (Util.ACTIVITY, Util.StringType),
    ])

    def showSubAct(self, ID1, ID2): #interaction ID1; substrate ID2
        self.int = ID1
        self.sub = ID2
        self.deleteAll()
        self.lines = getDataActivitySub(self.model, ID1, ID2)
        if not self.lines and ID2: self.insert(1,0)
        self.fireTableDataChanged()
        
    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.Int[self.int][self.sub].act = Util.Tool.ArrayInsert(self.model.Int[self.int][self.sub].act, [QSSPN.Model.Activity()]*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            del self.model.Int[self.int][self.sub].act[r]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        act = self.model.Int[self.int][self.sub].act[r]
        if c == 0: act.t = change
        elif c == 1: act.a = change

class MetaboliteTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None: self.lines = getDataMetabolite(model)

    fields = Util.OrderedDict.OrderedDict([
        (Util.METABOLITE, Util.StringType),
        (Util.GSMNMET, Util.StringType),
    ])

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.metabolites = Util.Tool.ArrayInsert(self.model.metabolites, ['']*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            ID = self.model.metabolites[r]
            del self.model.metabolites[r]
            if ID: del self.model.Met[ID]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        ID  = self.model.metabolites[r]
        if ID: met = self.model.Met[ID]
        else: met = QSSPN.Model.Metabolite() #for insert new
        if c == 0:
            met.ID = change
            self.model.metabolites[r] = change
            if ID: self.model.Met[change] = self.model.Met.pop(ID)
            else: self.model.Met[change] = met #for insert new
        elif c == 1: intact.obj = change

class ActivityMetTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None:
            self.lines = getDataActivityMet(model, model.metabolites[0])

    fields = Util.OrderedDict.OrderedDict([
        (Util.THRESHOLDa, Util.StringType),
        (Util.ACTIVITY, Util.StringType),
    ])

    def showMetAct(self, ID): #metabolite ID
        self.met = ID
        self.deleteAll()
        self.lines = getDataActivityMet(self.model, ID)
        if not self.lines and ID: self.insert(1,0)
        self.fireTableDataChanged()

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.Met[self.met].act = Util.Tool.ArrayInsert(self.model.Met[self.met].act, [QSSPN.Model.Activity()]*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            del self.model.Met[self.met].act[r]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        act = self.model.Met[self.met].act[r]
        if c == 0: act.t = change
        elif c == 1: act.a = change

class EnzymeTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None: self.lines = getDataEnzyme(model)

    fields = Util.OrderedDict.OrderedDict([
        (Util.ENZYME, Util.StringType),
        (Util.CONSFLUX, Util.StringType),
    ])

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.enzymes = Util.Tool.ArrayInsert(self.model.enzymes, ['']*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            ID = self.model.enzymes[r]
            del self.model.enzymes[r]
            if ID: del self.model.Enz[ID]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        ID  = self.model.enzymes[r]
        if ID: enz = self.model.Enz[ID]
        else: enz = QSSPN.Model.Enzyme() #for insert new
        if c == 0:
            enz.ID = change
            self.model.enzymes[r] = change
            if ID: self.model.Enz[change] = self.model.Enz.pop(ID)
            else: self.model.Enz[change] = enz #for insert new
        elif c == 1: intact.rxn = change

class ActivityEnzTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None: self.lines = getDataActivityEnz(model, model.enzymes[0])

    fields = Util.OrderedDict.OrderedDict([
        (Util.THRESHOLDa, Util.StringType),
        (Util.LB, Util.StringType),
        (Util.UB, Util.StringType),
    ])

    def showEnzAct(self, ID): #enzyme ID
        self.enz = ID
        self.deleteAll()
        self.lines = getDataActivityEnz(self.model, ID)
        if not self.lines and ID: self.insert(1,0)
        self.fireTableDataChanged()

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.Enz[self.enz].act = Util.Tool.ArrayInsert(self.model.Enz[self.enz].act, [QSSPN.Model.ActivityEnz()]*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            del self.model.Enz[self.enz].act[r]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        act = self.model.Enz[self.enz].act[r]
        if c == 0: act.t = change
        elif c == 1: act.lb = change
        elif c == 2: act.ub = change

class RxnListTableModel(QSSPNTableModel):
    def __init__(self, model=None):
        QSSPNTableModel.__init__(self, model)
        if model!= None: self.lines = getDataRxnList(model, model.enzymes[0])

    fields = Util.OrderedDict.OrderedDict([
        (Util.REAC, Util.StringType),
        (Util.WEIGHT, Util.StringType),
    ])

    def showRxnList(self, ID): #enzyme ID
        self.enz = ID
        self.deleteAll()
        self.lines = getDataRxnList(self.model, ID)
        if not self.lines and ID: self.insert(1,0)
        self.fireTableDataChanged()

    def insert(self, n, start):
        newlines = [self.emptyRow() for i in range(n)]
        self.lines = Util.Tool.ArrayInsert(self.lines, newlines, start)
        self.model.Enz[self.enz].list = Util.Tool.ArrayInsert(self.model.Enz[self.enz].list, [QSSPN.Model.RxnList()]*n, start)
        self.fireTableDataChanged()

    def delete(self, rows):
        rows.reverse()
        for r in rows:
            del self.lines[r]
            del self.model.Enz[self.enz].list[r]
        self.fireTableDataChanged()

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        lst = self.model.Enz[self.enz].list[r]
        if c == 0: lst.ID = change
        elif c == 1: lst.wei = change
#---------------------------------Control----------------------------------------
def loadControl(path, model):
        paras=[]
        inistate = []
        function = []
        for node in model.Node.values(): node.moni = 0
        f = open(path,'r')
        line=f.readline()
        while line:
            if re.match('PETRI_NET_MONITORS', line):
                line=f.readline()
                while not re.match('END', line):
                    ID = line.strip()
                    model.Node[ID].moni = 1
                    line=f.readline()
            elif re.match('INITIAL_STATE', line):
                line=f.readline()
                while not re.match('END', line):
                    inistate += [line.strip().split()]
                    line=f.readline()
            elif '_FUNCTION'in line:
                func = line.strip().split()
                line=f.readline()
                func += [line.strip()]
                function += [func]
            elif 'SIMULATION' in line:
                paras.append(['QSSPN_ALGORITHM', line.strip()])
            elif not (re.match('\*/', line) or re.match('/\*', line) or re.match('#', line)):
                if len(line.strip().split(' ',1))==2:
                    paras.append(line.strip().split(' ',1))
            line=f.readline()
        f.close()
        return paras, inistate, function

class ControlParaTableModel(QSSPNTableModel): #for Control file
    def __init__(self, model, lines):
        QSSPNTableModel.__init__(self, model)
        self.monitors = []
        self.lines=[]
        if not lines:
            self.lines = [['MODEL','./gsmn.tmp'],
            ['NUMBER_OF_SAMPLES', '1'],
            ['SEED', '87622134'],
            ['TIME_MAX', '10.0'],
            ['MAXIMAL_TIMESTEP', '0.01'],
            ['OUTPUT', self.model.name+'.output.xls'],
            ['LOG', self.model.name+'.log.txt'],
            ['MONITOR', '1'],
            ['PROGRESS', '1'],
            ['TARGET', ''],
            ['QSSPN_ALGORITHM', 'HYBRID_SIMULATION3'],
            ['GSMN_MODEL', self.model.gsmn.ID],
            ['GSMN_EXT_TAG', self.model.gsmn.extag]]
        else:
            self.lines=lines
            self.lines.append(['GSMN_MODEL', self.model.gsmn.ID])
            self.lines.append(['GSMN_EXT_TAG', self.model.gsmn.extag])

    fields = Util.OrderedDict.OrderedDict([
        (Util.CONTROLPARA , Util.StringType),
        (Util.PARAVALUE, Util.StringType),
    ])

#    def load(self, path):
#        self.lines=[]
#        self.monitors = []
#        for node in self.model.Node.values(): node.moni = 0
#        f = open(path,'r')
#        line=f.readline()
#        while line:
#            if re.match('PETRI_NET_MONITORS', line):
#                line=f.readline()
#                while not re.match('END', line):
#                    ID = line.strip()
#                    self.model.Node[ID].moni = 1
#                    self.monitors += [ID]
#                    line=f.readline()
#            elif re.match('HYBRID_SIMULATION', line):
#                self.lines.append(['QSSPN_ALGORITHM', line.strip()])
#            elif len(line.strip().split(' ',1))==2:
#                self.lines.append(line.strip().split(' ',1))
#            line=f.readline()
#        f.close()
#        self.lines.append(['GSMN_MODEL', self.model.gsmn.ID])
#        self.lines.append(['GSMN_EXT_TAG', self.model.gsmn.extag])
#        self.fireTableDataChanged()
#        self.model.para = dict(self.lines)
    def loadData(self, lines):
        self.lines=lines
        self.lines.append(['GSMN_MODEL', self.model.gsmn.ID])
        self.lines.append(['GSMN_EXT_TAG', self.model.gsmn.extag])
        self.fireTableDataChanged()

#    def getParameter(self):
#        return dict(self.lines), self.monitors

    def updateModelData(self, r, c):
        change = self.getValueAt(r, c)
        if r == 10: self.model.gsmn.ID = change
        elif r == 11: self.model.gsmn.extag = change

class ControlFluxTableModel(QSSPNTableModel): #for Control file
    def __init__(self, model, lines):
        QSSPNTableModel.__init__(self, model)
        self.lines = self.model.gsmn.flux

    fields = Util.OrderedDict.OrderedDict([
        (Util.FLUXTRAN , Util.StringType),
        (Util.FLUXRXN, Util.StringType),
    ])

class ControlInitTableModel(QSSPNTableModel): #for Control file
    def __init__(self, model, lines):
        QSSPNTableModel.__init__(self, model)
        self.lines = []
        if lines: self.lines = lines

    fields = Util.OrderedDict.OrderedDict([
        (Util.SUBSTRATE , Util.StringType),
        (Util.PARAVALUE, Util.StringType),
    ])

class ControlFuncTableModel(QSSPNTableModel): #for Control file
    def __init__(self, model, lines):
        QSSPNTableModel.__init__(self, model)
        self.lines = []
        if lines: self.lines = lines

    fields = Util.OrderedDict.OrderedDict([
        (Util.FUNCTYPE , Util.StringType),
        (Util.TRANSITION , Util.StringType),
        (Util.FORMULA, Util.StringType),
    ])

#--------------------------------------------------------------------------
class QSSPNOutputTableModel(OutputTable.OutputTableModel):#w#
    def __init__(self, lines = None, monitors=None):
        fids=[(Util.TIME, Util.StringType),
              (Util.EVENT, Util.StringType),
        ]
        for x in monitors: fids+=[(x, Util.StringType)]
        self.fields=Util.OrderedDict.OrderedDict(fids)
#        self.lines = [line.split('\t') for line in lines]
        self.super.__init__(self)
        for line in lines:
            row = self.emptyRow()
#            line, comment = Data.Tabular.splitLineAndComment(line)
            toks = line.split('\t')
            for i in range(len(toks)):
                tok = toks[i]
                if not tok.isspace() and len(tok) > 0:
                    row[i] = self.fields.getValueAt(i)(tok)
#            if comment: row[-1] = self.formatComment(comment, toks)
            self.lines.append(row)

def output2table(output):
    samples = []
    output = output.strip().split('\n')
    monitors = output[0].strip().split('\t')[3:]
    lines = []
    for line in output[1:]:
        if not re.match('Trajectory',line):
            tt = line.split('\t',1)
            if len(tt)==2: lines += [tt[1].strip()]
        else:
            samples += [lines]
            lines = []
    samples += [lines]
    return samples, monitors


MODELS = {
    'Node': NodeTableModel,
    'Intact': InteractTableModel,
    'Sub': SubstrateTableModel,
    'ActS': ActivitySubTableModel,
    'Met': MetaboliteTableModel,
    'ActM': ActivityMetTableModel,
    'Enz': EnzymeTableModel,
    'ActE': ActivityEnzTableModel,
    'List': RxnListTableModel,
    'ConPara': ControlParaTableModel,
    'ConInit': ControlInitTableModel,
    'ConFunc': ControlFuncTableModel,
    'ConFlux': ControlFluxTableModel,
    'Output': QSSPNOutputTableModel,
}

#JTable class
class QSSPNTable(PyMetTable.PyMetTable):
    def __init__(self, metModel = None):
        PyMetTable.PyMetTable.__init__(self, PyMetTable.PyMetTableModel())
        self.metModel = metModel

    def write(self, model, tableMod):
        tableModel = MODELS[tableMod](model)#, writeresult = program in STATUSPROGS)
        self.setModel(tableModel)
        self.setRowSorter(PyMetTable.MyTableRowSorter(tableModel))#w#
#        self.getRowSorter.setModel(tableModel)

    def write_control(self, tableMod, model=None, lines=None):
        tableModel = MODELS[tableMod](model, lines)#, writeresult = program in STATUSPROGS)
        self.setModel(tableModel)
        self.setRowSorter(PyMetTable.MyTableRowSorter(tableModel))#w#

    def write_output(self, lines, monitors, tableMod): #write output table
        tableModel = MODELS[tableMod](lines, monitors)#, writeresult = program in STATUSPROGS)
        self.setModel(tableModel)
        

#***************************************************************************************************
#TableModelListener
class SelectionListenerInt(swing.event.ListSelectionListener):
    def __init__(self, table_int, table_sub, table_actS, model, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableI=table_int
        self.tableS=table_sub
        self.tableA=table_actS
        self.model=model
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.tableI
        lsm = e.getSource() #get ListSelectionModel
        if not lsm.isSelectionEmpty():
            minIndex = lsm.getMinSelectionIndex()
            ID1=self.tableI.getValueAt(minIndex, 0) #intact ID
#            ID=self.tableI.getModel().getRowValueAt(minIndex, 0)
            self.tableS.getModel().showIntSub(ID1)
            if ID1 and self.model.Int[ID1].subs:
                ID2 = self.model.Int[ID1].subs[0]
            else: ID2 = ''
            self.tableA.getModel().showSubAct(ID1, ID2)

class SelectionListenerSub(swing.event.ListSelectionListener):
    def __init__(self, table_sub, table_actS, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableS=table_sub
        self.tableA=table_actS
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.tableS
        lsm = e.getSource() #get ListSelectionModel
        if not lsm.isSelectionEmpty():
            minIndex = lsm.getMinSelectionIndex()
            ID1=self.tableS.getModel().int
            ID2=self.tableS.getValueAt(minIndex, 0)
            self.tableA.getModel().showSubAct(ID1, ID2)

class SelectionListenerActS(swing.event.ListSelectionListener):
    def __init__(self, table_actS, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableA=table_actS
        self.curtb = curtable
        
    def valueChanged(self, e):
        self.curtb[0] = self.tableA

class SelectionListenerMet(swing.event.ListSelectionListener):
    def __init__(self, table_met, table_actM, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableM=table_met
        self.tableA=table_actM
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.tableM
        lsm = e.getSource() #get ListSelectionModel
        if not lsm.isSelectionEmpty():
            minIndex = lsm.getMinSelectionIndex()
            ID=self.tableM.getValueAt(minIndex, 0)
            self.tableA.getModel().showMetAct(ID)

class SelectionListenerActM(swing.event.ListSelectionListener):
    def __init__(self, table_actM, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableA=table_actM
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.tableA

class SelectionListenerEnz(swing.event.ListSelectionListener):
    def __init__(self, table_enz, table_actE, table_list, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableE=table_enz
        self.tableA=table_actE
        self.tableL=table_list
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.tableE
        lsm = e.getSource() #get ListSelectionModel
        if not lsm.isSelectionEmpty():
            minIndex = lsm.getMinSelectionIndex()
            ID=self.tableE.getValueAt(minIndex, 0)
            self.tableA.getModel().showEnzAct(ID)
            self.tableL.getModel().showRxnList(ID)

class SelectionListenerActE(swing.event.ListSelectionListener):
    def __init__(self, table_actE, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.tableA=table_actE
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.tableA

class SelectionListenerList(swing.event.ListSelectionListener):
    def __init__(self, table_list, curtable):
        swing.event.ListSelectionListener.__init__(self)
        self.table=table_list
        self.curtb = curtable

    def valueChanged(self, e):
        self.curtb[0] = self.table

class TableModelListenerBase(swing.event.TableModelListener):
    def __init__(self):
        swing.event.TableModelListener.__init__(self)

    def tableChanged(self, e):
        if e.getType()==swing.event.TableModelEvent.UPDATE:
            r = e.getFirstRow()
            c = e.getColumn()
            if c!= -1: #not a insert/delete
                model = e.getSource()
                model.updateModelData(r, c)