__author__="Albert Gevorgyan"
__date__ ="$09-Mar-2010 15:39:50$"

from java import lang
from java import awt
from javax import swing
from javax import imageio
from java import io

import com.mxgraph.swing
import com.mxgraph.util
import com.mxgraph.view
from com.mxgraph.layout import *

from Model.Element import Gene, Enzyme, Reaction, Metabolite

if __name__ == "__main__":
    print "Hello World"

WIDTH = 50
HEIGHT = 30

JOIN = ':'
BORDERWIDTH = 10

LAYOUTCLASSES = {
    "Organic" : mxOrganicLayout,
    "Fast organic" : mxFastOrganicLayout,
    "Compact tree" : mxCompactTreeLayout,
    "Circle" : mxCircleLayout,
}
DEFAULT_LAYOUT = "Fast organic"

STYLES = {
    Gene : "shape=rhombus;strokeColor=black;fillColor=white",
    Enzyme: "",
    Reaction: "shape=rectangle;strokeColor=blue;fillColor=white",
    Metabolite: "shape=ellipse;strokeColor=red;fillColor=white"
}




super = com.mxgraph.swing.mxGraphComponent
class GraphPanel(super):

    def __init__(self, metabolicModel, lists, layoutclassname):

        graph = com.mxgraph.view.mxGraph()

        super.__init__(self, graph)
        self.getViewport().setOpaque(False)
        self.setBackground(awt.Color.white)

        self.IDs = {}
        self.metabolicModel = metabolicModel
        self.parseLists(lists)
        graph.getModel().beginUpdate()
        try:
            for ID in self.Lists:
                self.insertElement(ID)
        except lang.Exception, e:
            print e
        finally:
           graph.getModel().endUpdate()
        self.layOut(LAYOUTCLASSES[layoutclassname])

    def parseLists(self, lists):
        self.Lists = {}
        for List in lists:
            ID = str(List[0])
            if ID in self.metabolicModel:
                values = map(str, List[1:])
                value = JOIN.join([ID] + values)
                self.Lists[ID] = value

    def insertElement(self, ID):
        if not ID in self.IDs:
            style = STYLES[self.metabolicModel[ID].__class__]
            value = self.Lists.get(ID, ID)
            label = swing.JLabel(value)
            d = label.getPreferredSize()
            width, height = d.getWidth() + BORDERWIDTH, d.getHeight() + BORDERWIDTH

            self.IDs[ID] = self.graph.insertVertex(self.graph.getDefaultParent(), "ID", value, 0, 0, width, height, style) #wtr# ID -> "ID"
            sto = self.metabolicModel[ID].getChildren()
            for childID in sto:
                coef = sto[childID]
                self.insertElement(childID)
                self.insertEdge(ID, childID, coef)
                

    def insertEdge(self, parentID, childID, coef = 1):
        sourceID, targetID = parentID, childID
        if coef < 0:
            sourceID, targetID = childID, parentID
        source = self.IDs[sourceID]
        target = self.IDs[targetID]

        label = ""
        coef = abs(coef)
        if coef != 1:
            label = str(coef)
        self.graph.insertEdge(self.graph.getDefaultParent(), None, label, source, target)


    def layOut(self, layoutclass):
        layout = layoutclass(self.graph)
        layout.execute(self.graph.getDefaultParent())



    def save(self, path):
        img = com.mxgraph.util.mxCellRenderer.createBufferedImage(self.graph, None, 1, awt.Color.WHITE, False, None)
        imageio.ImageIO.write(img, "png", io.File(path))


    

#            stylesheet = graph.getStylesheet()
#        style = {}
#        style[mxConstants.STYLE_SHAPE] = mxConstants.SHAPE_ELLIPSE
##        style[mxConstants.STYLE_OPACITY] = 0
##style.put(mxConstants.STYLE_FONTCOLOR, "#774400");
#        stylesheet.putCellStyle("ROUNDED", style)