#!/bin/env python

import sys
import math

def parse( files, markers = ["PLAN"] ):
#def parse( files, markers = ["MakeSpan","TotalCost"] ):

    makespans = []

    for filename in files:
        try:
            f = open(filename, 'r')
        except IOError:
            print "Cannot open file", filename
            sys.exit(1)

        result_found = False

        for line in f.readlines():
            # DAE
            # MakeSpan is for temporal problem, while TotalCost is for sequential ones
            for m in markers:

                if m in line:
                    try:
                        makespans.append( float(line.split()[-1]) )
                        result_found = True
#                        print "Found",makespans[-1]
                        break
                    except:
                        pass

                if result_found:
                    break

        if not result_found:
            makespans.append( 0 )
#            print "Not found"

    return makespans



def parse_by_instances( files, instances = range(31), markers = ["PLAN"] ):

    makespans_by_instances = {} #[[]] * len(instances)

    # search for those isntances
    makespans = ()
    for i in instances:
        # pattern to search for
        # 01 .. 30
        pattern = "p"+str(i).zfill(2)

        found = False
        # search in the given files
        instances_files = []
        for file in files:
            if pattern in file:
                found = True
                instances_files.append( file )

        # get makespans for DAE or JACK from the files matching this instance
        makespans_by_instances[i] = parse( instances_files, markers )

    return makespans_by_instances


def csv_list( table, head, mark="; " ):

    return head + mark + mark.join( [str(i) for i in table] )



def func_by_instance( func, dico ):
   
    res = []

    for k in dico:
        res.append( func( dico ) )

    return res


def parse_csv( files, marker=";" ):

    res = {} # res[solver] = [i1,i2...]

    for filename in files: 
        try:
            f = open(filename, 'r')
        except IOError:
            print "Cannot open file", filename
            sys.exit(1)


        for line in f.readlines():

            data_str = line.split(marker)
            solver = data_str[0]
            # floor, because TFD output floats
            data = [ math.floor( float(i) ) for i in data_str[1:] ]
            #data = [ int( math.floor( float(i) ) ) for i in data_str[1:] ]

            res[solver]= data

            # FIXME verifier que tout les data ont la meme taille

    return res


def solved_instances( solvers ):

    res = {}
    for s in solvers:
        nb_solved = 0
        for i in solvers[s]:
            if i != 0.0:
                nb_solved += 1

        res[s] = nb_solved

    return res


def quality( solvers ):

    tab_len = sys.maxint

    # find the minimum number f instances on which we can compare
    for s in solvers:
        if len(solvers[s]) < tab_len:
            tab_len = len( solvers[s] )
            print tab_len,"considered instances"

    # find the minimums for each instances across solvers
    min_i = []
    for i in range( tab_len ):
        tab = []
        for s in solvers:
            if solvers[s][i] != 0:
                tab.append( solvers[s][i] )

        if len(tab) == 0: # nobody managed to solve this instance
            min_i.append( 0.0 )
        else:
            min_i.append( min( tab ) )

    # compute the quality
    quality_si = {}
    for s in solvers:
        for i in range( tab_len ):

            # we have already seen this solver
            if quality_si.has_key(s):
                # no division by zero
                if solvers[s][i] != 0:
                    # add
                    quality_si[s] += min_i[i] / solvers[s][i]

            # first time we see this solver
            else:
                if solvers[s][i] != 0:
                    # initialize
                    quality_si[s] = min_i[i] / solvers[s][i]
                else:
                    quality_si[s] = 0


    # FIXME divide by nb of runs or not?
    return quality_si



def QC_ratio( quality, coverage ):

    assert( len(coverage) == len(quality) )

    res = {}
    for s in quality:
        res[s] = quality[s] / coverage[s] * 100

    return res



def references( solvers, instances_nb = 30 ):
    # solvers[solver] = [ value_i1, value_i2, ... ]
    refs = []

    for i in range( instances_nb ):
        min = sys.maxint
        solvers_rep = "NONE"

        for solver in solvers:
            value = solvers[solver][i]

            if value < min and value != 0:
                min = value
                solvers_rep = solver

            elif value == min and value != 0:
                solvers_rep += "/" + solver

            else:
                pass

        refs.append( (min,solvers_rep) )


    return refs





def print_column( solvers, head="="*10 ):

    print "="*10,head,"="*10

    fmt_s = "%10s "
    fmt_f = "%10.2f "

    tab_lens = []

    for s in solvers:
        print fmt_s % s,
    print

    for s in solvers:
        print fmt_f % solvers[s],
    print




def print_column_instances( solvers, head="="*10 ):

    print "="*10,head,"="*10

    fmt_s = "%10s "
    fmt_f = "%10.2f "

    tab_lens = []
    tab_len = 0

    try:
        for s in solvers:
            tab_lens.append( len( solvers[s] ) )

    except TypeError: # len() of unsized object
        print "Solvers[s] is not a list"
        return
    
    if len(tab_lens) != 0:
        tab_len = min(tab_lens)
        print tab_len,"items used over",max(tab_lens)

    for s in solvers:
        print fmt_s % s,
    print

    for i in range( tab_len ):
        for s in solvers:
            print fmt_f % solvers[s][i],
        print
        


if __name__ == "__main__":

    import optparse
    parser = optparse.OptionParser()

    parser.add_option("-c", "--create-csv", dest="create_csv", action="store_true",
            help="Create a CSV file, one line per solver, one column per instance" )

    parser.add_option("-n", "--name", dest="name", default="DAE",
            help="Default name for the data being parsed" )

    parser.add_option("-r", "--reference", dest="reference", action="store_true",
            help="Create a reference file from a list of CSV with solvers data (warning: indicate only files from the same domain!)" )

    (opts, files) = parser.parse_args()

    if opts.create_csv:
        makespans = parse_by_instances( files, range(31), ["PLAN"] )

        mins = []
        for i in makespans:

            if len( makespans[i] ) == 0:
                mins.append( 0 )
                continue

            mins.append( min( [m for m in makespans[i] if m!=0] ) )


        assert( len(mins) == len(makespans) )

        print csv_list( mins, opts.name )

    elif opts.reference:
        dico = parse_csv( files )
    
        refs = references( dico )

        print "# References"
        print "# Value\tSolver"
        for val,solver in refs:
            print "%s\t%s" % (val,solver)


    else:
        dico = parse_csv( files )
        
        quality = quality( dico )
        print_column( quality, "Quality" )

        coverage = solved_instances( dico )
        print_column( coverage,"Coverage" )
        
        qc = QC_ratio( quality, coverage )
        print_column( qc, "Q/C ratio" )
        
