import plotly
from plotly.graph_objs import Scatter, Layout
import sys

def get_list(x):
    with open(x) as f:
        lines = f.read().splitlines()
    return lines

def plot_list(xList, yList1, yList2, title, xName, yName):

    layout = dict(title = title, titlefont=dict(
            family='Courier New, monospace',
            size=48,
            color='#7f7f7f'
        ),
            xaxis = dict(title = xName, linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                         titlefont=dict(
                             family='Courier New, monospace',
                             size=30,
                             color='black'),
                         tickfont=dict(
                            family='Old Standard TT, serif',
                            size=24,
                            color='black')),
            yaxis = dict(title = yName, linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                         titlefont=dict(
                             family='Courier New, monospace',
                             size=30,
                             color='black'),
                         tickfont=dict(
                            family='Old Standard TT, serif',
                            size=24,
                            color='black')),
            legend = dict(
                        font=dict(
                            family='sans-serif',
                            size=20,
                            color='#000'
                        )))

    plotly.offline.plot({
    "data": 
    [Scatter(x=xList, y=yList1, mode = 'lines+markers', name = 'Dragon', marker = dict(color = 'rgba(0, 200, 0, .8)')),
     Scatter(x=xList, y=yList2, mode = 'lines+markers', name = 'Man', marker = dict(color = 'rgba(0, 0, 200, .8)'), )],

    "layout": Layout(layout)})


static_list = get_list('./PerfData/Dragon/fps1.txt')
static_list1 = get_list('./PerfData/Man/fps1.txt')

unpop_list = get_list('./PerfData/Dragon/fps2.txt')
unpop_list1 = get_list('./PerfData/Man/fps2.txt')

cpnt_list = get_list('./PerfData/Dragon/fps3.txt')
cpnt_list1 = get_list('./PerfData/Man/fps3.txt')

phong_list = get_list('./PerfData/Dragon/fps4.txt')
phong_list1 = get_list('./PerfData/Man/fps4.txt')

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



if techniqueToPlot == 4:

    static_list = get_list('./OutputTxt/Static.txt')
    unpopping_list = get_list('./OutputTxt/Unpopping.txt')
    cpnt_list = get_list('./OutputTxt/Cpnt.txt')
    phong_list = get_list('./OutputTxt/Phong.txt')

    numbers = []
    i = 0
    while i < 150000:
        i = i + 1
        numbers.append(i)

    layout = dict(title = 'Continuous Error',
            xaxis = dict(title = 'Capture point'),
            yaxis = dict(title = 'Error'),)

    plotly.offline.plot({
    "data": 
    [Scatter(x=numbers, y=static_list, mode = 'lines+markers', name = 'Static', marker = dict(color = 'rgba(0, 200, 0, .8)')),
        Scatter(x=numbers, y=unpopping_list, mode = 'lines+markers', name = 'Unpopping', marker = dict(color = 'rgba(0, 0, 200, .8)')),
        Scatter(x=numbers, y=cpnt_list, mode = 'lines+markers', name = 'Cpnt', marker = dict(color = 'rgba(200, 0, 200, .8)')),
        Scatter(x=numbers, y=phong_list, mode = 'lines+markers', name = 'Phong', marker = dict(color = 'rgba(200, 0, 0, .8)'))],

    "layout": Layout(layout)})