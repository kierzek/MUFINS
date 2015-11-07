#!/usr/bin/python
#
# 26/04/2011
# Parser for snoopy extended petri net file, with functions writing PNFBA file.
#
# 01/10/2011
# v2 - Warns about nodes without names
#
# 13/12/2011
# v3 - Improved replace of CDATA that allows for "[" within CDATA lines
# add "useid" argument to make the software use id rather than name
#
# 27/03/2012
# v4 - Handling of reaction LIST in ENZYME was added.
#
# 03/01/2012
# v6 - Using ID when there is no name and using name when available.
#
# 23/03/2013
# v7 - Removing regexp that remove comments and adapting script to large number of models
#      Function setMax has been edited to set MAX to default if literal after MAX is not a digit
#
# 03/12/2014
# v9 - Support for RESET and FLUX transitions

import re
import sys
import xml.dom.minidom

class Place:
    id = ""
    name = ""
    state = 0
    annotation = ""
    type = 1
    max = 1

    def printActivity(self, type, cutoff, annot):
        ss = ''
        if annot.find("ACTIVITY") >= 0:
            l = annot.split()
            for i in l:
                w = l.pop(0)
                if w == "ACTIVITY":
                    levels = l.pop(0)
                    ss += "         ACTIVITY " + levels + '\n'
                    for j in range(0, int(levels)):
                        ss += "            " + l.pop(0) + " " + l.pop(0) + '\n'
            ss += "         END" + '\n'
        elif type == "Inhibitor Edge":
            ss += "         ACTIVITY 2" + '\n'
            ss += "           0 1.0" + '\n'
            ss += "           " + cutoff + " 0.0" + '\n'
            ss += "         END" + '\n'
        else:
            ss += "         ACTIVITY 2" + '\n'
            ss += "           0 0.0" + '\n'
            ss += "           " + cutoff + " 1.0" + '\n'
            ss += "         END" + '\n'
        return ss

    def printStoichiometry(self, annot, name, substrate):
        ss = ''
        if annot.find("STOICHIOMETRY") >= 0:
            l = annot.split()
            for i in range(0, len(l)):
                if l[i] == "STOICHIOMETRY":
                    if substrate:
                        ss += "         STOICHIOMETRY " + l[i + 1] + '\n'
#               return False
                    else:
                        ss += "      PRODUCT_STOICHIOMETRY " + name + " " + l[i + 1] + '\n'
#               return True
#      return False
        return ss

    def testStoichiometry(self, annot, name, substrate):
        if annot.find("STOICHIOMETRY") >= 0:
            l = annot.split()
            for i in range(0, len(l)):
                if l[i] == "STOICHIOMETRY":
                    if substrate:
    #               ss += "         STOICHIOMETRY " + l[i+1] + '\n'
                        return False
                    else:
    #               ss += "      PRODUCT_STOICHIOMETRY " + name + " " + l[i+1] + '\n'
                        return True
        return False

    def setType(self):
        if self.annotation.find("ENZYME") >= 0:
            self.type = 3
        if self.annotation.find("METABOLITE") >= 0:
            self.type = 2
        if self.annotation.find("NUTRIENT") >= 0:
            self.type = 4
        if self.annotation.find("OBJECTIVE") >= 0:
            self.type = 5

    def setMax(self, mx):
        if self.annotation.find("MAX") >= 0:
            l = self.annotation.split()
            if len(l) < 2:
                self.max = mx
                return
            for i in l:
                w = l.pop(0)
                if w == "MAX":
                    w1 = l.pop(0)
                    if w1.isdigit():
                        self.max = int(w1)
                    else:
                        self.max = mx
        else:
            self.max = mx

    def printEnzyme(self):
        ss = ''
        l = self.annotation.split()
        for i in l:
            w = l.pop(0)
            if w == "ENZYME":
                rname = l.pop(0)
                ss += "   ENZYME " + self.name + " " + rname + '\n'
            if w == "ACTIVITY":
                levels = l.pop(0)
                ss += "      ACTIVITY " + levels + '\n'
                for i in range(0, int(levels)):
                    ss += "         " + l.pop(0) + " " + l.pop(0) + " " + l.pop(0) + '\n'
                ss += "      END" + '\n'
            if w == "LIST":
                items = l.pop(0)
                ss += "      LIST " + items + '\n'
                for i in range(0, int(items)):
                    ss += "         " + l.pop(0) + " " + l.pop(0) + '\n'
                ss += "      END" + '\n'
        ss += "   END" + '\n'
        return ss

    def printNutrient(self):
        ss = ''
        l = self.annotation.split()
        while len(l) > 0:
            w = l.pop(0)
            if w == "NUTRIENT":
                ss += "   NUTRIENT " + self.name + " " + l.pop(0) + " " + l.pop(0) + " " + l.pop(0) + '\n'
            if w == "ACTIVITY":
                levels = l.pop(0)
                ss += "      ACTIVITY " + levels + '\n'
                for i in range(0, int(levels)):
                    ss += "         " + l.pop(0) + " " + l.pop(0) + " " + l.pop(0) + '\n'
                ss += "      END" + '\n'
                ss += "   END" + '\n'
        return ss

    def printObjective(self):
        ss = ''
        l = self.annotation.split()
        while len(l) > 0:
            w = l.pop(0)
            if w == "OBJECTIVE":
                ss += "   OBJECTIVE " + self.name + " " + l.pop(0) + '\n'
            if w == "ACTIVITY":
                levels = l.pop(0)
                ss += "      ACTIVITY " + levels + '\n'
                for i in range(0, int(levels)):
                    ss += "         "  + l.pop(0) + " " + l.pop(0) + '\n'
                ss += "      END" + '\n'
                ss += "   END" + '\n'
        return ss


    def printMetabolite(self):
        ss = ''
        l = self.annotation.split()
        for i in l:
            w = l.pop(0)
            if w == "METABOLITE":
                ss += "   METABOLITE " + self.name + " " + l.pop(0) + '\n'
            if w == "ACTIVITY":
                levels = l.pop(0)
                ss += "      ACTIVITY " + levels + '\n'
                for i in range(0, int(levels)):
                    ss += "         " + l.pop(0) + " " + l.pop(0) + '\n'
                ss += "      END" + '\n'
                ss += "   END" + '\n'
        return ss

class Transition:
    id = ""
    name = ""
    annotation = ""
    rate = 1.0
    type = 0
    sync = ""
    delay = 0.0

    def setType(self):
        self.type = 0
        if self.annotation.find("MODEL") >= 0:
            self.type = 1
        if self.annotation.find("FAST") >= 0:
            self.sync = "FAST"
        else:
            self.sync = "SLOW"
        if self.annotation.find("INSTANT") >= 0:
            self.sync = "INSTANT"
        if self.annotation.find("RESET") >= 0:
            self.sync = "RESET"
        if self.annotation.find("FLUX") >= 0:
            self.type = 2
            self.sync = "FLUX"

    def printMetabolism(self):
        ss = ''
        l = self.annotation.split()
        while len(l) > 0:
            w = l.pop(0)
            if w == "MODEL":
                ss += "   MODEL " + l.pop(0) + '\n'
            if w == "EXT_TAG":
                ss += "   EXT_TAG " + l.pop(0) + '\n'
        return ss

    def printFlux(self):
        ss = ''
        l = self.annotation.split()
        while len(l) > 0:
            w = l.pop(0)
            if w == "FLUX":
                ss += "   FLUX " + self.name + " " + l.pop(0) + '\n'
        return ss

    def setDelay(self):
        if self.annotation.find("DELAY") >= 0:
            l = self.annotation.split()
            while len(l) > 0:
                w = l.pop(0)
                if w == "DELAY":
                    self.delay = float(l.pop(0))
        else:
            self.delay = 0.0

    def setRate(self):
        if self.annotation.find("RATE") >= 0:
            l = self.annotation.split()
            for i in l:
                w = l.pop(0)
                if w == "RATE":
                    self.rate = float(l.pop(0))
        else:
            self.rate = 1.0


class Edge:
    id = ""
    source = 0
    target = 0
    type = ""
    annotation = ""
    multiplicity = 1

places = {}
transitions = {}
edges = {}

def handlePlaces(placeClass, useid):
    placeList = placeClass.getElementsByTagName("node")
    pcount = 0
    for i in placeList:
        pl = Place()
        pl.id = i.getAttribute("id")
        atts = i.getElementsByTagName("attribute")
        for j in atts:
            if j.getAttribute("name") == "Name":
                if useid == 1:
                    pl.name = "s" + pl.id
                else:
                    pl.name = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
                    if len(pl.name) == 0:
                        pl.name = "s" + pl.id
            if j.getAttribute("name") == "Marking":
                pl.state = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
            if j.getAttribute("name") == "Comment":
                pl.annotation = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
        places[pl.id] = pl
        pcount = pcount + 1

def handleTransitions(transitionClass, useid):
    trList = transitionClass.getElementsByTagName("node")
    tcount = 0
    for i in trList:
        tr = Transition()
        tr.id = i.getAttribute("id")
        atts = i.getElementsByTagName("attribute")
        for j in atts:
            if j.getAttribute("name") == "Name":
                if useid == 1:
                    tr.name = "tr" + tr.id
                else:
                    tr.name = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
                    if len(tr.name) == 0:
                        tr.name = "tr" + tr.id
            if j.getAttribute("name") == "Comment":
                tr.annotation = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
        transitions[tr.id] = tr
        tcount = tcount + 1

def handleEdges(edgeClass, type):
    edList = edgeClass.getElementsByTagName("edge")
    for i in edList:
        ed = Edge()
        ed.id = i.getAttribute("id")
        ed.source = i.getAttribute("source")
        ed.target = i.getAttribute("target")
        atts = i.getElementsByTagName("attribute")
        for j in atts:
            if j.getAttribute("name") == "Comment":
                ed.annotation = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
            if j.getAttribute("name") == "Multiplicity":
                ed.multiplicity = j.getElementsByTagName("cdata")[0].getAttribute("cdata")
        ed.type = type
        edges[ed.id] = ed

def parse(fname, useid):
    f = open(fname)
    content = f.read()
    f.close()

#   content = re.sub('<!\[CDATA.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\n.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\n.*\n.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\n.*\n.*\n.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\]\]>','',content)
#   content = re.sub('<!\[CDATA.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\]\]>','',content)

#   content = re.sub('<attribute name=\"Comment\".*?</attribute>','',content,flags=re.DOTALL)

    id = 0
    while id > -1:
        id = content.find("<attribute name=\"Comment\"", id + 1)
        id1 = content.find("</attribute>", id)
        id3 = content.find("<![", id, id1)
        id4 = content.find("]>", id3, id1)
        if content[id3:id4].find("&") > -1:
            content = content[0:id3] + content[id3:id4].replace("&", "and") + content[id4:]
        if content[id3:id4].find(">") > -1:
            content = content[0:id3 + 1] + content[id3 + 1:id4].replace(">", "") + content[id4:]
        if content[id3:id4].find("<") > -1:
            content = content[0:id3 + 1] + content[id3 + 1:id4].replace("<", "") + content[id4:]

    content = content.replace('![CDATA[', 'cdata cdata="')
    content = content.replace(']]]', ']"/')
    content = content.replace(']]', '"/')

    doc = xml.dom.minidom.parseString(content)
    nodeclasses = doc.getElementsByTagName("nodeclasses")[0].getElementsByTagName("nodeclass")
    for nc in nodeclasses:
        type = nc.getAttribute("name")
        if type == "Place":
            handlePlaces(nc, useid)
        if type == "Transition":
            handleTransitions(nc, useid)
    edgeclasses = doc.getElementsByTagName("edgeclasses")[0].getElementsByTagName("edgeclass")
    for ec in edgeclasses:
        type = ec.getAttribute("name")
        handleEdges(ec, type)

def writePnfba(max, fout):
    OUT = open(fout, 'w')
    for i in places.keys():
        places[i].setMax(max)
        places[i].setType()
    for i in transitions.keys():
        transitions[i].setType()
        transitions[i].setRate()
        transitions[i].setDelay()
    out = '\n'
    out += "SUBSTANCES" + '\n'
    for i in places.keys():
        out += "   " + places[i].name + " " + places[i].state + " " + str(places[i].max) + " " + str(places[i].type) + '\n'
    out += "END" + '\n'
    print 'SUBSTANCES' #--------------------------------------
    out += '\n'
    out += "REACTIONS" + '\n'
    for i in transitions.keys():
        tr = transitions[i]
        if tr.type == 1:
            continue
        out += "  INTERACTION" + " " + tr.name + " " + str(tr.rate) + " " + tr.sync + " " + str(tr.delay) + '\n'
        for j in edges.keys():
            if edges[j].target == tr.id:
                out += "      SUBSTRATE " + places[edges[j].source].name + '\n'
                out += places[edges[j].source].printStoichiometry(edges[j].annotation, places[edges[j].source].name, True)
                out += places[edges[j].source].printActivity(edges[j].type, edges[j].multiplicity, edges[j].annotation)
                out += "      END" + '\n'
        for j in edges.keys():
            if edges[j].source == tr.id:
                out += places[edges[j].target].printStoichiometry(edges[j].annotation, places[edges[j].target].name, False) #w#
                if not places[edges[j].target].testStoichiometry(edges[j].annotation, places[edges[j].target].name, False):
                    out += "      PRODUCT " + places[edges[j].target].name + '\n'
        for j in edges.keys():
            if edges[j].target == tr.id:
                if edges[j].type == "Edge":
                    out += "      CONSUMED " + places[edges[j].source].name + '\n'
        out += "  END" + '\n'
    out += "END" + '\n'
    print 'REACTIONS' #-------------------------------------
    out += '\n'
    out += "GSMN" + '\n'
    for i in transitions.keys():
        if transitions[i].type == 1:
            out += transitions[i].printMetabolism()
    for i in transitions.keys():
        if transitions[i].type == 2:
            out += transitions[i].printFlux()
    for i in places.keys():
        if places[i].type == 5:
            out += places[i].printObjective()
    for i in places.keys():
        if places[i].type == 2:
            out += places[i].printMetabolite()
    for i in places.keys():
        if places[i].type == 3:
            out += places[i].printEnzyme()
    for i in places.keys():
        if places[i].type == 4:
            out += places[i].printNutrient()
    out += "END" + '\n'
    print 'GSMN' #-------------------------------------
    OUT.write(out)#w#
    OUT.close()

def parseSPEPT(fin, fout, maxt, useidx=None):#w#
    print 'parseSPEPT' #t#
    useid = 0
    if(useidx == "useid"): useid = 1
    parse(fin, useid)
    writePnfba(maxt, fout)


############## MAIN #########################

#if len(sys.argv)<3 :
#  print "**************************************************************"
#  print "**       spept2qsspn version 10 05/01/2014                   **"
#  print "**************************************************************"
#  print
#  print "USAGE: spept2qsspn spept_file_name default_max_level"
#  print ""
#  print "where:"
#  print "spept_file_name - Extended PN file in Snoopy *spept format"
#  print "default_max_level - Default maximal state of a place"
#  print
#  exit(0)
#useid = 0
#if len(sys.argv)==4 :
#  if(sys.argv[3]=="useid") :
#   useid = 1
#parse(sys.argv[1],useid)
#writePnfba(sys.argv[2])

