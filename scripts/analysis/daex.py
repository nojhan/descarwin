#!/bin/env python
#encoding=utf-8

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

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
    if len(filepatterns) == 0:
        print "EMPTY PATTERN"
        yield (i for i in [] )

    for base_dir,filepattern in dir_file(filepatterns):
        for dirname, dirs, files in os.walk( base_dir ):
            for filename in fnmatch.filter(files, filepattern):
                yield os.path.join(dirname, filename)

@unstr
def fopen( filenames):
    """Yields every files corresponding to the given filenames"""
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
#    print "PATTERN",pattern
#    if pattern == "":
#        print "WARNING empty pattern"
#        return ( i for i in [] ) # empty generator

    files = expand( pattern )
    fds = fopen(files)
    return cat( fds )

def parse( pattern, keyword = "MakeSpan" ):
    """Open files matchin pattern, filter the lines matching the keyword and returns the list of the last item of each line, converted in float"""
    if pattern == "":
#        print "WARNING empty pattern"
        return []

    rawlines = lines(pattern)
    greped = grep( rawlines, keyword )
    strs = cut( greped, -1 ) # -1 = the last item

    # NOTE scipy 0.6 does not handle generators, thus we must expand it in a list
    res = [ float(i) for i in strs ]

    if len(res) == 0:
        print "ERROR cannot find the \"",keyword,"\" keyword in files"
        sys.exit(1)
    else:
        return res


def parse_func( filenames, functions, keyword = "MakeSpan", labels=False ):
    """Call a set of functions on the parsed data and build a table with the results, 
    if label==True, the labels are part of the table and every item is a str, else a float."""
    tab = []
    trow = ['input']

    if labels:
        for f in functions:
            trow.append( f.__name__ )
        tab.append(trow)

    fnames = []
    for p in filenames:
        fnames.append( p )

    for i in xrange(len(fnames)):
        fn = copy.deepcopy( fnames )
        p_i = fn.pop( i )
        # FIXME BUG
        #name = different_characters( p_i, fn )
        name = p_i

        makespans = parse( fnames[i], keyword )

        trow = []
        if labels:
            trow.append(name)

        for f in functions:
            if len(makespans) != 0:
                trow.append( f( makespans ) )
            else:
                trow.append( None )
        tab.append(trow)

    return tab


def printas( tab, transpose=False):
    """Print a table using scipy formating"""
    a = toarray(tab)
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


def toarray( tab ):
    """Convert a list of list into a scipy array, filling with zero when values are missing"""
    isize = len(tab)

    jsize = 0
    for row in tab:
        jsize = max( len(row), jsize )

    a = scipy.zeros( (isize, jsize) )
    for i in xrange(len(tab)):
        for j in xrange(len(tab[i])):
            a[i,j] = tab[i][j]

    return a


def tomat( tab ):
    """Convert a list of list into a square list of list with padding "None", filling with zero when values are missing"""
    isize = len(tab)

    jsize = 0
    for row in tab:
        jsize = max( len(row), jsize )

    a = []
    for i in xrange(len(tab)):
        row = []
        for j in xrange(len(tab[i])):
            row.append( tab[i][j] )
        for j in xrange(len(tab[i]), jsize):
            row.append( None )

        a.append(row)

    return a



def transposemat( mat ):
    nrows= len(mat)
    ncols = len(mat[0]) # must be equals

    tmat = []
    for j in xrange(ncols):
        row = []
        for i in xrange(nrows):
            row.append( mat[i][j] )
        tmat.append(row)

    return tmat



def printa2( tab, transpose=False ):
    """Pretty print a 2D table"""

    a = tomat(tab)

    if transpose:
        a = transposemat(a)

    nrows = len(a)
    ncols = len(a[0]) # all rows have the same size, thanks to totab

    for i in xrange(nrows):
        for j in xrange(ncols):
            isize = len_max_col(a,j)
            fmt_s = "%"+str(isize)+"s "
            fmt_f = "%"+str(isize)+".2f "
            
            try:
                print fmt_f % float(a[i][j]),
            except:
                print fmt_s % a[i][j],
        print



def printa( tab, transpose=False ):
    """Pretty print a 2D table"""

    a = toarray(tab)

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
    import pylab as p

    fig = p.figure()
    ax = fig.add_subplot(111)

    n, bins, patches = ax.hist( tab, label=labels )

    ax.legend()
    ax.set_title(title)
    ax.set_ylabel("Number of occurences")
    ax.set_xlabel("Makespan/Cost")

    p.show()


def plot_line( tab, title="", labels=[], ylabel="", xlabel=""):
    import pylab as p

    fig = p.figure()
    ax = fig.add_subplot(111)
    
    for i in xrange(len(tab)):
        ax.plot( tab[i], 'o-', label=labels[i] )

    ax.legend()
    ax.set_title(title)
    ax.set_ylabel(ylabel)
    ax.set_xlabel(xlabel)

    p.show()


def plot_box( tab, title="", labels=[], ylabel="", xlabel=""):
    import pylab as p
    import numpy as np

    fig = p.figure()
    ax = fig.add_subplot(111)

    for i in xrange(len(tab)):
        for j in xrange(len(tab[i])):
            if tab[i][j] == None:
                tab[i][j] = []
    
    ax.boxplot( [x for l in zip(*tab) for x in l] )

    ax.legend()
    ax.set_title(title)
    ax.set_ylabel(ylabel)
    ax.set_xlabel(xlabel)

#    xtickNames = p.setp(ax, xticklabels = labels * len(tab[0]) )
#    p.setp(xtickNames, rotation=225, fontsize=8)

    p.show()


######################################
# COMMANDS
######################################

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
    
   
def split_by_instance( filenames, nb_instances = 31, ins_mark = "p[0-9]{2}" ):

        fnames = []
        for pattern in filenames:
            for f in expand( pattern ):
                fnames.append( f )

        if len(fnames) == 0:
            print "ERROR files not found"
            sys.exit(1)

        fnames.sort()
        finalnames = [[]] * nb_instances
      
        marks = re.findall( ins_mark, fnames[0] )
        if len(marks) == 0:
            raise "ERROR the following file name does not contains the instance marker (%s):\n%s" % ( ins_mark, fname[0] )

        prev_ins = marks[0]
        
        ins_files = [ fnames[0] ]

        for fname in fnames[1:]:

            ins_counter = int( prev_ins.strip("p") )

            marks = re.findall( ins_mark, fname ) 
            if len(marks) == 0:
                raise "ERROR the following file name does not contains the instance marker (%s):\n%s" % ( ins_mark, fname )

            cur_ins = marks[0]
        
            # if the same instance
            if cur_ins == prev_ins:
                # adding this file to the current list
                ins_files.append( fname )

            else: # if we change of instance
                # store the list of files for the previous instance
                finalnames[ins_counter] = ins_files
                ins_files = [fname]
                 
            prev_ins = cur_ins
        
        finalnames[ins_counter] = ins_files

#        for f in finalnames:
#            print len(f),
#            if len(f) > 0:
#                print f[0],"..."
#            else:
#                print []
#            sys.stdout.flush()
#        print

        filenames = []
        for i in xrange(len(finalnames)):
            f = common_characters( finalnames[i] )
            filenames.append( f.strip("*") )

        return filenames


def print_filenames( filenames ):
    efiles = []
    for pattern in filenames:
        dir,file = dir_file( pattern ).next()
        print dir,":",file
    
        for f in expand( pattern ):
            print "\t",os.path.basename(f)
            efiles.append( f )

    print common_characters(efiles)


def process_commands( opts, filepatterns ):

    # the result to return, if necessary
    tab = []

    key="Makespan"
    if opts.seq:
        key="TotalCost"

    has_labels = False
    if opts.labels:
        has_labels = True

    # display targeted files, for debug purpose
    if opts.files:
        print_filenames( filepatterns )

    if opts.basestats:
        #stats = [len,min,median,mean,std,skew,kurtosis]
        stats = [f for k,f in available_functions.items() ]
        tab = parse_func( filepatterns, stats, key, labels=has_labels )
        #printa(tab,transpose=False)

    if opts.compare:
        compare_median(filepatterns,key)

    if opts.plotdistrib:
        plotdistrib(filepatterns,key)

    if opts.function:
        f = available_functions[opts.function]
        tab = parse_func( filepatterns, [f], key, labels=has_labels )
        #printa(tab,transpose=False)

    if opts.plot:
        f = available_functions[opts.function]
        tab = parse_func( filepatterns, [f], key, labels=False )
        plot_line(
                tab, 
                title= common_characters( filepatterns ), 
                labels = f.__name__, 
                ylabel=f.__name__, 
                xlabel="instances"
            )

    return tab


if __name__=="__main__":

    import optparse
    import os
    import re

    parser = optparse.OptionParser("""daex.py [options] file_patterns
Parse the set of file patterns given in entry and apply various functions to it.
The file patterns must be given in double quotes.

Examples:
Getting basic statistics on sequential problems for two algoritms:
    daex.py --seq --basestats "algo1_domain_p01_r*.soln" "algo2_domain_p01_r*.soln"

Print the table comparing the kurtosis of the results for 2 algorithms, by instance, show the file patterns used as labels:
    daex.py "algo1*elevators*.soln" "algo2*elevators*.soln" --byinstance --function=kurtosis --labels

Show the histogram of the absolute makespan/cost for 2 algorithms:
    daex.py "algo1*elevators*.soln" "algo2*elevators*.soln" --plotdistrib

Compare the absolute makespan/cost distributions of 3 algorithms and tell which one is significantly different from all the others:
    daex.py "algo1*elevators*.soln" "algo2*elevators*.soln" "algo3*elevators*.soln" --compare

Comparing the median of several runs for 3 algorithms, all instances behind shown on a single screen:
    daex.py "algo1*elevators*.soln" "algo2*elevators*.soln" "algo3*elevators*.soln" --byinstance --function=median

Comparing the minima of several runs for 3 algorithms, by instance and plot a graphic:
    daex.py "algo1*elevators*.soln" "algo2*elevators*.soln" "algo3*elevators*.soln" --byinstance --function=min --plotbyinstance """)

    parser.add_option("-s", "--seq", dest="seq", action="store_true",
            help="sequential problem (default to temporal)" )

    parser.add_option("-l", "--labels", dest="labels", action="store_true",
            help="display labels related to the data" )

    parser.add_option("-i", "--byinstance", dest="byinstance", action="store_true",
            help="data parsed by instances" )

    parser.add_option("-f", "--files", dest="files", action="store_true",
            help="list the files that match the given patterns" )

    parser.add_option("-b", "--basestats", dest="basestats", action="store_true",
            help="display basic statistics" )

    parser.add_option("-d", "--plotdistrib", dest="plotdistrib", action="store_true",
            help="plot as histograms" )

    parser.add_option("-p", "--plot", dest="plot", action="store_true",
            help="plot data that can be plotted as a single line" )

    parser.add_option("-o", "--plotbyinstance", dest="plotbyinstance", action="store_true",
            help="plot several lines on the same graphics, when parsing by instance" )

    parser.add_option("-B", "--boxplotbyinstance", dest="boxplotbyinstance", action="store_true",
            help="plot several box plots on the same graphics, when parsing by instance" )

    parser.add_option("-c", "--compare", dest="compare", action="store_true",
            help="compare distributions" )

    parser.add_option("-n", "--nbinstances", dest="nbinstances", 
            action="store", type="int", default=31, metavar="NB",
            help="number of instances to seek when splitting by instances" )

    parser.add_option("-m", "--instancemarker", dest="instancemarker", 
            action="store", type="str", default="p[0-9]{2}", metavar="REGEXP",
            help="regexp marking the instance number in the files names" )


    av_funcs = [len,min,max,mean,median,std,skew,kurtosis,copy.copy]
    available_functions = {}
    for f in av_funcs:
        available_functions[f.__name__] = f

    parser.add_option("-u", "--function", dest="function", metavar="FUNC", 
            help="display result of the call of a function on the data, among %s" % available_functions.keys())

    (opts, filepatterns) = parser.parse_args()

    results = []
    
    # re-split data by instances
    # let this switch be the first, the commands using the "filepatterns"
    if opts.byinstance:
        if len( filepatterns ) == 1:
            patterns = split_by_instance( filepatterns, opts.nbinstances, opts.instancemarker )
            results = process_commands( opts, patterns )
            printa(results,transpose=False)
        else:
            print opts.function
            plabels = []
            for filepattern in filepatterns:
                #print "====================================================================="
                #print filepattern
                patterns = split_by_instance( [filepattern], opts.nbinstances, opts.instancemarker )
                result = []
                res = process_commands( opts, patterns )
                for r in res:
                    result.append( r[0] )

                results.append( result )
                plabels.append( filepattern )
              
            print ' '.join( plabels )
            printa2( results, transpose=True )
           
            if opts.plotbyinstance:
                plot_line (
                        results,
                        title= common_characters( filepatterns ), 
                        labels = plabels,
                        ylabel = opts.function,
                        xlabel="instances"
                    )
            elif opts.boxplotbyinstance:
                plot_box (
                        results,
                        title= common_characters( filepatterns ), 
                        labels = plabels,
                        ylabel = opts.function,
                        xlabel="instances"
                    )


    else: # if not by instance
        results = process_commands( opts, filepatterns )
        printa(results,transpose=False)

