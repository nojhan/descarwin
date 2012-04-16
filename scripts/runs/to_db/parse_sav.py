import re
from itertools import chain
import json


def parse_Parser( content ):
    #print("Parser")
    parser = {}
    for line in content.split("\n"):
        found = re.match("^[#\s]*--([-\w]+)=(\S+)\s+.*$", line)
        if found:
            opt,val = found.groups()
            parser[opt] = val
            #print(opt,val)

    return parser


def parse_eoPop( content ):
    #print("eoPop")
    
    pop = []
    for line in content.split("{"):
        if "}" in line:
            sline = line.replace("\n","") # FIXME after that, we need to reformat plans
            pop.append( json.loads("{"+sline) )

    return pop


def parse_sav( filename, section_mark_start = "\\section{", section_mark_end = "}"):

    res = {}

    # Read everything
    with open(filename) as fd:
        data = fd.read()

    sections = data.split( section_mark_start )

    for section in sections:
        if section == "":
            continue

        end = section.find( section_mark_end )
        name = section[:end]
        content = section[end+1:]

        # Call the corresponding parser
        func = globals()["parse_"+name]
        parsed = func( content )

        res[name] = parsed

    return res


if __name__ == "__main__":
    import sys
    data = parse_sav(sys.argv[1])
    print(len(data["Parser"]),"options")
    print(len(data["eoPop"]),"decompositions")

