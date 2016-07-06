from pdiffer import PDiffer
import os
import threading
import re

def get_num(x):
    return int(''.join(ele for ele in x if ele.isdigit()))

def get_first_num(x):
    return int(re.findall('\d+',x)[0])

def get_last_num(x):
    return int(re.findall('\d+',x)[-1])

def compare_first(item1, item2):
    if get_first_num(item1) < get_first_num(item2):
        return -1
    elif get_first_num(item1) > get_first_num(item2):
        return 1
    else:
        return 0

def compare_last(item1, item2):
    if get_last_num(item1) < get_last_num(item2):
        return -1
    elif get_last_num(item1) > get_last_num(item2):
        return 1
    else:
        return 0

#Handles the pop-comparison for static, cpnt, and phong
def handle_dir(dirName, ref_list):

    dir_list = next(os.walk('C:/Users/Jonas/Desktop/ManPop/' + dirName))[1]
    dir_list.sort(cmp = compare_last)

    for dir in dir_list:
        image_list = []
        #Get names of all files in the folder
        for content in os.listdir('C:/Users/Jonas/Desktop/ManPop/' + dirName + '/' + dir):
            image_list.append(content)
    
        image_list.sort(cmp = compare_first)

        #Open and erase the contents of the output text file
        target = open('OutputTxtMan/' + dirName + dir + '.txt', 'w')
        target.truncate()

        #Go through and compare all images in the folder to the reference,
        #which is rendered with the mesh of the level above
        i = 0
        while i < len(image_list):
    
            result = pdiffer.pdiff('C:/Users/Jonas/Desktop/ManPop/' + dirName + '/' + dir + '/' + image_list[i], 'C:/Users/Jonas/Desktop/ManPop/' + dirName + '/' + dir + '/' + image_list[i + 1], threshold = 0, fov = 70)
    
            str = unicode(result)
            if not result:
                print('PASS ' + image_list[i] + ' VS ' + image_list[i + 1])
            else:
                print('FAIL ' + image_list[i] + ' VS ' + image_list[i + 1])
                print(unicode(get_num(str)) + '\n')

            if result:
                target.write(unicode(get_num(str)) + '\n')
            else:
                target.write(unicode(0) + '\n')
    
            i = i + 2
       
        target.close()

######################## Start of program ########################

pdiffer = PDiffer(bin='perceptualdiff')

###################### Static, Cpnt, Phong #######################
#Get sub-directories containing the images for each technique
dir_list = next(os.walk('C:/Users/Jonas/Desktop/ManPop'))[1]
dir_list.remove('Unpopping')

for dir in dir_list:
    t = threading.Thread(target = handle_dir, args = (dir, dir_list))
    t.start()


########################### Unpopping ############################
dir_list = next(os.walk('C:/Users/Jonas/Desktop/ManPop/Unpopping'))[1]
dir_list.sort(cmp = compare_last)

for dir in dir_list:
    subdir_list = next(os.walk('C:/Users/Jonas/Desktop/ManPop/Unpopping/' + dir))[1]
    subdir_list.sort(cmp = compare_last)

    #Holds results of folders 1,2,3,4 in every Run# directory.
    dir_result_list = []

    for subdir in subdir_list:
        image_list = []
        #Get names of all files in the folder
        for content in os.listdir('C:/Users/Jonas/Desktop/ManPop/Unpopping/' + dir + '/' + subdir):
            image_list.append(content)
    
        image_list.sort(cmp = compare_first)

        #Go through and compare all images in the folder to the reference,
        #which is rendered with the mesh of the level above
        i = 0

        result_list = []
        while i < len(image_list)-1:
   
            result = pdiffer.pdiff('C:/Users/Jonas/Desktop/ManPop/Unpopping/' + dir + '/' + subdir + '/' + image_list[i], 'C:/Users/Jonas/Desktop/ManPop/Unpopping/' + dir + '/' + subdir + '/' + image_list[i + 1], threshold = 0, fov = 70)
    
            str = unicode(result)
            if not result:
                print('PASS ' + image_list[i] + ' VS ' + image_list[i + 1])
            else:
                print('FAIL ' + image_list[i] + ' VS ' + image_list[i + 1])
                print(unicode(get_num(str)) + '\n')

            if result:
                result_list.append(int(get_num(str)))
            else:
                result_list.append(int(0))
    
            i = i + 1
       
        dir_result_list.append(result_list)

    #Output the dir_result_list, which holds all results of folders 1,2,3,4
    #Open and erase the contents of the output text file

    avg = open('OutputTxtMan/Unpopping' + dir + '_avg.txt', 'w')
    avg.truncate()
    min = open('OutputTxtMan/Unpopping' + dir + '_min.txt', 'w')
    min.truncate()
    max = open('OutputTxtMan/Unpopping' + dir + '_max.txt', 'w')
    max.truncate()

    for pop_result in dir_result_list:
        pop_result.sort()
        avgv = 0
        for v in pop_result:
            avgv = avgv + v;
        avg.write(unicode(avgv/len(pop_result))+ '\n')
        min.write(unicode(pop_result[0]) + '\n')
        max.write(unicode(pop_result[-1])+ '\n')

    avg.close()
    min.close()
    max.close()
