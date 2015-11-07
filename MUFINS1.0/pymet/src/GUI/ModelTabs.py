import QSSPN.Table
import GUI.PlotPanel
__author__="Albert Gevorgyan"
__date__ ="$26-Feb-2010 14:18:16$"



from javax import swing
from java import awt

from Table import LevelTable, ProblemTable, OutputTable#w#
from Table import ExpressTable, NutrientTable#w#
import Data.Tabular#w#
import Util
import Graph
import GUI #w#
import Solver
import Data.Problem
import Data.XGMML
import Data.SBML#w#
import Data.XLS
import Model.Element
import PlotPanel
import BaseTabs#w#
from BaseTabs import TabCloser#w#
#from Graph.JGraphPanel import *#wtr#

PROBLEM = 'Problem'
OUTPUT  = 'Output'
GRAPH   = 'Network'
ERRORMSG = 'Solver error'
ID_COL = 0
CLOSELABEL = 'x'
CLOSESIZE  = 15
BARTABLES  = ['FBA', 'Elementary modes', 'FSA','Knock-out analysis']#w#
BARTABLES1  = ['DPAplot', 'DPAsig']#w#
FVATABLES  = ['FVA', Util.GFVA2, Util.GFVA3, Util.GIM3E]#w#
PLOTTABLES = ['Plot']#,
PLOT3DTABLES = ['Plot 3D']
MXSTABLES  = ['Minimal substrate sets', 'Minimal product sets']
EXPRESSION = 'Expression'#w#
ENDTAG = ';'#w#
TABENDTAG = '///'#w#
#from Solver import GSA#w#

import Model.Element
REACTION = Model.Element.Reaction.__name__ + 's'
class ModelTabs(BaseTabs.BaseTabs):
    def __init__(self, model, probrecs = None, listener = False):
        GUI.BaseTabs.BaseTabs.__init__(self, listener)
        self.metabolicModel = model
        levelmodels = []
        tables = []
        nlevels = len(model.ClassLevels)
        nlevel = 0
        while nlevel < nlevels:
            theclass = model.getLevelClass(nlevel)
            fields = theclass.fields.copy()
            if nlevel == 0:
                del fields[theclass.ParentField]
            if nlevel == nlevels - 1:
                del fields[theclass.ChildField]
            tm = LevelTable.LevelTableModel(model, theclass, self, nlevel, fields)
            levelmodels.append(tm)
            if nlevel > 0:
                tm.addTableModelListener(tables[nlevel - 1])
            table = LevelTable.LevelTable(tm)
            tables.append(table)
            scrollpane = swing.JScrollPane(table)

            theclass = self.metabolicModel.getLevelClass(nlevel)
            if theclass == Model.Element.Reaction:
                self.reaclevel = nlevel
            title = "%d %s" % (table.getRowCount(), theclass.__name__.lower() + 's')
            self.addTab(title, scrollpane)
            nlevel += 1
        self.setSelectedIndex(2)

        while nlevel > 0:
            nlevel -= 1
            levelmodels[nlevel - 1].addTableModelListener(tables[nlevel])

        self.problemtable = ProblemTable.ProblemTable()
        if probrecs != None:
            self.problemtable.getModel().addRecords(probrecs)
        self.addTab(PROBLEM, swing.JScrollPane(self.problemtable))

    def setLevelTitle(self, number, rowcount):
        table = self.getTableAt(number)
        title = "%d %s" % (rowcount, self.metabolicModel.getLevelClass(number).__name__.lower() + 's')
        self.setTitleAt(number, title)

    def getReactionModel(self):
        return self.getTableAt(self.reaclevel).getModel()

    def saveXLS(self, path):
        wb = Data.XLS.PyMetWorkBook()
        nlevels = len(self.metabolicModel.ClassLevels)
        for nlevel in range(nlevels):
            title = self.metabolicModel.getLevelClass(nlevel).__name__ + 's'
            table = self.getTableAt(nlevel)
            tmodel = table.getModel()
            wb.add_table_model(title, tmodel)
        wb.add_table_model("Problem", self.getTableAt("Problem").getModel())
        wb.set_active_sheet(2)
        wb.save(path)
            
    def saveExtendedModel(self, path):
        s = self.metabolicModel.getName() + '\n' + Data.Tabular.ENDOFRECORD + '\n'
        nlevels = len(self.metabolicModel.ClassLevels)
        for n in range(nlevels):
            name = self.getTitleAt(n)
            s += name + '\n'
            s += str(self.getTableAt(n).getModel())
            s += '\n' + Data.Tabular.ENDOFRECORD + '\n'
        f = open(path, 'w')
        f.write(s)
        f.close()

    def saveModel(self, path):
        self.getReactionModel().save(path)

    def readProblemTable(self, s):
        table = self.getTableAt(PROBLEM)
        table.deleteAll()
        table.read(s)
        self.setSelectedIndex(self.indexOfTab(PROBLEM))

    def clearProblems(self):
        self.problemtable.deleteAll()

    def addProblem(self, problem,  program, clear = False):#w#
        if clear:
            self.clearProblems()
        self.problemtable.addProblem(problem, program)#w#
        self.setSelectedIndex(self.indexOfTab(PROBLEM))

    def getScript(self):
        return self.problemtable.getScript()

    def isProblemTableSelected(self):
        return self.getTitleAt(self.getSelectedIndex()) == PROBLEM

    def isProblemEmpty(self):
        return self.getTableAt(PROBLEM).isEmpty()

    def showChildren(self):
        nlevel = self.getSelectedIndex()
        IDs = self.getSelectedIDs()
        if len(IDs) > 0:
            nlevel = self.metabolicModel.getLevelNumber(IDs[0]) + 1
            if nlevel < len(self.metabolicModel.ClassLevels):
                children = self.metabolicModel.getChildren(IDs)
                self.getTableAt(nlevel).filter(children, ID_COL)
                self.setSelectedIndex(nlevel)
   
    def showParents(self):
        nlevel = self.getSelectedIndex()
        IDs = self.getSelectedIDs()
        if len(IDs) > 0:
            nlevel = self.metabolicModel.getLevelNumber(IDs[0]) - 1
            if nlevel >= 0:
                parents = self.metabolicModel.getParents(IDs)
                self.getTableAt(nlevel).filter(parents, ID_COL)
                self.setSelectedIndex(nlevel)

    def showPlot(self):
        title = self.getTitleAt(self.getSelectedIndex())
#        print str(title)#t#
        if title in BARTABLES:
            self.showBarChart(title, PlotPanel.BarPanel)
        elif title in FVATABLES:
            self.showBarChart(title, PlotPanel.FVAPanel)
        elif title in MXSTABLES:
            self.showBarChart(title, PlotPanel.MXSPanel)
        elif title in PLOT3DTABLES:
            self.showXYZ()
        elif title in PLOTTABLES:
            self.showXY()
        elif title == Util.GFAA:#w#
            self.showBarChart(title, PlotPanel.GFAAPanel)
        elif title == 'WGNI':#w#
            self.showBarChart(title, PlotPanel.WGNIPanel)
        elif title in BARTABLES1:#w#
            self.showBarChart(title, PlotPanel.BarPanel1, -1)

    def showBarChart(self, title, klass, colrange=0):
        lists = []
        table = self.getTableAt(self.getSelectedIndex())
        if colrange==0: colinds=range(table.getColumnCount())#w#
        elif colrange==-1: colinds = [0]+list(table.getSelectedColumns())#w#
        else: colinds = range(colrange)#w#
        model = table.getModel()
        lists =[[model.fields.getOrder()[i] for i in colinds]]#w# get column names
        lists += table.getSelectedLists(colinds)
#        print lists #t#
        if klass==PlotPanel.BarPanel1: bc=klass(lists,str(title))
        else: bc = klass(lists)
        TabCloser(title+' chart').put(bc.panel, self)

#    def showBarChart(self, title, klass, colrange=0):
#        lists = []
#        for i in range(self.getTabCount()):
#            if self.getTitleAt(i) in ['WGNI']:
#                table = self.getTableAt(i)
#                model = table.getModel
#                if colrange==0: colinds=range(0,table.getColumnCount())#w#
#                elif colrange==-1: colinds = [0]+list(table.getSelectedColumns())#w#
#                else: colinds = range(0, colrange)#w#
#                lists =[model.fields.getOrder()]#w# get column names
#                lists += table.getSelectedLists(colinds)
#                print lists #t#
#        if klass==PlotPanel.BarPanel1: bc=klass(lists,title)
#        else: bc = klass(lists)
#        TabCloser(title+' chart').put(bc.panel, self)

    def showXY(self):
        xy = PlotPanel.XYPanel()
        for i in range(self.getTabCount()):
            if self.getTitleAt(i) in PLOTTABLES:
                table = self.getTableAt(i)
                lists = table.getSelectedLists(range(0, 3))
                xy.addLists(lists)
        scrollpane = swing.JScrollPane(xy)
        TabCloser("Line plot").put(scrollpane, self)

    def showXYZ(self):
        table = self.getTableAt(self.getSelectedIndex())
        lists = table.getSelectedLists(range(0, 4))
        xyz = PlotPanel.XYZPanel(lists)
        scrollpane = swing.JScrollPane(xyz)
        TabCloser("Block plot").put(scrollpane, self)

    def showGraph(self, layoutclassname, logicals):
        lists = self.allSelectedLists()
        gp = Graph.JGraphPanel.JGraphPanel(self.metabolicModel, lists, layoutclassname, logicals)
        TabCloser(layoutclassname).put(gp, self)
#        TabCloser(layoutclassname).put2(gp, self,gp.graph)#wtr# put(gp, self)->
    
    def showDot(self, logicals):
        lists = self.allSelectedLists()
        dp = Graph.DotPanel.DotPanel(self.metabolicModel, lists, logicals)
        scrollpane = swing.JScrollPane(dp)
        TabCloser("Dot").put(scrollpane, self)

    def solve(self, cmdarray, program, xtag):
        self.program = program
        model = self.getReactionModel()
        script = self.getScript()#problem file
#        if not script: script='!\n;'#w# for empty problem
        script_exp=''#w# expression data
        script_m2g=''#w# DPAm2g data
        if self.program in Util.GSA2:#w#
            script_exp = self.expresstable.getScript()#w#
#        express = self.getTableAt(EXPRESSION).getModel()#w#
        if self.program == 'DPAsig':#w# DPAplot output
            script_m2g=self.getScript_DPAm2g(self.table_m2g)
        Data.Problem.writeTmpFiles(str(model), script, script_exp, script_m2g)#w#
        if xtag == '':
            externals = self.metabolicModel.getExternals()
            cmdarray += Data.Problem.getExternalCmd(externals)
        else:
            cmdarray += Data.Problem.getExternalTagCmd(xtag)
#        print cmdarray#t/
        self.solver = Solver.Solver(self, cmdarray)
        self.solver.start()

    def getScript_DPAm2g(self, table):#w# from DPAm2g table to string for DPAsig problem
        data=str(table.getModel())
        script='Metabolites to genes:\n\n'
        script+=data.replace(' ','\t')
        return script

    def reportSolution(self, output):
        outputtable = OutputTable.OutputTable(self.metabolicModel)
        outputtable.write(output, self.program)
        tab = swing.JScrollPane(outputtable)
        self.outputs.append(tab)
        TabCloser(self.program).put(tab, self)
        outputtable.setColWidths(self.getWidth())
        if self.listener:
            self.listener()

    def reportSolution_gsa(self, output, cmdarray):#w#for genome-scale analysis
        recordtabs = swing.JTabbedPane()
#        records = output.split(ENDTAG)
        records = output.split(';\n')
        outputtable=[]
        pscript = self.getScript()#problem file
        precs=pscript.split(';')
        for i in range(len(records)-1):
            record = records[i]
            outtabs = swing.JTabbedPane()
            if self.program == Util.GFAA:
                lines_reac, lines_gene=Data.Tabular.output2table_gfaa(record)
                table_reac = OutputTable.OutputTable(self.metabolicModel)
                table_reac.write(lines_reac, 'GFAAreac')
                tab_reac = swing.JScrollPane(table_reac)
                table_gene = OutputTable.OutputTable(self.metabolicModel)
                table_gene.write(lines_gene, 'GFAAgene')
                tab_gene = swing.JScrollPane(table_gene)
                outtabs.addTab('Reaction', tab_reac)
                outtabs.addTab('Gene', tab_gene)
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Reaction'), 'Flux-activity state for reactions')
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Gene'), 'Flux-activity state for Gene')
                outputtable+=[table_reac, table_gene]
                recordtabs.addTab('Record'+str(i+1), outtabs)
            elif self.program in (Util.GFVA2,Util.GFVA3, Util.GIMME2):
                lines_obj, lines_reac=Data.Tabular.output2table_gfva(record, self.program)
                table_obj = OutputTable.OutputTable(self.metabolicModel)
                table_reac = OutputTable.OutputTable(self.metabolicModel)
                table_obj.write(lines_obj, 'GFVAobj')
                table_reac.write(lines_reac, 'GFVAreac')
                tabpanel = swing.JPanel()
#                tabpanelsp = swing.JScrollPane(tabpanel)
                tabpanel.setLayout(swing.BoxLayout(tabpanel, swing.BoxLayout.Y_AXIS))
                tab1 = swing.JScrollPane(table_obj)
                tab1.setPreferredSize(awt.Dimension(0, 60))
                tab2 = swing.JScrollPane(table_reac)
                tabpanel.add(tab1)
                tabpanel.add(tab2)
#                outtabs.addTab('Results', tabpanel)
                outputtable+=[table_obj, table_reac]
                recordtabs.addTab('Record'+str(i+1), tabpanel)
            elif self.program == 'SGNI':
                media=precs[i].strip().split('\n')[0].strip().lstrip('!').split(' ')
                lines_blp, lines_gni=Data.Tabular.output2table_sgni(record, media)
                table_blp = OutputTable.OutputTable(self.metabolicModel)
                table_blp.write_gni(lines_blp, 'SGNIblp', media)
                tab_blp = swing.JScrollPane(table_blp)
                table_gni = OutputTable.OutputTable(self.metabolicModel)
                table_gni.write_gni(lines_gni, 'SGNIgni', media)
                for col in range(table_gni.getColumnCount())[1:]:
                    table_gni.getColumnModel().getColumn(col).setCellRenderer(OutputTable.SGNIRenderer())
                tab_gni = swing.JScrollPane(table_gni)
                outtabs.addTab('Results', tab_blp)
                outtabs.addTab('GNI matrix', tab_gni)
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Results'), 'bi-level LP optimization results')
                outtabs.setToolTipTextAt(outtabs.indexOfTab('GNI matrix'), 'the predicted gene-nutrient interactions')
                outputtable+=[table_blp, table_gni]
                recordtabs.addTab('Record'+str(i+1), outtabs)
            elif self.program == 'WGNI':
                media=precs[i].strip().split('\n')[0].strip().lstrip('!').split(' ')
                lines_out, lines_gni=Data.Tabular.output2table_wgni(record, media)
                table_out = OutputTable.OutputTable(self.metabolicModel)
                table_out.write(lines_out, 'WGNIout')
                tab_out = swing.JScrollPane(table_out)
                table_gni = OutputTable.OutputTable(self.metabolicModel)
                table_gni.write_gni(lines_gni, 'WGNIgni', media)
                tab_gni = swing.JScrollPane(table_gni)
                outtabs.addTab('Results', tab_out)
                outtabs.addTab('GNI matrix', tab_gni)
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Results'), 'weak GNI results')
                outtabs.setToolTipTextAt(outtabs.indexOfTab('GNI matrix'), 'the predicted gene-nutrient interactions')
                outputtable+=[table_out, table_gni]
                recordtabs.addTab('Record'+str(i+1), outtabs)
            elif self.program == 'DPAplot':
                lines_m2g, lines_g2m, lines_mat, glist=Data.Tabular.output2table_dpaplot(record)
                self.table_m2g = OutputTable.OutputTable(self.metabolicModel)
                table_g2m = OutputTable.OutputTable(self.metabolicModel)
                table_mat = OutputTable.OutputTable(self.metabolicModel)
                self.table_m2g.write(lines_m2g, 'DPAm2g')
                table_g2m.write(lines_g2m, 'DPAg2m')
                table_mat.write_gni(lines_mat, 'DPAmat', glist)
                tab_m2g = swing.JScrollPane(self.table_m2g)
                tab_g2m = swing.JScrollPane(table_g2m)
                tab_mat = swing.JScrollPane(table_mat)
                outtabs.addTab('Metabolites', tab_m2g)
                outtabs.addTab('Genes', tab_g2m)
                outtabs.addTab('Producibility of WT and KO', tab_mat)
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Metabolites'), 'mapping from metabolites to essential genes')
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Genes'), 'mapping from gene to metabolites')
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Producibility of WT and KO'), 'metabolic producibility in wildtype and gene-knockout models')
                outputtable+=[self.table_m2g, table_g2m, table_mat]
                recordtabs.addTab('Record'+str(i+1), outtabs)
            elif self.program == 'DPAsig':
                lines_up, lines_dw, list=Data.Tabular.output2table_dpasig(record)
                table_up = OutputTable.OutputTable(self.metabolicModel)
                table_dw = OutputTable.OutputTable(self.metabolicModel)
                table_up.write_gni(lines_up, 'DPAsig', list)
                table_dw.write_gni(lines_dw, 'DPAsig', list)
                tab_up = swing.JScrollPane(table_up)
                tab_dw = swing.JScrollPane(table_dw)
                outtabs.addTab('Signals_UP', tab_up)
                outtabs.addTab('Signals_DOWN', tab_dw)
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Signals_UP'), 'Signals for up-regulated genes')
                outtabs.setToolTipTextAt(outtabs.indexOfTab('Signals_DOWN'), 'Signals for down-regulated genes')
                outputtable+=[table_up, table_dw]
                recordtabs.addTab('Record'+str(i+1), outtabs)
            else:
                reclines = record.strip().split('\n')
                table = OutputTable.OutputTable(self.metabolicModel)
                table.write(reclines, self.program)
                tab = swing.JScrollPane(table)
#                table.setColWidths(self.getWidth())
                outputtable+=[table]
                recordtabs.addTab('Record'+str(i+1), tab)
#            if self.program in ('GFAA','SGNI','WGNI','DPAplot','DPAsig'):
#                recordtabs.addTab('Record'+str(i+1), outtabs)
#            else: recordtabs.addTab('Record'+str(i+1), outtab)

        INFO='#Analysis : '+self.program+'\n\n'
        for i in range(len(precs)-1): INFO+='#Record'+str(i+1)+' :\n'+precs[i].strip()+'\n;\n\n'
        INFO+='\n#Command :\n'+' '.join(cmdarray)+'\n\n'
        INFO+='\n#Time spent : '+records[-1].split(':')[1].strip()+' (second)\n\n'

        textArea=swing.JTextArea()
        textArea.append(INFO)
        textArea.setLineWrap(True);
        tab_info = swing.JScrollPane(textArea)
        recordtabs.addTab('INFO.', tab_info)
        TabCloser(self.program).put(recordtabs, self)
        for table in outputtable: table.setColWidths_gsa(self.getWidth())
        if self.listener:
            self.listener()
        #self.table=table#wtr#
        #print self.table#wtr#
        self.tabtitle=self.getTitleAt(self.getSelectedIndex())#wtr#

    def saveSBML(self, path, xtag):#w#
        Data.SBML.exportSBML(xtag, self.metabolicModel, path)

    def saveXGMML(self, path, logicals):
        lists = self.allSelectedLists()
        Data.XGMML.write(self.metabolicModel, lists, path, logicals)

    
    def showExpress(self, expr):#w# show express table
        tm = ExpressTable.ExpressTableModel(expr)
        self.expresstable = ExpressTable.ExpressTable(tm)
        scrollpane = swing.JScrollPane(self.expresstable)
        TabCloser("Expression").put(scrollpane, self)
        self.expresstable.setColWidths_gsa(self.getWidth())

    def showDPAm2g(self, lines_m2g):#w# show DPA mapping from metabolites to essential genes
        self.table_m2g = OutputTable.OutputTable(self.metabolicModel)
        self.table_m2g.write(lines_m2g, 'DPAm2g')
        tab_m2g = swing.JScrollPane(self.table_m2g)
        TabCloser("DPA_met2genes").put(tab_m2g, self)
        self.table_m2g.setColWidths_gsa(self.getWidth())


    def showNutrient(self):#w# show nutrients settings for sgni/wgni problem
        externals = self.metabolicModel.getExternals()
        self.nutset=NutrientTable.NutrientSet(externals)
        tm = NutrientTable.NutsTableModel(self.nutset)
        table = NutrientTable.NutsTable(tm)
#        test, present=nutset.getNutSetting()
        scrollpane = swing.JScrollPane(table)
        TabCloser("Nutrients").put(scrollpane, self)
        table.setColWidths_gsa(self.getWidth())

    
    #####Wataru#######################################################
    def openGraph(self,path):#w#
        value={}
        tabTile=self.getTitleAt(self.getSelectedIndex())
        table = self.getTableAt(self.getSelectedIndex())
        if tabTile==Util.GFAA: value=table.getSelectedGraphValue_GFAA()
        elif tabTile==Util.GFVA2: value=table.getSelectedGraphValue_GFVA2()
        elif tabTile in (Util.GFVA3,Util.GIMME2): value=table.getSelectedGraphValue_GFVA3()
        elif tabTile=='FVA': value=table.getSelectedGraphValue_FVA()
        elif tabTile=='FBA': value=table.getSelectedGraphValue_FBA()
        elif tabTile==Util.GIMME: value=table.getSelectedGraphValue_GIMME()
        gp=Graph.JGraphPanel.ReGraphPanel(path,value, tabTile)
        tabtitle=gp.getClassName()
#        self.setRows(tabtitle,gp.row)
        TabCloser(tabtitle).put(gp, self)
#        TabCloser(tabtitle).put2(gp, self,gp.graph)
        #self.getRows(gp.graph,gp.row,gp.tabtitle)
        #cell=Graph.JGraphPanel.ReGaphPanel.getCell()
    #####################################################################
        
    """
    def getRows(self,graph,row,tabtitle):
        row.insert(0,tabtitle)
        Graph.JGraphPanel.rows[graph]=row
        
    def setRows(self,tabtitle,row):
        if self.indexOfTab(row[0])!=-1:
            del row[0]
            try:
                self.table.clearSelection()
                for x in row:
                    self.table.changeSelection(int(x),0,1,0)
                    self.table.changeSelection(int(x),1,1,1)
            except:
                pass
   """     
