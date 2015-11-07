from xml.dom.minidom import getDOMImplementation

from GUI.Graph.Graph import *
from javax import swing

BORDERWIDTH = 10

ATTRS = {
    "xmlns:dc": "http://purl.org/dc/elements/1.1/",
    "xmlns:xlink": "http://www.w3.org/1999/xlink",
    "xmlns:rdf": "http://www.w3.org/1999/02/22-rdf-syntax-ns#",
    "xmlns:cy": "http://www.cytoscape.org",
    "xmlns": "http://www.cs.rpi.edu/XGMML",
    "directed": "1"
}

def write(model, lists, path, logicals):
    XGMMLKernel(model, lists, logicals).save(path)


class XGMMLKernel:

    def __init__(self, model, lists, logicals = []):
        self.metabolicModel = model

        self.Doc = getDOMImplementation().createDocument(None, "graph", None)
        self.Graph = self.Doc.documentElement
        self.Graph.setAttribute("label", model.getName())
        for attr, value in ATTRS.items():
            self.Graph.setAttribute(attr, value)

        self.edgeCounter = 1
        self.Nodes = {}
        self.graphModel = GraphModel(model, self, logicals)
        self.graphModel.insertLists(lists)


    def addElement(self, ID, label, style):
        node = self.Doc.createElement('node')
        node.setAttribute('label', ID)

        nID = len(self.Nodes)
        node.setAttribute('id', "%d" % nID)
        self.Nodes[ID] = nID

        klass = self.createAttribute('class', style)
        node.appendChild(klass)
        name = self.createAttribute('name', label)
        node.appendChild(name)

        wlabel = swing.JLabel(label)
        d = wlabel.getPreferredSize()
        width = self.createAttribute('width', "%f" % (d.getWidth() + BORDERWIDTH))
        node.appendChild(width)
        height = self.createAttribute('height', "%f" % (d.getHeight() + BORDERWIDTH))
        node.appendChild(height)
        self.Graph.appendChild(node)
        return node

    def addEdge(self, sourceID, targetID, label, style):
        edge = self.Doc.createElement('edge')
        edge.setAttribute('label', label)
        edge.setAttribute('source', "%d" % self.Nodes[sourceID])
        edge.setAttribute('target', "%d" % self.Nodes[targetID])

        att = self.createAttribute('coef', label)
        edge.appendChild(att)
        att = self.createAttribute('interaction', '')
        edge.appendChild(att)
        
        self.Graph.appendChild(edge)

    def createAttribute(self, name, value):
        att = self.Doc.createElement('att')
        att.setAttribute('type', 'string')
        att.setAttribute('name', name)
        att.setAttribute('value', value)
        return att

    def save(self, path):
        f = open(path, 'w')
        self.Doc.writexml(f, addindent = "\t", newl="\n")
        f.close()
