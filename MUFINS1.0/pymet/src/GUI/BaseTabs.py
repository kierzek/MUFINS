# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="John"
__date__ ="$23-Feb-2014 19:11:31$"

from javax import swing
from java import awt

import Util
import Graph
import GUI #w#
import Solver
import Data.Problem
import Data.XGMML
import Data.SBML#w#
import Data.XLS
import PlotPanel

PROBLEM = 'Problem'
OUTPUT  = 'Output'
GRAPH   = 'Network'
ERRORMSG = 'Solver error'
ID_COL = 0
CLOSELABEL = 'x'
CLOSESIZE  = 15
BARTABLES  = ['FBA', 'Elementary modes', 'FSA','Knock-out analysis']#w#
BARTABLES1  = ['DPAplot', 'DPAsig']#w#
FVATABLES  = ['FVA', Util.GFVA2, Util.GFVA3]#w#
PLOTTABLES = ['Plot']#,
PLOT3DTABLES = ['Plot 3D']
MXSTABLES  = ['Minimal substrate sets', 'Minimal product sets']
EXPRESSION = 'Expression'#w#
ENDTAG = ';'#w#
TABENDTAG = '///'#w#


class BaseTabs(swing.JTabbedPane):
    def __init__(self, listener):
        self.solver = None
        self.program = None#w#
        self.outputs=[]
        self.curtable = [None]#w#
        self.listener = listener

    def setColWidths(self, width):
        for i in range(self.getTabCount()):
            table = self.getTableAt(i)
#            table.setColWidths(width)
            table.setColWidths_gsa(width)#w#

    def remove(self, component):
        swing.JTabbedPane.remove(self, component)
        if component in self.outputs:
            self.outputs.remove(component)

#    def getSelectedTable(self):
#        rv = self.getSelectedComponent()
#        if rv.__class__ == swing.JScrollPane:
#            rv = rv.getViewport().getView()
#        elif rv.__class__ == swing.JSplitPane: #w#
##            rv = self.getSplitpaneTable(rv)
#            rv = self.curtable[0]
#        elif rv.__class__ == swing.JPanel:#w# Jpanel conatin 2 tables
#            rv=rv.getComponents()[1].getViewport().getView()#select second table
#        return rv

    def getSelectedTable(self, rv=None): #w# rv loop judge
        if rv: rv = rv.getSelectedComponent()
        else: rv = self.getSelectedComponent()
        if rv.__class__ == swing.JScrollPane:
            rv = rv.getViewport().getView()
        elif rv.__class__ == swing.JSplitPane: #w#
#            rv = self.getSplitpaneTable(rv)
            rv = self.curtable[0]
        elif rv.__class__ == swing.JPanel:#w# Jpanel conatin 2 tables
            rv=rv.getComponents()[1].getViewport().getView()#select second table
        elif rv.__class__ not in [Graph.JGraphPanel.JGraphPanel, Graph.JGraphPanel.ReGraphPanel, GUI.PlotPanel.BarPanel.ShowPanel]:#w# for results in records of tabbedPane
            rv=self.getSelectedTable(rv)
        return rv

    def getSplitpaneTable(self, sp): #w#
        for x in [sp.getLeftComponent(), sp.getRightComponent(), sp.getTopComponent(), sp.getBottomComponent()]:
            if x.__class__ == swing.JScrollPane:
                table=x.getViewport().getView()#get table
                if not table.getSelectedRow() == -1: return table
            elif x.__class__ == swing.JSplitPane:
                return self.getSplitpaneTable(x)

    def getTableAt(self, title_or_index):
        if type(title_or_index) in Util.StringTypes:
            title_or_index  = self.indexOfTab(title_or_index)
        rv = self.getComponentAt(title_or_index)
        while rv.__class__ not in [swing.JScrollPane, swing.JPanel, Graph.JGraphPanel.JGraphPanel, Graph.JGraphPanel.ReGraphPanel, GUI.PlotPanel.BarPanel.ShowPanel]:#w#, #wtr#ReGraphPanel
            rv=rv.getSelectedComponent()
        if rv.__class__ == swing.JScrollPane:
            rv = rv.getViewport().getView()
        elif rv.__class__ == swing.JPanel:#w# Jpanel conatin 2 tables
            rv=rv.getComponents()[1].getViewport().getView()#select second table
        return rv

    def saveTable(self, path):
        self.getSelectedTable().save(path)

    def printTable(self):
        self.getSelectedTable().print()

    def insertElement(self, ID):
        tm = self.getSelectedTable().getModel()
        tm.insert(ID)

    def insertRows(self):
        self.getSelectedTable().insertRows()

    def deleteRows(self):
        self.getSelectedTable().deleteSelectedRows()

    def filterByRegEx(self, re):
        self.getSelectedTable().filterByRegEx(re) #w# use getSelectedTable(self, rv)

    def getSelectedIDs(self):
        IDs = self.getSelectedTable().getSelectedValues(ID_COL)
        IDs = set(IDs).intersection(self.metabolicModel.keys())
        return list(IDs)

    def allSelectedLists(self):
        rv = []
        for i in range(self.getTabCount()):
            table = self.getTableAt(i)
            try:
                rv += table.getSelectedLists()
            except:
                pass
        return list(rv)

    def showAll(self):
        self.getSelectedTable().unFilter()

    def reportError(self, e):
        swing.JOptionPane.showMessageDialog(self, str(e), ERRORMSG, swing.JOptionPane.ERROR_MESSAGE)
#        if self.listener:
#            self.listener()

    def stopSolution(self):
        if self.solver != None:
            self.solver.stop()


super = swing.Box
class TabCloser(super):
    BORDERWIDTH = 5

    def __init__(self, name, fun = None):
        super.__init__(self, swing.BoxLayout.LINE_AXIS)
        self.add(swing.JLabel(name))
        self.add(swing.Box.createRigidArea(awt.Dimension(self.BORDERWIDTH, 0)))

        button = swing.JButton(CLOSELABEL, actionPerformed = self.close)
        button.setContentAreaFilled(False)
        button.setBorder(swing.BorderFactory.createEtchedBorder())
        button.setPreferredSize(awt.Dimension(CLOSESIZE, CLOSESIZE))
        self.add(button)
        self.fun = fun
        self.name = name

    def put(self, component, pane):
        pane.addTab(self.name, component)
        ind = pane.getTabCount() - 1
        pane.setTabComponentAt(ind, self)
        pane.setSelectedIndex(ind)
        self.component = component
        self.pane = pane

    def close(self, e = None):
        p=self.pane.indexOfComponent(self.component)-5 #wtr#
        self.pane.remove(self.component)
        if self.fun != None:
            self.fun()

