#from GUI.Table.ProblemTable import setValueAt
import QSSPN.Table
# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="John"
__date__ ="$23-Feb-2014 15:19:40$"

from javax import swing
from java import awt
from java import io

import Util
import GUI #w#
import Data
import GUI.ModelTabs
import QSSPN
from Table import QSSPNTable
from GUI.BaseTabs import TabCloser#w#

class QSSPNTabs(GUI.BaseTabs.BaseTabs):
    def __init__(self, model, listener = False):
        GUI.BaseTabs.BaseTabs.__init__(self, listener)
        self.qsspn = model
        self.showQSSPN(self.qsspn)

    def showQSSPN(self, model):#w# open graph building tabs
        table_node = QSSPNTable(); table_node.write(model, 'Node')
        tab_node = swing.JScrollPane(table_node)
        table_int = QSSPNTable(); table_int.write(model, 'Intact')
        tab_int = swing.JScrollPane(table_int)
        table_sub = QSSPNTable(); table_sub.write(model, 'Sub')
        tab_sub = swing.JScrollPane(table_sub)
        table_actS = QSSPNTable(); table_actS.write(model, 'ActS')
        tab_actS = swing.JScrollPane(table_actS)
        table_met = QSSPNTable(); table_met.write(model, 'Met')
        tab_met = swing.JScrollPane(table_met)
        table_actM = QSSPNTable(); table_actM.write(model, 'ActM')
        tab_actM = swing.JScrollPane(table_actM)
        table_enz = QSSPNTable(); table_enz.write(model, 'Enz')
        tab_enz = swing.JScrollPane(table_enz)
        table_actE = QSSPNTable(); table_actE.write(model, 'ActE')
        tab_actE = swing.JScrollPane(table_actE)
        table_list = QSSPNTable(); table_list.write(model, 'List')
        tab_list = swing.JScrollPane(table_list)
        splitPane_int = swing.JSplitPane(swing.JSplitPane.HORIZONTAL_SPLIT)
        splitPane_sub = swing.JSplitPane(swing.JSplitPane.VERTICAL_SPLIT)
        panel_int = swing.JPanel().add(tab_int)
        splitPane_int.setLeftComponent(panel_int)
        panel_sub = swing.JPanel().add(tab_sub)
        panel_actS = swing.JPanel().add(tab_actS)
        splitPane_sub.setTopComponent(panel_sub)
        splitPane_sub.setBottomComponent(panel_actS)
        splitPane_int.setRightComponent(splitPane_sub)
        splitPane_int.setDividerLocation(520)
        splitPane_sub.setDividerLocation(300)
#        splitPane_int.setResizeWeight(1)
#        splitPane_sub.setResizeWeight(0.5)

        splitPane_met = swing.JSplitPane(swing.JSplitPane.HORIZONTAL_SPLIT)
        panel_met = swing.JPanel().add(tab_met)
        splitPane_met.setLeftComponent(panel_met)
        panel_actM = swing.JPanel().add(tab_actM)
        splitPane_met.setRightComponent(panel_actM)
        splitPane_met.setDividerLocation(500)

        splitPane_enz = swing.JSplitPane(swing.JSplitPane.HORIZONTAL_SPLIT)
        splitPane_actList = swing.JSplitPane(swing.JSplitPane.VERTICAL_SPLIT)
        panel_enz = swing.JPanel().add(tab_enz)
        splitPane_enz.setLeftComponent(panel_enz)
        panel_actE = swing.JPanel().add(tab_actE)
        panel_list = swing.JPanel().add(tab_list)
        splitPane_actList.setTopComponent(panel_actE)
        splitPane_actList.setBottomComponent(panel_list)
        splitPane_enz.setRightComponent(splitPane_actList)
        splitPane_enz.setDividerLocation(500)
        splitPane_actList.setDividerLocation(250)

        self.addTab('PN nodes', tab_node)
        self.addTab('Interactions', splitPane_int)
        self.addTab('Metabolites', splitPane_met)
        self.addTab('Enzymes', splitPane_enz)
#        tabs.setToolTipTextAt(tabs.indexOfTab('PN nodes'), 'define reactions into graph-groups ')
#        tabs.setToolTipTextAt(tabs.indexOfTab('Interactions'), 'list of graph-groups')
#        tabs.setToolTipTextAt(tabs.indexOfTab('Metabolites'), 'reactions and related metabolites for a group')
#        tabs.setToolTipTextAt(tabs.indexOfTab('Enzymes'), 'map between name and tag')
#        TabCloser('QSSPN').put(tabs, self)
        table_actS.setRowSorter(None)
        table_actM.setRowSorter(None)
        table_actE.setRowSorter(None)
        table_node.setColWidths_gsa(self.getWidth())
        table_int.setColWidths_gsa(self.getWidth())
        table_sub.setColWidths_gsa(self.getWidth())
        table_actS.setColWidths_gsa(self.getWidth())
        table_met.setColWidths_gsa(self.getWidth())
        table_actM.setColWidths_gsa(self.getWidth())
        table_enz.setColWidths_gsa(self.getWidth())
        table_actE.setColWidths_gsa(self.getWidth())
        table_list.setColWidths_gsa(self.getWidth())
        #add listeners
        SeModel = table_int.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerInt(table_int, table_sub, table_actS, self.qsspn, self.curtable))
        SeModel = table_sub.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerSub(table_sub, table_actS, self.curtable))
        SeModel = table_actS.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerActS(table_actS, self.curtable))
        SeModel = table_met.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerMet(table_met, table_actM, self.curtable))
        SeModel = table_actM.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerActM(table_actM, self.curtable))
        SeModel = table_enz.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerEnz(table_enz, table_actE, table_list, self.curtable))
        SeModel = table_actE.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerActE(table_actE, self.curtable))
        SeModel = table_list.getSelectionModel()
        SeModel.addListSelectionListener(QSSPN.Table.SelectionListenerList(table_list, self.curtable))


    def showControlTable(self):
        self.contabs = swing.JTabbedPane()
        #lines_para, lines_monitor, lines_state, lines_func = loadControl()
        self.table_para = QSSPNTable(); self.table_para.write_control('ConPara', self.qsspn);tab_para = swing.JScrollPane(self.table_para)
        self.table_init = QSSPNTable(); self.table_init.write_control('ConInit', self.qsspn);tab_init = swing.JScrollPane(self.table_init)
        self.table_func = QSSPNTable(); self.table_func.write_control('ConFunc', self.qsspn);tab_func = swing.JScrollPane(self.table_func)
        self.table_flux = QSSPNTable(); self.table_flux.write_control('ConFlux', self.qsspn);tab_flux = swing.JScrollPane(self.table_flux)
        self.contabs.addTab('Parameters', tab_para)
        self.contabs.addTab('InitialStates', tab_init)
        self.contabs.addTab('Functions', tab_func)
        self.contabs.addTab('FluxMap', tab_flux)
#        scrollpane = swing.JScrollPane(table_control)
        self.addTab("Control", self.contabs)
#            TabCloser("Control").put(scrollpane, self)
        self.table_para.setColWidths_gsa(self.getWidth())
        self.table_init.setColWidths_gsa(self.getWidth())
        self.table_func.setColWidths_gsa(self.getWidth())
        self.table_flux.setColWidths_gsa(self.getWidth())

    def readControlTable(self, path):
        lines_para, lines_init, lines_func = QSSPN.Table.loadControl(path, self.qsspn)
        self.table_para.getModel().loadData(lines_para)
        self.table_init.getModel().loadData(lines_init)
        self.table_func.getModel().loadData(lines_func)
#        self.contabs.getComponentAt(self.contabs.indexOfTab("Parameters")).getViewport().getView().getModel().loadData(lines_para)
#        self.contabs.getComponentAt(self.contabs.indexOfTab("InitialStates")).getViewport().getView().getModel().loadData(lines_state)
#        self.contabs.getComponentAt(self.contabs.indexOfTab("Functions")).getViewport().getView().getModel().loadData(lines_func)
#        self.contabs.getComponentAt(self.contabs.indexOfTab("Parameters")).getViewport().getView().write_control('ConPara', self.qsspn, lines_para)
#        self.contabs.getComponentAt(self.contabs.indexOfTab("InitialStates")).getViewport().getView().write_control('ConInit', self.qsspn, lines_state)
#        self.contabs.getComponentAt(self.contabs.indexOfTab("Functions")).getViewport().getView().write_control('ConFunc', self.qsspn, lines_func)
        self.getTableAt("PN nodes").getModel().updateNode()

    def getModelScript(self):
        script = '\nSUBSTANCES\n'
        for x in self.qsspn.nodes:
            if x:
                node = self.qsspn.Node[x]
                script += ' '*3 + ' '.join([node.ID, node.ini, node.max, node.type]) + '\n'
        script += 'END\n'
        script += '\nREACTIONS\n'
        for x in self.qsspn.interactions:
            if x:
                intact = self.qsspn.Int[x]
                prod = []
                cons = []
                script_int = ' '*2 + ' '.join(['INTERACTION', intact.ID.split('__')[0], intact.rate, intact.type, intact.delay]) + '\n'
                for x in intact.subs:
                    if x:
                        sub = intact[x]
                        script_sub = ' '*6 + 'SUBSTRATE ' + sub.ID + '\n'
                        if sub.actr>=0:
                            script_sub += ' '*9 + 'ACTIVITY ' + str(sub.actr) + '\n'
                            for act in sub.act:
                                if act.t and act.a:
                                    script_sub += ' '*11 + ' '.join([act.t, act.a]) + '\n'
                            script_sub += ' '*9 + 'END\n' + ' '*6 + 'END\n'
                            script_int += script_sub
                        if bool(sub.prod) == True: prod += [sub.ID]
                        if bool(sub.cons) == True: cons += [sub.ID]
                for x in prod:
                    sub = intact[x]
                    if sub.sto: script_int += ' '*6 + 'PRODUCT_STOICHIOMETRY ' + x + ' '+ sub.sto + '\n'
                    else: script_int += ' '*6 + 'PRODUCT ' + x + '\n'
                for x in cons: script_int += ' '*6 + 'CONSUMED ' + x + '\n'
                script_int += ' '*2 + 'END\n'
                script += script_int
        script += 'END\n'
        script += '\nGSMN\n'
        script += ' '*3 + 'MODEL ' + self.qsspn.gsmn.ID + '\n'
        script += ' '*3 + 'EXT_TAG ' + self.qsspn.gsmn.extag + '\n'
        for flux in self.qsspn.gsmn.flux:
            script += ' '*3 + ' '.join(['FLUX']+flux) + '\n'
        for x in self.qsspn.metabolites:
            if x:
                met = self.qsspn.Met[x]
                Nactm = 0
                script_met = ' '*3 + ' '.join(['METABOLITE', met.ID, met.obj]) + '\n'
                script_met += ' '*6 + 'ACTIVITY ' + str(len(met.act)) + '\n'
                for act in met.act:
                    if act.t and act.a:
                        script_met += ' '*9 + ' '.join([act.t, act.a]) + '\n'
                        Nactm += 1
                script_met += ' '*6 + 'END\n' + ' '*3 + 'END\n'
                if Nactm > 0: script += script_met
        for x in self.qsspn.enzymes:
            if x:
                enz = self.qsspn.Enz[x]
                Nacte = 0
                Nrxn = 0
                script_enz = ' '*3 + ' '.join(['ENZYME', enz.ID, enz.rxn]) + '\n'
                script_enz_act = ' '*6 + 'ACTIVITY ' + str(len(enz.act)) + '\n'
                for act in enz.act:
                    if act.t and act.lb and act.ub:
                        script_enz_act += ' '*9 + ' '.join([act.t, act.lb, act.ub]) + '\n'
                        Nacte += 1
                script_enz_act += ' '*6 + 'END\n'
                script_enz_lst = ' '*6 + 'LIST ' + str(len(enz.list)) + '\n'
                for rxn in enz.list:
                    if rxn.ID and rxn.wei:
                        script_enz_lst += ' '*9 + ' '.join([rxn.ID, rxn.wei]) + '\n'
                        Nrxn += 1
                script_enz_lst += ' '*6 + 'END\n'
                if Nacte > 0:
                    script_enz += script_enz_act
                    if Nrxn > 0: script_enz += script_enz_lst
                    script_enz += ' '*3 + 'END\n'
                    script += script_enz
        script += 'END\n'
        return script

#    def getControlScript(self, para, monitors):
#        script = ''
#        for x in ['MODEL', 'NUMBER_OF_SAMPLES', 'SEED', 'TIME_MAX', 'MAXIMAL_TIMESTEP']: script += x + ' ' + para[x] + '\n'
#        script += '\n'
#        for x in ['OUTPUT', 'LOG', 'MONITOR']: script += x + ' ' + para[x] + '\n'
#        script += '\n'
#        script += 'TARGET ' + para['TARGET'] + '\n\n'
#        script += 'PETRI_NET_MONITORS\n'
#        for x in monitors: script += x + '\n'
#        script += 'END\n\n'
#        script += para['QSSPN_ALGORITHM'] + '\n'
#        return script

    def getControlScript(self):
        script = ''
        lines_para = self.table_para.getModel().getData()
        lines_init = self.table_init.getModel().getData()
        lines_func = self.table_func.getModel().getData()
        self.para = dict(lines_para)
        for x in lines_para:
            if x[0] not in ['GSMN_MODEL', 'GSMN_EXT_TAG', 'QSSPN_ALGORITHM']:
                if x[0] == 'MODEL': script += ' '.join([x[0], 'gsmn.tmp']) + '\n'
                else: script += ' '.join(x) + '\n'
        script += '\n'+'INITIAL_STATE'+'\n'
        for x in lines_init: script += ' '.join(x) + '\n'
        script += 'END\n\n'
        for x in lines_func: script += ' '.join(x[0:2]) + '\n' + x[2] +'\nEND\n\n'
        script += 'PETRI_NET_MONITORS\n'
        for ID in self.qsspn.Node.keys():
            if self.qsspn.Node[ID].moni==1:  script += ID+'\n'
        script += 'END\n\n'
        script += self.para['QSSPN_ALGORITHM'] + '\n'
        return script

    def writeModel(self, path):
        script_model = self.getModelScript()
        Data.Problem.writeTmp(path, script_model)

    def writeControl(self, path):
        script_control = self.getControlScript()
        Data.Problem.writeTmp(path, script_control) 

    def solve(self):
        self.writeModel(Util.MODELTMP)
        self.table_para.getModel().setValueAt('gsmn.tmp', 0, 1)
        self.writeControl(Util.CONTROLTMP)
        cmdarray = Data.Problem.getCmdArrayQSSPN(sep = io.File.separator)
        self.solver = GUI.Solver.Solver(self, cmdarray)
        self.solver.start()

    def reportSolution(self, output):#w#for genome-scale analysis
        sampletabs = swing.JTabbedPane()
        samples, monitors = QSSPN.Table.output2table(output)
        outputtable=[]
        for i in range(len(samples)):
            sample = samples[i]
            table = QSSPNTable(); table.write_output(sample, monitors, 'Output')
            tab = swing.JScrollPane(table)
            sampletabs.addTab('Sample'+str(i+1), tab)
            outputtable+=[table]
        TabCloser('Output').put(sampletabs, self)
        for table in outputtable: table.setColWidths_gsa(self.getWidth())
        if self.listener: self.listener()

    def showTrajectory(self):
        table = self.getTableAt(self.getSelectedIndex())
#        colinds=[0,2]
        colinds = [0]+list(table.getSelectedColumns())#w#
        if 2 in colinds: colinds.remove(2)
        model = table.getModel()
        lists =[[model.fields.getOrder()[i] for i in colinds]]#w# get column names
        lists += table.getSelectedLists(colinds)
        xy = GUI.PlotPanel.XYPanelTrajectory(lists)
        scrollpane = swing.JScrollPane(xy)
        TabCloser("Plot trajectory").put(scrollpane, self)


