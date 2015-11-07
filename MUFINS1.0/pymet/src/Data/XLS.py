# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$12-Jul-2011 17:18:31$"

from exceptions import Exception

import pyExcelerator as pxls

import Model.Element


class PyMetWorkBook(pxls.Workbook):

    def add_table_model(self, title, tmodel):
        ws = self.add_sheet(title)
        ncols = tmodel.getColumnCount()
        bold = pxls.XFStyle()
        bold.font.bold = True
        for c in range(ncols):
            ws.write(0, c, tmodel.getColumnName(c), bold)
        for r in range(0, tmodel.getRowCount()):
            for c in range(ncols):
                try:
                    ws.write(r + 1, c, tmodel.getStringValueAt(r, c))
                except:
                    raise Exception("Exception in %s row %d, column %d: %s" % (title, r, c, tmodel.getStringValueAt(r, c)))


def loadModel(model, path):
    for sheetname, recs in parse_file(path):
        if sheetname == 'Problem':
            return recs
        else:
            theclass = Model.Element.__getattribute__(sheetname.strip('s'))
            for rec in recs:
                el = theclass()
                ID = rec['ID']
                model.setdefault(ID, el)
                for key in el.getOrder():
                    if key != Model.Element.USEDBY:
                        val = rec[key]
                        if val != None:
                            model.editElement(ID, key, str(val))

def parse_file(fname):
    rv = []
    for name, sheet in pxls.parse_xls(fname, encoding=None):
        rv.append((name, parse_sheet(sheet)))
    return rv

def parse_sheet(recs):
    rv = []
    keys = sorted(recs)
    colkeys = filter(lambda x: x[0] == 0, keys)
    colnames = [recs[key] for key in colkeys]
    numcols = len(colkeys)
    numrows = len(filter(lambda x: x[1] == 0, keys))
    for r in range(1, numrows):
        rec = {}
        for c in range(0, numcols):
           rec[colnames[c]] = recs.get((r, c), None)
        rv.append(rec)

    return rv