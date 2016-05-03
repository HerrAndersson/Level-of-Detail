from pdiffer import PDiffer
import os

pdiffer = PDiffer(bin='perceptualdiff')

def get_num(x):
    return int(''.join(ele for ele in x if ele.isdigit()))


content_list = []

for content in os.listdir("./Images"): # "." means current directory
    content_list.append(content)

i = 0
while i < len(content_list):
    print(content_list[i])
    print(content_list[i+1])
    i = i + 2

result = pdiffer.pdiff('Images/log.png', 'Images/dog.png')

str = unicode(result)

#open and erase the contents of the file
target = open('OutputTxt/out.txt', 'w')
target.truncate()

if not result:
	print('PASS')
else:
	print('FAIL')
	print(get_num(str))

if result:
    target.write(unicode(get_num(str)))
else:
    taget.write(unicode(0))

target.close()

#pdiff('image1.png', 'image2.png',
#    fov=65,
#    threshold=40,
#    gamma=2.2,
#    luminance=100,
#    luminanceonly=True,
#    colorfactor=1.0,
#    downsample=0,
#    output='o.ppm')
