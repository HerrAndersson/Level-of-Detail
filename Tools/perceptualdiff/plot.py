# Warning! This file was created in a hurry and is a complete mess of ugly code and an even worse structure.
# Proceed at your own risk!

import plotly
from plotly.graph_objs import Scatter, Layout, Figure, Histogram
import sys
import os
from random import randint

def get_list(x):
    with open(x) as f:
        lines = f.read().splitlines()
    return lines

def get_avg_list(list, count):
    index = 0
    temp = 0
    avg_list = []
    while index < len(list)-1:
        for i in range(count):
            temp += int(list[index+1])
        avg_list.append(temp/count)
        temp = 0
        index+=count
    return avg_list;

def compare_first(item1, item2):
    if get_first_num(item1) < get_first_num(item2):
        return -1
    elif get_first_num(item1) > get_first_num(item2):
        return 1
    else:
        return 0


def plot_list(xList, yList1, yList2, title, xName, yName):

    layout = dict(title = title, titlefont=dict(family='Courier New, monospace',
            size=48,
            color='#7f7f7f'),
            xaxis = dict(title = xName, linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                         titlefont=dict(family='Courier New, monospace',
                             size=30,
                             color='black'),
                         tickfont=dict(family='Old Standard TT, serif',
                            size=24,
                            color='black')),
            yaxis = dict(title = yName, linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                         titlefont=dict(family='Courier New, monospace',
                             size=30,
                             color='black'),
                         tickfont=dict(family='Old Standard TT, serif',
                            size=24,
                            color='black')),
            legend = dict(font=dict(family='sans-serif',
                            size=20,
                            color='#000')))

    plotly.offline.plot({
    "data": 
    [Scatter(x=xList, y=yList1, mode = 'lines+markers', name = 'Dragon', marker = dict(color = 'rgba(0, 200, 0, .8)')),
     Scatter(x=xList, y=yList2, mode = 'lines+markers', name = 'Man', marker = dict(color = 'rgba(0, 0, 200, .8)'),)],

    "layout": Layout(layout)})


#static_list = get_list('./PerfData/Dragon/fps1.txt')
#static_list1 = get_list('./PerfData/Man/fps1.txt')

#unpop_list = get_list('./PerfData/Dragon/fps2.txt')
#unpop_list1 = get_list('./PerfData/Man/fps2.txt')

#cpnt_list = get_list('./PerfData/Dragon/fps3.txt')
#cpnt_list1 = get_list('./PerfData/Man/fps3.txt')

#phong_list = get_list('./PerfData/Dragon/fps4.txt')
#phong_list1 = get_list('./PerfData/Man/fps4.txt')

static_list = get_list('./PerfData/Dragon/primitives1.txt')
static_list1 = get_list('./PerfData/Man/primitives1.txt')

unpop_list = get_list('./PerfData/Dragon/primitives2.txt')
unpop_list1 = get_list('./PerfData/Man/primitives2.txt')

cpnt_list = get_list('./PerfData/Dragon/primitives3.txt')
cpnt_list1 = get_list('./PerfData/Man/primitives3.txt')

phong_list = get_list('./PerfData/Dragon/primitives4.txt')
phong_list1 = get_list('./PerfData/Man/primitives4.txt')

lengths = [len(static_list), len(unpop_list), len(cpnt_list), len(phong_list), len(static_list1), len(unpop_list1), len(cpnt_list1), len(phong_list1)]
lengths.sort()

numbers = []
count = lengths[7]
i = 0
while i < count:
    i = i + 1
    numbers.append(i)


techniqueToPlot = int(sys.argv[1])

#Plot performance
if techniqueToPlot == 0:
    plot_list(numbers, static_list, static_list1, 'Static LoD', 'Half-seconds', 'Number of triangles')
if techniqueToPlot == 1:
    plot_list(numbers, unpop_list, unpop_list1, 'Unpopping LoD', 'Seconds', 'Frames')
if techniqueToPlot == 2:
    plot_list(numbers, cpnt_list, cpnt_list1, 'Curved PN Triangles', 'Seconds', 'Frames')
if techniqueToPlot == 3:
    plot_list(numbers, phong_list, phong_list1, 'Phong Tessellation', 'Seconds', 'Frames')


#Plot continuous error
if techniqueToPlot == 4:

    model = sys.argv[2]
    technique = sys.argv[3]
    name = sys.argv[4]

    list1 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output '+model+' level above/'+technique+'.txt')
    list2 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output '+model+' highest level/'+technique+'.txt')

    numbers = []
    yl = []
    i = 0
    while i < len(list1):
        i = i + 1
        numbers.append(i)
        yl.append(1000)


    layout = dict(title = 'Dragon - Error using '+name, titlefont=dict(family='Courier New, monospace',
        size=48,
        color='#7f7f7f'),
        xaxis = dict(title = 'Images', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        yaxis = dict(title = 'Perceptually different pixels', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        legend = dict(font=dict(family='sans-serif',
                        size=20,
                        color='#000')))

    plotly.offline.plot({
    "data": 
    [Scatter(x=numbers, y=list2, mode = 'lines+markers', name = 'Highest level', marker = dict(color = 'rgba(0, 200, 0, .8)')),
        Scatter(x=numbers, y=list1, mode = 'lines+markers', name = 'Level above', marker = dict(color = 'rgba(0, 0, 200, .8)')),
        Scatter(x=numbers, y=yl, mode = 'lines+markers', name = 'Level above', marker = dict(color = 'grey'))],
    "layout": Layout(layout)})

if techniqueToPlot == 5:

    numbers = [1,2,3,4]
    trace_list = []

    folder = sys.argv[2]
    object_name = sys.argv[3]

    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/'+folder+'/UnpoppingRun' + unicode(index) + '_max.txt'))

    for index in range(len(result_list)):
        trace_list.append(Scatter(x=numbers, y=result_list[index], mode = 'lines+markers', name = 'Run'+unicode(index)+'_max', marker = dict(color = 'rgba(200, 0, 0, .8)')))

    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/'+folder+'/UnpoppingRun' + unicode(index) + '_min.txt'))

    for index in range(len(result_list)):
        trace_list.append(Scatter(x=numbers, y=result_list[index], mode = 'lines+markers', name = 'Run'+unicode(index)+'_min', marker = dict(color = 'rgba(0, 200, 0, .8)')))

    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/'+folder+'/UnpoppingRun' + unicode(index) + '_avg.txt'))

    for index in range(len(result_list)):
        trace_list.append(Scatter(x=numbers, y=result_list[index], mode = 'lines+markers', name = 'Run'+unicode(index)+'_avg', marker = dict(color = 'rgba(0, 0, 200, .8)')))

    layout = dict(title = 'Unpopping LoD - Error caused by LoD-switches ('+object_name+')', titlefont=dict(family='Courier New, monospace',
    size=48,
    color='#7f7f7f'),
    xaxis = dict(title = 'Switches', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                    titlefont=dict(family='Courier New, monospace',
                        size=30,
                        color='black'),
                    tickfont=dict(family='Old Standard TT, serif',
                    size=24,
                    color='black')),
    yaxis = dict(title = 'Perceptually different pixels', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                    titlefont=dict(family='Courier New, monospace',
                        size=30,
                        color='black'),
                    tickfont=dict(family='Old Standard TT, serif',
                    size=24,
                    color='black')),
    legend = dict(font=dict(family='sans-serif',
                    size=20,
                    color='#000')))

    data = []
    for index in range(0, len(trace_list)):
        data.append(trace_list[index])

    plotly.offline.plot({
    "data": data,
    "layout": Layout(layout)})

if techniqueToPlot == 6:
    
    numbers = [1,2,3,4]
    trace_list = []

    folder = sys.argv[2]
    filename = sys.argv[3]
    technique_name = sys.argv[4]
    object_name = sys.argv[5]

    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/'+folder+'/'+filename+'' + unicode(index) + '.txt'))

    for index in range(len(result_list)):
        trace_list.append(Scatter(x=numbers, y=result_list[index], mode = 'lines+markers', name = 'Run'+unicode(index), marker = dict(color = 'rgba(0, 200, 0, .8)')))

    avg = []
    v = 0
    for i in range(4):
        for index in range(len(result_list)):
            v = v + int(result_list[index][i])
        avg.append(v/len(result_list))
        v=0

    trace_list.append(Scatter(x=numbers, y=avg, mode = 'lines+markers', name = 'Avg', marker = dict(color = 'rgba(0, 0, 200, .8)')))

    layout = dict(title = technique_name + ' - Error caused by LoD-switches ('+object_name+')', titlefont=dict(family='Courier New, monospace',
    size=48,
    color='#7f7f7f'),
    xaxis = dict(title = 'Switches', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                    titlefont=dict(family='Courier New, monospace',
                        size=30,
                        color='black'),
                    tickfont=dict(family='Old Standard TT, serif',
                    size=24,
                    color='black')),
    yaxis = dict(title = 'Perceptually different pixels', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                    titlefont=dict(family='Courier New, monospace',
                        size=30,
                        color='black'),
                    tickfont=dict(family='Old Standard TT, serif',
                    size=24,
                    color='black')),
    legend = dict(font=dict(family='sans-serif',
                    size=20,
                    color='#000')))

    data = []
    for index in range(0, len(trace_list)):
        data.append(trace_list[index])

    plotly.offline.plot({
    "data": data,
    "layout": Layout(layout)})


if techniqueToPlot == 7:

    numbers = [1,2,3,4]
    trace_list = []

    ####################################### Unpopping #######################################
    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/DragonPopResults/UnpoppingRun' + unicode(index) + '_avg.txt'))

    unpop_avg_list = [0,0,0,0]
  
    for index in range(len(result_list)):
        unpop_avg_list[0] += int(result_list[index][0]);
        unpop_avg_list[1] += int(result_list[index][1]);
        unpop_avg_list[2] += int(result_list[index][2]);
        unpop_avg_list[3] += int(result_list[index][3]);

    unpop_avg_list[0] /= len(result_list)
    unpop_avg_list[1] /= len(result_list)
    unpop_avg_list[2] /= len(result_list)
    unpop_avg_list[3] /= len(result_list)

    trace_list.append(Scatter(x=numbers, y=unpop_avg_list, mode = 'lines+markers', name = 'Unpopping LoD', marker = dict(color = 'rgba(204, 0, 204, .8)')))


    ######################################### Static #########################################
    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/DragonPopResults/StaticRun' + unicode(index) + '.txt'))

    avg = []
    v = 0
    for i in range(4):
        for index in range(len(result_list)):
            v = v + int(result_list[index][i])
        avg.append(v/len(result_list))
        v=0

    trace_list.append(Scatter(x=numbers, y=avg, mode = 'lines+markers', name = 'Static LoD', marker = dict(color = 'rgba(0, 0, 200, .8)')))

    ######################################### Cpnt #########################################
    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/DragonPopResults/CpntRun' + unicode(index) + '.txt'))

    avg = []
    v = 0
    for i in range(4):
        for index in range(len(result_list)):
            v = v + int(result_list[index][i])
        avg.append(v/len(result_list))
        v=0

    trace_list.append(Scatter(x=numbers, y=avg, mode = 'lines+markers', name = 'Curved PN Triangles', marker = dict(color = 'rgba(200, 0, 0, .8)')))

    ######################################### Phong #########################################
    result_list = []
    for index in range(5, 85):
        result_list.append(get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/DragonPopResults/PhongRun' + unicode(index) + '.txt'))

    avg = []
    v = 0
    for i in range(4):
        for index in range(len(result_list)):
            v = v + int(result_list[index][i])
        avg.append(v/len(result_list))
        v=0

    trace_list.append(Scatter(x=numbers, y=avg, mode = 'lines+markers', name = 'Phong Tessellation', marker = dict(color = 'rgba(0, 200, 0, .8)')))

    ######################################### Plot #########################################
    layout = dict(title = 'Average error caused by switching (Dragon)', titlefont=dict(family='Courier New, monospace',
    size=48,
    color='#7f7f7f'),
    xaxis = dict(title = 'Switches', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                    titlefont=dict(family='Courier New, monospace',
                        size=30,
                        color='black'),
                    tickfont=dict(family='Old Standard TT, serif',
                    size=24,
                    color='black')),
    yaxis = dict(title = 'Perceptually different pixels', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                    titlefont=dict(family='Courier New, monospace',
                        size=30,
                        color='black'),
                    tickfont=dict(family='Old Standard TT, serif',
                    size=24,
                    color='black')),
    legend = dict(font=dict(family='sans-serif',
                    size=20,
                    color='#000')))

    data = []
    for index in range(0, len(trace_list)):
        data.append(trace_list[index])

    plotly.offline.plot({
    "data": data,
    "layout": Layout(layout)})



if techniqueToPlot == 8:

    numbers = []
    i = 0
    while i < 15000:
        i = i + 1
        numbers.append(i)

    list1 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon level above/Static.txt')
    list2 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon highest level/Static.txt')
    list3 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon level above/Unpopping.txt')
    list4 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon highest level/Unpopping.txt')
    list5 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon level above/Cpnt.txt')
    list6 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon highest level/Cpnt.txt')
    list7 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon level above/Phong.txt')
    list8 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output Dragon highest level/Phong.txt')

    count = 4

    avg_trace_list = []
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list1, count), mode = 'lines+markers', name = 'Static - Above', marker = dict(color = 'rgba(0, 100, 0, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list2, count), mode = 'lines+markers', name = 'Static - Highest', marker = dict(color = 'rgba(0, 200, 0, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list3, count), mode = 'lines+markers', name = 'Unpopping- Above', marker = dict(color = 'rgba(0, 0, 100, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list4, count), mode = 'lines+markers', name = 'Unpopping - Highest', marker = dict(color = 'rgba(0, 0, 200, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list5, count), mode = 'lines+markers', name = 'Phong - Above', marker = dict(color = 'rgba(100, 0, 0, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list6, count), mode = 'lines+markers', name = 'Phong - Highest', marker = dict(color = 'rgba(200, 0, 0, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list7, count), mode = 'lines+markers', name = 'Curved PNT - Above', marker = dict(color = 'rgba(100, 0, 100, .8)')))
    #avg_trace_list.append(Scatter(x=numbers, y=get_avg_list(list8, count), mode = 'lines+markers', name = 'Curved PNT - Highest', marker = dict(color = 'rgba(200, 0, 200, .8)')))

    avg_trace_list.append(Scatter(x=numbers, y=list1, mode = 'lines+markers', name = 'Static - Above', marker = dict(color = 'rgba(0, 100, 0, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list2, mode = 'lines+markers', name = 'Static - Highest', marker = dict(color = 'rgba(0, 200, 0, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list3, mode = 'lines+markers', name = 'Unpopping- Above', marker = dict(color = 'rgba(0, 0, 100, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list4, mode = 'lines+markers', name = 'Unpopping - Highest', marker = dict(color = 'rgba(0, 0, 200, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list5, mode = 'lines+markers', name = 'Curved PNT - Above', marker = dict(color = 'rgba(100, 0, 100, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list6, mode = 'lines+markers', name = 'Curved PNT - Highest', marker = dict(color = 'rgba(200, 0, 200, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list7, mode = 'lines+markers', name = 'Phong - Above', marker = dict(color = 'rgba(100, 0, 0, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list8, mode = 'lines+markers', name = 'Phong - Highest', marker = dict(color = 'rgba(200, 0, 0, .8)')))

    layout = dict(title = 'Dragon - Continuous Error', titlefont=dict(family='Courier New, monospace',
        size=48,
        color='#7f7f7f'),
        xaxis = dict(title = 'Images x500', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        yaxis = dict(title = 'Perceptually different pixels', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        legend = dict(font=dict(family='sans-serif',
                        size=20,
                        color='#000')))


    data = []
    for index in range(0, len(avg_trace_list)):
        data.append(avg_trace_list[index])


    plotly.offline.plot({
    "data": data,
    "layout": Layout(layout)})



def pad_list(list, targetCount):
    for index in range(targetCount - len(list)):
        i = randint(int(0), len(list)-1)                           
        list.insert(i, list[i])
    return list

if techniqueToPlot == 9:

    list1 = phong_list
    list2 = phong_list1
    name1 = 'Dragon'
    name2 = 'Textured Man'
    technique = 'Phong Tessellation'

    biggest = len(list1)
    if len(list2) > biggest:
        biggest = len(list2)

    targetCount = biggest

    list1 = pad_list(list1, targetCount)
    list2 = pad_list(list2, targetCount)

    count = 2
    list1 = get_avg_list(list1, count)
    list2 = get_avg_list(list2, count)

    numbers = []
    i = 0
    while i < 150000:
        i = i + 1
        numbers.append(i)

    avg_trace_list = []
    avg_trace_list.append(Scatter(x=numbers, y=list1, mode = 'lines+markers', name = name1, marker = dict(color = 'rgba(0, 200, 0, .8)')))
    avg_trace_list.append(Scatter(x=numbers, y=list2, mode = 'lines+markers', name = name2, marker = dict(color = 'rgba(0, 0, 200, .8)')))

    layout = dict(title = technique + ' - Number of Triangles', titlefont=dict(family='Courier New, monospace',
        size=48,
        color='#7f7f7f'),
        xaxis = dict(title = 'Seconds', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        yaxis = dict(title = 'Frames', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        legend = dict(font=dict(family='sans-serif',
                        size=20,
                        color='#000')))


    data = []
    for index in range(0, len(avg_trace_list)):
        data.append(avg_trace_list[index])


    plotly.offline.plot({
    "data": data,
    "layout": Layout(layout)})



if techniqueToPlot == 10:

    model = sys.argv[2]
    level = ' level above'
    #level = ' highest level'

    list1 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output '+model+level+'/Static.txt')
    list2 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output '+model+level+'/Unpopping.txt')
    list3 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output '+model+level+'/Cpnt.txt')
    list4 = get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output '+model+level+'/Phong.txt')

    static = unpop = cpnt = phong = 0
    index = 0

    l = []
    while index < len(list1):
        temp = [list1[index], list2[index], list3[index], list4[index]]
        i = temp.index(min(temp))

        if i == 0:
            static += 1
            l.append('static')
            l.append('unpop')
        if i == 1:
            unpop += 1
            l.append('unpop')
        if i == 2:
            cpnt += 1
            l.append('cpnt')
        if i == 3:
            phong += 1
            l.append('phong')

        print 'done ' + unicode(index)
        
        index += 1 

    print static
    print unpop
    print cpnt
    print phong

    #l = [static, unpop, cpnt, phong]

    layout = dict(title = 'Title', titlefont=dict(family='Courier New, monospace',
            size=48,
            color='#7f7f7f'),
            xaxis = dict(title = 'xname', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                         titlefont=dict(family='Courier New, monospace',
                             size=30,
                             color='black'),
                         tickfont=dict(family='Old Standard TT, serif',
                            size=24,
                            color='black')),
            yaxis = dict(title = 'yname', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                         titlefont=dict(family='Courier New, monospace',
                             size=30,
                             color='black'),
                         tickfont=dict(family='Old Standard TT, serif',
                            size=24,
                            color='black')),
            legend = dict(font=dict(family='sans-serif',
                            size=20,
                            color='#000')))

    plotly.offline.plot({
    "data": 
    [Histogram(x = l, name = 'Dragon', marker = dict(color = 'rgba(0, 200, 0, .8)'))],

    "layout": Layout(layout)})


if techniqueToPlot == 11:

    #model = ' Man'
    model = ' Dragon'
    #level = ' level above'
    level = ' highest level'

    staticList = map(int, get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output'+model+level+'/Static.txt'))
    unpopList = map(int, get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output'+model+level+'/Unpopping.txt'))
    cpntList = map(int, get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output'+model+level+'/Cpnt.txt'))
    phongList = map(int, get_list('C:/Users/Jonas/Desktop/Level-of-Detail/Results/Output'+model+level+'/Phong.txt'))
    
    numbers = []
    i = 0
    while i < len(staticList):
        i = i + 1
        numbers.append(i)

    l1 = []
    l2 = []
    l3 = []
    l4 = []
    i = 0
    p = 65
    while i < 4225-p+1:
        temp1 = staticList[i:i+p]
        l1.append(max(temp1))

        temp2 = unpopList[i:i+p]
        l2.append(max(temp2))

        temp3 = cpntList[i:i+p]
        l3.append(max(temp3))

        temp4 = phongList[i:i+p]
        l4.append(max(temp4))

        i+=p


    trace_list = []
    trace_list.append(Scatter(x=numbers, y=l1, mode = 'lines+markers', name = 'Static LoD', marker = dict(color = 'rgba(0, 0, 200, .8)')))
    trace_list.append(Scatter(x=numbers, y=l2, mode = 'lines+markers', name = 'Unpopping LoD', marker = dict(color = 'rgba(204, 0, 204, .8)')))
    trace_list.append(Scatter(x=numbers, y=l3, mode = 'lines+markers', name = 'Curved PN Triangles', marker = dict(color = 'rgba(200, 0, 0, .8)')))
    trace_list.append(Scatter(x=numbers, y=l4, mode = 'lines+markers', name = 'Phong Tessellation', marker = dict(color = 'rgba(0, 200, 0, .8)')))

    layout = dict(title = 'Dragon - Highest level', titlefont=dict(family='Courier New, monospace',
        size=48,
        color='#7f7f7f'),
        xaxis = dict(title = 'Groups of images', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        yaxis = dict(title = 'Perceptually different pixels', linewidth = 0, linecolor = 'black', gridwidth = 2, gridcolor = 'grey', zerolinewidth = 4,
                        titlefont=dict(family='Courier New, monospace',
                            size=30,
                            color='black'),
                        tickfont=dict(family='Old Standard TT, serif',
                        size=24,
                        color='black')),
        legend = dict(font=dict(family='sans-serif',
                        size=20,
                        color='#000')))


    data = []
    for index in range(0, len(trace_list)):
        data.append(trace_list[index])

    plotly.offline.plot({
    "data": data,
    "layout": Layout(layout)})


if techniqueToPlot == 12:

    for i in range(1,4225):
        if 4225 % i == 0:
            print i


    