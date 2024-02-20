
import os
from os import listdir
from os.path import isfile, join


def get_file_list(path):
    file_list = [path+f for f in listdir(path) if isfile(join(path, f))]
    return file_list

def rename_files(file_list):
    for file in file_list:
        file_name = str(file)
        if file_name.find("Декан") != -1:
            file_name = file_name.replace("Декан", "Decan")       
        if file_name.find(" расход ") != -1:
            file_name = file_name.replace(" расход ", "_")       
        os.rename(file, file_name)
    return
