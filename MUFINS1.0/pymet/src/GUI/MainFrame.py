import os.path
import Data.XLS
import Data.Merge
# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$25-Feb-2010 14:47:33$"

from javax import swing
from java import awt
from java import lang
from java import io

import Model
import ModelTabs
import ProblemPanel
import Data.Tabular
import Data.SBML
import Table.ExpressTable#w#
import Data.Problem#w#
from Graph import JGraphPanel as GraphPanel
from Graph import *#wtr#
from java.awt.event import *#wtr#

#from com.mxgraph.swing import mxGraphComponent#wtr#
#from com.mxgraph.view  import mxGraph#wtr#
from javax.swing import *#wtr#
import Util#w#
import QSSPN
import QSSPN.Spept
from QSSPN import QSSPNTabs
import BaseTabs#w#


WIDTH           = 0.6
HEIGHT          = 0.6
#NAME            = 'PyMet'
BORDERWIDTH     = ProblemPanel.BORDERWIDTH
OPENMODEL       = 'Open model'
SAVEMODEL       = 'Save model'
#MANUAL          = 'file:doc/Jymet.html'
MANUAL_JYMET          = 'file:docs/Manual.html'
#MANUAL_SFBA          = 'file:doc/Manual_SFBA.html'

CWD = '.'
LOGICALS = ['H2O', 'ATP', 'NAD+', 'NADH', 'NADPH', 'NADP+', 'Oxygen', 'ADP', 'Orthophosphate', 'H+']
XTAG = ''


class MainFrame(swing.JFrame):
    def __init__(self, model = None):
        swing.JFrame.__init__(self)
        self.solver = Data.Problem.DEFAULT_SOLVER
        self.cwd = CWD
        self.initFrame()
        self.initMenu()
        self.initToolBar()
        self.initTabs()
        self.enableMenus()
        if model != None:
            self.addModel(model)
        self.Logicals = LOGICALS
        self.Xtag = XTAG
        self.sparam = '1e-6' #w# solver parameter, currently set tolerance for Gurobi
        self.program=ProblemPanel.DEFAULT#w#
        
                
    def initFrame(self):
        self.setDefaultCloseOperation(swing.JFrame.EXIT_ON_CLOSE)
#        self.setTitle(NAME)
        self.setLayout(awt.BorderLayout())
        d = awt.Toolkit.getDefaultToolkit().getScreenSize();
        self.setBounds(0, 0, int(d.getWidth() * WIDTH), int(d.getHeight() * HEIGHT))

    def initToolBar(self):
        self.toolbar = swing.Box(swing.BoxLayout.LINE_AXIS)
        self.toolbar.setBorder(swing.BorderFactory.createEmptyBorder(BORDERWIDTH, BORDERWIDTH, BORDERWIDTH, BORDERWIDTH));
        self.add(self.toolbar, awt.BorderLayout.NORTH)

#        self.toolbar.add(swing.JLabel(INSERT))
#        self.insertfield = swing.JTextField(actionPerformed = self.insertElement)
#        self.toolbar.add(self.insertfield)

#        self.toolbar.add(swing.Box.createRigidArea(awt.Dimension(BORDERWIDTH * 5, 0)));

        self.problempanel = ProblemPanel.ProblemPanel()
        self.toolbar.add(self.problempanel)


    def initMenu(self):
        menubar = swing.JMenuBar()
        self.menuitems = []

        menu = swing.JMenu('File')
        menu.add(swing.JMenuItem('New model', actionPerformed = self.newModel, toolTipText = "Create a new model"))
        menu.add(swing.JMenuItem(OPENMODEL, actionPerformed = self.openModel, toolTipText = "Open a model from a tab-separated file (input file for FBACLT)"))
        menu.add(swing.JMenuItem('Import SBML', actionPerformed = self.openSBML, toolTipText = "Import SBML file"))

        #menu.add(swing.JMenuItem('Open extended model', actionPerformed = self.openModel, toolTipText = "Open a model"))
        items = [
            #swing.JMenuItem(SAVEMODEL, actionPerformed = self.saveModel, toolTipText = "Save the model as a workbook"),
            #swing.JMenuItem('Save extended model', actionPerformed = self.saveModel, toolTipText = "Save the model"),
            swing.JMenuItem('Save table', actionPerformed = self.saveTable, toolTipText = "Save the selected table in a tab-separated file"),
            swing.JMenuItem('Save graph', actionPerformed = self.saveGraph, toolTipText = "Save the selected graph in a tab-separated file"),#wtr#
#            swing.JMenuItem('Export SBML', actionPerformed = None),
            swing.JMenuItem('Open problem', actionPerformed = self.openProblem, toolTipText = "Open a problem from a tab-separated file"),
#            swing.JMenuItem('Close model', actionPerformed = self.closeModel)
            swing.JMenuItem('Open expression', actionPerformed = self.openExpression, toolTipText = "Open a gene/enzyme expression from a tab-separated file"),#Wataru#
            swing.JMenuItem('Open DPAplot', actionPerformed = self.openDPAplot, toolTipText = "Open a DPAplot output file for DPAsig problem"),#Wataru#
            swing.JMenuItem('Export SBML', actionPerformed = self.writeSBML, toolTipText = "Export to SBML format from SFBA format"),#Wataru#
            swing.JMenuItem('Export XGMML', actionPerformed = self.writeXGMML, toolTipText = "Export to XGMML (for Cytoscape).")
            ]
        for item in items: menu.add(item)
        menu.add(swing.JMenuItem('Print', actionPerformed = self.printTable, toolTipText = "Print the selected table"))
        menu.add(swing.JMenuItem('Exit', actionPerformed = self.exit))
        menubar.add(menu)
        self.menuitems += items

        menu = swing.JMenu('Edit')
        self.menuitems += [menu]
        menu.addSeparator()#wtr#
        menu.add(swing.JMenuItem('Insert rows', actionPerformed = self.insert))
        menu.add(swing.JMenuItem('Delete rows', actionPerformed = self.delete))
        menu.addSeparator();
        menu.add(swing.JMenuItem('Search', actionPerformed = self.filterRegEx, toolTipText = "Regular-expression based row filter"))
        menu.add(swing.JMenuItem('Merge', actionPerformed = self.merge, toolTipText = "Merge with another model"))
        menubar.add(menu)

        menu = swing.JMenu('View')
        self.menuitems += [menu]
        submenu = swing.JMenu('Show')
        submenu.add(swing.JMenuItem('Children', actionPerformed = self.showChildren, toolTipText = "Lower level in hierarchy"))
        submenu.add(swing.JMenuItem('Parents', actionPerformed = self.showParents, toolTipText = "Higher level in hierarchy"))
        submenu.add(swing.JMenuItem('All', actionPerformed = self.showAll, toolTipText = "All rows in the table"))
        menu.add(submenu)
        menu.addSeparator();
#        menu.add(swing.JMenuItem('XY plot', actionPerformed = self.showXY, toolTipText = "Show an XY-plot"))
#        menu.add(swing.JMenuItem('XYZ plot', actionPerformed = self.showXYZ, toolTipText = "Show an XYZ-plot")
        menu.add(swing.JMenuItem('Plot', actionPerformed = self.showPlot, toolTipText = "View plot output as a line plot, or 3D-plot output as a block plot"))

        submenu = swing.JMenu('Layout', toolTipText = "Show a graph representation")
        self.menuitems += [submenu]
        group = swing.ButtonGroup();
        for layoutclass in GraphPanel.LAYOUTCLASSES:
            rbMenuItem = swing.JRadioButtonMenuItem(layoutclass, layoutclass == GraphPanel.DEFAULT_LAYOUT, actionPerformed = self.showGraph);
            group.add(rbMenuItem);
            submenu.add(rbMenuItem);
        submenu.addSeparator()
        submenu.add(swing.JMenuItem('Custom',layoutclass==GraphPanel.DEFAULT_LAYOUT,actionPerformed = self.openGraph))#Wataru#
        submenu.addSeparator()
#        submenu.add(swing.JMenuItem('DOT', actionPerformed = self.dot, toolTipText = "Graphviz must be installed!"))
#        submenu.addSeparator()
        submenu.add(swing.JMenuItem('Logical metabolites', actionPerformed = self.setLogicals, toolTipText = "Define the metabolites to be shown as logical graph nodes"))
        menu.add(submenu)        
        menubar.add(menu)
        
        menu = swing.JMenu('Analyse')
        self.menuitems += [menu]
        group = swing.ButtonGroup()
        for problem in ProblemPanel.PROBLEMS:
            rbMenuItem = swing.JRadioButtonMenuItem(problem, problem == ProblemPanel.DEFAULT,
                actionPerformed = self.switchProgram);
            group.add(rbMenuItem)
            menu.add(rbMenuItem)
        menubar.add(menu)

        menu = swing.JMenu('Solve')#w#
        self.menuitems += [menu]
        menu.add(swing.JMenuItem('Write problem', actionPerformed = self.writeProblem, toolTipText = "Write the problem"))
        menu.add(swing.JMenuItem('Clear problem', actionPerformed = self.clearProblem, toolTipText = "Clear the problem"))
        menu.add(swing.JMenuItem('Externality tag', actionPerformed = self.setExternalTag, toolTipText = "Define the tag indicating external metabolites"))
        menu.addSeparator()
        self.solveMenu = swing.JMenuItem('Solve', actionPerformed = self.solve, toolTipText = "Solve the problem")
        self.stopMenu = swing.JMenuItem('Stop', actionPerformed = self.stop, enabled = False, toolTipText = "Stop solving")
        menu.add(swing.JMenuItem('Tolerance', actionPerformed = self.setTolerance, toolTipText = "Set Dual&Primal feasibility tolerance for Gurobi solver, tightening this tolerance can produce smaller constraint violations; default: 1e-6, Min: 1e-9, Max: 1e-2"))
        menu.add(self.solveMenu)
        menu.add(self.stopMenu)
        menubar.add(menu)

        menu = swing.JMenu('QSSPN')#w#---QSSPN analysis---
        self.menuitems += [menu]
        menu.add(swing.JMenuItem('Open QSSPN', actionPerformed = self.openQSSPN, toolTipText = "Open a QSSPN model file"))#w#
        menu.add(swing.JMenuItem('Import SPEPT', actionPerformed = self.openSPEPT, toolTipText = "Import snoopy SPEPT file and converted into QSSPN model"))#w#
        self.qrunMenu = swing.JMenuItem('Run', actionPerformed = self.runQSSPN, toolTipText = "Run simulation")#w#
        self.qstopMenu = swing.JMenuItem('Stop', actionPerformed = self.stopQSSPN, toolTipText = "Stop simulation")#w#
        items = [
#            swing.JMenuItem('Open GSMN', actionPerformed = self.openModel, toolTipText = "Open a GSMN model file"),
            swing.JMenuItem('Load control', actionPerformed = self.openControl, toolTipText = "Load control file for parameters"),#w#
            self.qrunMenu,#w#
            self.qstopMenu,#w#
            swing.JMenuItem('Plot trajectory', actionPerformed = self.plotTrajectory, toolTipText = "Plot trajectory from output"),#w#
            swing.JMenuItem('Save QSSPN', actionPerformed = self.saveQSSPN, toolTipText = "Save into QSSPN model file"),#w#
            swing.JMenuItem('Save control', actionPerformed = self.saveControl, toolTipText = "Save into QSSPN control file"),#w#
            swing.JMenuItem('Open output', actionPerformed = self.openOutput, toolTipText = "Open a QSSPN output file"),#w#
        ]#w#
        for item in items: menu.add(item)
        self.menuitems += items #active only if a qsspn model loaded
        menubar.add(menu)

        menu = swing.JMenu('Help')
        menu.add(swing.JMenuItem('Manual', actionPerformed = self.showHelp, toolTipText = "Show the JyMet manual"))
        #menu.add(swing.JMenuItem('About', actionPerformed = self.showAbout, toolTipText = "About JyMet"))
        menubar.add(menu)

        self.setJMenuBar(menubar)


    def enableMenus(self):
        able = False #self.tabs.getTabCount()
        for i in range(self.tabs.getTabCount()):
            if self.tabs.getComponentAt(i).__class__ == ModelTabs.ModelTabs:
                able = True
                break
            elif self.tabs.getComponentAt(i).__class__ == QSSPNTabs.QSSPNTabs:#w#
                able = True
                break
        for item in self.menuitems:
            item.setEnabled(able)

    def initTabs(self):
        self.tabs = swing.JTabbedPane()
        self.add(self.tabs, awt.BorderLayout.CENTER)

    def getSelectedTab(self):
        rv = self.tabs.getSelectedComponent()
        if not rv.__class__ in [ModelTabs.ModelTabs, QSSPNTabs.QSSPNTabs]: #w# add QSSPNTabs.QSSPNTabs
            raise Exception("Select a model tab!")
        return rv
        

########################################################
##dialogs

    def write(self, s):
        # print "write"#t#
        swing.JOptionPane.showMessageDialog(self, s, Util.ERRORMSG, swing.JOptionPane.ERROR_MESSAGE)

    def flush(self):
        pass


########################################################
##file menu

    def addModel(self, model, probrecs = None):
        model.setOutput(self)        
        self.modtabs = ModelTabs.ModelTabs(model, probrecs, listener = self.disableStop)
        BaseTabs.TabCloser(model.getName(), self.enableMenus).put(self.modtabs, self.tabs)
        self.modtabs.setColWidths(self.getWidth())
        self.enableMenus()

    def chooseFile(self, save = True, filter = None):
        rv = None
        chooser = swing.JFileChooser(self.cwd)
        if self.cwd != None:
            curfile = io.File(self.cwd)
            if curfile.isFile():
                chooser.setSelectedFile(curfile)
        fun = {True: chooser.showSaveDialog, False: chooser.showOpenDialog}[save]
        if fun(self) == swing.JFileChooser.APPROVE_OPTION:
            rv = chooser.getSelectedFile()
            if rv:
                rv = rv.getAbsolutePath()
                self.cwd = rv
        return rv

    def newModel(self, event = None):
        try:
            name = swing.JOptionPane.showInputDialog(self, 'Enter a name')
            if name != None:
                model = Model.Model(name = name.strip())
                self.addModel(model)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))


    def openModel(self, event):
        try:
            path = self.chooseFile(False)
            if path:
                model = Model.Model()
                notextended = False
                if path.endswith('.xls') and path.find('.sfba')==-1:#w# path.find('.sfba')!=-1 to avoid fake 'xls' files
                    probrecs = Data.XLS.loadModel(model, path)
                    model.setName(os.path.split(path)[-1])
                    self.addModel(model, probrecs)
                else:
                    notextended = Data.Tabular.load(model, path)
                    self.addModel(model)                    
                    if notextended:
                        swing.JOptionPane.showMessageDialog(self, \
                        "Define the external metabolites in the metabolites table, in the problem table or by an externality tag.",\
                        "Data missing", swing.JOptionPane.WARNING_MESSAGE)
        except lang.Exception, e:
            self.write(str(e))
            print e
        except Exception, e:
            self.write(str(e))
            print e

    def openSBML(self, event = None):
        try:
            path = self.chooseFile(False)
            if path:
                model = Model.Model()
                Data.SBML.loadModel(model, path)
                #model.setName(path)
                self.addModel(model)        
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def openProblem(self, event = None):
        try:
            path = self.chooseFile(False)
            if path:
                s = open(path).read()
                self.getSelectedTab().readProblemTable(s)
        except lang.Exception, e:
            self.write(str(e))

    def openExpression(self, event = None):#w#
        try:
            path = self.chooseFile(False)
            if path:
                expr = Table.ExpressTable.Express()
                Data.Tabular.loadExpress(expr, path)
                modeltabs=self.getSelectedTab()#w#self.getSelectedTab() -return ModelTab
                if modeltabs.indexOfTab('Expression')!=-1:
                    modeltabs.remove(modeltabs.indexOfTab('Expression'))
                modeltabs.showExpress(expr)
                
        except lang.Exception, e:
            self.write(str(e))

    def openDPAplot(self, event = None):#w#
        try:
            path = self.chooseFile(False)
            if path:
                lines_m2g=Data.Tabular.loadDPAplot(path)
                self.getSelectedTab().showDPAm2g(lines_m2g)#w#self.getSelectedTab() -return ModelTabs
        except lang.Exception, e:
            self.write(str(e))

########Wataru###############
    def openGraph(self,event=None):
        try:
            path = self.chooseFile(False)
            self.getSelectedTab().openGraph(path)
        except lang.Exception,e:
            self.write(str(e))
#############################

    def saveModel(self, event):
        try:
            path = self.chooseFile(True)
            if path:
                tabs = self.getSelectedTab()
                tabs.saveXLS(path)
                #tabs.saveExtendedModel(path)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def saveTable(self, event = None):#w#
        try:
            path = self.chooseFile(True)
            if path:
                rv=self.getSelectedTab()#w#
                while (rv.__class__ != swing.JScrollPane) and (rv.__class__ != swing.JPanel):#w#
                    rv=rv.getSelectedComponent()
                if rv.__class__ == swing.JPanel: #save tab of multi-table
                    f = open(path, 'w')
                    s=''
                    rv=rv.getComponents()
                    for tt in rv:
                        tt = tt.getViewport().getView()
                        table=tt.getModel()
                        if path != None:
                            s = s+str(table)+'\n\n'
                    f.write(s)
                    f.close()
                else:
                    rv = rv.getViewport().getView()
                    if rv.__class__ == swing.JTextArea: #save INFO. tab
                        f = open(path, 'w')
                        s=rv.getText()
                        f.write(s)
                        f.close()
                    else: rv.save(path) #save tab of OuputTable 

        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))
    
    ###Wataru####
    def saveGraph(self,event = None):
        try:
            tab=self.getSelectedTab().getSelectedIndex()
            graph=self.getSelectedTab().getSelectedComponent().getGraph()
            name=self.getSelectedTab().getTitleAt(tab)
            path = self.chooseFile(True)
            if path:
               GraphPanel.SaveGraph(path,graph,name)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))
    ##################
    
    def printTable(self, event = None):
        try:
            self.getSelectedTab().printTable()
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def exit(self, event = None):
        lang.System.exit(0)

    def writeSBML(self, event = None):#w#
        try:
            path = self.chooseFile(True)
            if path:
                tabs = self.getSelectedTab()
                tabs.saveSBML(path, self.Xtag)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def writeXGMML(self, event = None):
#        try:
            path = self.chooseFile(True)
            if path:
                tabs = self.getSelectedTab()
                tabs.saveXGMML(path, self.Logicals)
#        except lang.Exception, e:
#            self.write(str(e))
#        except Exception, e:
#            self.write(str(e))
########################################################
##edit menu

    def insert(self, event = None):
        try:
            self.getSelectedTab().insertRows()
        except Exception, e:
            self.write(str(e))
        
    def delete(self, event = None):
        self.getSelectedTab().deleteRows()

    def filterRegEx(self, event = None):
        re = swing.JOptionPane.showInputDialog(self, 'Enter a regular expression')
        if re != None:
            try:
                self.getSelectedTab().filterByRegEx(re)
            except lang.Exception, e:
                self.write(str(e))
            except:
                self.write("Uknown error!")

    def merge(self, event = None):
        try:
            tags2models = {}
            for i in range(self.tabs.getTabCount()):
                modeltabs = self.tabs.getComponentAt(i)
                if modeltabs.__class__ == ModelTabs.ModelTabs:
                    title = self.tabs.getTitleAt(i)
                    tag = swing.JOptionPane.showInputDialog(self, 'Enter a tag for ' + title)
                    model = modeltabs.metabolicModel
                    if tags2models.has_key(tag):
                        raise Exception(tag + ": duplicate tag!")
                    tags2models[tag] = model
            exchange = swing.JOptionPane.showInputDialog(self, 'Enter a space-separated list of exchange metabolites').split(' ')
            name = swing.JOptionPane.showInputDialog(self, 'Enter the merged model name')
            model = Data.Merge.mergedModel(tags2models, exchange, name)
            self.addModel(model)
        except Exception, e:
            self.write(str(e))
        except lang.Exception, e:
            self.write(str(e))

########################################################
##view menu

    def showChildren(self, e = None):
        self.getSelectedTab().showChildren()

    def showParents(self, e = None):
        self.getSelectedTab().showParents()


    def showAll(self, e = None):
        self.getSelectedTab().showAll()

    def showPlot(self, e):
        try:
            self.getSelectedTab().showPlot()
        except Exception, e:
            self.write(str(e))

    def showGraph(self, e = None):
        try:
            layoutclassname = e.getActionCommand()
            self.getSelectedTab().showGraph(layoutclassname, self.Logicals)
            
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def dot(self, event = None):
        try:
            self.getSelectedTab().showDot(self.Logicals)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def setLogicals(self, event = None):
        try:
            logicals = ' '.join(self.Logicals)
            logicals = swing.JOptionPane.showInputDialog(self, 'Enter a space-separated list of logical metabolite IDs', logicals)
            if logicals != None:
                self.Logicals = logicals.split()
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))
  
########################################################
##analyse menu

    def switchProgram(self, e):
        self.program = e.getActionCommand()#w# self.program for current program
        self.problempanel.switch(self.program)

        modeltabs=self.getSelectedTab()#w#
        if self.program in ('SGNI','WGNI') and modeltabs.indexOfTab('Nutrients') == -1:#w#
            modeltabs.showNutrient()
        elif self.program not in ('SGNI','WGNI') and modeltabs.indexOfTab('Nutrients')!=-1:
            modeltabs.remove(modeltabs.indexOfTab('Nutrients'))

########################################################
##Solve menu

    def setExternalTag(self, event = None):
        try:
            xtag = swing.JOptionPane.showInputDialog(self, 'Enterg the tag indicating external metabolites', self.Xtag)
            if xtag != None:
                self.Xtag = xtag
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def setTolerance(self, event = None):#w# set solver parameter of tolerance
        try:
            stol = swing.JOptionPane.showInputDialog(self, 'Set a tolerance for Gurobi solver', self.sparam)
            if stol != None:
                self.sparam = stol
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def writeProblem(self, event = None, clear = False):
        if self.program in ('SGNI', 'WGNI'):#w#
            problem = self.problempanel.getProblem(self.getSelectedTab().nutset)
        else: problem = self.problempanel.getProblem()
        self.getSelectedTab().addProblem(problem, self.problempanel.program, clear)#w# program

    def clearProblem(self, event = None, clear = True):#w#
        self.getSelectedTab().clearProblems()

    def solve(self, event = None):
        if self.getSelectedTab().isProblemEmpty():
            if self.program in ('Live reactions','DPAplot'):#w# when empty problem, they can run without bfile
                self.gosolver(bfile=False)
            else: self.write("Empty problem!")
        else:
            self.gosolver()#w#

    def gosolver(self, bfile=True):#w#
        cmdarray = self.problempanel.getCmdArray(self.sparam, bfile)#w# self.sparam
        self.getSelectedTab().solve(cmdarray, self.problempanel.program, self.Xtag)
        self.stopMenu.setEnabled(True)
        self.solveMenu.setEnabled(False)

    def stop(self, event = None):
        self.getSelectedTab().stopSolution()
        self.disableStop()

    def disableStop(self):
        self.stopMenu.setEnabled(False)
        self.solveMenu.setEnabled(True)

########################################################
##QSSPN menu #w#
    def openQSSPN(self, event = None, path = None):
        try:
            if path == None: path = self.chooseFile(False)
            if path:
                self.program='QSSPN'
                self.problempanel.switch(self.program)
                model = QSSPN.Model.Model(path)
                model.name = os.path.split(path)[-1].strip('.txt').strip('.qsspn')
                tabs = QSSPN.QSSPNTabs.QSSPNTabs(model, listener = self.disableStopQSSPN)
                BaseTabs.TabCloser(model.name, self.enableMenus).put(tabs, self.tabs)
#                tabs.setColWidths(self.getWidth())
                self.getSelectedTab().showControlTable()
                self.enableMenus()

        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def openControl(self, event = None):
        try:
            path = self.chooseFile(False)
            if path:
                self.getSelectedTab().readControlTable(path)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def setMaxToken(self, event = None):#w# set solver parameter of tolerance
        try:
            maxt = swing.JOptionPane.showInputDialog(self, 'Set default maximal number of tokens', 2)
            if maxt != None: return maxt
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def openSPEPT(self, event = None):
        try:
            path = self.chooseFile(False)
            if path:
                maxt = self.setMaxToken()
                QSSPN.Spept.parseSPEPT(path, Util.MODELTMP, maxt)
                self.openQSSPN(path = Util.MODELTMP)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def runQSSPN(self, event=None):
        script_gsmn = str(self.modtabs.getReactionModel())
        Data.Problem.writeTmp(Util.GSMNTMP, script_gsmn)
        self.getSelectedTab().solve()
        self.qstopMenu.setEnabled(True)
        self.qrunMenu.setEnabled(False)

    def saveQSSPN(self, event):
        try:
            path = self.chooseFile(True)
            if path:
                self.getSelectedTab().writeModel(path)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def saveControl(self, event):
        try:
            path = self.chooseFile(True)
            if path:
                self.getSelectedTab().writeControl(path)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def plotTrajectory(self, e):
        try:
            self.getSelectedTab().showTrajectory()
        except Exception, e:
            self.write(str(e))

    def openOutput(self, event = None):
        try:
            path = self.chooseFile(False)
            if path:
                s = open(path).read()
                self.getSelectedTab().reportSolution(s)
        except lang.Exception, e:
            self.write(str(e))
        except Exception, e:
            self.write(str(e))

    def stopQSSPN(self, event = None):
        self.getSelectedTab().stopSolution()
        self.qstopMenu.setEnabled(False)
        self.qrunMenu.setEnabled(True)

    def disableStopQSSPN(self):
        self.qstopMenu.setEnabled(False)
        self.qrunMenu.setEnabled(True)
    
########################################################
##help menu
    def showHelp(self, e):
        pane = swing.JEditorPane(editable = False)
        pane.setContentType('text/html')
        pane.setPage(MANUAL_JYMET)
        helptab = swing.JScrollPane(pane)
        BaseTabs.TabCloser("Manual").put(helptab, self.tabs)

    def showAbout(self, e):
        pass
