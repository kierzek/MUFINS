import Model
__author__="Albert Gevorgyan"
__date__ ="$02-Mar-2010 14:57:37$"

import os.path

import Model.Element
import Util

TAB = Util.TAB

def save(table, path):
    if path != None:
        s = str(table)
        f = open(path, 'w')
#        f = open(path, 'a')#w#
        f.write(s)
        f.close()

def load(model, path):
    notextended = False
    s = open(path).read().strip()
    levels = s.split(Util.ENDOFRECORD)
    if len(levels) == 1:
        lines = levels[0].split('\n')
        readModelLines(lines, model, Model.Element.Reaction, 'sfba')
        notextended = True
        model.setName(os.path.split(path)[-1])
    else:
        model.setName(levels[0].strip())
        for level in levels[1:]:
            if len(level) > 0 and not level.isspace():
                loadLevel(model, level)
    return notextended
 
def loadLevel(model, level):
    lines = level.strip().split('\n')
    name = lines[0].strip()
    name = name.split(' ')[-1]
    name = name[0].upper() + name[1:-1]
    theclass = Model.Element.__getattribute__(name)
    readModelLines(lines[1:], model, theclass)

def readModelLines(lines, model, theclass, mt=None): #w# mt:model type, if mt='sfba', then rxn parent -> gene
    i = 0
    for line in lines:
        i += 1
        if len(line) > 0 and not line.isspace() and not line.startswith(Util.COMMENT):
            try:
                readModelLine(line, model, theclass, mt)
            except:
                raise SyntaxError("Error in line %d: %s" % (i, line))


def readModelLine(line, model, theclass, mt=None): #w# mt:model type, if mt='sfba', then rxn parent -> gene
    line, comment = splitLineAndComment(line)
    if len(line) > 0 and not line.isspace():
        r = theclass()
        toks = line.split(TAB)
        ID = toks[0]#w# ID reaction
        model.setdefault(ID, r)
        fields = r.getOrder()
        for i in range(1, len(toks)):
            if len(toks[i]) > 0:
                tok = toks[i].strip('"')
                if fields[i] != Util.USEDBY:
                    model.editElement(ID, fields[i], tok, mt)
        model.editElement(ID, fields[-1], Util.COMMENT + comment)




def splitLineAndComment(line):
    comment = ''
    if Util.COMMENT in line:
        toks = line.rsplit(Util.COMMENT, 1)
        line, comment = toks[0], toks[1]
    return line, comment
    



def mysplit(line):
        rv = []
        toks = line.split(TAB)
        for tok in toks:
            if '=' in tok:#w#
                rv += [x.strip() for x in tok.split('=', 1)]
            else:
                rv.append(tok)
            if ':' in tok:
                rv += [x.strip() for x in tok.split(':', 1)]
            else:
                rv.append(tok)
        return rv


#w# load expression profile from TAB-delimitated txt file
def loadExpress(expr, path):#w#
    s = open(path).read().strip()
    lines = s.split('\n')
    expr.fields=lines[0].split(TAB)
#    print expr.fields#t/
    for line in lines[1:]:
        gg={}
        tt=line.strip().split(TAB)
        i=0
        for value in tt:
            gg[expr.fields[i]]=value
            i+=1
        expr[tt[0]]=gg

#w# load DPAplot output file
def loadDPAplot(path):#w#
    s = open(path).read().strip()
    lines_m2g=[]
    lines = s.split('\n')
    blks=s.split(Util.ENDOFRECORD)
    lines=blks[0].strip().split('\n\n')[1].strip().split('\n')
    for line in lines:
        tt=line.split(TAB)
        lines_m2g.append(tt[0]+TAB+' '.join(tt[1:]))
    return lines_m2g



#w# parse the gfaa output into two table model data of OutputTableModel_gsa
def output2table_gfaa(record):
    lines_reac=[]
    lines_gene=[]
    record = record.strip()
    results = record.split(Util.ENDOFRECORD)
    lines_reac = results[0].strip().split('\n')[4:]
    lines_gene = results[1].strip().split('\n')[2:]
    return lines_reac, lines_gene

#w# parse the gfva output into two table model data of OutputTableModel_gsa
def output2table_gfva(record, problem):
    lines_obj=[]
    lines_reac=[]
    lines = record.strip().split('\n')
    tt=lines[0].lstrip('#')
    max='Max'
    if problem==Util.GIMME2: max='Min'
    lines_obj.append(tt.replace(': ',TAB).replace(' ',TAB+max,1))
    lines_reac = lines[2:]
    return lines_obj, lines_reac


#w# parse the sgni output into two table model data of OutputTableModel_gsa
def output2table_sgni(record, media):
    lines_blp=[]
    lines_gni=[]
    record = record.strip()
    gblks=record.split('\n\n')
    for gblk in gblks:
        lines=gblk.strip().split('\n')
        gni=['' for i in range(len(media))]
        line_blp=''
        line_gni=''
        for line in lines:
            tt=line.strip().split(TAB)
            if 'Gene' in tt[0]:
                line_blp+=tt[1]+TAB
                line_gni+=tt[1]+TAB
            elif 'KO' in tt[0]:
                line_blp+=' '.join(tt[1:])+TAB
            elif 'BLP' in tt[0]:
                line_blp+=tt[1].split('=')[1].split(':')[0].strip()+TAB
                line_blp+=tt[2].split('=')[1].strip()
            elif tt[0].split(':')[0].strip() in media:
                line_blp+=TAB+tt[0].split(':')[1]
            elif 'GNI-SP' in tt[0]:
                for nut in tt[1:]: gni[media.index(nut)]='SP'
            elif 'GNI-SN' in tt[0]:
                for nut in tt[1:]: gni[media.index(nut)]='SN'  
        line_gni+=TAB.join(gni)
        lines_blp.append(line_blp)
        lines_gni.append(line_gni)
    return lines_blp, lines_gni


#w# parse the wgni output into two table model data of OutputTableModel_gsa
def output2table_wgni(record, media):
    lines_out=[]
    lines_gni=[]
    record = record.strip()
    gblks=record.split('\n\n')
    for gblk in gblks[1:]:        
        lines=gblk.strip().split('\n')
        line_out=''
        line_gni=''
        tt=lines[0].strip().split(TAB)#Gene
        line_out+=tt[1]+TAB
        line_gni+=tt[1]
        tt=lines[1].strip().split(TAB)#KO Reactions
        line_out+=' '.join(tt[1:])+TAB
        line_out+=lines[2].split('=')[1].strip()+TAB#Number of Essential medium
        for line in lines[4:]:
            tt=line.split(TAB)
            wp=tt[1].split('=')[1].strip()
            wn=tt[2].split('=')[1].strip()
            line_gni+=TAB+wp+TAB+wn
#        print line_out#t#
#        print line_gni#t#
        lines_out.append(line_out)
        lines_gni.append(line_gni)
    return lines_out, lines_gni

##w# parse the wgni output into two table model data of OutputTableModel_gsa
#def output2table_wgni(record, media):
#    lines_out=[]
#    lines_gni=[]
#    gblks=record.split('\n\n')
#    for gblk in gblks:
#        lines=gblk.strip().split('\n')
#        line_out=''
#        line_gni=''
#        for i in range(len(lines)):
#            line=lines[i]
#            tt=line.strip().split(TAB)
#            if 'Gene' in tt[0]:
#                line_out+=tt[1]+TAB
#                line_gni+=tt[1]
#            elif 'KO' in tt[0]:
#                line_out+=' '.join(tt[1:])+TAB
#            elif 'Essential' in tt[0]:
#                line_out+=tt[0].split('=')[1].strip()+TAB
#            elif 'Nutrients' in tt[0]:
#                ++i
#                while i<len(lines):
#                    tt=lines[i].split(TAB)
#                    wp=tt[0].split('=')[1].strip()
#                    wn=tt[1].split('=')[1].strip()
#                    line_gni+=TAB+wp+TAB+wn
#                    ++i
#        lines_out.append(line_out)
#        lines_gni.append(line_gni)
#    return lines_out, lines_gni


#w# parse the dpaplot output into 4 table model data of OutputTableModel_gsa
def output2table_dpaplot(record):
    lines_m2g=[]
    lines_g2m=[]
    lines_mat=[]
    record = record.strip()
    blks=record.split(Util.ENDOFRECORD)

    lines=blks[0].strip().split('\n\n')[1].strip().split('\n')
    for line in lines:
        tt=line.split(TAB)
        lines_m2g.append(tt[0]+TAB+' '.join(tt[1:])+TAB)
    lines=blks[2].strip().split('\n\n')[1].strip().split('\n')
    glist=lines[0].strip().split(TAB)
    lines_mat=lines[1:]

    gblks=blks[1].strip().split('\n\n')
    for gblk in gblks[1:]:
        lines=gblk.strip().split('\n')
        line_g2m=''
        tt=lines[0].strip().split(TAB)#Gene
        line_g2m+=tt[1]+TAB
        tt=lines[1].strip().split(TAB)#KO Reactions
        line_g2m+=' '.join(tt[1:])+TAB
        tt=lines[2].strip().split(TAB)#Metabolites
        line_g2m+=' '.join(tt[1:])+TAB
        lines_g2m.append(line_g2m)
    return lines_m2g, lines_g2m, lines_mat, glist


#w# parse the dpasig output into 2 table model data of OutputTableModel_gsa
def output2table_dpasig(record):
    lines_up=[]
    lines_dw=[]
    record = record.strip()
    blks=record.split(Util.ENDOFRECORD)

    lines=blks[0].strip().split('\n\n')[2].strip().split('\n')#for up-regulated genes
    list=lines[0].strip().split(TAB)
    lines_up=lines[1:]
    lines_dw=blks[1].strip().split('\n\n')[1].strip().split('\n')[1:]#for down-regulated genes

    return lines_up, lines_dw, list





#
#


#def loadModel(model, path):
#    if path != None:
#        f = open(path)
#        i = 0
#        for line in f:
#            i += 1
#            try:
#                readModelLine(line, model)
#            except:
#                raise Exception("Parsing error in line %d: %s" % (i, line))
#        f.close()
#
#def readModelLine(line, model):
#    line, comment = splitLineAndComment(line)
#    if not line.isspace():
#        r = Model.Element.Reaction()
#        toks = line.split(TAB)
#        ID = toks[0]
#        model[ID] = r
#        fields = r.getOrder()
#        for i in range(1, len(toks)):
#            if len(toks[i]) > 0:
#                tok = toks[i].strip('"')
#                model.editElement(ID, fields[i], tok)
#        model.editElement(ID, fields[-1], Util.COMMENT + comment)



#def loadExtendedModel(model, lines):
#        levels = s.split(Util.ENDOFRECORD)
#        for level in levels:
#            if len(level) > 0 and not level.isspace():
#                loadLevel(model, level)
#
#
#def loadModel(model, lines):
#    print "loading model"
#    if path != None:
#        f = open(path)
#        lines = f.readlines()
#        f.close()
#        readModelLines(lines, model, Model.Element.Reaction)


        

