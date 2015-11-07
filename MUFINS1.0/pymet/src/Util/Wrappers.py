__author__="Albert Gevorgyan"
__date__ ="$02-Mar-2010 16:31:19$"

COMMENT = '#'

class Wrapper:

    def __init__(self, s):
        self.__dict__['core'] = s

    def __getattr__(self, attr):
        return getattr(self.core, attr)

    def __setattr__(self, attr, val):
        if not attr in self.__dict__:
            setattr(self.core, attr, val)
        else:
            self.__dict__[attr] = val


class Comment(Wrapper):

    def __init__(self, s = ''):
        if not s.startswith(COMMENT):
            s = COMMENT + s
        Wrapper.__init__(self, s)






#class FloatWrapper(Wrapper):
#
#    Strings = {
#        INF  : 'inf',
#        -INF : '-inf',
#        NaN  : ''
#    }
#
#    Values = {
#        'inf' : INF,
#        '-inf': INF,
#        'nan' : NaN
#    }
#
#    def __init__(self, f = 0.0):
#        if type(f) == types.StringType:
#            f = f.lower()
#            f = self.Values[f]
#        Wrapper.__init__(self, f)
#
#
#    def __str__(self):
##        rv = str(self.core)
##        if self.core == lang.Float.NaN:
##            rv = ''
##        return rv
#        rv = str(self.core)
#        return self.Strings.get(self.core, rv)
#
#
#    def toString(self):
#        return self.__str__()

