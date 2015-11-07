__author__="Albert Gevorgyan"
__date__ ="$29-Apr-2010 12:13:00$"

from java import lang
import java.awt.MouseInfo#wtr#
from GUI.Graph.JGraphPanel import *#wtr#
#from com.mxgraph.util import mxUndoableEdit#wtr#
import JGraphPanel#wtr#
from Model.Element import Gene, Enzyme, Reaction, Metabolite
from javax.swing import *#wtr#
import Util.Stoich #w#

super = dict
CHARCODE='UTF-8'#wtr#

class GraphModel(super):
    STYLES = {}
    JOIN = '; '

    def __init__(self, metabolicModel, kernel = None, logicals = []):
        self.metabolicModel = metabolicModel
        self.Kernel = kernel
        self.Logicals = logicals
        
    def insertLists(self, lists):
        try:#wtr#
            if lists[0][0].find('(+)')!=-1 or lists[0][0].find('(-)')!=-1:#wtr#
                lists,nlists=self.insertNewLists(lists)#wtr#
                self.parseLists(lists)
                for ID in self.Lists:
                    self.insertElement(ID)
                self.parseLists(nlists)#wtr#
                for ID in self.Lists:#wtr#
                    self.insertNewElement(ID) #wtr#
            else:#wtr#
                self.parseLists(lists) #wtr#
                for ID in self.Lists: #wtr#
                    self.insertElement(ID) #wtr#
        except lang.Exception, e:
            print e

    def parseLists(self, lists):
        self.Lists = {}
        for List in lists:
            ID = str(List[0])
            if ID in self.metabolicModel:
                name = self.metabolicModel[ID].get(Util.NAME, ID) or ID
                values = map(str, List[1:])
                if name in values:
                    values.remove(name)
                #value = self.JOIN.join([name] + values)
                self.Lists[ID] = values

    def insertElement(self, ID, parentID = ''):
        if not ID in self:
            element = self.metabolicModel[ID]
            name = element.get(Util.NAME, ID) or ID

            values = self.Lists.get(ID, [])
            label = self.JOIN.join([name] + values)
            theclass = element.__class__
            style = self.STYLES.get(theclass, theclass.__name__)
            if ID in self.Logicals or name in self.Logicals:
                ID += '_' + parentID
            self[ID] = self.Kernel.addElement(ID, label, style)
            sign = 1
            if theclass == Reaction and len(values) > 0:
                try: sign = float(values[0])
                except: pass
            sto = element.getChildren()
            if theclass == Reaction: sig = element.getEqSigmet() #w#get sigmet for equation
            for childID in sto:
                coef = sto[childID] * sign
                childID = self.insertElement(childID, ID)
                self.insertEdge(ID, childID, theclass, sig, coef) #w# add sig
        return ID

    def insertEdge(self, parentID, childID, parentClass, sig, coef = 1):#w# add sig
        sourceID, targetID = parentID, childID
        if coef < 0:
            sourceID, targetID = childID, parentID
        label = ""
        coef = abs(coef)
        if coef != 1:
            label = str(coef)
        style = self.STYLES.get(parentClass, parentClass.__name__)
        self.Kernel.addEdge(sourceID, targetID, label, style, sig)#w# add sig
        self.Kernel.graph.selectVertices() #wtr#

    def insertNewLists(self,lists): #wtr#
        nlists=[]
        n2lists=[]
        plus=[]
        minus=[]
        for ID in lists:
            if ID[0].find('(+)')!=-1:
                plus.append(ID)
            else:
                minus.append(ID)
        for x in range(len(plus)):
            sub=plus[x][0].split(' ')
            sub.insert(1,str(plus[x][5]))
            nlists.append(sub)
        for x in range(len(minus)):
            sub=minus[x][0].split(' ')
            sub.insert(1,str(minus[x][5]))
            n2lists.append(sub)
        return nlists,n2lists
    
    def insertNewElement(self, ID, parentID = ''):
        if ID in self:
            element = self.metabolicModel[ID]
            name = element.get(Util.NAME, ID) or ID

            values = self.Lists.get(ID, [])
            label = self.JOIN.join([name] + values)
            theclass = element.__class__
            style = self.STYLES.get(theclass, theclass.__name__)
            if ID in self.Logicals or name in self.Logicals:
                ID += '_' + parentID
            self[ID] = self.Kernel.addElement(ID, label, style)
            sign = 1
            if theclass == Reaction and len(values) > 0:
                try: sign = float(values[0])
                except: pass
            sto = element.getChildren()
            if theclass == Reaction: sig = element.getEqSigmet() #w#get sigmet for equation
            for childID in sto:
                coef = sto[childID] * sign
                childID = self.insertElement(childID, ID)
                self.insertEdge(ID, childID, theclass, sig, coef)
        return ID
        
class GraphSabMenu():
    def __init__(self,e,component,graph,cells,edges):
        self.component=component
        self.graph=graph
        self.cell=self.graph.getSelectionCells()
        self.cells=cells
        self.edges=edges
        
        popup=JPopupMenu()  

        menuitem2 =JMenu("Select")
        selectsubmenu1=JMenuItem("All Vertices",actionPerformed=self.selectVertices)
        selectsubmenu2=JMenuItem("All Edges",actionPerformed=self.selectEdge)
        selectsubmenu3=JMenuItem("All",actionPerformed=self.selectAll)
        
        #menuitem3 =JMenuItem("Delete",actionPerformed=self.Delete)
        menuitem2.add(selectsubmenu1)
        menuitem2.add(selectsubmenu2)
        menuitem2.addSeparator()
        menuitem2.add(selectsubmenu3)
        menuitem2.addSeparator()
        
        menuitem1=JMenu("Zoom")
        zoomitem1 =JMenuItem("+",actionPerformed=self.zoomIn)
        zoomitem2 =JMenuItem("-",actionPerformed=self.zoomOut)
        zoomitem3=JMenuItem("Default",actionPerformed=self.zoomDefault)
        menuitem1.add(zoomitem1)
        menuitem1.add(zoomitem2)
        menuitem1.add(zoomitem3)
        
        self.textfield=JTextField(10)
        self.textfield2=JTextField(10)
        serachbutton=JButton("Search",actionPerformed=self.serach)
        menuitem2.add(self.textfield)
        menuitem2.add(serachbutton)
        threshold=JButton("Highlight",actionPerformed=self.highlight)
        
        menuitem3=JMenu("Edge")
        menuitem3.add(self.textfield2)
        menuitem3.add(threshold)

#        menuitem4 =JMenuItem("Delete",actionPerformed=self.delete) #w#
#        menuitem5=JMenuItem("Undo",actionPerformed=undo)
        
        
        popup.add(menuitem1)
        popup.add(menuitem2)
        popup.add(menuitem3)
#        popup.add(menuitem4)
        #popup.add(menuitem5)
        
        mouse=self.component.getMousePosition()
        popup.show(self.component, mouse.x+10, mouse.y+10)
    
    def zoomIn(self,e=None):
        self.component.zoomIn()
        
    def zoomOut(self,e=None):
        self.component.zoomOut()
        
    def zoomDefault(self,e=None):
        self.graph.getView().setScale(1.0)
    
    def selectAll(self,e=None):
        self.graph.selectAll()
    
    def selectVertices(self,e=None):
        self.graph.selectVertices()
        
    def selectEdge(self,e=None):
        self.graph.selectEdges()
        
    def delete(self,e=None):#w#
        self.graph.removeSelectionCells(self.cell)
        self.graph.repaint()
    
    def serach(self,e=None):
        text=self.textfield.getText()
        labels={}
        self.graph.selectVertices()
        cells=self.graph.getSelectionCells()
        self.graph.clearSelection()
        for x in cells:
            labels[x.getId()]=x
        try:
            if labels.keys().count(text)!=0:
                self.graph.setSelectionCell(labels[text])
                self.component.scrollCellToVisible(labels[text],1) 
            else:
                pass
        finally:
            self.component.scrollCellToVisible(text)
        
    def highlight(self,e=None):
        #tame=[0.0,5.0,7.0,9.0,10.0]
        text=self.textfield2.getText()
        thre=text.split(' ')
        for edge in self.edges:
            value=self.getEdgeWidth(edge.getValue(),thre)
            style=edge.getStyle().split(';')
            style[1]="strokeWidth=%d"%value
            Z=style[0]+";"+style[1]
            self.graph.setSelectionCell(edge)
            self.graph.setCellStyle(Z)
        self.graph.clearSelection()
        
    
    def getReaction(self,label):
        try:
            label=abs(float(label))
            return label
        except:
            return 0.0
        
    def getEdgeWidth(self,label,thre): #It decides a width of edge
        flux=abs(self.getReaction(label))
        max=len(thre)
        try:
            if flux<=float(thre[0]):
                return 1
            elif flux<=float(thre[1]):
                return 3
            elif flux<=float(thre[2]):
                return 4
            elif flux<=float(thre[3]):
                return 5
            elif flux<=float(thre[4]):
                return 6
            else:
                return 7
        except:
            return max+2
        

class EdgeSabMenu():
    def __init__(self,e,component,graph,cell):
        self.component=component
        self.graph=graph
        self.cell=cell 
        popupmenu=JPopupMenu()
        menuitem0=JTextField(15)
        menuitem1=JTextField(15)
        menuitem0.setText(self.cell.getSource().getValue().encode(CHARCODE))
        menuitem1.setText(self.cell.getTarget().getValue().encode(CHARCODE))
        menuitem2 =JMenu("Color")
        coloritem2=JMenuItem("color",actionPerformed=self.color)
        menuitem2.add(coloritem2)
        
        #menuitem2 =JMenuItem("Delete",actionPerformed=self.delete)
        
        popupmenu.add(menuitem0)
        popupmenu.add(menuitem1)
        popupmenu.add(menuitem2)
        
        mouse=self.component.getMousePosition()
        popupmenu.show(self.component, mouse.x+5, mouse.y+5)
    
    def color(self,e=None):
        style=self.cell.getStyle().split(';')
        style[0]="strokeColor=red"
        Z=style[0]+";"+style[1]
        self.graph.setCellStyle(Z)
        
    def delete(self,e=None):
        self.graph.removeSelectionCell(self.cell)
        self.graph.repaint()

class CellsSabMenu(): #w#
    def __init__(self,e,component,graph,cell):
        self.component=component
        self.graph=graph
        self.cell=cell
        popup=JPopupMenu()

        menuitem2 =JMenuItem("Delete",actionPerformed=self.delete)
        popup.add(menuitem2)

        mouse=self.component.getMousePosition()
        popup.show(self.component, mouse.x+5, mouse.y+5)
        
    def delete(self,e=None):
        self.graph.removeCells()
#        self.graph.removeCells(self.cell)

#    def delete(self,e=None):
#        self.graph.removeSelectionCell(self.cell)
#        self.graph.repaint()