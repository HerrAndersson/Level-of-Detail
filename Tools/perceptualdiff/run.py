from pdiffer import PDiffer
import os

def get_num(x):
    return int(''.join(ele for ele in x if ele.isdigit()))

pdiffer = PDiffer(bin='perceptualdiff')

reference_list = []
#Get names of all reference images
for content in os.listdir('./Images/Reference'): # "." means current directory
    reference_list.append(content)

reference_list.sort();

#Get sub directories containing the images for each technique
dir_list = next(os.walk('./Images'))[1]
dir_list.remove('Reference')

for dir in dir_list:
    print(dir)

    image_list = []
    #Get names of all files in the folder
    for content in os.listdir('./Images/'+ dir): # "." means current directory
        image_list.append(content)
    
    image_list.sort();

    #Open and erase the contents of the output text file
    target = open('OutputTxt/' + dir + '.txt', 'w')
    target.truncate()
    
    #Go through and compare all images in the folder. Should be compared to the reference, which is rendered with the mesh of the level above
    i = 0
    while i < len(image_list):
    
        #Todo: Compare content_list with reference_list
        result = pdiffer.pdiff('Images/' + dir + '/' + image_list[i], 'Images/Reference/' + reference_list[i], threshold = 0)
    
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
