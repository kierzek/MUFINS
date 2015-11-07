__author__="Albert Gevorgyan"
__date__ ="$25-Feb-2010 14:42:18$"

import OrderedDict

from types import *

import Wrappers
from Wrappers import Comment as CommentType, COMMENT
from PyMetFloat import *
#from PyMetFloat import PyMetFloat, INF, NaN



#w#****************************** LABEL & SYMBO *********************************


FLUXBOUND = 100000
ERRORMSG = 'Error message'
#-----------------------------------------------

OBJTAG = '!'
DOLTAG = '$'#w#
ARGTAG = '?'
COMTAG = '#'
COMMENT = '#'
ENDTAG = ';'
EXTTAG = '\\'
ARGDELIM = ' '
TAB = TABDELIM = '\t'
ENDOFRECORD = ENDTAB= '///'

#--------------------PANEL----------------------

COMMENTS  = 'Comments:'
DIRECTION = 'Direction: '
OBJECTIVE = 'Objective: '
LIST      = 'List: '
REACTION  = 'Reactions: '
FROM      = 'From: '
TO        = 'To: '
STEP      = 'Step: '
REACTINV  = 'Reaction involved: '
SCALEFAC  = 'Scaling factor: '
VMAXLABEL = 'Vmax: '
REDUCE    = 'Reduce matrix:'
SOLVER    = 'Solver: '
NUMBER    = 'Number: '
FLUXRANGE = 'Flux range: '
EXPRESSION = 'Expression'
EXPRESSIONp = 'Expression '
EXPRESSIONC = 'Expression: '#w#
THRESHOLD = 'Threshold: '#w#
SAMSIZE = 'Sample size: '#w#

WRITE     = 'Write'
SOLVE     = 'Solve'

GFAA = 'iMAT'
GFVA2 = 'Fast iMAT'
GFVA3 = 'Linear iMAT'
GIMME = 'GIMME FBA'
GIMME2 = 'GIMME FVA'
GIM3E = 'GIM3E'
GSA = (GFAA, GFVA2, GFVA3,'SGNI','WGNI','DPAplot','DPAsig')
GSA2 = (GFAA, GFVA2, GFVA3,'DPAsig', GIMME, GIMME2, GIM3E)
GRBPROBLEM = (GFAA, GFVA2, GFVA3,'SGNI','WGNI','DPAplot')

#-------------------TABLE-----------------------

IDKEY       = 'ID'
NAME        = 'Name'
CHILDREN    = 'Children'
PARENTS     = 'Parents'
COMMENTt     = 'Comment'
RULE        = 'Rule'
EXTERNAL    = 'External'
LB          = 'LB'
UB          = 'UB'
EQUATION    = 'Equation'
USEDBY      = 'Used by'
COMPARTMENT = 'Compartment'
SEQUENCE    = 'Sequence'
ANNOTATION  = 'Predictions'
FORMULA     = 'Formula'
SMILES      = 'Canonical SMILES'
ISO_SMILES  = 'Isomeric SMILES'
MASS        = 'Molecular mass'
CHARGE      = 'Charge'
DELTAG0MIN   = 'DeltaG0 LB'
DELTAG0MAX   = 'DeltaG0 UB'
STATUS = 'Status'
NUTRIENT = 'Nutrient'
TEST = 'Test'
PRESENT = 'Present'
ABSENT = 'Absent'


OBJVALUE = 'Objective value'
CONSTRAINT = 'Constraint'
CONSTRAINTS = ['Constraint 1', 'Constraint 2']
RATE = 'Flux rate'
MIN = 'Min.'
MAX = 'Max.'

#w#-----------GSA-------------
REAC = 'Reaction'
REACD = 'Reaction(dir)'#w#
GENE = 'Gene'#w#
GENES = 'Genes'#w#
ACTIVE = 'Active'#w#
ACTIVITY = 'Activity'#w#
INACT = 'Inactive'#w#
ACTLEV = 'ACT_LEV'#w#
EXPLEV = 'EXP_LEV'#w#
STATE = 'State'#w#
POSTREG = 'Postregulation'#w#
KO = 'KO reactions'#w#
BLPDROP = 'BLP drop'#w#
BLPLEV = 'BLP Level'#w#
NUMESS = 'NUM_ESS_medium'#w#
MET = 'Metabolite'#w#
METS = 'Metabolites'
ESSGENES = 'Related essential genes'#w#
RELMETS = 'Related metabolites'#w#
OBJV = 'OBJ_Value'
OBJF = 'Objective function'

#q#-----------QSSPN-------------
PNNODE = 'PN node'
INTERACT = 'Interaction'
SUBSTRATE = 'Substrate'
RESET = 'Product Sto.'
METABOLITE = 'Metabolite'
GSMNMET = 'Name in GSMN '
ENZYME = 'Enzyme'
THRESHOLDa = 'Threshold'
TOKINI = 'Initial tokens'
TOKMAX = 'Maximal tokens'
NODETYPE ='Node type'
RATE = 'Transition rate'
TRANTYPE = 'Transition type'
DELAY = 'Transition delay'
PRODUCT = 'Product'
CONSUMED = 'Consumed'
CONSFLUX = 'Constrained Flux'
WEIGHT ='Weight'
MONITOR = 'Monitor'
CONTROLPARA = 'Control parameter'
PARAVALUE = 'Value'
TRAJSAM = 'Sample'
TIME = 'Time'
EVENT = 'Event'
FUNCTYPE = 'Function Type'
FORMULA = 'Formula'
FLUXTRAN = 'Flux Transition'
FLUXRXN = 'Flux reaction'
TRANSITION = 'Transition'

GSMNTMP = 'gsmn.tmp'
MODELTMP = 'QSSPN_model.tmp'
CONTROLTMP = 'QSSPN_control.tmp'
QSSPNCMD = 'qsspn'

QSSPN = [THRESHOLDa, TOKINI, TOKMAX, NODETYPE, RATE, TRANTYPE, DELAY, PRODUCT, CONSUMED, WEIGHT, MONITOR]

#w#-----------table column width------------------

WID1 = [EXTERNAL, CHARGE, ACTIVE, INACT, ACTLEV, EXPLEV, STATE, BLPDROP, BLPLEV]
WID2 = [NAME, PARENTS, COMPARTMENT, SEQUENCE, ANNOTATION, DELTAG0MIN, DELTAG0MAX, REAC, REACD, GENE, GENES, MET, LB, UB, SMILES, ISO_SMILES, STATUS, OBJV, ACTIVITY]
WID3 = [IDKEY, CHILDREN, RULE, USEDBY, FORMULA, MASS, KO, POSTREG, MIN, MAX, RATE, CONSTRAINT, NUMESS, RESET] + CONSTRAINTS + QSSPN
WID4 = [EQUATION, COMMENTt, OBJVALUE, EXPRESSIONp, PNNODE, INTERACT, METABOLITE, GSMNMET, ENZYME, CONSFLUX, SUBSTRATE, FUNCTYPE, TRANSITION]
WID5 = [ESSGENES, RELMETS, EXPRESSION, CONTROLPARA, PARAVALUE, FLUXTRAN, FLUXRXN]
WID6 = [OBJF, FORMULA]

tableColWidth = {}
tableColWidth.update(dict([(coln,80) for coln in WID1]))
tableColWidth.update(dict([(coln,90) for coln in WID2]))
tableColWidth.update(dict([(coln,100) for coln in WID3]))
tableColWidth.update(dict([(coln,200) for coln in WID4]))
tableColWidth.update(dict([(coln,400) for coln in WID5]))
tableColWidth.update(dict([(coln,550) for coln in WID6]))



