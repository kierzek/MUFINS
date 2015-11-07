import Data.Problem
__author__="Albert Gevorgyan"
__date__ ="$03-Mar-2010 16:05:19$"


from javax import swing#w#
from java import awt#w#
import Util
import Util.OrderedDict
import PyMetTable
import Data.Tabular
import re#w#

BORDERWIDTH = 5
STATUSDELIM = ':'
ASSIGNSYMB = '='#w#
TAB = Util.TAB
ENDOFRECORD = 'END'




class OutputTableModel(PyMetTable.PyMetTableModel):
    super = PyMetTable.PyMetTableModel
    
    def __init__(self, output = None, metabolicModel = None):
        self.super.__init__(self)
        self.metabolicModel = metabolicModel
        if output != None:
            for line in output:
                row = self.emptyRow()
                line, comment = Data.Tabular.splitLineAndComment(line)
                toks = self.split(line)
                for i in range(len(toks)):
                    tok = toks[i]
                    if not tok.isspace() and len(tok) > 0:
                        row[i] = self.fields.getValueAt(i)(tok)
                if comment: row[-1] = self.formatComment(comment, toks)
                self.lines.append(row)
#                print self.lines#t#
        
    def getColumnName(self, c):
        return self.super.getColumnName(self, c).strip().strip(':')

    def emptyRow(self):
        rv = self.super.emptyRow(self)
        for i in range(len(rv)):
            if rv[i].__class__ == Util.PyMetFloat:
                rv[i] = Util.PyMetFloat(Util.NaN)
        return rv

    def formatComment(self, comment, toks = []):
        return Util.CommentType(comment)


super = OutputTableModel

class ObjStatTableModel(super):
    fields = Util.OrderedDict.OrderedDict([
        (Util.OBJVALUE, Util.PyMetFloat),
        (Util.STATUS, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])

    def split(self, line):
        rv = []
        toks = line.split(TAB)
        for tok in toks:
            if STATUSDELIM in tok:
                rv += tok.split(STATUSDELIM, 1)
            elif ASSIGNSYMB in tok:#w#
                rv.append(tok.split(ASSIGNSYMB, 1)[1].strip())#w#
            else:
                rv.append(tok)
        return rv

class FBATableModel(super):
    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.RATE, Util.PyMetFloat),
        (Util.COMMENTt, Util.CommentType),
    ])
#
#    def getRowValueAt(self, r, c):
#        print r, c, self.lines[r][c]
#        return self.lines[r][c]
#
#    def getColumnClass(self, c):
#        print c, self.colfields[c]
#        return self.colfields[c]




class MXSTableModel(FBATableModel):
#    def getRowValueAt(self, r, c):
#        if c < 2:
#            return FBATableModel.getRowValueAt(self, r, c)

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        ("Substrates", Util.PyMetFloat),
        ("Products", Util.PyMetFloat),
        (Util.COMMENTt, Util.CommentType),
    ])
    def formatComment(self, comment, toks):
        if len(toks) > 1:
            ID = toks[0]
            if ID in self.metabolicModel:
                val = toks[1].strip()
                if len(val) == 0:
                    val = toks[2]
                comment = ' ' + val + ' ' + self.metabolicModel[ID].getName()
        return Util.CommentType(comment)


class KOTableModel(ObjStatTableModel):

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.RATE, Util.PyMetFloat),
        (Util.STATUS, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])

class GimmeTableModel(ObjStatTableModel):#w#

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.RATE, Util.PyMetFloat),
        (Util.STATE, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])

class FVATableModel(ObjStatTableModel):

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.MIN, Util.PyMetFloat),
        (Util.STATUS, Util.StringType),
        (Util.MAX, Util.PyMetFloat),
        (Util.STATUS, Util.StringType),
        (Util.ACTIVITY, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])



class PlotTableModel(super):
    fields = Util.OrderedDict.OrderedDict([
        (Util.CONSTRAINT, Util.PyMetFloat),
        (Util.OBJVALUE, Util.PyMetFloat),
        (Util.COMMENTt, Util.CommentType),
    ])

class Plot3DTableModel(super):
    fields = Util.OrderedDict.OrderedDict([
        (Util.CONSTRAINTS[0], Util.PyMetFloat),
        (Util.CONSTRAINTS[1], Util.PyMetFloat),
        (Util.OBJVALUE, Util.PyMetFloat),
        (Util.COMMENTt, Util.CommentType),
    ])


class ListTableModel(super):
    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])


class OutputTableModel_gsa(ObjStatTableModel):#w#
    def __init__(self, lines = None, metabolicModel = None):
        self.super.__init__(self)
        self.metabolicModel = metabolicModel
        for line in lines:
            row = self.emptyRow()
            line, comment = Data.Tabular.splitLineAndComment(line)
            toks = self.split(line)
            for i in range(len(toks)):
                tok = toks[i]
                if not tok.isspace() and len(tok) > 0:
                    row[i] = self.fields.getValueAt(i)(tok)
            if comment: row[-1] = self.formatComment(comment, toks)
            self.lines.append(row)



class GFAATableModel_reac(OutputTableModel_gsa):#w#
    fields = Util.OrderedDict.OrderedDict([
        (Util.REAC, Util.StringType),
        (Util.ACTIVE, int),
        (Util.STATUS, Util.StringType),
        (Util.INACT, int),
        (Util.STATUS, Util.StringType),
        (Util.ACTLEV, int),
        (Util.STATE, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])

class GFAATableModel_gene(OutputTableModel_gsa):#w#
    fields = Util.OrderedDict.OrderedDict([
        (Util.GENE, Util.StringType),
        (Util.ACTIVE, int),
        (Util.STATUS, Util.StringType),
        (Util.INACT, int),
        (Util.STATUS, Util.StringType),
        (Util.ACTLEV, int),
        (Util.EXPLEV, Util.StringType),
        (Util.POSTREG, Util.StringType),
    ])

class GFVATableModel_obj(OutputTableModel_gsa):#w#

    fields = Util.OrderedDict.OrderedDict([
        (Util.OBJV, Util.StringType),
        (Util.STATUS, Util.StringType),
        (Util.OBJF, Util.StringType),
    ])

class GFVATableModel_reac(OutputTableModel_gsa):#w#

    fields = Util.OrderedDict.OrderedDict([
        (Util.REAC, Util.StringType),
        (Util.MIN, Util.PyMetFloat),
        (Util.STATUS, Util.StringType),
        (Util.MAX, Util.PyMetFloat),
        (Util.STATUS, Util.StringType),
        (Util.ACTIVITY, int),
        (Util.STATE, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])

class SGNITableModel_blp(OutputTableModel_gsa):#w#
    def __init__(self, lines = None, media=None, metabolicModel = None):
        self.fields=Util.OrderedDict.OrderedDict([
            (Util.GENE, Util.StringType),
            (Util.KO, Util.StringType),
            (Util.BLPDROP, Util.StringType),
            (Util.BLPLEV, Util.StringType)]+\
            [(nut, Util.StringType) for nut in media])
        OutputTableModel_gsa.__init__(self, lines, metabolicModel)


class SGNITableModel_gni(OutputTableModel_gsa):#w#
    def __init__(self, lines = None, media=None, metabolicModel = None):
        self.fields=Util.OrderedDict.OrderedDict([
            (Util.GENE, Util.StringType),]+\
            [(nut, Util.StringType) for nut in media])
        OutputTableModel_gsa.__init__(self, lines, metabolicModel)


class WGNITableModel_out(OutputTableModel_gsa):#w#
    fields = Util.OrderedDict.OrderedDict([
        (Util.GENE, Util.StringType),
        (Util.KO, Util.StringType),
        (Util.NUMESS, Util.StringType),
    ])

class WGNITableModel_gni(OutputTableModel_gsa):#w#
    def __init__(self, lines = None, media=None, metabolicModel = None):
        fids=[(Util.GENE, Util.StringType)]
        for nut in media:
            fids+=[(nut+'(WP)', int),(nut+'(WN)', int)]
        self.fields=Util.OrderedDict.OrderedDict(fids)
        OutputTableModel_gsa.__init__(self, lines, metabolicModel)

class DPATableModel_m2g(OutputTableModel_gsa):#w#
    fields = Util.OrderedDict.OrderedDict([
        (Util.MET, Util.StringType),
        (Util.ESSGENES, Util.StringType),
    ])

class DPATableModel_g2m(OutputTableModel_gsa):#w#
    fields = Util.OrderedDict.OrderedDict([
        (Util.GENE, Util.StringType),
        (Util.KO, Util.StringType),
        (Util.RELMETS, Util.StringType),
    ])

class DPATableModel_mat(OutputTableModel_gsa):#w#
    def __init__(self, lines = None, list=None, metabolicModel = None):
        self.fields=Util.OrderedDict.OrderedDict([
            (Util.MET, Util.StringType),]+\
            [(gene, Util.PyMetFloat) for gene in list])
        OutputTableModel_gsa.__init__(self, lines, metabolicModel)


MODELS = {
    'Objective value': ObjStatTableModel,
    'Objective value and status': ObjStatTableModel,
    'FBA': FBATableModel,
    'FVA': FVATableModel,
    'FSA': FBATableModel,
    'Elementary modes': FBATableModel,
    'Knock-out analysis': KOTableModel,
    Util.GIMME: GimmeTableModel,
    'Essential reactions': ListTableModel,
    'Live reactions': ListTableModel,
    'Plot': PlotTableModel,
    'Plot 3D': Plot3DTableModel,
    'Minimal substrate sets': MXSTableModel,
    'Minimal product sets': MXSTableModel,
    'Bottlenecks': ListTableModel,
    'Unconserved metabolites': ListTableModel,
    'Connected components' : ListTableModel,
    'Orphan metabolites' : ListTableModel,
    'GFAAreac': GFAATableModel_reac,#w#
    'GFAAgene': GFAATableModel_gene,#w#
    'GFVAobj': GFVATableModel_obj,#w#
    'GFVAreac': GFVATableModel_reac,#w#
    'SGNIblp': SGNITableModel_blp,#w#
    'SGNIgni': SGNITableModel_gni,#w#
    'WGNIout': WGNITableModel_out,#w#
    'WGNIgni': WGNITableModel_gni,#w#
    'DPAm2g': DPATableModel_m2g,#w#
    'DPAg2m': DPATableModel_g2m,#w#
    'DPAmat': DPATableModel_mat,#w#
    'DPAsig': DPATableModel_mat,#w#
    Util.GIM3E: FVATableModel,#w#
}


super = PyMetTable.PyMetTable
class OutputTable(super):
    def __init__(self, metabolicModel = None):
        super.__init__(self, OutputTableModel())
        self.metabolicModel = metabolicModel
        
    def write(self, output, program):
        model = MODELS[program](output, self.metabolicModel)#, writeresult = program in STATUSPROGS)
        self.setModel(model)
        
    def write_gni(self, output, program, list):#w#
        model = MODELS[program](output, list, self.metabolicModel)#, writeresult = program in STATUSPROGS)
        self.setModel(model)


    def getSelectedGraphValue_FBA(self):#w#
        value={}
        model=self.getModel()
        for r in self.getSelectedRows():
            rxn=model.getRowValueAt(r,0)
            va=model.getRowValueAt(r,1)
            va=Util.formatFloat(va)
            value[rxn]=va
        return value

    def getSelectedGraphValue_FVA(self):#w#
        value={}
        model=self.getModel()
        for r in self.getSelectedRows():
            rxn=model.getRowValueAt(r,0)
            va=[Util.formatFloat(model.getRowValueAt(r,x)) for x in [1,3]]
            value[rxn]='~'.join(va)
        return value

    def getSelectedGraphValue_GFAA(self):#w# shlomi
        value={}
        model=self.getModel()
        for r in self.getSelectedRows():
            rxn, dir=model.getRowValueAt(r,0).split(' ')
            dir={'(+)':'f','(-)':'r'}[dir]
            lev=model.getRowValueAt(r,5)
            if rxn in value: value[rxn]+=','+str(lev)+dir
            else: value[rxn]=str(lev)+dir
        return value

    def getSelectedGraphValue_GFVA2(self):#w# fshlomi
        value={}
        ttf={}
        ttr={}
        model=self.getModel()
        for r in self.getSelectedRows():
            rxn=model.getRowValueAt(r,0)
            min=model.getRowValueAt(r,1)
            max=model.getRowValueAt(r,3)
            lev=model.getRowValueAt(r,5)
            print 'lev:',type(lev)#t#
            if re.search('__r$',rxn): ttr[rxn.rstrip('__r')]=lev #reverse
            elif float(min)>=0: ttf[rxn]=lev #forward
            elif float(max)<=0: #only reverse
                if lev==1: value[rxn]='r'
                else: value[rxn]=str(lev)
        for rxn in ttf:
            levf=ttf[rxn]
            if levf==1: value[rxn]='f' #must forward
            elif rxn in ttr:
                levr=ttr[rxn]
                if levr==1: value[rxn]='r' #must reverse
                elif levf>=levr: value[rxn]=str(levf)
                else: value[rxn]=str(levr)
            else: value[rxn]=str(levf)
        return value

    def getSelectedGraphValue_GFVA3(self):#w# lshlomi, gimmefva
        value={}
        ttf={}
        ttr={}
        model=self.getModel()
        for r in self.getSelectedRows():
            rxn=model.getRowValueAt(r,0)
            lev=model.getRowValueAt(r,5)
            if re.search('__r$',rxn): ttr[rxn.rstrip('__r')]=lev
            else: ttf[rxn]=lev
        for rxn in ttf:
            levf=ttf.get(rxn,-1)
            levr=ttr.get(rxn,-1)
            if levf==1 and levr==1: value[rxn]='0' #active in forward and reverse is undetermined
            elif levf==1: value[rxn]='f'
            elif levr==1: value[rxn]='r'
            elif levf>=levr: value[rxn]=str(levf)
            else: value[rxn]=str(levr)
        return value

    def getSelectedGraphValue_GIMME(self):#w# gimme
        value={}
        model=self.getModel()
        for r in self.getSelectedRows():
            rxn=model.getRowValueAt(r,0)
            va=model.getRowValueAt(r,1)
            if re.search('__r$',rxn):
                rxn=rxn.rstrip('__r')
                va=-va
            if rxn in value: value[rxn]+=va #flux = flux_f - flux_r
            else: value[rxn]=va
        for k,v in value.items(): value[k]=Util.formatFloat(v)
        return value
    
#    def getSelectedGraphValue(self, program):
#        value={}
#        for r in self.getSelectedRows():
#            rxn=self.getValueAt(r,0)
#            if program in (Util.GFAA, Uitl.GFVA2, Uitl.GFVA3, Util.GIMME2):
#                vas=int(self.getValueAt(r,5))
#            elif program=='FVA':
#                vas=[float(self.getValueAt(r,x)) for x in [1,3]]
#            else: vas=float(self.getValueAt(r,1))
#            for va in vas:
#                if type(va)==float:
#                    if abs(va)<1e-10: va=0#w#
#                    elif va<1e-4: va='%.e'%va#w#
#                    else: va='%.4f'%va #w#
#                value[rxn]+=[va]
#        return value



#w# set renderer for show red SN/green SP for SGNI
class SGNIRenderer(swing.table.TableCellRenderer):

    def getTableCellRendererComponent(self, table, value, isSelected, hasFocus, r, c):
        label=swing.JLabel('',swing.JLabel.CENTER)
        if c>0:
            if value=='SP':
                label.setText('SP')
                label.setBackground(awt.Color.GREEN.brighter())
            elif value=='SN':
                label.setText('SN')
                label.setBackground(awt.Color.RED.brighter())
        label.setOpaque(True)
        return label














#super = swing.JPanel
#class OutputPanel(swing.JPanel):
#
#    def __init__(self):
#        fieldseditable = True
#        super.__init__(self, layout = awt.BorderLayout())
#        self.objfield = swing.JTextField(editable = fieldseditable)
#        self.statusfield = swing.JTextField(editable = fieldseditable)

#        panel = swing.JPanel(layout = awt.GridLayout())
#        objpanel = swing.Box(swing.BoxLayout.LINE_AXIS, border = swing.BorderFactory.createEmptyBorder(0, 0, 0, BORDERWIDTH))
#        objpanel.add(swing.JLabel(OBJVALUE))
#        objpanel.add(self.objfield)
#        panel.add(objpanel)
#        statuspanel = swing.Box(swing.BoxLayout.LINE_AXIS, border = swing.BorderFactory.createEmptyBorder(0, BORDERWIDTH, 0, 0))
#        statuspanel.add(swing.JLabel(STATUS))
#        statuspanel.add(self.statusfield)
#        panel.add(statuspanel)
#        self.add(panel, awt.BorderLayout.NORTH)

#        self.table = OutputTable(PyMetTable.PyMetTableModel())
#        self.add(swing.JScrollPane(self.table), awt.BorderLayout.CENTER)
#
#
#    def write(self, output, program):
#        self.program = program
#        model = MODELS[program](output, writeresult = program in STATUSPROGS)
#        self.table.setModel(model)

#
#    def writeObjStat(self, line):
#        obj, status = '', ''
#        if not self.program in NOSTATUSPROGS:
#            obj, status = line, ''
#            if STATUSDELIM in obj:
#                obj, status = obj.split(STATUSDELIM, 1)
#        self.objfield.setText(obj.strip().rstrip(ENDTAG))
#        self.statusfield.setText(status.strip().rstrip(ENDTAG))



