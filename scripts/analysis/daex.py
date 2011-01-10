#!/bin/env python
#encoding=utf-8

import os
import sys
import difflib
import fnmatch
import copy

import scipy
from scipy.stats import *

# Colors

csi = "\033[%im"
col = {
      "reset" : csi % 0,
      "red"   : csi % 91,
      "green" : csi % 92,
      "yellow": csi % 93,
      "gray"  : csi % 90,
      "cyan"  : csi % 94
      }



######################################
# GENERATORS
######################################


def unstr( func ):
    """Decorator that convert a str argument in a [str], for generators"""
    def wrapper( monad ):
        if isinstance( monad, str ):
            return func( [ monad ] )
        else:
            return func( monad )

    return wrapper

@unstr
def expand( filepatterns ):
    """Yelds file names matching each 'filepatterns'"""
#    filepatterns = degen( filepatterns )
    for base_dir,filepattern in dir_file(filepatterns):
        for dirname, dirs, files in os.walk( base_dir ):
            for filename in fnmatch.filter(files, filepattern):
                yield os.path.join(dirname, filename)

@unstr
def fopen( filenames):
    """Yields every files corresponding tothe given filenames"""
    for filename in filenames:
        yield open(filename)

@unstr
def cat( sources ):
    """Yields every lines of given streams"""
    for src in sources:
        for line in src:
            yield line

@unstr
def dir_file( paths ):
    """Yields the file name and its absolute path in a tuple, expand home and vars if necessary"""
    for path in paths:
        p = os.path.abspath( path )
        yield ( os.path.dirname(p),os.path.basename(p) )
    
def grep( strings, pattern ):
    """Yields given strings if they match a given pattern"""
    for string in strings:
        if pattern in string:
            yield string

def cut( strings, field, delimiter=None ):
    """Yields the field-th item of the strings splited with delimiter, if delimiter is None, any whitespace-like character is used to split."""
    for string in strings:
        if delimiter is None:
            yield string.split()[field]
        else:
            yield string.split(delimiter)[field]



######################################
# REDUCERS
######################################

def matching_blocks( s1, s2 ):
    """Return a string made of the common characters between s1 and s2 adds '*' for locations that differs"""
    sm = difflib.SequenceMatcher( None, s1, s2 )
    res = "*"
    mb = sm.get_matching_blocks()
    for m in mb[:len(mb)-1]:
        res += s1[ m[0] : m[0]+m[2] ] + "*"
    return res


def differing_blocks( s1, s2 ):
    """Return a string made of the parts of s1 that differs between s1 and s2"""
    sm = difflib.SequenceMatcher( None, s1, s2 )
    res = "_"
    mb = sm.get_matching_blocks()
    for i in xrange( len(mb[1:]) ):
        res += s1[ mb[i-1][0] + mb[i-1][2] : mb[i][0] ] + "_"
    return res


def different_characters( base, strings ):
    """Returns the characters of base that differs from all the strings in the list, add '_' for locations that match"""
    if len(strings) == 0:
        return ""
    diffs = map( lambda x : differing_blocks(base,x), strings )
    return common_characters(diffs).replace('_','')


def common_characters( strings ):   
    """Returns the common characters of a list of strings, add '*' for locations that differs"""
    if len(strings) == 0:
        return ""
    return reduce( matching_blocks, strings )


######################################
# UTILS
######################################

def lines( pattern ):
    """Open all files matching the given pattern and yields their descriptors"""
    files = expand( pattern )
    fds = fopen(files)
    return cat( fds )

def parse( pattern, keyword = "MakeSpan" ):
    """Open files matchin pattern, filter the lines matching the keyword and returns the list of the last item of each line, converted in float"""
    rawlines = lines(pattern)
    greped = grep( rawlines, keyword )
    strs = cut( greped, -1 ) # -1 = the last item

    # NOTE scipy 0.6 does not handle generators, thus we must expand it in a list
    return [ float(i) for i in strs ]


def parse_func( filenames, functions, keyword = "MakeSpan", labels=False ):
    """Call a set of functions on the parsed data and build a table with the results, 
    if label==True, the labels are part of the table and every item is a str, else a float."""
    tab = []
    trow = ['input']

    if labels:
        for f in functions:
            trow.append( f.__name__ )
        tab.append(trow)

    for pattern in filenames:
        makespans = parse( pattern, keyword )

        trow = []
        if labels:
            trow.append(pattern)

        for f in functions:
            trow.append( f( makespans ) )
        tab.append(trow)

    return tab


def printas( tab, transpose=False):
    """Print a table using scipy formating"""
    a = scipy.array(tab)
    if transpose:
        a = a.transpose()

    print scipy.array2string(
            a,
            separator=" "
        )#.replace("["," ").replace("]"," ")


def len_max( tab ):
    """Returns the max length of string representations of items of a 1D table"""
    max = 0
    for i in tab:
        if len(str(i)) > max:
            max = len(i)
    return max


def len_max_col( tab, j ):
    """Returns the max length of string representations of items of the j-th column of a 2D table"""
    max = 0
    for i in xrange(len(tab)):
        if len(str(tab[i][j])) > max:
            max = len(str(tab[i][j]))
    return max


def printa( tab, transpose=False ):
    """Pretty print a 2D table"""
    a = scipy.array(tab)
    if transpose:
        a = a.transpose()

    nrows = a.shape[0]
    ncols = a.shape[1]

    for i in xrange(nrows):
        for j in xrange(ncols):
            isize = len_max_col(a,j)
            fmt_s = "%"+str(isize)+"s "
            fmt_f = "%"+str(isize)+".2f "
            
            try:
                print fmt_f % float(a[i,j]),
            except:
                print fmt_s % a[i,j],
        print


def plot_histo( tab, title="Distribution", labels=[]):
    import numpy as np
    import pylab as p

    fig = p.figure()
    ax = fig.add_subplot(111)

    n, bins, patches = ax.hist( tab, label=labels )

    ax.legend()
    ax.set_title(title)
    ax.set_ylabel("Number of occurences")
    ax.set_xlabel("Makespan/Cost")

    p.show()


######################################
# COMMANDS
######################################

def basestats( filenames, key ):
    # base statistics
    stats = [len,min,median,mean,std,skew,kurtosis]
    tab = parse_func( filenames, stats, key, labels=True ) 
    printa(tab,transpose=False)
    

def plotdistrib( filenames, key ):
    # plot the distributions
    tab = []
    for pattern in filenames:
        tab.append( parse( pattern, key ) )

    plot_histo( tab, common_characters(filenames), labels=filenames )


def compare_median( filenames, key, p_thresh = 0.95 ):
    ms = []
    for pattern in filenames:
        ms.append( (pattern, parse( pattern, key ) ) )

    for i in xrange(len(ms)):
        for j in xrange(i,len(ms)):
            if i != j:

                fn = copy.deepcopy(filenames)
                fn_i = fn.pop(i)
                repr_i = different_characters( fn_i, fn )
                fn = copy.deepcopy(filenames)
                fn_j = fn.pop(j)
                repr_j = different_characters( fn_j, fn )

                # Calculates the Wilcoxon signed-rank test 
                # for the null hypothesis that two samples come from the same distribution. 
                # A non-parametric T-test. (need N > 20)
                p_value = wilcoxon(ms[i][1], ms[j][1])[1]

                median_i = median( ms[i][1] )
                median_j = median( ms[j][1] )
                if median_i < median_j and p_value > p_thresh:
                    print col["green"],repr_i,col["reset"],col["red"],"<",col["reset"],repr_j,
                    print "%8.2f" % p_value
                elif median_i > median_j and p_value > p_thresh:
                    print "[",col["green"],repr_j,col["reset"],col["yellow"],"<",col["reset"],repr_i,
                    print "%8.2f" % p_value
                elif median_i == median_j and p_value > p_thresh:
                    print repr_i,col["cyan"],"=",col["cyan"],repr_j,
                    print "%8.2f" % p_value
                else:
                    print repr_i,col["gray"],"~",col["reset"],repr_j,
                    print "%8.2f" % p_value



def compare( filenames, key, p_thresh = 0.95 ):
    ms = []
    for pattern in filenames:
        ms.append( (pattern, parse( pattern, key ) ) )

    print "WILCOXON:\td1 VS d2\tmin_i<=>min_j\tmedian_i<=>median_j\tp-val\tbest"

    for i in xrange(len(ms)):
        for j in xrange(i,len(ms)):
            if i != j:

                fn = copy.deepcopy(filenames)
                fn_i = fn.pop(i)
                repr_i = different_characters( fn_i, fn )
                fn = copy.deepcopy(filenames)
                fn_j = fn.pop(j)
                repr_j = different_characters( fn_j, fn )

                # Calculates the Wilcoxon signed-rank test 
                # for the null hypothesis that two samples come from the same distribution. 
                # A non-parametric T-test. (need N > 20)
                p_value = wilcoxon(ms[i][1], ms[j][1])[1]

                print "\t%s VS %s" % ( repr_i, repr_j ),"\t",

                min_i = min( ms[i][1] )
                min_j = min( ms[j][1] )
                print "%i" % min_i,
                if min_i < min_j:
                    print "<",
                elif min_i > min_j:
                    print ">",
                else:
                    print "=",
                print "%i" % min_j,"\t",

                median_i = median( ms[i][1] )
                median_j = median( ms[j][1] )
                print "%i" % median_i,
                if median_i < median_j:
                    print "<",
                elif median_i > median_j:
                    print ">",
                else:
                    print "=",
                print "%i" % median_j,

                if median_i < median_j and p_value > p_thresh:
                    best = repr_i
                elif median_i > median_j and p_value > p_thresh:
                    best = repr_j
                else:
                    best = "None"

                print "%8.2f" % p_value,"\t",
                print best
               
                #print kruskal(ms[0], ms[1]) # FIXME error from scipy
    
    


if __name__=="__main__":

    import optparse
    import os

    parser = optparse.OptionParser("""daex.py [options] file_patterns
Parse the set of file patterns given in entry and apply various functions to it.
The file patterns must be given in double quotes.
Example:
    daex.py --seq --basestats "plan_1_r*.soln" "plan_2_r*.soln" """)

    parser.add_option("-s", "--seq", dest="seq", action="store_true",
            help="sequential problem (default to temporal)" )

    parser.add_option("-f", "--files", dest="files", action="store_true",
            help="list the files that match the given patterns" )

    parser.add_option("-b", "--basestats", dest="basestats", action="store_true",
            help="display basic statistics" )

    parser.add_option("-d", "--plotdistrib", dest="plotdistrib", action="store_true",
            help="plot as histograms" )

    parser.add_option("-c", "--compare", dest="compare", action="store_true",
            help="compare distributions" )

    (opts, filenames) = parser.parse_args()

    key="MakeSpan"
    if opts.seq:
        key="TotalCost"
 
    if opts.files:
        efiles = []
        for pattern in filenames:
            dir,file = dir_file( pattern ).next()
            print dir,":",file
        
            for f in expand( pattern ):
                print "\t",os.path.basename(f)
                efiles.append( f )

        print common_characters(efiles)

    if opts.basestats:
        basestats(filenames,key)

    if opts.compare:
        compare_median(filenames,key)

    if opts.plotdistrib:
        plotdistrib(filenames,key)

