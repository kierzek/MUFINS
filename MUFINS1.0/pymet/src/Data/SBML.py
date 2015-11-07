import Util
import Model
import xml.dom.minidom
import string
from xml.dom.minidom import getDOMImplementation#w#
import re#w#
from java import io

import Model.Element

def loadModel(model, path):
    print "starting"
    xmodel = XML_Model(path)
    model.setName(xmodel.GetName() or xmodel.GetID())#w# add GetID

    IDs = xmodel.Species.keys()
    IDs.sort()
    for ID in IDs:
        sp = xmodel.Species[ID]
        m = model[sp.GetID()] = Model.Element.Metabolite()
        m.setName(sp.GetName())
        m.setCompartment(sp.GetCompartment())
        m.externalise(sp.IsExternal())
        sptype = sp.GetSpeType()#w# v4l2
        if sptype:
            st = xmodel.SpeType[sptype]
            m.setFormula(st.GetFormula())
            m.setCharge(st.GetCharge())
            m.setComment(st.GetComment())
        else:
            m.setFormula(sp.GetFormula())
            m.setCharge(sp.GetCharge()) 
    M = IDs
    print "species done"

    IDs = xmodel.Reactions.keys()
    IDs.sort()
    for ID in IDs:
        rx = xmodel.Reactions[ID]
        ID = rx.GetID()
        rn = model[ID] = Model.Element.Reaction()
        sto = rx.GetStoich()
        rn.updateChildren(sto)
        lb, ub = rx.LowerBound(), rx.UpperBound()
        rn.setBounds(lb, ub)
        generule = rx.GetGenes()
        ec = rx.GetEC() or rx.GetProteins() or {True: '', False : '_HE_' + ID}[generule == '']
        model.editElement(ID, Util.RULE, ec)#ec -enzyme EC number
        ecs = GetECNames(ec) #w# if no ec, then no proteins for gene table
        for e in ecs: model.editElement(e, Util.RULE, generule)#w#
        model.editElement2(ID, Util.RULE, generule)#w# set reaction rule as in GENE_ASSOCIATION
        subs = rx.GetSubsystem()#w#
        name = rx.GetName()
        eq = rx.GetEquation() or rn.GetEquation() #v4l2 'Equation' in notes
        if subs: rn.setComment("#%s; %s; %s" % (subs, name, eq))#remove "SUBSYS:" and ':'
        else: rn.setComment("#%s: %s" % (name, eq))

        for k in sto:
            if not k in M:
                m = model[k] = Model.Element.Metabolite()
                m.setName(k)
                m.externalise(True)
                m.setCompartment("X")
            else:
                m = model[k]
            m.updateParents({ID : sto[k]})
    print "reactions done"

def GetECNames(namestr):#w# get list of names
        names= namestr.replace(' OR ', '\t').replace(' AND ', '\t').replace('( ', '').replace(' )', '')
        names=names.strip().split('\t')
        return names

class Element(dict):

    def __init__(self, node, parent = None):
        for key in node.attributes.keys():
            self[str(key)] = str(node.getAttribute(key))
        self.Parent = parent

    def GetID(self):
        return str(self.get('id'))

    def GetName(self):
        return str(self.get('name', '')) or self.get('Official name', '') or self.get('ABBREVIATION', '') #w# v4l2: 'Official name'

    def __str__(self):
        rv = self.GetID() + ':\n'
        for key in self.keys():
            if not key == 'id':
                rv += '\t%s:\t%s\n' % (key, self[key])
        return rv



class Compartment(Element):

    def __init__(self, node, parent = None):
        Element.__init__(self, node, parent = parent)
        self.Species = {}

    def AddSpecies(self, species):
        ID = species.GetID()
        self.Species[ID] = species



class Species(Element):

    def __init__(self, node, parent = None):
        Element.__init__(self, node, parent = parent)
        attrs = node.getElementsByTagName('p')#for FORMULA and CHARGE if there is <note>
        for attr in attrs:
            child = attr.firstChild
            data =  child.data
            key, val = data.split(':', 1)
            self[str(key.strip()).upper()] = str(val.strip())

    def GetCompartment(self):
        return self.get('compartment', '')

    def IsExternal(self):
        return self.get('boundaryCondition') == 'true'

    def GetFormula(self):#w#
        return self.get('FORMULA') or self.GetName().rsplit('_')[-1]

    def GetCharge(self):#w#
        return self.get('CHARGE') or float(self.get('charge', 0))

    def GetSpeType(self):#w#
        return self.get('speciesType', '')

    def GetComment(self):#w# v4l2
        tt = ''
        if self.get('OFFICIAL NAME', ''): tt += 'OFFICIAL_NAME='+ self.get('OFFICIAL NAME')
        if self.get('CAS NUMBER', ''): tt += '; CAS_NUMBER='+ self.get('CAS NUMBER')
        if self.get('BIOCYC ID', ''): tt += '; BIOCYC_ID='+ self.get('BIOCYC ID')
        if self.get('KEGG ID', ''): tt += '; KEGG_ID='+ self.get('KEGG ID')
        if tt: tt = '#' + tt
        return tt


class Reaction(Element):

    def __init__(self, node, parent = None):
        Element.__init__(self, node, parent = parent)
        assocs = node.getElementsByTagName('p') or node.getElementsByTagName('html:p')#'html:p' or 'p' for association
        for assoc in assocs:
            child = assoc.firstChild
            data =  child.data
            key, val = data.split(':',1)
            self[str(key.strip()).upper()] = str(val.strip())
        self.Stoich = self.ParseStoich(node)
        self.ParseBounds(node)
        self.ParseModifiers(node)#w# get gene rule for iHuman1512 model


    def ParseStoich(self, node):
        rv = self.ParseSide(node, 'listOfReactants', -1)
        rs = self.ParseSide(node, 'listOfProducts', 1)
        if len(rv) == 0 and len(rs) == 1:
            ID, coef = rs.items()[0]
            rv["x_" + ID] = -coef
        elif len(rs) == 0 and len(rv) == 1:
            ID, coef = rv.items()[0]
            rs["x_" + ID] = -coef
        for ID in rs:
            if not ID in rv:
                rv[ID] = 0
            rv[ID] += rs[ID]
        return rv


    def ParseSide(self, node, tag, sign):
        rv = {}
        nodelist = node.getElementsByTagName(tag)
        if len(nodelist) > 0:
            nodelist = nodelist[0]
            nodes = nodelist.getElementsByTagName('speciesReference')
            for node in nodes:
                ID = str(node.getAttribute('species'))
                coefstr = str(node.getAttribute('stoichiometry'))#w# add str
                if not coefstr: coefstr=1 #w#
                rv[ID] = sign * float(coefstr)
        return rv


    def ParseBounds(self, node):
        kinlaws = node.getElementsByTagName('kineticLaw')
        if len(kinlaws) > 0:
            kinlaw = kinlaws[0]
            paramlist = kinlaw.getElementsByTagName('listOfParameters')[0]
            params = paramlist.getElementsByTagName('parameter')
            for param in params:
               pid = str(param.getAttribute('id'))
               pname = str(param.getAttribute('name')) #for case: there is key 'name'
               value = str(param.getAttribute('value')).upper() #in case: -inf, inf
               if pname in ('LOWER_BOUND', 'UPPER_BOUND', 'OBJECTIVE_COEFFICIENT'):
                    self[pname] = value
               else: self[pid] = value

    def ParseModifiers(self, node):#w# get modifiers which act as a catalyst or inhibitor of a reaction
        gid=[]
        modifiers = node.getElementsByTagName('listOfModifiers')
        if len(modifiers) > 0:
            modifiers = modifiers[0]
            genes = modifiers.getElementsByTagName('modifierSpeciesReference')
            for gene in genes:
               gid+= [str(gene.getAttribute('species'))]
        self['MODIFIER']=' OR '.join(gid)

    def IsIrrev(self):
        return self.get('reversible') == 'false'

    def GetEC(self):#w# EC Number
        tt = self.get('EC NUMBER', '') or self.get('PROTEIN_CLASS', '') or self.get('PROTEIN', '') or self.get('MODIFIER', '')
        return tt.replace(' or ', ' OR ').replace(' and ', ' AND ')

    def GetSubsystem(self):#w#
        return self.get('SUBSYSTEM', '')

    def GetGenes(self):
        tt = self.get('GENE_ASSOCIATION', '') or self.get('GENE ASSOCIATION', '')
        tt = tt.replace(' or ', ' OR ').replace(' and ', ' AND ')
        tt = re.sub('\(\s*','( ', tt)#w#
        return re.sub('\s*\)',' )', tt)#w#

    def GetProteins(self):
        return self.get('PROTEIN_ASSOCIATION', '').replace(' or ', ' OR ').replace(' and ', ' AND ')

    def GetStoich(self):
        return self.Stoich.copy()

    def GetEquation(self):#w# v4l2: 'Equation'
        return self.get('EQUATION', '')

    def __str__(self):
        return Element.__str__(self)[:-1] + '\n\t' + str(self.GetStoich())

    def Equation(self):
        sto = self.GetStoich()
        subs = filter(lambda m: sto[m] <= 0, sto)
        prods = filter(lambda m: sto[m] >= 0, sto)
        return self._join(subs, sto) + ' = ' + self._join(prods, sto)

    def _join(self, mets, sto):
        tojoin = [str(float(abs(sto[met]))) + ' ' + self.Parent.GetSpecies(met).Translate() for met in mets]
        return string.join(tojoin, ' + ')

    def LowerBound(self):
        if self.get('LOWER_BOUND')=='-INF': #w#
            return -float(Util.FLUXBOUND)
        else: return float(self.get('LOWER_BOUND', -Util.FLUXBOUND))

    def UpperBound(self):
        if self.get('UPPER_BOUND')=='INF': #w#
            return float(Util.FLUXBOUND)
        return float(self.get('UPPER_BOUND', Util.FLUXBOUND))

    def ObjCoef(self):
        return float(self['OBJECTIVE_COEFFICIENT'])


class XML_Model(Element):

    def __init__(self, ifile):
        self.Doc = xml.dom.minidom.parse(ifile)
        modelnode = self.Doc.getElementsByTagName('model')[0]
        Element.__init__(self, modelnode)
        self.Compartments = self.ParseElementsByTagName('compartment', Compartment)
        self.SpeType = self.ParseElementsByTagName('speciesType', Species)#w# v4l2
        self.Species = self.ParseElementsByTagName('species', Species)
        self.Reactions = self.ParseElementsByTagName('reaction', Reaction)

    def ParseElementsByTagName(self, tag, klass):
        rv = {}
        nodes = self.Doc.getElementsByTagName(tag)
        for node in nodes:
            try:
                el = klass(node, parent = self)
                rv[el.GetID()] = el
            except:
                print node.getAttribute('id')
        print tag, "parsed"
        return rv

    def GetSpecies(self, ID):
        return self.Species[ID]

    def GetReaction(self, ID):
        return self.Reaction[ID]

    def GetCompartment(self, ID):
        return self.Compartments[ID]


#w#------------------------- write SBML -----------------------------------------
class exportSBML:
    def __init__(self, Xtag, metabolicModel, path):
        if Xtag!='': self.xtag= Xtag
        else: self.xtag= '_b'
        self.write(metabolicModel, path)

    def SIdConvert(self, name):
        name=name.strip()
        if self.xtag!='_b' and name.endswith(self.xtag):
            name=name.rstrip(self.xtag)+'_b'
        if name[0].isdigit(): name='_'+name
        if ',' in name: name=name.replace(',','_COMMA_')
        if '(' in name: name=name.replace('(','_LPAREN_')
        if ')' in name: name=name.replace(')','_RPAREN_')
        if '[' in name: name=name.replace('[','_LSQBKT_')
        if ']' in name: name=name.replace(']','_RSQBKT_')
        if '?' in name: name=name.replace('?','_63_')
        if ':' in name: name=name.replace(':','_58_')
        if '-' in name: name=name.replace('-','_DASH_')
        if '*' in name: name=name.replace('*','_42_')
        if '.' in name: name=name.replace('.','_PERIOD_')
        if '/' in name: name=name.replace('/', '_FSLASH_')
        if '\\' in name: name=name.replace('\\', '_BSLASH_')
        if '\'' in name: name=name.replace('\'','_APOS_')
        return name

    def makeEasyTag(self, dom, tagname, value, type='text'):
        tag = dom.createElement(tagname)
        if value.find(']]>') > -1:
            type = 'text'
        if type == 'text':
            value = value.replace('&', '&amp;')
            value = value.replace('<', '&lt;')
            text = dom.createTextNode(value)
        elif type == 'cdata':
            text = dom.createCDATASection(value)
        tag.appendChild(text)
        return tag

    def setAttrList(self, dom, attlist, nodelist, nodename):
        for attr in attlist:
            node=dom.createElement(nodename)
            for att in attr: node.setAttribute(att[0], att[1])
            nodelist.appendChild(node)

    def writeSpeciesReference(self, dom, listEqu, stoEqu):
        for sto in stoEqu:
            speciesReference = dom.createElement('speciesReference')
            sto=sto.split()
            if len(sto)==1:
                speciesReference.setAttribute('species', self.SIdConvert(sto[0]))#character to ASCII Dec number
                speciesReference.setAttribute('stoichiometry', '1.0')
            elif len(sto)==2:
                speciesReference.setAttribute('species', self.SIdConvert(sto[1]))
                speciesReference.setAttribute('stoichiometry', sto[0])
            listEqu.appendChild(speciesReference)

    def writeEquation(self, dom, reaction, equation):
        reactants = dom.createElement('listOfReactants')
        products = dom.createElement('listOfProducts')
        equ=equation.split(' = ')
        stoLeft=equ[0].split(' + ')
        stoRight=equ[1].split(' + ')
        self.writeSpeciesReference(dom, reactants, stoLeft)
        self.writeSpeciesReference(dom, products, stoRight)
        reaction.appendChild(reactants)
        reaction.appendChild(products)


    def write(self, model, path):
        sep = io.File.separator
        fname=str(path).rsplit(sep,1)[1].rstrip('.xml')
        dom = getDOMImplementation().createDocument(None,'sbml', None)
        root = dom.documentElement

        root.setAttribute('xmlns', 'http://www.sbml.org/sbml/level2')
        root.setAttribute('level', '2')
        root.setAttribute('version', '1')
        root.setAttribute('xmlns:html', 'http://www.w3.org/1999/xhtml')

        mod = dom.createElement('model')
        mod.setAttribute('id', fname)
        mod.setAttribute('name', fname)
        root.appendChild(mod)

        listUnit = dom.createElement('listOfUnits')
        unitatt=[[('kind','mole'), ('scale','-3')],
                 [('kind','gram'), ('exponent','-1')],
                 [('kind','second'), ('multiplier','.00027777'), ('exponent','-1')]
        ]
        self.setAttrList(dom, unitatt, listUnit, 'unit')
        listUnitDef=dom.createElement('listOfUnitDefinitions')
        UnitDef=dom.createElement('unitDefinition')
        UnitDef.setAttribute('id', 'mmol_per_gDW_per_hr')
        UnitDef.appendChild(listUnit)
        listUnitDef.appendChild(UnitDef)
        mod.appendChild(listUnitDef)

        mets=model.Lists[Model.Element.Metabolite]#------metabolites--------------------
        listSpecies = dom.createElement('listOfSpecies')
        for met in mets:
            species=dom.createElement('species')
            species.setAttribute('id', self.SIdConvert(met))
            species.setAttribute('compartment', '')
            listSpecies.appendChild(species)
        mod.appendChild(listSpecies)

        listReacs = dom.createElement('listOfReactions')#-------Reactions-----------------
        reacs=model.Lists[Model.Element.Reaction]
        for reac in reacs:
            SIdreac=self.SIdConvert(reac)
            Ratt=model[reac]
            reaction=dom.createElement('reaction')
            reaction.setAttribute('id', SIdreac)
            reaction.setAttribute('name', SIdreac)
            reaction.setAttribute('reversible', str((Ratt['LB']<0 and Ratt['UB']>0)).lower())

            notes=dom.createElement('notes')
            htmlp = self.makeEasyTag(dom, 'html:p', 'GENE_ASSOCIATION: '+str(Ratt['Rule']))
            notes.appendChild(htmlp)
    #        htmlp = self.makeEasyTag(dom, 'html:p', 'PROTEIN_ASSOCIATION: ')
    #        notes.appendChild(htmlp)
    #        htmlp = self.makeEasyTag(dom, 'html:p', 'SUBSYSTEM: ')
    #        notes.appendChild(htmlp)
            reaction.appendChild(notes)

            self.writeEquation(dom, reaction, str(Ratt['Equation']))

            kinetic=dom.createElement('kineticLaw')
            math=dom.createElement('math')
            math.setAttribute('xmlns', 'http://www.w3.org/1998/Math/MathML')
            math.appendChild(self.makeEasyTag(dom, 'ci', ' FLUX_VALUE '))
            parameters=dom.createElement('listOfParameters')
            if ('biomass' in reac) or ('BIOMASS' in reac): objcoef='1'
            else: objcoef='0'
            attlist=[[('id','LOWER_BOUND'), ('value',str(Ratt['LB'])), ('units','mmol_per_gDW_per_hr')],
                     [('id','UPPER_BOUND'), ('value',str(Ratt['UB'])), ('units','mmol_per_gDW_per_hr')],
                     [('id','OBJECTIVE_COEFFICIENT'), ('value',objcoef)],
                     [('id','FLUX_VALUE'), ('value','0'), ('units','mmol_per_gDW_per_hr')]
            ]
            self.setAttrList(dom, attlist, parameters, 'parameter')
            kinetic.appendChild(math)
            kinetic.appendChild(parameters)
            reaction.appendChild(kinetic)
            listReacs.appendChild(reaction)
        mod.appendChild(listReacs)
    #    print sbml.toprettyxml()#t#
    #    print root.toprettyxml()#t#

        f = open(path, 'w')
        dom.writexml(f, addindent = "\t", newl="\n", encoding='UTF-8')
        f.close()