__author__="Albert Gevorgyan"
__date__ ="$01-Mar-2010 12:55:25$"


import Util

PROGRAMS = {
    'Objective value': 'objvalue',
    'Objective value and status': 'objstat',
    'FBA': 'fba',
    'FVA': 'fva',
    'FSA': 'fsa',
    'Elementary modes': 'em',
    'Knock-out analysis': 'ko',
    'Essential reactions': 'ess',
    'Live reactions': 'live',
    'Plot': 'plot',
    'Plot 3D': 'plot3d',
    'Minimal substrate sets': 'mss',
    'Minimal product sets': 'mps',
    #'Bottlenecks' : 'bottlenecks',
    'Unconserved metabolites': 'uncons',
    'Orphan metabolites': 'orphans',
    'Connected components': 'cc',
    Util.GFAA: 'imat',#w#
   # 'GFVA': 'gfva',#w#
    Util.GFVA2: 'fimat',#w#
    Util.GFVA3: 'lshlomi',#w#
    Util.GIMME: 'gimme',#w#
    Util.GIMME2: 'gimmefva',#w#
    Util.GIM3E: 'gim3e',#w#
    'SGNI': 'sgni',#w#
    'WGNI': 'wgni',#w#
    'DPAplot': 'dpaplot',#w#
    'DPAsig': 'dpasig',#w#
}

SOLVERS = ['Simplex', 'MILP', 'Exact']
DEFAULT_SOLVER = 'Simplex'
MILP = 'MILP'

MIN = 'min'
MAX = 'max'

OBJTAG = Util.OBJTAG
DOLTAG = Util.DOLTAG
ARGTAG = Util.ARGTAG
COMTAG = Util.COMTAG
ENDTAG = Util.ENDTAG
EXTTAG = Util.EXTTAG
ARGDELIM = Util.ARGDELIM
TABDELIM = Util.TABDELIM

CMD = 'sfba'
INPUT = 'model.tmp'
BFILE = 'bounds.tmp'
OUTPUT = 'out.tmp'
EXPRESS = 'express.tmp'#w#
DPAm2g = 'DPAm2g.tmp'#w#
FIXRATE = 0.001
VMAX = 100


class Problem():

    def __init__(self, objdir, obj, args = [], dollar=[]):#w# dollar
        self.objdir = objdir
        self.obj = obj
        self.args = args
        self.doll=dollar#w#

    def __str__(self):
        return '\n'.join(self.toLines())

    def toLines(self, program):#w#
        rv = []
        if self.obj != None:
            if program in Util.GSA2:#w#
                rv.append('%s%s' % (OBJTAG, self.obj))
            elif program in ('SGNI','WGNI','DPAplot'):#w#
                rv.append('%s%s' % (OBJTAG, ARGDELIM.join(self.obj)))
            else:
                rv.append('%s%s: %s' % (OBJTAG, self.objdir, self.obj))
        if len(self.doll) and self.doll[0]!=u'':#w#
            rv.append('%s%s' % (DOLTAG, ARGDELIM.join(self.doll)))
        if self.args!=None and len(self.args) and self.args[0]!=u'':#w#
            rv.append('%s %s' % (ARGTAG, ARGDELIM.join(self.args)))
        rv.append(ENDTAG)
        return rv


def getCmdArray(program, solver, target=None, trim = True, withcomments = False, limit = None, write = False, thresh = False, samsize = False, sep = '/', spar=None, bfile=True):#w#
    if solver!='Gurobi':
        cmd = {
                '\\' : '%s-glpk.exe' % CMD,
    #        '\\' : '%s.exe.lnk' % CMD,#w#
                '/'  : './%s-glpk'  % CMD
        }[sep]
    else:
        cmd = {
            '\\' : '%s-grb.exe' % CMD,
    #        '\\' : '%s.exe.lnk' % CMD,#w#
            '/'  : './%s-grb'  % CMD
        }[sep]
    rv=[cmd]
    prog=PROGRAMS[program]#w#
    if prog in ('shlomi','gimme','gimmefva') and thresh: rv += ['-p', prog+','+thresh]
    elif prog=='wgni' and samsize: rv += ['-p', prog+','+samsize]
    else: rv += ['-p', prog]
    if bfile==False:#w#
        rv +=[
        '-i', INPUT,
        '-f', OUTPUT
        ]
    else:
        rv +=[
            '-i', INPUT,
            '-b', BFILE,
            '-f', OUTPUT
        ]
    if target:
        rv += ['-o', target]
    if solver:
        if solver=='Gurobi': rv += ['-s', 'grb'+','+spar]
        else: rv += ['-s', solver.lower()]
    if program in Util.GSA2:#w#
        rv += ['-j', EXPRESS]
    if program == 'DPAsig':#w#
        rv += ['-u', DPAm2g]
    if limit:
        rv += ['-l', limit]
    if trim:
        rv += ['-t']
    if withcomments:
        rv += ['-c']
    if write:
        rv += ['-w']
    return rv

def getExternalCmd(externals):
    return ['-x', ' '.join(externals) + ' ']

def getExternalTagCmd(xtag):
    return ['-X', xtag]

def writeTmpFiles(model, bfile, express, script_m2g):#w#
    writeTmp(INPUT, model)
    writeTmp(BFILE, bfile)
    writeTmp(EXPRESS, express)#w#
    writeTmp(DPAm2g, script_m2g)#w#

def writeTmp(filename, s):
    f = open(filename, 'w')
    f.write(s)
    f.close()

def readOutput(fn = OUTPUT):
    f = open(fn)
    s = f.read()
    f.close()
    return s

def writeTmpFilesQSSPN(script_model, script_control):#w#
    writeTmp(Util.MODELTMP, script_model)
    writeTmp(Util.CONTROLTMP, script_control)

def getCmdArrayQSSPN(sep = '/'):#w#
    cmd = {
        '\\' : '%s.exe' % Util.QSSPNCMD,
        '/'  : './%s'  % Util.QSSPNCMD
    }[sep]
    rv=[cmd]
    rv +=[Util.MODELTMP, Util.CONTROLTMP]
    return rv
