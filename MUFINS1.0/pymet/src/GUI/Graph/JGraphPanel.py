__author__="Albert Gevorgyan"
__date__ ="$09-Mar-2010 15:39:50$"

from java import awt
from javax import swing
from javax import imageio
from java import io

import com.mxgraph.swing
import com.mxgraph.util
import com.mxgraph.view
from java.awt.event import * #wtr#
from com.mxgraph.layout import *
from com.mxgraph.layout.hierarchical import *
from com.mxgraph.layout.orthogonal import *
#from com.mxgraph.util import mxUndoManager #wtr#
import JGraphPanel #wtr#
import Util #w#

WIDTH = 50
HEIGHT = 30
BORDERWIDTH = 10
EDGE_LABEL_GEO= 27.0 #wtr#
EDGE_LABEL_COLOR= '#00ff00' #wtr#


LAYOUTCLASSES = {
    "Organic" : (mxOrganicLayout, [awt.Rectangle(2000, 4000)]),
    "Fast organic" : (mxFastOrganicLayout, []),
    "Compact tree" : (mxCompactTreeLayout, []),
    "Circle" : (mxCircleLayout, []),
    #"Edge label" : (mxEdgeLabelLayout, []),
    "Hierarchical" : (mxHierarchicalLayout, []),
    #"Orthogonal" : (mxOrthogonalLayout, []),
    #"Parallel edge" : (mxParallelEdgeLayout, [100]),
    #"Partition" : (mxPartitionLayout, [False, 100]),
    #"Stack" : (mxStackLayout, [False, 100]),
}
DEFAULT_LAYOUT = "Hierarchical"
from Graph import *

class JGraphModel(GraphModel):
    STYLES = {
        Gene : {"shape": "rhombus", "strokeColor" : "black", "fillColor" : "white", "align" : "center"},
        Enzyme: {"shape": "ellipse", "strokeColor" : "green", "fillColor" : "white", "align" : "center"},
        Reaction: {"shape": "rectangle", "strokeColor" : "blue", "fillColor" : "white", "align" : "center"},
        Metabolite: {"shape": "ellipse", "strokeColor" : "red", "fillColor" : "white", "align" : "center"},
    }

super = com.mxgraph.swing.mxGraphComponent
class JGraphPanel(super,GraphSabMenu): #wtr# GraphSabMenu
    def __init__(self, metabolicModel, lists, layoutclassname, logicals):
        self.dthre=[0,1,10]#w#[0.0,1.0,3,4]->[0,1,10]
#        graph = com.mxgraph.view.mxGraph(cellsDisconnectable = True, cellsDeletable = True)
        graph = com.mxgraph.view.mxGraph()
        map=graph.getStylesheet().getDefaultEdgeStyle().put("fontColor",EDGE_LABEL_COLOR) #wtr#
        super.__init__(self, graph)
        self.getViewport().setOpaque(False)
        self.setBackground(awt.Color.white)
        graph.getModel().beginUpdate()
        self.graphModel = JGraphModel(metabolicModel, self, logicals)
        try:
            self.graphModel.insertLists(lists)
        finally:
            graph.getModel().endUpdate()
        self.layOut(LAYOUTCLASSES[layoutclassname],graph) #wtr# graph
        self.getGraphControl().addMouseListener(ScreenMouseListener(self,graph,self.cells,self.edges)) #wtr#
    
    def addElement(self, ID, label, style):
        stylestr = ";".join([key + '=' + val for key, val in style.items()])
        wlabel = swing.JLabel(label)
        d = wlabel.getPreferredSize()
        width, height = d.getWidth() + BORDERWIDTH, d.getHeight() + BORDERWIDTH
        return self.graph.insertVertex(self.graph.getDefaultParent(), ID, label, 0, 0, width, height, stylestr)
        
    def addEdge(self, sourceID, targetID, label, style, sig):#w# add signalling edge style
        source = self.graphModel[sourceID]
        target = self.graphModel[targetID]
        stylestr = "strokeColor=%s" % style["strokeColor"]
        rate=self.getEdgeWidth(label,self.dthre) #wtr#
        stylestr = stylestr+";strokeWidth=%d"%rate #wtr#
        if sourceID in sig:
            if sig[sourceID]=='&': stylestr = stylestr + ';dashed=1' #w# activator
            elif sig[sourceID]=='~': stylestr = stylestr + ';dashed=1;endArrow=oval' #inhibitor
        self.graph.insertEdge(self.graph.getDefaultParent(), None, label, source, target, stylestr)

    def layOut(self, layoutclass,graph): #wtr# graph
        layoutclass, args = layoutclass[0], layoutclass[1]
        layout = layoutclass(self.graph, *args)
        layout.execute(self.graph.getDefaultParent())
        vet, edge=self.getCells(self.graph) #wtr#
        self.cells,self.edges=vet,edge #wtr#
        self.layoutEdge(edge,graph) #wtr#
        
    ####Wataru###########
    def layoutEdge(self,edges,graph): # move a location of a label of edge
        for edge in edges:
            edge.getGeometry().setY(EDGE_LABEL_GEO)
        graph.refresh()
    
    def getCells(self,graphname): #return all cells
        graphname.selectVertices()
        ve =graphname.getSelectionCells() #vertices
        graphname.selectEdges()
        edg=graphname.getSelectionCells() #edges
        graphname.clearSelection() 
        return ve,edg
    ###########################

    def save(self, path):
        format = path.rsplit('.', 1)[-1]
        img = com.mxgraph.util.mxCellRenderer.createBufferedImage(self.graph, None, 1, awt.Color.WHITE, False, None)
        imageio.ImageIO.write(img, format, io.File(path)) 

################################
#######Wataru######

class SaveGraph():
    def __init__(self,path,graph,graphName):
        
     if graph!="":
        try:
            fi=open(path,"w")
            fi.write(graphName)
            fi.write("\n")
            graph.selectVertices()
            ve =graph.getSelectionCells()
            graph.selectEdges()
            edg=graph.getSelectionCells()
            graph.clearSelection()
    
            for card in ve:
                vv=self.getVertex(card)
                fi.write(vv)
            for card in edg:
                try:
                    vv=self.getEdge(card)
                    fi.write(vv)
                except:
                    pass
            fi.close()
        except:
            pass
     else:
         pass
    
    def getVertex(self,card): #returns Vertexs information
        if card.getValue()!="":
            v=card.getValue()
        else:
            v=""
        g    =card.getGeometry()
        id   =card.getId()
        style=card.getStyle()
        gg="Node"+"\t"+"%s"%id+"\t"+"%s"%v+"\t"+"%d"%g.x+"\t"+"%d"%g.y+"\t"+"%d"%g.width+"\t"+"%d"%g.height+"\t"+style+"\n"
        return gg
    
    def getEdge(self,card): #returns Edges information
        id=card.getId()
        if card.getSource()!=None:
            c=card.getSource().getId()
        else:
            c=""
        if card.getTarget()!=None:
            t=card.getTarget().getId()
        else:
            t=""
        if card.getValue()!="":
            v=card.getValue()
        else:
            v=""
        if card.getStyle()!=None:
            style=card.getStyle()
        else:
            style=""
        gg="Edge"+"\t"+"%s"%id+"\t"+"%s"%v+"\t"+"%s"%c+"\t"+"%s"%t+"\t"+style+"\n"
        return gg
        
##################################
#######Wataru#####################
class ReGraphPanel(JGraphPanel):
    def __init__(self, path, value, program):
        self.tabtitle=""
        self.dthre=[0,1,10] #w#[0.0,0.1,2,3,4]->[0,1,10]
        node={}
        Cnode=[] #cell nodes
        Cedge=[] #cell edges
        graph = com.mxgraph.view.mxGraph(cellsDisconnectable = True, cellsDeletable = True)
        super.__init__(self, graph)
        Cnode,Cedge=self.loadFile(path,Cnode,Cedge) #cells : new graph readin cells
        self.getViewport().setOpaque(False)
        self.setBackground(awt.Color.white)
        graph.getModel().beginUpdate()
        if value:
            self.updateNode(Cnode,value)
            if program in ('FBA', Util.GIMME): self.updateEdge(value,Cedge)
        try:
            self.addElement(graph,Cnode,Cedge,node)
        finally:
            graph.getModel().endUpdate()
            self.getGraphControl().addMouseListener(ScreenMouseListener(self,graph,self.cells,self.edges))

    def addElement(self,graph,Cnode,Cedge,node):
        parent2=graph.getDefaultParent()
        map=graph.getStylesheet().getDefaultEdgeStyle().put('fontColor',EDGE_LABEL_COLOR)
        self.cells=[]
        self.edges=[]
        for cell in Cnode:
            v=graph.insertVertex(parent2, cell[1],cell[2],cell[3],cell[4],cell[5],cell[6],cell[7])
            self.cells.append(v)
            node[cell[1]]=v
        for cell in Cedge:
            e=graph.insertEdge(parent2,cell[1],cell[2],node[cell[3]],node[cell[4]],cell[5])
            e.getGeometry().setY(EDGE_LABEL_GEO)
            self.edges.append(e)

    def loadFile(self,path,Cnode,Cedge):
        f_GRAPH=open(path,"r")
        self.tabtitle=f_GRAPH.readline().strip()
        for line in f_GRAPH:
            tt=line.strip().split('\t')
            if tt[0]=='Node':
                tt[3]=int(tt[3])
                tt[4]=int(tt[4])
                tt[5]=int(tt[5])
                tt[6]=int(tt[6])
                Cnode.append(tt)
            else: Cedge.append(tt)
        f_GRAPH.close()
        return Cnode,Cedge

    def updateNode(self,Cnode,value):
        for cell in Cnode:
            if cell[1] in value.keys():
                cell[2]=cell[2].split(':')[0]+': '+value[cell[1]]
                cell[5]=7*len(cell[2])

    def updateEdge(self,value,Cedge):
        for cell in Cedge:
            if cell[3] in value.keys():
                if not cell[2]: cell[2]=1
                cell[2]=Util.formatFloat(float(value[cell[3]])*float(cell[2]))
            elif cell[4] in value.keys():
                if not cell[2]: cell[2]=1
                cell[2]=Util.formatFloat(float(value[cell[4]])*float(cell[2]))
            width=self.getEdgeWidth(cell[2],self.dthre)
            para=cell[5].split(';')
            para[1]="strokeWidth=%s"%width
            cell[5]=';'.join(para)

    def getClassName(self):
        return self.tabtitle

    def getCell(self):
        return self.cells

#####################################
#####WATARU##########################Mouse Listener
class ScreenMouseListener(MouseAdapter): 
    def __init__(self,component,graph,cells,edges):
        self.graph=graph
        self.component=component
        self.cells=cells
        self.edges=edges
        
    def mousePressed(self,e):
        mouse=self.component.getMousePosition()
        self.press=self.component.getCellAt(e.getX(),e.getY())###
        self.x=mouse.x
        self.y=mouse.y
        try:
            p=e.getButton()
            if p==3:
                cell=self.component.getCellAt(self.x,self.y)
                if cell.isEdge()==True or cell.isVertex()==True: #w#
                    CellsSabMenu(e,self.component,self.graph,cell) #w#
#                    EdgeSabMenu(e,self.component,self.graph,cell)
                else:
                    pass
                    #print "error"
            else:
                self.x=mouse.x
                self.y=mouse.y
        except:
            pass
                
    def mouseDragged(self,e):
        pass
    
    def mouseReleased(self,e):
        mouse=self.component.getMousePosition()
        if self.press==None:
                self.mouseMove(self.x,self.y,mouse.x,mouse.y)
        else:
            pass
    
    def mouseClicked(self,e):
        f=e.getClickCount()
        if f==2:
            if e.getButton()==1:
                self.component.zoomIn()
            else:
                self.component.zoomOut()
                self.component.repaint()
            
        else:
            p=e.getButton()
            if p==3:
                GraphSabMenu(e,self.component,self.graph,self.cells,self.edges)#GraphMenu
            
    def mouseEntered(self,e):
          pass
    
    def mouseMove(self,x,y,w,h):#drag and move
        hori=self.component.getHorizontalScrollBar()
        vert=self.component.getVerticalScrollBar()
        nx=x-w
        ny=y-h
        hori.setValue(hori.getValue()+nx)
        vert.setValue(vert.getValue()+ny)
        
    
    #def focus(self,x,y,w,h):
    #    if x!=w:
    #        if y!=h:
    #            if w-x>90:
    #                self.component.zoomIn()
    #            elif x-w>90:
    #                self.component.zoomOut()
    #        else:
    #            pass
#######################################################

#from java import lang
#from java import awt
#from javax import swing
#from javax import imageio
#from java import io
#
#import com.mxgraph.swing
#import com.mxgraph.util
#import com.mxgraph.view
#from com.mxgraph.layout import *
#from com.mxgraph.layout.hierarchical import *
#from com.mxgraph.layout.orthogonal import *
#
#from Model.Element import Gene, Enzyme, Reaction, Metabolite
#
#
#WIDTH = 50
#HEIGHT = 30
#
#JOIN = ':'
#BORDERWIDTH = 10
#
#
#from settings import LOGICAL
#
#LAYOUTCLASSES = {
#    "Organic" : (mxOrganicLayout, [awt.Rectangle(2000, 4000)]),
#    "Fast organic" : (mxFastOrganicLayout, []),
#    "Compact tree" : (mxCompactTreeLayout, []),
#    "Circle" : (mxCircleLayout, []),
#    #"Edge label" : (mxEdgeLabelLayout, []),
#    "Hierarchical" : (mxHierarchicalLayout, []),
#    #"Orthogonal" : (mxOrthogonalLayout, []),
#    #"Parallel edge" : (mxParallelEdgeLayout, [100]),
#    #"Partition" : (mxPartitionLayout, [False, 100]),
#    #"Stack" : (mxStackLayout, [False, 100]),
#}
#DEFAULT_LAYOUT = "Hierarchical"
#
#STYLES = {
#    Gene : {"shape": "rhombus", "strokeColor" : "black", "fillColor" : "white", "align" : "center"},
#    Enzyme: {"shape": "ellipse", "strokeColor" : "green", "fillColor" : "white", "align" : "center"},
#    Reaction: {"shape": "rectangle", "strokeColor" : "blue", "fillColor" : "white", "align" : "center"},
#    Metabolite: {"shape": "ellipse", "strokeColor" : "red", "fillColor" : "white", "align" : "center"},
#}
#
#from Model.Element import NAME
#
#
#super = com.mxgraph.swing.mxGraphComponent
#class GraphPanel(super):
#
#    def __init__(self, metabolicModel, lists, layoutclassname):
#
#        graph = com.mxgraph.view.mxGraph(cellsDisconnectable = True, cellsDeletable = True)
#
#        super.__init__(self, graph)
#        self.getViewport().setOpaque(False)
#        self.setBackground(awt.Color.white)
#
#        self.IDs = {}
#        self.metabolicModel = metabolicModel
#
#        self.parseLists(lists)
#
#        graph.getModel().beginUpdate()
#        try:
#            for ID in self.Lists:
#                self.insertElement(ID)
#        except lang.Exception, e:
#            print e
#        finally:
#           graph.getModel().endUpdate()
#
#        self.layOut(LAYOUTCLASSES[layoutclassname])
#
#
#
#    def parseLists(self, lists):
#        self.Lists = {}
#        for List in lists:
#            ID = str(List[0])
#            if ID in self.metabolicModel:
#                name = self.metabolicModel[ID].get(NAME, ID) or ID
#                values = map(str, List[1:])
#                if name in values:
#                    values.remove(name)
#                value = JOIN.join([name] + values)
#                self.Lists[ID] = value
#
#
#    def insertElement(self, ID, parentID = ''):
#        if not ID in self.IDs:
#            element = self.metabolicModel[ID]
#            name = element.get(NAME, ID) or ID
#            value = self.Lists.get(ID, name)
#            theclass = element.__class__
#
#            label = swing.JLabel(value)
#            d = label.getPreferredSize()
#            width, height = d.getWidth() + BORDERWIDTH, d.getHeight() + BORDERWIDTH
#
#            styledict = STYLES[theclass]
#            style = ";".join([key + '=' + val for key, val in styledict.items()])
#
#            if name in LOGICAL:
#                ID += '_' + parentID
#
#            self.IDs[ID] = self.graph.insertVertex(self.graph.getDefaultParent(), ID, value, 0, 0, width, height, style)
#            sto = element.getChildren()
#            for childID in sto:
#                coef = sto[childID]
#                childID = self.insertElement(childID, ID)
#                self.insertEdge(ID, childID, theclass, coef)
#        return ID
#
#
#    def insertEdge(self, parentID, childID, parentClass, coef = 1):
#        sourceID, targetID = parentID, childID
#        if coef < 0:
#            sourceID, targetID = childID, parentID

#
#        label = ""
#        coef = abs(coef)
#        if coef != 1:
#            label = str(coef)
#        style = "strokeColor=%s" % STYLES[parentClass]["strokeColor"]
#        self.graph.insertEdge(self.graph.getDefaultParent(), None, label, source, target, style)
#
#
#    def layOut(self, layoutclass):
#        layoutclass, args = layoutclass[0], layoutclass[1]
#        layout = layoutclass(self.graph, *args)
#        layout.execute(self.graph.getDefaultParent())
#
#
#
#    def save(self, path):
#        img = com.mxgraph.util.mxCellRenderer.createBufferedImage(self.graph, None, 1, awt.Color.WHITE, False, None)
#        imageio.ImageIO.write(img, "png", io.File(path))