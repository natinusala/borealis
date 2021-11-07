import json
import xml.etree.ElementTree as ET

def processFile(data: dict, root: ET.Element):
    keys = data.keys()

    for x in keys:
        if type(data[x]) == dict:
            lis = ET.SubElement(root, "brls:List", {"name": x})
            processFile(data[x], lis)
        else:
            ET.SubElement(root, "brls:String", {"name":x}).text = data[x]

fileToConvert   = input("Please enter the path to the file that needs converting: ")
pathToSaveUnder = input("Please enter the path of where the final converted file will go: ")

with open(fileToConvert, "r") as json_f:
    data = json.load(json_f)
    root = ET.Element("brls:i18nDoc")

    # Loop through the entire JSON file
    processFile(data, root)

    tree = ET.ElementTree(root)
    tree.write(pathToSaveUnder)