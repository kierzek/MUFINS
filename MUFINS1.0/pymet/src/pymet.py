__author__="Albert Gevorgyan"
__date__ ="$22-Feb-2010 12:31:27$"

#sys.path += ['lib/jython.jar', 'lib/jgraphx.jar', 'lib/jcommon-1.0.16.jar', 'lib/jfreechart-1.0.13.jar']


import GUI
import GUI.MainFrame
#from javax import swing
#from java import lang#t#

TITLE = 'JyMet2.34'
print 'Hello'#t#
class pymet:

    def main(self):
        frame = GUI.MainFrame.MainFrame()
        frame.setTitle(TITLE)
        frame.show()



if __name__ == "__main__":
    pymet().main()
#    process = lang.ProcessBuilder(['qsspn.exe', 'QSSPN_model.tmp', 'QSSPN_control.tmp']).start()
#    process = lang.Runtime.getRuntime().exec(['qsspn.exe', 'QSSPN_model.tmp', 'QSSPN_control.tmp'])#t#
#    print 'lang.Runtime.getRuntime'
#    process.waitFor()#t#
#    print 'process.waitFor'#t#
#    process.destroy()#t#
#    print 'process.destroy'#t#


    








