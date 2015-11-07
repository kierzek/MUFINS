__author__="Albert Gevorgyan"
__date__ ="$25-Feb-2010 14:47:33$"

import Util.Stoich as Stoich
Equation = Stoich.Equation
Stoichiometry = Stoich.Stoichiometry
Rule = Stoich.Rule
StoichList = Stoich.StoichList
import Util


class Element(dict):

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.CHILDREN, StoichList),
        (Util.PARENTS, StoichList),
        (Util.COMMENTt, Util.CommentType),
    ])

    ChildField = Util.CHILDREN
    ParentField = Util.PARENTS
    UneditableFields = [ChildField]


    def __init__(self, ID = '', parents = {}, children = {}, **argd):
        self.initfields()
        self.updateChildren(children)
        self.updateParents(parents)
        self.update(argd)

    def initfields(self):
        for field in self.getOrder():
            theclass = self.fields[field]
            self[field] = theclass()

    def getOrder(self):
        return self.__class__.fields.getOrder()


    def __str__(self):
        rv = self.getID()
        for field in self.getOrder():
            if field in self:
                rv += '\n\t' + field + ':\t' + str(self[field])
        return rv

    def readValue(self, key, s):
        klass = self.fields[key]
        if klass == bool:
            s = {"False" : False, "True" : True}.get(s, s)
        self[key] = klass(s)

    def readValueAt(self, ind, s):
        key = self.getOrder()[ind]
        self.readValue(key, s)
        
    def setID(self, ID = ''):
        self[Util.IDKEY] = ID

    def __updateDict(self, key, d):
        if key in self:
            typ = type(d)
            if typ in Util.StringTypes:
                self[key].read(d)
            else:
                if typ == Util.ListType:
                    d = dict.fromkeys(d, 1)
                self[key].update(d)

    def updateChildren(self, children = {}):
        self.__updateDict(self.ChildField, children)

    def deleteChild(self, childID):
        del self[self.ChildField][childID]

    def updateParents(self, parents = {}):
        self.__updateDict(self.ParentField, parents)

    def deleteParent(self, parentID):
        del self[self.ParentField][parentID]

    def setComment(self, comment = ''):
        self[Util.COMMENTt] = comment


    def getID(self):
        return self[Util.IDKEY]


    def getChildren(self):
        return self.get(self.ChildField, {}).copy()

    def getParents(self):
        return self.get(self.ParentField, {}).copy()


    def getComment(self):
        return self[Util.COMMENTt]



class Gene(Element):
    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.NAME, Util.StringType),
        (Util.CHILDREN, StoichList),
        #(Util.SEQUENCE, Util.StringType),
        (Util.ANNOTATION, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])


class Species(Element):
    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.NAME, Util.StringType),
        (Util.CHILDREN, StoichList),
        (Util.RULE, Rule),
        (Util.COMPARTMENT, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])

    def getName(self):
        return self[Util.NAME]

    def setName(self, name):
        self[Util.NAME] = Util.StringType(name)

class Enzyme(Species):
    pass
    ParentField = Util.RULE



class Reaction(Element):

    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.EQUATION, Equation),
        (Util.LB, Util.PyMetFloat),
        (Util.UB, Util.PyMetFloat),
        (Util.RULE, Rule),
#        (DELTAG0MIN, Util.PyMetFloat),
#        (DELTAG0MAX, Util.PyMetFloat),
        (Util.COMMENTt, Util.CommentType),
    ])

    ChildField = Util.EQUATION
    ParentField = Util.RULE
    #UneditableFields = [Util.IDKEY]


    def __init__(self, ID = '', eq = None, lb = -Util.FLUXBOUND, ub = Util.FLUXBOUND, **argd):
        Element.__init__(self, ID, **argd)
        if eq:
            self.updateChildren(eq)
        self.setBounds(lb, ub)

    def setBounds(self, lb = -Util.FLUXBOUND, ub = Util.FLUXBOUND):
        self[Util.LB] = lb
        self[Util.UB] = ub

    def getLB(self):
        return self[Util.LB]

    def getUB(self):
        return self[Util.UB]

    def getRule(self):
        return self[Util.RULE]

    def setBounds(self, lb, ub):
        self[Util.LB] = Util.PyMetFloat(lb)
        self[Util.UB] = Util.PyMetFloat(ub)

    def GetEquation(self):
        return self[Util.EQUATION]

    def getEqSigmet(self):#w# get equation's sigmet
        return self[Util.EQUATION].sigmet


class Metabolite(Species):

    XTAG = 'X_'
    XTAGS = ['x_', 'X_', '_ext', '_xt']


    fields = Util.OrderedDict.OrderedDict([
        (Util.IDKEY, Util.StringType),
        (Util.NAME, Util.StringType),
        (Util.EXTERNAL, bool),#Util.BooleanType),
        (Util.USEDBY, Stoichiometry),
        (Util.FORMULA, Util.StringType),
        (Util.SMILES, Util.StringType),
        (Util.ISO_SMILES, Util.StringType),
        (Util.MASS, Util.PyMetFloat),
        (Util.CHARGE, Util.PyMetFloat),
        (Util.COMPARTMENT, Util.StringType),
        (Util.COMMENTt, Util.CommentType),
    ])
    UneditableFields = [Util.USEDBY]
    ParentField = Util.USEDBY

    def __init__(self, ID = '', external = False, **argd):
        Element.__init__(self, ID, **argd)
        self.externalise(external)
        

    def externalise(self, on = True):
        self[Util.EXTERNAL] = on

    def isExternal(self):
        return self[Util.EXTERNAL]

    def containsXtag(self):
        name = self.getID()
        tag = '_'
        for xtag in self.XTAGS:
            if xtag.endswith(tag) and name.startswith(xtag):
                return True
            if xtag.startswith(tag) and name.endswith(xtag):
                return True
        return False

    def setID(self, ID):
        Element.setID(self, ID)
        self.externalise(self.containsXtag())

    def setCharge(self, charge):
        self[Util.CHARGE] = Util.PyMetFloat(charge)

    def setFormula(self, formula):
        self[Util.FORMULA] = Util.StringType(formula)

    def setCompartment(self, compartment):
        self[Util.COMPARTMENT] = Util.StringType(compartment)


