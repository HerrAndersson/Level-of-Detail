import plotly
from plotly.graph_objs import Scatter, Layout

#https://plot.ly/python/line-charts/

#Read output files and generate a plot with all techniques
plotly.offline.plot
({

"data": [Scatter(x=xl, y=yl)],
"layout": Layout(title="hello world" )

#layout = dict(title = 'Average High and Low Temperatures in New York',
#              xaxis = dict(title = 'Temperature (degrees F)'),
#              yaxis = dict(title = 'Month'),
#              )

})
