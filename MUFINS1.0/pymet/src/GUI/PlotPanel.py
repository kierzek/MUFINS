__author__="Albert Gevorgyan"
__date__ ="$26-Mar-2010 10:07:55$"

from java import awt
from java import io
from java import lang
from javax import swing

import org.jfree.chart
import org.jfree.chart.ChartFactory
import org.jfree.data.xy
import org.jfree.chart.plot
import org.jfree.chart.ChartUtilities
import org.jfree.chart.JFreeChart
import org.jfree.chart.axis.NumberAxis
import org.jfree.chart.axis.AxisLocation
import org.jfree.chart.plot.XYPlot
import org.jfree.chart.renderer
import org.jfree.chart.renderer.xy.XYBlockRenderer
import org.jfree.chart.title.PaintScaleLegend
import org.jfree.ui.RectangleEdge
import org.jfree.data.general
import org.jfree.data.statistics

#from  org.jfree.ui import RectangleInsets;#w#
from  java.awt.geom import Rectangle2D#w#
from org.jfree.chart import LegendItem, LegendItemCollection#w#
#from org.jfree.chart.title import LegendTitle#w#
from org.jfree.chart.renderer.category import BarRenderer; #w#
from java.awt import Color; #w#
#import java.awt.Paint; #w#

from Table.OutputTable import ENDOFRECORD
TAB = ' \t'
BACKGROUND = awt.Color.white  #(102, 102, 255)
GRIDCOLOR  = awt.Color.gray

#XYTITLE={#w#
#    'FBA': ('Reactions', 'Flux'),
#    'FVA': ('Reactions', 'Flux range'),
#    'GFAA': ('Reactions/Genes', 'Flux activity'),
#    'WGNI': ('Gene: Nutrients', 'Number of essential medium'),
#    }

class PlotPanel(org.jfree.chart.ChartPanel):
    TITLE  = ''
    XTITLE = ''
    YTITLE = ''

    def __init__(self, chart):
        org.jfree.chart.ChartPanel.__init__(self, chart)

    def save(self, path):
        rectangle = self.getScreenDataArea()
        w = rectangle.getWidth()
        h = rectangle.getHeight()
        org.jfree.chart.ChartUtilities.saveChartAsPNG(io.File(path), self.getChart(), int(w), int(h))

    def extractLists(self, lists):
        rv = []
        if len(lists) > 1:
            curlists = []
            for l in lists:
                if self.isEndOfSolution(l):
#                    print "end: ", l
                    if len(curlists) > 0 and not self.isValueEmpty(curlists[-1][0]):
                        rv.append(curlists)
                    curlists = []
                else:
                    curlists.append(l)
            if len(curlists) > 0 and not self.isValueEmpty(curlists[-1][0]):
                rv.append(curlists)
        return rv


    def addLists(self, lists):
        llists = self.extractLists(lists)#list of lists for multi-records
        for l in llists:
            self.addExtractedLists(l)

    def addExtractedLists(self, lists):
        pass

    def getTitle(self, lists):
        return lists[0][-1][1:].strip(':')

    def isValueEmpty(self, value):
        return lang.Float.isNaN(value)

    def isEndOfSolution(self, l):
        return l[-1][1:] == ENDOFRECORD

 
class XYPanel(PlotPanel):
    XTITLE = 'Constraint'
    YTITLE = 'Objective'

    def __init__(self):
        self.dataset = org.jfree.data.xy.XYSeriesCollection()
        chart = org.jfree.chart.ChartFactory.createXYLineChart(\
                self.TITLE,\
                self.XTITLE,\
                self.YTITLE, \
                self.dataset, \
                org.jfree.chart.plot.PlotOrientation.VERTICAL, \
                True, \
                True, \
                False \
            )       
        chart.getPlot().getRangeAxis().setAutoRangeIncludesZero(False)
        PlotPanel.__init__(self, chart)
        
    def getTitle(self, lists):
        title = PlotPanel.getTitle(self, lists)
        args = title.split(TAB)
        return "%s(%s)" % (args[-1], args[0])

    def addExtractedLists(self, lists):
        if len(lists) > 1:
            title = self.getTitle(lists)
            series = org.jfree.data.xy.XYSeries(title)
            for l in lists:
                x = l[0]
                if not lang.Float.isNaN(x):
                    series.add(x, l[1])
            self.dataset.addSeries(series)

    def isValueEmpty(self, value):
        return lang.Float.isNaN(value)


class XYPanelTrajectory(PlotPanel):
    XTITLE = 'Time'
    YTITLE = 'Level'

    def __init__(self, lists):
        self.dataset = org.jfree.data.xy.XYSeriesCollection()
        self.addDataset(lists)
        chart = org.jfree.chart.ChartFactory.createXYLineChart(\
                self.TITLE,\
                self.XTITLE,\
                self.YTITLE, \
                self.dataset, \
                org.jfree.chart.plot.PlotOrientation.VERTICAL, \
                True, \
                True, \
                False \
            )
        chart.getPlot().getRangeAxis().setAutoRangeIncludesZero(False)
        PlotPanel.__init__(self, chart)

    def addDataset(self, lists):
        title = lists[0]
        for c in range(1, len(title)):
            series = org.jfree.data.xy.XYSeries(title[c])
            for r in range(1, len(lists)):
                series.add(float(lists[r][0]), float(lists[r][c]))
            self.dataset.addSeries(series)


class XYZPanel(PlotPanel):

    def __init__(self, lists):
        llists = self.extractLists(lists)
        
        if len(llists) > 1:
            raise Exception("Can't plot more than one output record!")
        elif len(llists) == 0:
            raise Exception("Please select at least two rows to plot!")

        title, xlabel, ylabel = self.getLabels(lists)
        lists = self.convertLists(llists[0])
        width, height = self.getBlockDimension(lists)
        lb, ub = self.getScaleRange(lists)

        self.dataset = org.jfree.data.xy.DefaultXYZDataset()
        self.dataset.addSeries(title, lists)

        xAxis = org.jfree.chart.axis.NumberAxis(xlabel, autoRangeIncludesZero = False)
        yAxis = org.jfree.chart.axis.NumberAxis(ylabel, autoRangeIncludesZero = False)

        scale = org.jfree.chart.renderer.GrayPaintScale(lb, ub)

        renderer = org.jfree.chart.renderer.xy.XYBlockRenderer()
        renderer.setPaintScale(scale)
        renderer.setBlockWidth(width)
        renderer.setBlockHeight(height)

        plot = org.jfree.chart.plot.XYPlot(self.dataset, xAxis, yAxis, renderer)
        plot.setBackgroundPaint(BACKGROUND)
        plot.setRangeGridlinePaint(GRIDCOLOR)
        plot.setDomainGridlinePaint(GRIDCOLOR)

        chart = org.jfree.chart.JFreeChart(title, plot)
        chart.removeLegend()

        scaleAxis = org.jfree.chart.axis.NumberAxis("")
        scaleAxis.setLowerBound(lb)
        scaleAxis.setUpperBound(ub)


        legend = org.jfree.chart.title.PaintScaleLegend(scale, scaleAxis)
        legend.setAxisLocation(org.jfree.chart.axis.AxisLocation.BOTTOM_OR_LEFT)
        legend.setPadding(5, 5, 5, 5)
        legend.setPosition(org.jfree.ui.RectangleEdge.RIGHT)
        chart.addSubtitle(legend)

        PlotPanel.__init__(self, chart)



    def convertLists(self, lists):
        x, y, z = [], [], []
        for tup in lists:
            if not lang.Float.isNaN(tup[0]):
                x.append(tup[0])
                y.append(tup[1])
                z.append(tup[2])
        return [x, y, z]

    def getBlockDimension(self, lists):
        width , height = 1, 1
        if len(lists[0]) > 1:
            x1 = x0 = lists[0][0]
            i = 1
            while x1 == x0 and i < len(lists[0]):
                x1 = lists[0][i]
                i += 1
            width = (x1 - x0) or width
        if len(lists[1]) > 1:
            height = (lists[1][1] - lists[1][0]) or height
        return abs(width), abs(height)#w# abs

    def getScaleRange(self, lists):
        if len(lists[2]) > 0:
            lb = min(lists[2])
            ub = max(lists[2])
            if ub > lb:
                return lb, ub
        return 0, 1

    def getLabels(self, lists):
        title = PlotPanel.getTitle(self, lists)
        xlabel, ylabel = "", ""
        args = title.split(TAB)
        if len(args) == 3:
            xlabel = args[0]
            ylabel = args[1]
            title = "%s(%s, %s)" % (args[2], xlabel, ylabel)
        return title, xlabel, ylabel




class BarPanel(PlotPanel):
    XTITLE = 'Reactions'
    YTITLE = 'Flux rate'
    SHOWRANGE = 20
    
    class Listener(swing.event.ChangeListener):
        def __init__(self, owner):
            self.owner = owner

        def stateChanged(self, e):
            self.owner.moveSlider()

    class ShowPanel(swing.JPanel):
        def __init__(self, owner):
            self.owner = owner
            swing.JPanel.__init__(self, layout = awt.BorderLayout())
        def save(self, path):
            self.owner.save(path)

    def __init__(self, lists):
        self.titles = {}
        self.nlists = 0
        self.createChart(lists)
        PlotPanel.__init__(self, self.Chart)
        self.createSlider()


    def createChart(self, lists):
        self.dataset = org.jfree.data.category.DefaultCategoryDataset()
        self.addLists(lists)
        self.slideset = org.jfree.data.category.SlidingCategoryDataset(self.dataset, 0, self.SHOWRANGE)
        self.Chart = org.jfree.chart.ChartFactory.createBarChart(\
                self.TITLE,\
                self.XTITLE,\
                self.YTITLE, \
                self.slideset, \
                org.jfree.chart.plot.PlotOrientation.HORIZONTAL, \
                True, \
                True, \
                False \
            )


    def getTitle(self, lists):
        if lists[0][0]=='': rv = lists[0][-1][1:].strip(':')#w#
        else: rv='Flux'#w#
        while rv in self.titles:
            rv += "'"
        self.titles[rv] = 0
        return rv

    def addExtractedLists(self, lists):
        if len(lists) > 1:
            title = self.getTitle(lists[1:])
#            title =lists[0][1]
            for l in lists[1:]:#w# first row is column names
                if l[0]:
#                    pointname = l[0] + ': ' + l[-1][1:]
                    pointname = l[0]#w#
                    if len(self.titles) > 1:
                        pointname += " (%d)" % self.nlists
                    self.setValue(l, title, pointname)
            self.nlists += 1

    def setValue(self, l, title, name):
        self.dataset.setValue(l[1], title, name)

    def isEndOfSolution(self, l):
        return PlotPanel.isEndOfSolution(self, l) or (len(l[0]) == 0 and len(l[-1]) <= 1)

    def createSlider(self):
        range = max(0, self.getColumnCount() - self.SHOWRANGE)
        self.slider = swing.JSlider(swing.JSlider.VERTICAL, 0, range, 0)
        self.slider.setMajorTickSpacing(1)
        self.slider.setInverted(True)
        listener = self.Listener(self)
        self.slider.addChangeListener(listener)
        self.panel = self.ShowPanel(self)
        self.panel.add(self, awt.BorderLayout.CENTER)
        self.panel.add(self.slider, awt.BorderLayout.EAST)

    def isValueEmpty(self, value):
        return len(value) == 0

    def moveSlider(self):
        self.slideset.setFirstCategoryIndex(self.slider.getValue())

    def getColumnCount(self):
        return self.dataset.getColumnCount()


class FVAPanel(BarPanel):
    XTITLE = 'Reactions'
    YTITLE = 'Flux range'

    def __init__(self, lists):
        self.data = []
        self.showstart = 0
        BarPanel.__init__(self, lists)

    def createChart(self, lists):
        self.addLists(lists)
        self.createChartFromData()

    def createChartFromData(self):
        self.dataset = org.jfree.data.category.DefaultCategoryDataset()
        self.dataset = org.jfree.data.statistics.DefaultBoxAndWhiskerCategoryDataset()
        Range = min(self.SHOWRANGE, len(self.data) - self.showstart)
        for i in range(self.showstart, self.showstart + Range):
            bounds, title, name = self.data[i]
            self.dataset.add(bounds, title, name)
        self.Chart = org.jfree.chart.ChartFactory.createBoxAndWhiskerChart(\
                self.TITLE,\
                self.XTITLE,\
                self.YTITLE, \
                self.dataset, \
                True
        )
        self.Chart.getPlot().setOrientation(org.jfree.chart.plot.PlotOrientation.HORIZONTAL)

    def setValue(self, l, title, name):
        lb, ub = l[1], l[3]
        self.data.append([[lb, ub], title, name])

    def moveSlider(self):
        self.showstart = self.slider.getValue()
        self.createChartFromData()
        self.setChart(self.Chart)

    def getColumnCount(self):
        return len(self.data)



class MXSPanel(BarPanel):
    def setValue(self, l, title, name):
        val = l[1]
        if val.isNan():
            val = l[2]
        self.dataset.setValue(val, title, name)


class GFAAPanel(BarPanel):#w#
    XTITLE = 'Reactions/Genes'
    YTITLE = 'Flux activity'

    def __init__(self, lists):
        BarPanel.__init__(self, lists)
        self.state =self.getState(lists)
        plot = self.chart.getCategoryPlot();
        renderer = self.colorRenderer(self.state,self.dataset);
        plot.setRenderer(renderer);
        plot.setFixedLegendItems(self.setLegendItems())
#        plot.setInsets(RectangleInsets(5, 5, 5, 20));
#        legend = LegendTitle(plot);
#        legend.setPosition(org.jfree.ui.RectangleEdge.BOTTOM);
#        self.chart.addSubtitle(legend);
#        self.chart.setBackgroundPaint(Color.lightGray);

    def getState(self, lists):
        rv=[]
        for rd in lists[1:]: rv+=[rd[6]]
        return rv

    def setValue(self, l, title, name):
        self.dataset.setValue(l[5], title, name)

    class colorRenderer(BarRenderer):
        def __init__(self, state, dataset):
            BarRenderer.__init__(self)
            self.state=state
            self.dataset=dataset
            
        def getItemPaint(self, row, col):
            if (self.state[col]!='1' and self.dataset.getValue(row,col)>0): return Color.green
            elif (self.state[col]!='-1' and self.dataset.getValue(row,col)<0): return Color.red
            else: return Color.blue

    def setLegendItems(self):
        items = LegendItemCollection()
        items.add(LegendItem("PostUpRegulated",
                                 None,
                                 None,
                                 None,
                                 Rectangle2D.Double(-6.0, -3.0, 12.0, 6.0),
                                 Color.green));
        items.add(LegendItem("PostDownRegulated",
                                 None,
                                 None,
                                 None,
                                 Rectangle2D.Double(-6.0, -3.0, 12.0, 6.0),
                                 Color.red));
        return items


class WGNIPanel(BarPanel):#w#
    XTITLE = "Gene : Nutrient"
    YTITLE = "Number of essential medium"

    def __init__(self, lists):
        BarPanel.__init__(self, lists)
        plot = self.chart.getCategoryPlot();
        plot.getRenderer().setSeriesPaint(0, Color.red)
        plot.getRenderer().setSeriesPaint(1, Color.green)
#        self.chart.setBackgroundPaint(Color.lightGray);


    def createChart(self, lists):
        self.dataset = org.jfree.data.category.DefaultCategoryDataset()
        self.addExtractedLists(lists)
        self.slideset = org.jfree.data.category.SlidingCategoryDataset(self.dataset, 0, self.SHOWRANGE)
        self.Chart = org.jfree.chart.ChartFactory.createStackedBarChart(\
                self.TITLE,\
                self.XTITLE,\
                self.YTITLE, \
                self.slideset, \
                org.jfree.chart.plot.PlotOrientation.HORIZONTAL, \
                True, \
                True, \
                False \
            )

#    def addExtractedLists(self, lists):
#        if len(lists) > 1:
#            for rowl in lists[1:]:
#                print rowl#t#
#                rowkey=rowl[0]
#                for i in range(1,len(rowl)):
#                    colkey=lists[0][i]
#                    print rowkey,':',colkey#t#
#                    self.dataset.setValue(rowl[i], rowkey, colkey)


    def addExtractedLists(self, lists):
        if len(lists) > 1:
            for rowl in lists[1:]:
                rowkey=rowl[0]
                for i in range(1,len(rowl),2):
                    colkey=lists[0][i].split('(')[0]
                    self.dataset.setValue(-rowl[i+1], 'WN', rowkey+': '+colkey)
                    self.dataset.setValue(rowl[i], 'WP', rowkey+': '+colkey)
                    

class BarPanel1(BarPanel):#w#

    def __init__(self, lists, title):
        if title=='DPAsig': self.XTITLE, self.YTITLE = "Metabolites", "Metabolic state"
        elif title=='DPAplot': self.XTITLE, self.YTITLE = "Metabolites", "Producibility"
        BarPanel.__init__(self, lists)

    def createChart(self, lists):
        self.dataset = org.jfree.data.category.DefaultCategoryDataset()
        self.addExtractedLists(lists)
        self.slideset = org.jfree.data.category.SlidingCategoryDataset(self.dataset, 0, self.SHOWRANGE)
        self.Chart = org.jfree.chart.ChartFactory.createBarChart(\
                self.TITLE,\
                self.XTITLE,\
                self.YTITLE, \
                self.slideset, \
                org.jfree.chart.plot.PlotOrientation.HORIZONTAL, \
                True, \
                True, \
                False \
            )

    def addExtractedLists(self, lists):
        if len(lists) > 1:
            for rowl in lists[1:]:
                colkey=rowl[0]
                for i in range(1,len(rowl)):
                    rowkey=lists[0][i] 
                    self.dataset.setValue(rowl[i], rowkey, colkey)


