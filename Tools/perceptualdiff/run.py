from pdiffer import PDiffer
import os

def get_num(x):
    return int(''.join(ele for ele in x if ele.isdigit()))

pdiffer = PDiffer(bin='perceptualdiff')
content_list = []

#Get names of all files in the folder
for content in os.listdir("./Images"): # "." means current directory
    content_list.append(content)

#Open and erase the contents of the output text file
target = open('OutputTxt/out.txt', 'w')
target.truncate()

##Go through and compare all images in the folder. img0 compared to img1, img1 compared to img2 etc.
i = 0
while i < len(content_list) - 1:
    result = pdiffer.pdiff('Images/' + content_list[i], 'Images/' + content_list[i+1], threshold = 0)

    str = unicode(result)
    if not result:
    	print('PASS')
    else:
    	print('FAIL')
    	print(unicode(get_num(str)) + '\n')
    
    if result:
        target.write(unicode(get_num(str)) + '\n')
    else:
        target.write(unicode(0) + '\n')

    i = i + 1
    
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
