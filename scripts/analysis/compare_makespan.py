#!/usr/bin/env python

import sys
import getopt
import difflib
from scipy.stats import *

def parse( files, parse_zero = False ):
    makespans_dae = []
    makespans_jack = []

    for filename in files:
        try:
            f = open(filename, 'r')
        except IOError:
            print "Cannot open file", filename
            sys.exit(1)

        for line in f.readlines():
            # DAE
            # MakeSpan is for temporal problem, while TotalCost is for sequential ones
            if "MakeSpan" in line or "TotalCost" in line:
                try:
                    makespans_dae.append( float(line.split()[-1]) )
                except:
                    pass

            # Jack
            elif "PLAN" in line or "Makespan :" in line and not "BK" in line:
                makespans_jack.append( float(line.split()[-1]) )

            else:
                if parse_zero:
                    print "No makespan/cost found"

    return (makespans_dae, makespans_jack)


def get( func, a, b ):
    r = []
    try:
        r += [ func( a ) ]
    except:
        r += [ float('nan') ]

    try:
        r += [ func( b ) ]
    except:
        r += [ float('nan') ]

    r = [func.__name__] + r
    return tuple(r)


def stats_base( a, b, a_title="a", b_title="b" ):
    ab = [a,b]
    print "%15s\t%7s\t%7s" % ("STAT", a_title, b_title)
    fmt = "%15s\t%7.2f\t%7.2f"
    print fmt % get(len,a,b)
    print fmt % get(min,a,b)
    print fmt % get(median,a,b)
    print fmt % get(mean,a,b)
    print fmt % get(std,a,b)
    print fmt % get(skew,a,b)
    print fmt % get(kurtosis,a,b)

    try:
        na = normaltest( a )
        nb = normaltest( b )
        print fmt % ("normal test p", na[1], nb[1] )
    #except ValueError:
    #    print "ValueError while computing normal test"
    except:
        print "ERROR on normal test"

    print "Bayes confidence intervals (a center b)"
    
    ba = [( float('nan'),(float('nan'),float('nan') ) )]*3
    try:
        ba = bayes_mvs(a)
    except:
        pass

    bb = [( float('nan'),(float('nan'),float('nan') ) )]*3
    try:
        bb = bayes_mvs(b)
    except:
        pass

    print fmt % ( "on mean", ba[0][1][0], bb[0][1][0] )
    print fmt % ( " ",       ba[0][0],    bb[0][0]    )
    print fmt % ( " ",       ba[0][1][1], bb[0][1][1] )
    print fmt % ( "on var",  ba[1][1][0], bb[1][1][0] )
    print fmt % ( " ",       ba[1][0],    bb[1][0]    )
    print fmt % ( " ",       ba[1][1][1], bb[1][1][1] )
    print fmt % ( "on std",  ba[2][1][0], bb[2][1][0] )
    print fmt % ( " ",       ba[2][0],    bb[2][0]    )
    print fmt % ( " ",       ba[2][1][1], bb[2][1][1] )

#    print "histogram",a_title, histogram( a )
#    print "histogram",b_title, histogram( b )

def tests(a,b):
    print "wilcoxon=", wilcoxon(a,b)
    print "kruskal=", kruskal(a,b)


def parse_by_instances( files, instances = range(30) ):
#def parse_by_instances( files, instances = range(0,30) ):

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
        makespans = parse( instances_files )

        makespans_by_instances[i] = makespans

    return makespans_by_instances


def stats_by_instances_line( msi, func, a_title="DAE", b_title="JACK", sep=";" ):

    i_title="instance"

    # max size for space padding
    t_size = max( len(a_title), len(b_title), len(i_title) )
    fmt_title = "%"+str(t_size)+"s"+sep
    
    print "="*t_size,func.__name__,"="*t_size

    # on 8 characters, print floats with 2 digits precision
    fmt =  "%8.2f"+sep

    # first line : instances numbers
    print fmt_title % i_title,
    for i in msi:
        # print integer instead of float
        print fmt.replace("f","i") % i,
    print

    # second line: first table
    print fmt_title % a_title,
    for i in msi:
        ms_a,ms_b = msi[i]
        try:
            print fmt % func(ms_a),
        except:
            print fmt.replace("f","s") % "?",
    print
    
    # third line: second table
    print fmt_title % b_title,
    for i in msi:
        ms_a,ms_b = msi[i]
        try:
            print fmt % func(ms_b),
        except:
            print fmt.replace("f","s") % "?",
    print

def parse_other_solver( file ):

    f = open( file )

    dic = {}

    for line in f.readlines():
        l = line.split(";")
        name = l[0]
        v = l[1:]
        print name,v
        vals = [ float(i) for i in v ]

        dic[name] = vals

    return dic



def stats_by_instances_column( msi, func, a_title="DAE", b_title="JACK", solvers={} ):

    print "="*15,func.__name__,"="*15

    fmt = "%5s %10s %10s %5s"
    print fmt % ("inst.",a_title,b_title,"best?"),
    for s in solvers:
        print "%10s " % s,
    print

    for i in msi:
        ms_a,ms_b = msi[i]

        try:
            a = func(ms_a)
        except:
            a = float('nan')

        try:
            b = func(ms_b)
        except:
            b = float('nan')

        if a < b:
            mark = "*"
        else:
            mark = " "

        print fmt % (i,a,b,mark),

        for s in solvers:
            print "%10s " % solvers[s][i-1],

        print


def stats_by_instance( msi, a_title="a", b_title="b" ):
    for i in msi:
        ms_a,ms_b = msi[i]

        print "="*15,i,"="*15
        stats_base( ms_a, ms_b, a_title, b_title )


def matching_blocks( s1, s2 ):

    sm = difflib.SequenceMatcher( None, s1, s2 )

    res = ""
    for m in sm.get_matching_blocks():
        res += s1[ m[0] : m[0]+m[2] ] + "*"

    return res


def files_common_match( files ):   
    
    if len(files) == 0:
        print "Cannot find any file to parse"
        sys.exit(1)

    return reduce( matching_blocks, files )


def plot_histo( a, b, title="Distribution", a_title="DAE", b_title="JACK"):
    import numpy as np
    import pylab as p

    fig = p.figure()
    ax = fig.add_subplot(111)

    n, bins, patches = ax.hist( [a,b], label=(a_title,b_title) )

    ax.legend()
    ax.set_title(title)
    ax.set_ylabel("Number of occurences")
    ax.set_xlabel("Makespan/Cost")

    p.show()


if __name__ == "__main__":

    import optparse
    import os

    parser = optparse.OptionParser()

    parser.add_option("-a", "--abstract", dest="abstract", action="store_true",
            help="print an abstract of several statistics" )

    parser.add_option("-g", "--hist", dest="hist", default=False,
            help="number of instances to check" )

    parser.add_option("-i", "--instances", dest="by_instances", action="store_true",
            help="print abstracts, one for each instances" )

    parser.add_option("-b", "--best", dest="best_line", action="store_true",
            help="single stats on one line, each instances by column" )

    parser.add_option("-c", "--column", dest="best_column", action="store_true",
            help="single stats on one column, each instances by rows" )

    parser.add_option("-r", "--range", dest="range", default=30,
            help="number of instances to check" )

    (opts, files) = parser.parse_args()
    files_match = files_common_match( files )
    print files_match

    # abstract
    if opts.abstract:
        dae,jack = parse( files )
        stats_base( dae, jack, "DAE", "JACK" )
        #tests( dae, jack )
        #plot_histo(dae,jack)

    if opts.hist:
        dae,jack = parse(files)
        title = files_match
        plot_histo( dae, jack, title, "DAE ("+str(len(dae))+")", "JACK ("+str(len(jack))+")" )

    # sort files by instances (use the "p01" pattern of IPC 2008)
    if opts.by_instances:
        msi = parse_by_instances( files, range(1,opts.range) )
        stats_by_instance( msi, "DAE", "JACK" )

    # bests results only
    if opts.best_line:
        msi = parse_by_instances( files, range(1,opts.range) )
        stats_by_instances_line( msi, min )
        #stats_by_instances_line( msi, median )

    if opts.best_column:
        msi = parse_by_instances( files[0:-1], range(1,31) )
        solvers = parse_other_solver( files[-1] )
        stats_by_instances_column( msi, min, "DAE", "JACK", solvers )
        #stats_by_instances_column( msi, min, "DAE", "JACK", {} )

