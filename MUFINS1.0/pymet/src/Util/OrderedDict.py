# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$25-Feb-2010 14:47:33$"


class OrderedDict:

    def __init__(self, tuples = []):
        self.order = []
        self.dict = {}
        for key, value in tuples:
            self.order.append(key)
            self.dict[key] = value

    def __delitem__(self, key):
        if key in self.dict:
            self.order.remove(key)
            del self.dict[key]

    def __str__(self):
        rv = ''
        for key in self.order:
            rv += key + ': ' + str(self.dict[key]) + '\n'
        return rv

    def __getitem__(self, key):
        return self.dict[key]

    def getKeyAt(self, n):
        return self.order[n]

    def getValueAt(self, n):
        key = self.getKeyAt(n)
        return self[key]

    def getOrder(self):
        return self.order[:]

    def copy(self):
        rv = OrderedDict()
        rv.order = self.order[:]
        rv.dict = self.dict.copy()
        return rv

    def __len__(self):
        return len(self.order)

    def replace(self, key, value):
        self.dict[key] = value

    def keys(self):
        return self.dict.keys()

    def values(self):
        return [self[key] for key in self.order]

    def __iter__(self):
        return self.order.__iter__()