# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$26-Feb-2010 14:40:38$"


from javax import swing
from java import awt
from java import lang
from java import io

import Util
import Data.Problem

import Table.OutputTable#w#
import Table.ExpressTable #w#
import Table.NutrientTable#w#


DIRORDER  = [Data.Problem.MAX, Data.Problem.MIN]

COMMENTS  = Util.COMMENTS
DIRECTION = Util.DIRECTION
OBJECTIVE = Util.OBJECTIVE
LIST      = Util.LIST
REACTION  = Util.REACTION
FROM      = Util.FROM
TO        = Util.TO
STEP      = Util.STEP
REACTINV  = Util.REACTINV
SCALEFAC  = Util.SCALEFAC
VMAXLABEL = Util.VMAXLABEL
REDUCE    = Util.REDUCE
SOLVER    = Util.SOLVER
NUMBER    = Util.NUMBER
FLUXRANGE = Util.FLUXRANGE
EXPRESSION= Util.EXPRESSIONC
THRESHOLD = Util.THRESHOLD
CONSTRAINT= Util.CONSTRAINT

WRITE     = 'Write'
SOLVE     = 'Solve'
SHOWCOMMENTS = True

ARGDELIM = ','

BORDERWIDTH = 5
FIELDWIDTH  = 50
PANELHIGHT = 25

super = swing.JPanel#Box
class WriterPanel(super):
    def __init__(self):
#        super.__init__(self, swing.BoxLayout.LINE_AXIS)
        super.__init__(self)
        self.LayOut()

    def LayOut(self):
        self.setLayout(swing.BoxLayout(self, swing.BoxLayout.X_AXIS))
        
    def getDir(self):
        return None

    def getObj(self):
        return None

    def getArgs(self):
        return []


class ObjWriterPanel(WriterPanel):

    def __init__(self):
        WriterPanel.__init__(self)
        self.dirlabel = swing.JLabel(DIRECTION)
        self.add(self.dirlabel)
        self.dirbox = swing.JComboBox(DIRORDER)
        self.add(self.dirbox)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.objlabel = swing.JLabel(OBJECTIVE)
        self.add(self.objlabel)
        self.objfield = swing.JTextField()
        self.add(self.objfield)


    def getDir(self):
        return self.dirbox.getSelectedItem()

    def getObj(self):
        return self.objfield.getText()

    def getArgs(self):
        return None


class ArgWriterPanel(WriterPanel):

    def __init__(self, label = REACTION):
        WriterPanel.__init__(self)
        self.arglabel = swing.JLabel(label)
        self.add(self.arglabel)
        self.argfield = swing.JTextField()
        self.add(self.argfield)


    def getArgs(self):
        return self.argfield.getText().split(ARGDELIM)



class ObjArgWriterPanel(ObjWriterPanel):

    def __init__(self, label = LIST):
        ObjWriterPanel.__init__(self)
        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH * 2, 0)))
        self.argwriter = ArgWriterPanel(label)
        self.add(self.argwriter)

    def getArgs(self):
        return self.argwriter.getArgs()


class ElModeWriterPanel(WriterPanel):

    def __init__(self, reaclabel = REACTINV, scalelabel = SCALEFAC):
        WriterPanel.__init__(self)

        self.reacpanel = swing.Box(swing.BoxLayout.LINE_AXIS)
        self.reacpanel.add(swing.JLabel(reaclabel))
        self.reacfield = swing.JTextField()
        self.reacpanel.add(self.reacfield)
        self.reacpanel.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.add(self.reacpanel, awt.BorderLayout.CENTER)

        self.numscalepanel = swing.Box(swing.BoxLayout.LINE_AXIS)
        self.add(self.numscalepanel, awt.BorderLayout.EAST)

        self.numpanel = swing.Box(swing.BoxLayout.LINE_AXIS)
        self.numpanel.add(swing.JLabel(NUMBER))
        self.numfield = swing.JFormattedTextField(value = lang.Float(1.0))
        self.numfield.setPreferredSize(awt.Dimension(FIELDWIDTH, self.numfield.getHeight()))
        self.numpanel.add(self.numfield)

        self.scalepanel = swing.Box(swing.BoxLayout.LINE_AXIS)
        self.scalepanel.add(swing.JLabel(scalelabel))
        self.scalefield = swing.JFormattedTextField(value = lang.Float(Data.Problem.VMAX))
        self.scalefield.setPreferredSize(awt.Dimension(FIELDWIDTH, self.scalefield.getHeight()))
        self.scalepanel.add(self.scalefield)

        self.optLayout()

    def LayOut(self):
        self.setLayout(awt.BorderLayout())

    def optLayout(self):
        self.numscalepanel.add(self.numpanel)
        self.numscalepanel.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))


    def getArgs(self):
        return [self.reacfield.getText()] + [self.numfield.getText()] + [self.scalefield.getText()]

    def putScalePanel(self, add = True):
        if add:
            self.numscalepanel.add(self.scalepanel)
        else:
            self.numscalepanel.remove(self.scalepanel)
        self.validate()


class MxsWriterPanel(ElModeWriterPanel):

    def __init__(self):
        ElModeWriterPanel.__init__(self, reaclabel = OBJECTIVE, scalelabel = VMAXLABEL)
        self.putScalePanel(True)
#    def optLayout(self):
#        self.numscalepanel.add(self.scalepanel)
#        self.numscalepanel.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
#        self.numscalepanel.add(self.numpanel)

    def getDir(self):
        return Data.Problem.MAX

    def getObj(self):
        return self.reacfield.getText()

    def getArgs(self):
        return [self.numfield.getText()] + [self.scalefield.getText()]

    

class PlotWriterPanel(ObjArgWriterPanel):

    def __init__(self):
        ObjArgWriterPanel.__init__(self, CONSTRAINT)
        theclass = swing.JTextField
        self.args = Util.OrderedDict.OrderedDict([
            (FROM   , theclass()),
            (TO     , theclass()),
            (STEP   , theclass()),
        ])
        for label in self.args:
            self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
            self.add(swing.JLabel(label))
            self.add(self.args[label])
        self._3D = False

    def getArgs(self):
        #return ObjArgWriterPanel.getArgs(self) + [self.args[label].getText().strip() for label in self.args]
        args = ObjArgWriterPanel.getArgs(self) + [self.args[label].getText().strip() for label in self.args]
        splits = [arg.split(' ') for arg in args]
        rv = [s[0] for s in splits]
        if self._3D:
            rv += [s[-1] for s in splits]
        return rv


class GFAWriterPanel_gfaa(ArgWriterPanel):#w#

    def __init__(self):
        WriterPanel.__init__(self)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.explabel = swing.JLabel(EXPRESSION)
        self.add(self.explabel)
        self.expfield = swing.JTextField()
        self.expfield.setMinimumSize(awt.Dimension(1, 0))
        self.expfield.setPreferredSize(awt.Dimension(120, 0))
        self.expfield.setMaximumSize(awt.Dimension(120, 50))
        self.add(self.expfield)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.thrlabel = swing.JLabel(THRESHOLD)
        self.add(self.thrlabel)
        self.thrfield = swing.JTextField('1.0')
#        self.thrfield = swing.JFormattedTextField('1.0')
        self.thrfield.setMinimumSize(awt.Dimension(1, 0))
        self.thrfield.setPreferredSize(awt.Dimension(50, 0))
        self.thrfield.setMaximumSize(awt.Dimension(50, 50))
        self.add(self.thrfield)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.argwriter = ArgWriterPanel('Reactions/Genes: ')
        self.add(self.argwriter)

    def getObj(self):
        return self.expfield.getText()

    def getArgs(self):
        return self.argwriter.getArgs()

    def getThr(self):
        return self.thrfield.getText()

class GFAWriterPanel_gimme(GFAWriterPanel_gfaa):#w#
    def __init__(self):
        GFAWriterPanel_gfaa.__init__(self)
        self.argwriter.arglabel.setText('Reactions: ')
        self.thrfield.setText('12.0')

class GFAWriterPanel_gfva(GFAWriterPanel_gfaa):#w#
    def __init__(self):
        GFAWriterPanel_gfaa.__init__(self)
        self.remove(self.argwriter)
        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH * 2, 0)))
        self.argwriter = ArgWriterPanel('Reactions: ')
        self.add(self.argwriter)

class GFAWriterPanel_gfva2(GFAWriterPanel_gfva):#w#
    def __init__(self):
        GFAWriterPanel_gfva.__init__(self)
        self.remove(self.thrlabel)
        self.remove(self.thrfield)
        self.remove(self.argwriter)
        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH * 2, 0)))
        self.argwriter = ArgWriterPanel('Reactions: ')
        self.add(self.argwriter)

class GFAWriterPanel_gim3e(ArgWriterPanel):#w#
    def __init__(self):
        WriterPanel.__init__(self)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.objlabel = swing.JLabel(OBJECTIVE)
        self.add(self.objlabel)
        self.objfield = swing.JTextField()
        self.objfield.setMinimumSize(awt.Dimension(1, 0))
        self.objfield.setPreferredSize(awt.Dimension(120, 0))
        self.objfield.setMaximumSize(awt.Dimension(120, 50))
        self.add(self.objfield)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.explabel = swing.JLabel(EXPRESSION)
        self.add(self.explabel)
        self.expfield = swing.JTextField()
        self.expfield.setMinimumSize(awt.Dimension(1, 0))
        self.expfield.setPreferredSize(awt.Dimension(50, 0))
        self.expfield.setMaximumSize(awt.Dimension(50, 50))
        self.add(self.expfield)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.argwriter = ArgWriterPanel('Reactions: ')
        self.add(self.argwriter)

    def getObj(self):
        return self.objfield.getText()+','+self.expfield.getText()

    def getArgs(self):
        return self.argwriter.getArgs()


class GNIWriterPanel(ArgWriterPanel):#w#

    def __init__(self):
        WriterPanel.__init__(self)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.bmlabel = swing.JLabel('Biomass: ')
        self.add(self.bmlabel)
        self.bmfield = swing.JTextField()
        self.bmfield.setMinimumSize(awt.Dimension(1, 0))
        self.bmfield.setPreferredSize(awt.Dimension(120, 0))
        self.bmfield.setMaximumSize(awt.Dimension(120, 50))
        self.add(self.bmfield)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH * 2, 0)))
        self.argwriter = ArgWriterPanel('Genes: ')
        self.add(self.argwriter)

    def getBiom(self):
        return self.bmfield.getText()

    def getArgs(self):
        return self.argwriter.getArgs()

class GNIWriterPanel_wgni(GNIWriterPanel):#w#
    def __init__(self):
        GNIWriterPanel.__init__(self)
        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.samlabel = swing.JLabel(Util.SAMSIZE)
        self.add(self.samlabel)
        self.samfield = swing.JTextField()
        self.samfield.setMinimumSize(awt.Dimension(1, 0))
        self.samfield.setPreferredSize(awt.Dimension(80, 0))
        self.samfield.setMaximumSize(awt.Dimension(80, 50))
        self.add(self.samfield)

    def getSam(self):
        return self.samfield.getText()

class DPAplotWriterPanel(ArgWriterPanel):#w#

    def __init__(self):
        WriterPanel.__init__(self)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH * 2, 0)))
        self.argwriter = ArgWriterPanel('Metabolites: ')
        self.add(self.argwriter)

    def getArgs(self):
        return self.argwriter.getArgs()

class DPAsigWriterPanel(WriterPanel):#w#

    def __init__(self):
        WriterPanel.__init__(self)

        self.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.explabel = swing.JLabel(EXPRESSION)
        self.add(self.explabel)
        self.expfield = swing.JTextField()
        self.add(self.expfield)

    def getObj(self):
        return self.expfield.getText()



PROBLEMS = Util.OrderedDict.OrderedDict([
    ('Objective value', ObjWriterPanel),
    ('Objective value and status', ObjWriterPanel),
    ('FBA', ObjWriterPanel),
    ('FVA', ObjArgWriterPanel),
#    ('FSA', ObjArgWriterPanel),
    ('Knock-out analysis', ObjArgWriterPanel),
    ('Essential reactions', ObjArgWriterPanel),#5
    ('Plot', PlotWriterPanel),
    ('Plot 3D', PlotWriterPanel),
    ('Minimal substrate sets', MxsWriterPanel),
    ('Minimal product sets', MxsWriterPanel),#
    ('Elementary modes', ElModeWriterPanel),#10
    ('Live reactions', ArgWriterPanel),
#    ('Bottlenecks', ObjArgWriterPanel),
    ('Unconserved metabolites', WriterPanel),
    ('Orphan metabolites', WriterPanel),
    ('Connected components', WriterPanel),
    (Util.GFAA, GFAWriterPanel_gfaa),#w#15
   # ('GFVA', GFAWriterPanel_gfva),#w#
    (Util.GFVA2, GFAWriterPanel_gfva2),#w#
#    (Util.GFVA3, GFAWriterPanel_gfva2),#w# 
    (Util.GIMME, GFAWriterPanel_gimme),#w#
    (Util.GIMME2, GFAWriterPanel_gimme),#w#
    (Util.GIM3E, GFAWriterPanel_gim3e),#w# 19
    ('SGNI', GNIWriterPanel),#w# 20
    ('WGNI', GNIWriterPanel_wgni),#w#
    ('DPAplot', DPAplotWriterPanel),#w#
    ('DPAsig', DPAsigWriterPanel),#w# 23
    ('QSSPN', WriterPanel), #w#
])
DEFAULT = 'Objective value'
LPPROBLEMS = PROBLEMS.getOrder()[5:11]+[Util.GIMME, Util.GIMME2]
GRBPROBLEMS = PROBLEMS.getOrder()[:5]+PROBLEMS.getOrder()[15:17]+PROBLEMS.getOrder()[19:23]#w# for gurobi problem




super = swing.JPanel
class ProblemPanel(swing.JPanel):

    def __init__(self):

        super.__init__(self, layout = awt.BorderLayout())
        
        self.arglayout = awt.CardLayout()
        self.argpanel = swing.JPanel(layout = self.arglayout)
        self.add(self.argpanel, awt.BorderLayout.CENTER)
#        self.setMaximumSize(awt.Dimension(self.getWidth(), 25))#t#

        self.program = DEFAULT
        self.writers = {}
        self.addWriters()
        self.addOptions()
        self.switch(self.program)
        self.addOptions()


    def addWriters(self):
        panels = {}
        for panelclass in set(PROBLEMS.values()):
            panels[panelclass] = panelclass()

        for problem in PROBLEMS:
            panelclass = PROBLEMS[problem]
            self.writers[problem] = panels[panelclass]
            self.argpanel.add(self.writers[problem], str(panelclass))

    def addOptions(self):
        self.optpanel = swing.JPanel(layout = awt.BorderLayout())
        self.add(self.optpanel, awt.BorderLayout.EAST)

        self.commentcheck = swing.JCheckBox(COMMENTS, horizontalTextPosition = swing.SwingConstants.LEFT)
        self.commentcheck.setSelected(SHOWCOMMENTS)
        self.optpanel.add(self.commentcheck, awt.BorderLayout.EAST)

#        self.grbcheck = swing.JCheckBox('Gurobi:', horizontalTextPosition = swing.SwingConstants.LEFT)#w#
#        self.grbcheck.setSelected(False)#w#

        self.LPbox = swing.Box(swing.BoxLayout.LINE_AXIS)
        self.LPbox.add(swing.Box.createRigidArea(awt.Dimension(2 * BORDERWIDTH, 0)))
        self.LPbox.add(swing.JLabel(SOLVER))

        self.solverbox = swing.JComboBox(Data.Problem.SOLVERS)
        self.solverbox.addActionListener(self.SolverSwitcher(self))

        self.LPbox.add(self.solverbox)
        self.LPbox.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
        self.trimcheck = swing.JCheckBox(REDUCE, horizontalTextPosition = swing.SwingConstants.LEFT)
        self.LPbox.add(self.trimcheck)
        self.LPbox.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH, 0)))
 
        self.optpanel.add(self.LPbox, awt.BorderLayout.WEST)#CENTER->WEST



    def switch(self, s):
        self.program = s
        self.writers['Plot']._3D = (s == 'Plot 3D')
        cn = str(PROBLEMS[s])
        self.arglayout.show(self.argpanel, cn)
        self.addLPbox()#w#
#        if self.program in GRBPROBLEMS:#w# if gurobi gsa problem, add gurobi checkbox
#            self.optpanel.add(self.grbcheck, awt.BorderLayout.CENTER)
#        else:
#            self.optpanel.remove(self.grbcheck)
#            self.grbcheck.setSelected(False)
        
        ismxs = self.program in ['Minimal substrate sets', 'Minimal product sets']
        solver = {True : Data.Problem.MILP, False : Data.Problem.DEFAULT_SOLVER}[ismxs]
        self.solverbox.setSelectedItem(solver)
        self.solverbox.setEnabled(not ismxs)
        if self.program == 'QSSPN': self.optpanel.remove(self.commentcheck)#w#
        else: self.optpanel.add(self.commentcheck, awt.BorderLayout.EAST)#w#
        
        

    def addLPbox(self):
        if self.program in LPPROBLEMS:#w# 
            if self.solverbox.getItemCount()>3: self.solverbox.removeItem('Gurobi')
            self.optpanel.add(self.LPbox,awt.BorderLayout.WEST)#awt.BorderLayout.WEST
        elif self.program in GRBPROBLEMS:#w#
            if self.solverbox.getItemCount()<4: self.solverbox.addItem('Gurobi')
            self.optpanel.add(self.LPbox,awt.BorderLayout.WEST)#awt.BorderLayout.WEST
        else:
            self.optpanel.remove(self.LPbox)
        self.validate()


        
    def getProblem(self, nutset=Table.NutrientTable.NutrientSet()):#w#
        panel = self.writers[self.program]
        if self.program in ('SGNI','WGNI'):
            nuts_test, nuts_pre=nutset.getNutSetting()
            return Data.Problem.Problem(panel.getDir(), nuts_test, panel.getArgs(), nuts_pre)
        else: return Data.Problem.Problem(panel.getDir(), panel.getObj(), panel.getArgs())


    def getCmdArray(self, sparam, bfile=True):
        if self.program in (Util.GFAA, Util.GIMME, Util.GIMME2): thr = self.writers[self.program].getThr()#w#
        else: thr=False
        if self.program == 'WGNI': size = self.writers[self.program].getSam()#w#
        else: size=False
        if self.program in ('SGNI','WGNI'): biom=self.writers[self.program].getBiom()#w#
        else: biom=''
        cmdarray = Data.Problem.getCmdArray(
            solver = self.solverbox.getSelectedItem(),
            program = self.program,
            trim = self.trimcheck.isSelected(),
            withcomments = self.commentcheck.isSelected(),
            target = biom,#w#
            thresh = thr,#w#
            samsize = size,#w#
            sep = io.File.separator,
            spar = sparam, #w#
            bfile = bfile
        )
        return cmdarray

    def commentsChecked(self):
        return self.commentcheck.isSelected()


    class SolverSwitcher(awt.event.ActionListener):
        def __init__(self, parent):
            self.parent = parent

        def actionPerformed(self, evt):
            add = self.parent.solverbox.getSelectedItem() == Data.Problem.MILP
            self.parent.writers['Elementary modes'].putScalePanel(add)

            

