import Model.Element
__author__="Albert Gevorgyan"
__date__ ="$23-Sep-2010 15:06:52$"

import copy

import Model
import Util

TX_TAG = "_tx"

def mergedModel(tags2models, exchange, name):
    rv = Model.Model(name)
    for tag, model in tags2models.items():
        tag = '_' + tag
        addModel(rv, tag, model)
        buildExchange(rv, tag, model, exchange)
    return rv

def addModel(rv, tag, model):
    nlevels = len(model.ClassLevels)
    for nlevel in range(nlevels):
        levellist = model.getLevelList(nlevel)
        for ID in levellist:
            if not ID + tag in rv:
                el = copy.deepcopy(model[ID])
                if nlevel > 0:
                    el[el.ParentField].tagKeys(tag)
                if nlevel < nlevels - 1:
                    el[el.ChildField].clear()
                rv[ID + tag] = el

def buildExchange(rv, tag, model, exchange):
    for x in exchange:
        if x in model:
            eq = Util.Stoich.Equation()
            eq[x + tag] = -1
            eq[x] = 1
            rv[x + tag + TX_TAG] = Model.Element.Reaction(eq = eq, lb = -1, ub = 1)
            rv[x].externalise(False)
            rv[x + tag].externalise(False)
