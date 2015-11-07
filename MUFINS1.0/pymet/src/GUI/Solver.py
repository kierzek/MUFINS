import Data.Problem
# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="Albert Gevorgyan"
__date__ ="$16-Mar-2010 12:21:29$"


from java import lang, io

class Solver(lang.Thread):

    def __init__(self, panel, cmdarray):
        lang.Thread.__init__(self)
        self.panel = panel
        self.cmdarray = cmdarray
        self.outfile = ''
        if 'qsspn' in self.cmdarray[0]: #self.panel.__class__ == QSSPNTabs.QSSPNTabs; if 'qsspn' in self.cmdarray[0]
            self.outfile = self.panel.para['OUTPUT'] #w#
            open(self.outfile, 'w').close() #w# erase content

    def run(self):
        try:
            output = self.solve()
#            if self.panel.program in GSA:#w#
            if 'qsspn' in self.cmdarray[0]:
                self.panel.reportSolution(output)#w#
            else: self.panel.reportSolution_gsa(output, self.cmdarray)#w#
        except lang.Exception, e:
            self.panel.reportError(e)
        except Exception, e:
            self.panel.reportError(e)

    def solve(self):
        output, error = '', ''
        print self.cmdarray#t#
#        self.process = lang.Runtime.getRuntime().exec(['sfba.exe', '-i', 'model.tmp', '-b', 'bounds.tmp', '-p', 'fva', '-o', 'BIOMASS', '-f', 'out.tmp'])#t#
#        self.process = lang.Runtime.getRuntime().exec(' '.join(self.cmdarray))#t#
#        self.process = lang.ProcessBuilder(self.cmdarray).start()#t#
        self.process = lang.Runtime.getRuntime().exec(self.cmdarray)
        self.process.waitFor()#
#        lang.Runtime.getRuntime().freeMemory()#w#
#        output = self.readStream(self.process.getInputStream())
        if 'qsspn' in self.cmdarray[0]: #w#
            output = Data.Problem.readOutput(self.outfile)
        else: output = Data.Problem.readOutput()
        error  = self.readStream(self.process.getErrorStream())
        #error += self.readStream(self.process.getInputStream())
        self.process.destroy()#w# for free memory
        if len(error) > 0:
            raise Exception(error)
        return output

    def stop(self):
        self.process.destroy()

    def readStream(self, stream):
        rv = ''
        outreader = io.BufferedReader(io.InputStreamReader(stream))
        while outreader.ready():
            rv += outreader.readLine() + '\n'
        return rv