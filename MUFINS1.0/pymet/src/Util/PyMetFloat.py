__author__="Albert Gevorgyan"
__date__ ="$01-Mar-2010 18:20:57$"

from java import lang

INF = float(lang.Float.POSITIVE_INFINITY)
NaN = float(lang.Float.NaN)

class PyMetFloat(float):

    Strings = {
        INF  : 'inf',
        -INF : '-inf',
    }

    def __init__(self, f = 0):
        float.__init__(self, f)
        
    def isNan(self):
        return lang.Float.isNaN(self)

    def __str__(self):
        rv = float.__str__(self)
        if lang.Float.isNaN(self):
            rv = ''
        return self.Strings.get(self, rv)


def formatFloat(va):#w#
        if abs(va)<1e-10: va='0'
        elif abs(va)%1==0: va=str(int(va))
        elif va<1e-4: va='%.e'%va
        else: va='%.4f'%va
        return va

#class PyMetInt(lang.Integer):#w#
#
#    Strings = {
#        INF  : 'inf',
#        -INF : '-inf',
#    }
#
#    def __init__(self, f = 0):
#        lang.Integer.__init__(self, f)
#
#    def isNan(self):
#        return lang.Float.Integer(self)
#
#    def __str__(self):
#        rv = float.__str__(self)
#        if lang.Integer.isNaN(self):
#            rv = ''
#        return self.Strings.get(self, rv)

