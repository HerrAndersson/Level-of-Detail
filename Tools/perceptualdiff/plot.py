import plotly
from plotly.graph_objs import Scatter, Layout
import sys

#https://plot.ly/python/line-charts/
with open('./PerfData/One/fps1.txt') as f1:
    lines1 = f1.read().splitlines()
with open('./PerfData/One/fps2.txt') as f2:
    lines2 = f2.read().splitlines()
with open('./PerfData/One/fps3.txt') as f3:
    lines3 = f3.read().splitlines()
with open('./PerfData/One/fps4.txt') as f4:
    lines4 = f4.read().splitlines()

lengths = [len(lines1), len(lines2), len(lines3), len(lines4)]
lengths.sort()

numbers = []
count = lengths[3]
i = 0
while i < count:
    i = i + 1
    numbers.append(i)

techniqueToPlot = int(sys.argv[1])

############################# Plot Static LoD #############################
if techniqueToPlot == 0:
    with open('./PerfData/One/primitives1.txt') as f11:
        lines11 = f11.read().splitlines()

    layout = dict(title = 'Static',
              xaxis = dict(title = 'Seconds'),
              yaxis = dict(title = 'Frames'),)

    plotly.offline.plot({
    "data": 
    [Scatter(x=numbers, y=lines1, mode = 'lines+markers', name = 'Frames', marker = dict(color = 'rgba(0, 255, 0, .8)')),
        Scatter(x=numbers, y=lines11, mode = 'lines+markers', name = 'Primitives', marker = dict(color = 'rgba(255, 0, 0, .8)'))],
    "layout": Layout(layout)
    })

############################# Plot Unpopping LoD #############################
if techniqueToPlot == 1:
    with open('./PerfData/One/primitives2.txt') as f22:
        lines22 = f22.read().splitlines()


    layout = dict(title = 'Unpopping',
            xaxis = dict(title = 'Seconds'),
            yaxis = dict(title = 'Frames'),)

    plotly.offline.plot({
    "data": 
    [Scatter(x=numbers, y=lines2, mode = 'lines+markers', name = 'Frames', marker = dict(color = 'rgba(0, 255, 0, .8)')),
        Scatter(x=numbers, y=lines22, mode = 'lines+markers', name = 'Primitives', marker = dict(color = 'rgba(255, 0, 0, .8)'))],
    "layout": Layout(layout)})

################################ Plot Cpnt ###################################
if techniqueToPlot == 2:
    with open('./PerfData/One/primitives3.txt') as f33:
        lines33 = f33.read().splitlines()


    layout = dict(title = 'Curved PN Triangles',
            xaxis = dict(title = 'Seconds'),
            yaxis = dict(title = 'Frames'),)

    plotly.offline.plot({
    "data": 
    [Scatter(x=numbers, y=lines3, mode = 'lines+markers', name = 'Frames', marker = dict(color = 'rgba(0, 255, 0, .8)')),
        Scatter(x=numbers, y=lines33, mode = 'lines+markers', name = 'Primitives', marker = dict(color = 'rgba(255, 0, 0, .8)'))],
    "layout": Layout(layout)})
################################ Plot Phong ###################################
if techniqueToPlot == 3:
    with open('./PerfData/One/primitives4.txt') as f44:
        lines44 = f44.read().splitlines()

    layout = dict(title = 'Phong Tessellation',
        xaxis = dict(title = 'Seconds'),
        yaxis = dict(title = 'Frames'),)

    plotly.offline.plot({
    "data":
    [Scatter(x=numbers, y=lines4, mode = 'lines+markers', name = 'Frames', marker = dict(color = 'rgba(0, 255, 0, .8)')),
        Scatter(x=numbers, y=lines44, mode = 'lines+markers', name = 'Primitives', marker = dict(color = 'rgba(255, 0, 0, .8)'))],
    "layout": Layout(layout)})

################################ Plot all four techniques
################################ ###################################
if techniqueToPlot == 4:

    layout = dict(title = '',
        xaxis = dict(title = 'Seconds'),
        yaxis = dict(title = 'Frames'),)

    plotly.offline.plot({
        "data": 
        [Scatter(x=numbers, y=lines1, mode = 'lines+markers', name = 'Static'),
            Scatter(x=numbers, y=lines2, mode = 'lines+markers', name = 'Unpopping'),
            Scatter(x=numbers, y=lines3, mode = 'lines+markers', name = 'Cpnt'),
            Scatter(x=numbers, y=lines4, mode = 'lines+markers', name = 'Phong'),],
        "layout": Layout(layout)
        })
