__author__="Albert Gevorgyan"
__date__ ="$28-Apr-2010 16:33:11$"


from javax import swing

import pydot.pydot as PD
from Graph import *

FONTSIZE = "16"


FORMAT = "png"
IMGFILE = "tmp." + FORMAT
SHOWTEXT = """<html lang='en'><head></head><body><img src="file:%s"></body></html>""" % IMGFILE

class DotModel(GraphModel):
    STYLES = {
        Gene : {"shape": "diamond", "color" : "black"},
        Enzyme: {"shape": "ellipse", "color" : "green"},
        Reaction: {"shape": "rectangle", "color" : "blue"},
        Metabolite: {"shape": "ellipse", "color" : "red"},
    }

super = swing.JEditorPane
class DotPanel(super):

    def __init__(self, metabolicModel, lists, logicals):
        self.G = PD.Dot()
        self.G.set_center('true')
        self.graphModel = DotModel(metabolicModel, self, logicals)
        self.graphModel.insertLists(lists)
        path = IMGFILE
        self.G.write(path, format = FORMAT)
        self.setContentType("text/html")
        self.setText(SHOWTEXT)
        
    def addElement(self, ID, label, style):
        Node = PD.Node(ID)
        Node.set_shape(style['shape'])
        Node.set_color(style['color'])
        #Node.set_fontsize(FONTSIZE)
        Node.set_label(label)
        self.G.add_node(Node)
        return Node

    def addEdge(self, sourceID, targetID, label, style):
        Edge = PD.Edge(sourceID, targetID)
        Edge.set_label(label)
        Edge.set_style('filled')
        Edge.set_color(style['color'])
        self.G.add_edge(Edge)

    def save(self, path):
        format = path.rsplit('.', 1)[-1]
        self.G.write(path, format = format)





#from Model.Element import Gene, Enzyme, Reaction, Metabolite
#from Model.Element import NAME
#import pydot.pydot as PD
#
#
#JOIN = ':'
#FONTSIZE = "16"
#
#STYLES = {
#    Gene : {"shape": "diamond", "color" : "black"},
#    Enzyme: {"shape": "ellipse", "color" : "green"},
#    Reaction: {"shape": "rectangle", "color" : "blue"},
#    Metabolite: {"shape": "ellipse", "color" : "red"},
#}
#
#
#FORMAT = "png"
#IMGFILE = "tmp." + FORMAT
#SHOWTEXT = """<html lang='en'><head></head><body><img src="file:%s"></body></html>""" % IMGFILE
#
#
#from settings import LOGICAL


#super = swing.JEditorPane
#class DotPanel(super):
#
#    def __init__(self, metabolicModel, lists):
#
#        self.IDs = {}
#        self.metabolicModel = metabolicModel
#
#        self.G = PD.Dot()
#        self.G.set_center('true')
#        self.parseLists(lists)
#
#        try:
#            for ID in self.Lists:
#                self.insertElement(ID)
#        except lang.Exception, e:
#            print e
#
#        path = IMGFILE
#        self.G.write(path, format = FORMAT)
#        self.setContentType("text/html")
#        self.setText(SHOWTEXT)
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
#
#            label = self.Lists.get(ID, name)
#            theclass = element.__class__
#            style = STYLES[theclass]
#            if name in LOGICAL:
#                ID += '_' + parentID
#            Node = PD.Node(ID)
#            Node.set_shape(style['shape'])
#            Node.set_color(style['color'])
#            Node.set_fontsize(FONTSIZE)
#            Node.set_label(label)
#            self.G.add_node(Node)
#            self.IDs[ID] = Node
#
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
#        label = ""
#        coef = abs(coef)
#        if coef != 1:
#            label = str(coef)
#
#        Edge = PD.Edge(sourceID, targetID)
#        Edge.set_label(label)
#        Edge.set_style('filled')
#
#        style = STYLES[parentClass]
#        Edge.set_color(style['color'])
#
#        self.G.add_edge(Edge)
#
#
#    def save(self, path):
#        self.G.write(path, format = FORMAT)