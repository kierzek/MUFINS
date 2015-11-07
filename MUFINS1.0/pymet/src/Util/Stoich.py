__author__="Albert Gevorgyan"
__date__ ="$22-Feb-2010 12:31:27$"

EQ    = '='
SPACE = ' '
PLUS  = '+'
OPS = ['AND', 'OR', '(', ')']


class Stoichiometry(dict):

    def __init__(self, eq = None):
        dict.__init__(self)
        self.sigmet = {}#w# for signalling activator('&')/inhibitor('~')
        if type(eq) == self.__class__:
            self.update(eq)
        elif type(eq) == str:
#            print 'type=str'#t#
            self.read(eq)

    def __str__(self):
#        print self.sigmet#t#
        toks = []
        keys = self.keys()
        keys.sort()
        for k in keys:
            val = self[k]
            strval = ''
            if val != 1.0:
                strval = str(val) + ' '
            tok = strval + k
            if k in self.sigmet:
                tok = self.sigmet[k] + ' ' + tok #w# for signalling mets
            toks.append(tok)
        return ' + '.join(toks)

#    def read(self, eq):
#        eq = eq.strip()
#        if len(eq) > 0:
#            toks = eq.split(PLUS)
#            for tok in toks:
#                tok = tok.strip()
#                if len(tok) == 0:
#                    raise SyntaxError('Empty token: ' + tok)
#                val, key = 1.0, tok
#                valkey = filter(lambda x: len(x), tok.split(SPACE))
#                n = len(valkey)
#                if n == 2:
#                    val, key = float(valkey[0]), valkey[1]
#                elif n > 2:
#                    raise SyntaxError('Name contains spaces: ' + tok)
#                self.add(key, val)

    def read(self, eq): #w# modify, add read activator and inhibitor
        eq = eq.strip()
        if len(eq) > 0:
            toks = eq.split(PLUS)
            for tok in toks:
                tok = tok.strip()
                if len(tok) == 0:
                    raise SyntaxError('Empty token: ' + tok)
                val, key = 1.0, tok
                valkey = filter(lambda x: len(x), tok.split(SPACE))
                n = len(valkey)
                if valkey[0]=="&":#w#
#                    print '&', valkey#t#
                    if n==2:
                        val, key = 1.0, valkey[1]
                        self.sigmet[key]="&"
                    elif n > 2:
                        raise SyntaxError('Name contains spaces: ' + tok)
                elif valkey[0]=="~":#w#
#                    print '~', valkey#t#
                    if n==2:
                        val, key = 1.0, valkey[1]
                        self.sigmet[key]="~"
                    elif n==3:
                        val, key = float(valkey[1]), valkey[2]
                        self.sigmet[key]="~"
                    elif n>3:
                        raise SyntaxError('Name contains spaces: ' + tok)
                elif n == 2:
                    val, key = float(valkey[0]), valkey[1]
                elif n > 2:
                    raise SyntaxError('Name contains spaces: ' + tok)
                self.add(key, val)

    def add(self, k, val):
        if not self.has_key(k):
            self[k] = 0
        self[k] += val

    def __imul__(self, coef):
        for k in self:
            val = self[k]
            if val != 0:
                val *= coef
            self[k] = val
        return self

    def __mul__(self, coef):
        rv = self.__class__()
        rv.update(self)
        rv *= coef
        return rv

    def __neg__(self):
        return self * -1

    def tagKeys(self, tag):
        for k in self.keys():
            self[k + tag] = self[k]
            del self[k]



class StoichList(Stoichiometry):
    def __str__(self):
        keys = self.keys()
        keys.sort()
        return ', '.join(keys)

#    def read(self, eq):
#        pass
    

class Equation(Stoichiometry):

#    def __init__(self, eq=None):#t# enable Equation to access self.sigmet
#        self.tt='test'#t#
#        Stoichiometry.__init__(self, eq)

    def IO(self):
        input, output = Stoichiometry(), Stoichiometry()
        for k in self:
            val = self[k]
            if val <= 0:
                input[k] = val
            if val >= 0:
                output[k] = val
        return input, output

    def __str__(self):
#        print 'Equation_str'#t#
        input, output = self.IO()
        input = input * -1
        input.sigmet = self.sigmet#w#
        output.sigmet = self.sigmet#w#
        return (' %s ' % EQ).join([str(input), str(output)])

    def read(self, eq):
#        print 'eq_read'#t#
        lhs, rhs = eq.split(EQ)
        Stoichiometry.read(self, lhs)
        self *= -1
        Stoichiometry.read(self, rhs)


class Rule(Stoichiometry):
    Ops = OPS

    def __init__(self, eq = ''):
        Stoichiometry.__init__(self, eq)
        if type(eq) == str:
            self.string = eq
        else:
            self.string = eq.string

    def __str__(self):
        return self.string


    def read(self, eq):
        self.string = eq
        toks = eq.split(' ')
        for tok in toks:
            if tok and (not tok.upper() in self.Ops):
                self.add(tok, 1)

    def tagKeys(self, tag):
        for k in self:
            self.string = self.string.replace(k, k + tag)
            
        Stoichiometry.tagKeys(self, tag)

