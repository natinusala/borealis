import json
import xml.etree.ElementTree as ET

# Function from https://stackoverflow.com/a/65808327
def _pretty_print(current, parent=None, index=-1, depth=0):
    for i, node in enumerate(current):
        _pretty_print(node, current, i, depth + 1)
    if parent is not None:
        if index == 0:
            parent.text = '\n' + ('\t' * depth)
        else:
            parent[index - 1].tail = '\n' + ('\t' * depth)
        if index == len(parent) - 1:
            current.tail = '\n' + ('\t' * (depth - 1))

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

    # Pretty-print
    _pretty_print(root)

    tree = ET.ElementTree(root)
    tree.write(pathToSaveUnder)