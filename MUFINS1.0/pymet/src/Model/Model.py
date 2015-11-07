__author__="Albert Gevorgyan"
__date__ ="$25-Feb-2010 14:47:33$"

import Util
import Element


DEFAULT_LEVELS = [
        Element.Gene,
        Element.Enzyme,
        Element.Reaction,
        Element.Metabolite
    ]


class Model(dict):

    def __init__(self, name = '', classlevels = DEFAULT_LEVELS, output = None):
        self.setName(name)
        self.initLevels(classlevels)
        self.setOutput(output)

    def initLevels(self, classlevels):
        self.ClassLevels = classlevels
        self.Lists = {}
        nlevels = len(self.ClassLevels)
        for nlevel in range(nlevels):
            level = self.ClassLevels[nlevel]
            self.Lists[level] = []
#            if (nlevel < nlevels - 1):
#                childlevel = self.ClassLevels[nlevel + 1]
#                parentfield = childlevel.ParentField
#                if childlevel.fields[parentfield] == Element.Rule:
#                    level.UneditableFields += [level.ChildField]


    def setOutput(self, output):
        self.output = output

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getLevelNumber(self, theclass):
        if not theclass in self.ClassLevels:
            if type(theclass) in Util.StringTypes:
                theclass = self[theclass]
            theclass = theclass.__class__
        return self.ClassLevels.index(theclass)

    def getLevelClass(self, nlevel):
        return self.ClassLevels[nlevel]

    def getLevelList(self, level):
        if type(level) == Util.IntType:
            level = self.ClassLevels[level]
        return self.Lists[level]#w# return elements for this level?

    def getElementValue(self, ID, field):
        return self[ID][field]#w# double dict, reaction/gene ID->field

    def __setitem__(self, ID, element):
        if not self.checkID(ID):
            dict.__setitem__(self, ID, element)
            element.setID(ID)
            nlevel = self.getLevelNumber(element.__class__)
            self.getLevelList(nlevel).append(element.getID())
            if nlevel > 0:
                self.linkToParents(ID, nlevel)
            if nlevel < len(self.ClassLevels) - 1:
                self.linkToChildren(ID, nlevel)

    def releaseParents(self, ID):
        parents = self[ID].getParents()
        for parentID in parents:
            self[parentID].deleteChild(ID)
            
    def releaseChildren(self, ID):
        children = self[ID].getChildren()
        for childID in children:
            self[childID].deleteParent(ID)

    def linkToParents(self, ID, nlevel):
        parents = self[ID].getParents()
        for parentID in parents:
            parent = self.setdefault(parentID, self.ClassLevels[nlevel - 1]())
            parent.updateChildren({ID : parents[parentID]})

    def linkToChildren(self, ID, nlevel):
        children = self[ID].getChildren()
        for childID in children:
            child = self.setdefault(childID, self.ClassLevels[nlevel + 1]())
            child.updateParents({ID : children[childID]})

    def editElement(self, ID, field, value, mt=None): #w# mt:model type, if mt='sfba', then rxn parent -> gene
        try:
            element = self[ID]
            if field == Util.IDKEY and value != element.getID():
                self.renameElement(ID, value)
            elif field not in element.UneditableFields:
                if field == element.ParentField:
                    self.releaseParents(ID)
                if field == element.ChildField:
                    self.releaseChildren(ID)
                element.readValue(field, value)
                nlevel = self.getLevelNumber(element.__class__)
                if field=='Rule' and mt=='sfba': nlevel-=1 #w# rxn parent -> gene
                if field == element.ParentField:
                    self.linkToParents(ID, nlevel) #w#rxn->enzyme->gene
                if field == element.ChildField:
                    self.linkToChildren(ID, nlevel)
        except Exception, e:
            self.message(e)

    def editElement2(self, ID, field, value):#w# ignore the parent and children
        try:
            element = self[ID]
            if field == Util.IDKEY and value != element.getID():
                self.renameElement(ID, value)
            elif field not in element.UneditableFields:
#                if field == element.ParentField:
#                    print 'element.ParentField=',element.ParentField#t#
#                    self.releaseParents(ID)
#                if field == element.ChildField:
#                    self.releaseChildren(ID)
                element.readValue(field, value)
#                nlevel = self.getLevelNumber(element.__class__)
#                if field == element.ParentField:
#                    self.linkToParents(ID, nlevel)
#                if field == element.ChildField:
#                    self.linkToChildren(ID, nlevel)
        except Exception, e:
            self.message(e)

    def checkID(self, ID):
        rv = False
        if self.has_key(ID):
            self.message('Existing name %s' % ID)
            rv = True
        rv = rv or len(ID) == 0 or ID.isspace()
        return rv

    def renameElement(self, oldID, newID):
        try:
            if not self.checkID(newID):
                element = self[oldID]
                element.readValue(Util.IDKEY, newID)
                theclass = element.__class__
                level = self.getLevelList(theclass)
                ind = level.index(oldID)
                self.delete([oldID], theclass)
                self[newID] = element
                level[:] = level[:ind] + [newID] + level[ind: -1]
        except Exception, e:
            self.message(e)


                    

    def setdefault(self, ID, element):
        if not ID in self:
            self[ID] = element
        return self[ID]

    def delete(self, IDs, theclass):
        checkchildren = theclass.ChildField in theclass.UneditableFields
        level = self.getLevelList(theclass)
        nlevel = self.getLevelNumber(theclass)
        remained = []
        for ID in IDs:
            if checkchildren and len(self[ID].getChildren()) > 0:
                remained.append(ID)
            else:
                level.remove(ID)
                if nlevel > 0:
                    self.releaseParents(ID)
                if nlevel < len(self.ClassLevels) - 1:
                    self.releaseChildren(ID)
                dict.__delitem__(self, ID)
        if len(remained) > 0:
            lst = ',\n'.join(remained)
            self.message("%s\ncannot be deleted:\nhave children involved in rules!" % lst)
                

    def insert(self, IDs, theclass, start):
        for ID in IDs[:]:
            if not self.checkID(ID):
                self[ID] = theclass()
            else:
                IDs.remove(ID)
        level = self.getLevelList(theclass)
        level[:] = level[:start] + IDs + level[start : -len(IDs)]


    def __str__(self):
        rv = ''
        for level in self.ClassLevels:
            rv += level.__name__ + 's:\n'
            levellist = self.Lists[level]
            for ID in levellist:
                rv += str(self[ID]) + '\n'
            rv += '\n'
        return rv

    def getChildren(self, IDs):
        rv = set()
        for ID in IDs:
            children = self[ID].getChildren()
            rv.update(children)
        return list(rv)

    def getParents(self, IDs):
        rv = set()
        for ID in IDs:
            parents = self[ID].getParents()
            rv.update(parents)
        return list(rv)
#
#
#    def Include(self, elements, check = True):
#        for element in elements:
#            ID = element.getID()
#            if not ID in self:
#                self[ID] = element
#            elif check:
#                self.message('Existing name %s' % ID)





    def message(self, msg = ''):
        if self.output:
            self.output.write(str(msg) + '\n')
            self.output.flush()


    def getExternals(self):
        rv = []
        for MID in self.getLevelList(-1):
            if self[MID].isExternal():
                rv.append(MID)
        return rv