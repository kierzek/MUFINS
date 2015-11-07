# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="John"
__date__ ="$19-Feb-2014 12:09:59$"

import Util
import re

class Element(dict):
    def __init__(self):
        pass


class Activity(Element): #Activity for substrates and objective node of metabolites
    def __init__(self):
        Element.__init__(self)
        self.t = ''
        self.a = ''

class ActivityEnz(Element): #Activity for constraint node of enzymes
    def __init__(self):
        Element.__init__(self)
        self.t = ''
        self.lb = ''
        self.ub = ''

class RxnList(Element):
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.wei = ''

class Node(Element):
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.ini = ''
        self.max = ''
        self.type = ''
        self.moni =0

class Interaction(Element): #ID : Substrate
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.rate = ''
        self.type = ''
        self.delay = ''
        self.subs = []

class Substrate(Element):
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.prod=0 #producte
        self.cons=0 #consumed
        self.sto='' #for RESET INTERACTION
        self.actr=-1 #activity table row number, -1: no activity table
        self.act=[] #activity table

class Metabolite(Element):
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.obj = '' #objevtive of QSSF
        self.act=[] #lsit of ActivityObj

class Enzyme(Element):
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.rxn = '' #flux reaction of QSSF, or lsit of rxns
        self.act=[] #lsit of ActivityCon
        self.list=[] #list of RxnList

class GSMN(Element):
    def __init__(self):
        Element.__init__(self)
        self.ID = ''
        self.extag = ''
        self.flux=[] #FLUX reaction ID map

#-------------------------------Model-------------------------------------------------------------------
class Model():
    def __init__(self, path=None):
        self.name=''
        self.Node={} #node map
        self.Int={} #Interact map
        self.Met={} #Metabolite map
        self.Enz={} #Enzyme map
        self.nodes=[] #list of node ID
        self.interactions=[] #list of interaction ID
        self.metabolites=[] #list of metabolite ID
        self.enzymes=[] #list of enzyme ID
        self.gsmn=GSMN()
#        self.para={}
        if path: self.load(path)

    def load(self, path):
        f = open(path,'r')
        line=f.readline()
        while line:
            if re.match('SUBSTANCES', line): #-----------------------------------------
                line=f.readline()
                while not re.match('END', line): 
                    self.readNode(line.strip())
                    line=f.readline()
            elif re.match('REACTIONS', line): #----------------------------------------
                line=f.readline()
                inttmp = []
                while not re.match('END', line):
                    if re.match('INTERACTION', line.strip()):
                        intact=Interaction()
                        self.readInteract(intact, line.strip(), inttmp)
                        line=f.readline()
                        while not re.match('  END', line): #INTERACTION END
                            if re.match('SUBSTRATE', line.strip()):
                                sub=Substrate()
                                self.readSubstrate(sub, intact, line.strip())
                                line=f.readline()
                                if re.match('ACTIVITY', line.strip()):
                                    sub.actr = int(line.strip().split()[1])
                                    line=f.readline()
                                    while not re.match('END', line.strip()):
                                        self.readActivity(sub, line.strip())
                                        line=f.readline()
                            elif re.match('PRODUCT_STOICHIOMETRY', line.strip()): #for RESET INTERACTION
                                self.readProductSto(intact, line.strip())
                            elif re.match('PRODUCT', line.strip()): 
                                self.readProduct(intact, line.strip())
                            elif re.match('CONSUMED', line.strip()): 
                                self.readConsumed(intact, line.strip())
                            line=f.readline()
                    line=f.readline()
            elif re.match('GSMN', line): #---------------------------------------------
                line=f.readline()
                while not re.match('END', line):
                    if re.match('MODEL', line.strip()):
                        self.gsmn.ID=line.strip().split()[1]
                    elif re.match('EXT_TAG', line.strip()):
                        self.gsmn.extag=line.strip().split()[1]
                    elif re.match('FLUX', line.strip()):
                        tt=line.strip().split()
                        self.gsmn.flux+=[tt[1:]]
                    elif re.match('METABOLITE', line.strip()):
                        met=Metabolite()
                        self.readMetabolite(met, line.strip())
                        line=f.readline()
                        if re.match('ACTIVITY', line.strip()):
                            line=f.readline()
                            while not re.match('END', line.strip()):
                                self.readActivity(met, line.strip())
                                line=f.readline()
                    elif re.match('ENZYME', line.strip()):
                        enz=Enzyme()
                        self.readEnzyme(enz, line.strip())
                        line=f.readline()
                        if re.match('ACTIVITY', line.strip()):
                            line=f.readline()
                            while not re.match('END', line.strip()):
                                self.readActivity(enz, line.strip())
                                line=f.readline()
                        line=f.readline()
                        if re.match('LIST', line.strip()):
                            line=f.readline()
                            while not re.match('END', line.strip()):
                                self.readRxnList(enz, line.strip())
                                line=f.readline()
                    line=f.readline()
            line=f.readline()
        f.close()
                                            
    def readNode(self, line):
        tt=line.split()
        node=Node()
        node.ID=tt[0]
        node.ini=tt[1]
        node.max=tt[2]
        node.type=tt[3]
        self.Node[tt[0]]=node
        self.nodes.append(tt[0])

    def readInteract(self, intact, line, inttmp):
        tt=line.split()
        ID = tt[1].strip()
        inttmp.append(ID)
        c = inttmp.count(ID)
        if c>1: ID = ID + '__' + str(c)
        intact.ID=ID
        intact.rate=tt[2]
        intact.type=tt[3]
        intact.delay=tt[4]
        self.Int[ID]=intact
        self.interactions.append(ID)  

    def readSubstrate(self, sub, intact, line):
        tt=line.split()
        sub.ID=tt[1]
        intact[tt[1]]=sub
        intact.subs.append(tt[1])

    def readProduct(self, intact, line):
        subs=line.split()[1:]
        for ID in subs:
            if ID in intact: intact[ID].prod=1
            else:
                subx=Substrate()
                subx.ID=ID
                subx.prod=1
                subx.cons=0
                intact.subs.append(ID)
                intact[ID]=subx

    def readConsumed(self, intact, line):
        subs=line.split()[1:]
#        for ID in subs: intact[ID].cons=1
        for ID in subs:
            if ID in intact: intact[ID].cons=1
            else:
                subx=Substrate()
                subx.ID=ID
                subx.prod=0
                subx.cons=1
                intact[ID]=subx

    def readProductSto(self, intact, line):
        tt=line.split()
        ID=tt[1]
        sto=tt[2]
        if ID in intact:
            intact[ID].prod=1
            intact[ID].sto=sto
        else:
            subx=Substrate()
            subx.ID=ID
            subx.prod=1
            subx.cons=0
            subx.sto=sto
            intact.subs.append(ID)
            intact[ID]=subx

    def readActivity(self, sub, line):
        tt=line.split()
        act=Activity()
        act.t=tt[0]
        if len(tt)==2: act.a=tt[1]
        elif len(tt)==3:
            act.lb=tt[1]
            act.ub=tt[2]
        sub.act.append(act)

    def readMetabolite(self, met, line):
        tt=line.split()
        met.ID=tt[1]
        met.obj=tt[2]
        self.Met[tt[1]]=met
        self.metabolites.append(tt[1])

    def readEnzyme(self, enz, line):
        tt=line.split()
        enz.ID=tt[1]
        enz.rxn=tt[2]
        self.Enz[tt[1]]=enz
        self.enzymes.append(tt[1])

    def readRxnList(self, enz, line):
        tt=line.split()
        rxns=RxnList()
        rxns.ID=tt[0]
        rxns.wei=tt[1]
        enz.list.append(rxns)