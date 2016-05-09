import plotly
from plotly.graph_objs import Scatter, Layout
import sys

def get_list(x):
    with open(x) as f:
        lines = f.read().splitlines()
    return lines

#def plot_list(xList, yList, title, xName, yName):
#    plotly.offline.plot
#    ({
#        "data": 
#        [Scatter(x=xList, y=yList, mode = 'lines+markers', name = title, marker = dict(color = 'rgba(0, 255, 0, .8)'))],
#        "layout": Layout(dict(title = title,
#                 xaxis = dict(title = xName),
#                 yaxis = dict(title = yName)))
#    })
#    return 0

def plot_list(xList, yList1, yList2, title, xName, yName):

    layout = dict(title = title,
            xaxis = dict(title = xName),
            yaxis = dict(title = yName),)

    plotly.offline.plot({
    "data": 
    [Scatter(x=xList, y=yList1, mode = 'lines+markers', name = 'Dragon', marker = dict(color = 'rgba(0, 200, 0, .8)')),
     Scatter(x=xList, y=yList2, mode = 'lines+markers', name = 'Truck', marker = dict(color = 'rgba(0, 0, 200, .8)'))],

    "layout": Layout(layout)})


static_list = get_list('./PerfData/Dragon/Primitives/primitives1.txt')
static_list1 = get_list('./PerfData/Truck/Primitives/primitives1.txt')

unpop_list = get_list('./PerfData/Dragon/Primitives/primitives2.txt')
unpop_list1 = get_list('./PerfData/Truck/Primitives/primitives2.txt')

cpnt_list = get_list('./PerfData/Dragon/Primitives/primitives3.txt')
cpnt_list1 = get_list('./PerfData/Truck/Primitives/primitives3.txt')

phong_list = get_list('./PerfData/Dragon/Primitives/primitives4.txt')
phong_list1 = get_list('./PerfData/Truck/Primitives/primitives4.txt')

lengths = [len(static_list), len(unpop_list), len(cpnt_list), len(phong_list), len(static_list1), len(unpop_list1), len(cpnt_list1), len(phong_list1)]
lengths.sort()

numbers = []
count = lengths[7]
i = 0
while i < count:
    i = i + 1
    numbers.append(i)


techniqueToPlot = int(sys.argv[1])

if techniqueToPlot == 0:
    plot_list(numbers, static_list, static_list1, 'Static LoD', 'Seconds', 'Frames')
if techniqueToPlot == 1:
    plot_list(numbers, unpop_list, unpop_list1, 'Unpopping LoD', 'Seconds', 'Frames')
if techniqueToPlot == 2:
    plot_list(numbers, cpnt_list, cpnt_list1, 'Curved PN Triangles', 'Seconds', 'Frames')
if techniqueToPlot == 3:
    plot_list(numbers, phong_list, phong_list1, 'Phong Tessellation', 'Seconds', 'Frames')