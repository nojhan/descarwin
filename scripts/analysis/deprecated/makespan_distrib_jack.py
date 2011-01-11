#!/usr/bin/env python
# example: makespan_distrib_jack.py -b -i /home/human/experiences/2010-10-20_dae_vs_jack_ipc3_ipc6/tempo-sat/plans/jack*elevator* &
import difflib
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
#            if "MakeSpan" in line or "TotalCost" in line:
#                try:
#                    makespans_dae.append( float(line.split()[-1]) )
#                except:
#                    pass

            # Jack
            if "PLAN" in line or "Makespan :" in line and not "BK" in line:
                makespans_jack.append( float(line.split()[-1]) )

            else:
                if parse_zero:
                    print "No makespan/cost found"

#    return (makespans_dae, makespans_jack)
#    return makespans_dae
    return makespans_jack

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

def get( func, a ):
    r = []
    try:
        r += [ func( a ) ]
    except:
        r += [ float('nan') ]

    r = [func.__name__] + r
    return tuple(r)


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


def histo( data, file_match='', range_max=0, interactive=False ):

    output = "makespan_distribution_" + file_match.replace('*', '') + ".png"

    #data = parse(files)

    #pp = PdfPages( output )

    fig = p.figure()
    fig.suptitle( "Distribution of makespan" )

    ax = fig.add_subplot(111)

    ax.set_title( "%s" % file_match )

    text = ''
    for f in [len,min,median,mean,std,skew,kurtosis]:
        text += "%s %5.2f\n" % get(f,data)
        
    print text
    p.annotate( text, (range_max - 2000, 10) ) #,
#        horizontalalignment='right',
#        verticalalignment='top' ) #,
        #transform=ax.transAxes)

    print "normal test: ", normaltest( data )




    n,bins,patches = ax.hist( data, bins=100, range=(0,range_max) )

    if interactive:
        p.show()

    else:
        fig.savefig( output, format='png' )
        #pp.savefig()
        #pp.close()

        print "Output written to:"
        print output


def boxplot( data, file_match='', interactive=False ):

    output = "makespan_distributions_" + file_match.replace('*', '') + ".png"

    #data = parse(files)

    #pp = PdfPages( output )

    fig = p.figure(figsize=(20, 10), dpi=80)
    fig.suptitle( "Distributions of makespan by instances" )

    ax = fig.add_subplot(111)

    ax.set_title( "%s" % file_match )

        
    # search the max median across all instances
    maxmed = 0
    for i in data_by.keys():
        if data_by[i] != []:
            now = median( data_by[i] )
            if now > maxmed:
                maxmed = now

    # do not plot points higher than the higher median
    #ax.set_ylim(0, maxmed+10 )

    ax.boxplot( data )

    if interactive:
        p.show()
     
    else:

        fig.savefig( output, format='png' )
        #pp.savefig()
        #pp.close()

        print "Output written to:"
        print output



if __name__=="__main__":
    import sys
    import os
    import pylab as p
    from scipy.stats import *
    #from matplotlib.backends.backend_pdf import PdfPages
    import optparse

    parser = optparse.OptionParser()

    parser.add_option("-b", "--by-instances", dest="by_instances", action="store_true",
            help="Consider that the input files and the output must be sorted by instances" )

    parser.add_option("-i", "--interactive", dest="interactive", action="store_true",
            help="interactive plotting" )

    #parser.add_option("-o", "--output", dest="output", default='nodes_distributions.pdf',
    #        help="PDF file to output figure to" )

    (opts, files) = parser.parse_args()

    path_match = files_common_match( files )
    print path_match
    file_match = os.path.basename(path_match)
    print file_match

    if opts.by_instances:

        data_by = parse_by_instances( files, range(1,31) )

        data_by[0] = []
        
        # a boxplot for each instance
        boxplot( data_by, file_match, opts.interactive )
        
#        for i in data_by.keys():
#            filename = "%s_%s" % ( file_match, str(i).zfill(2) )
#            print filename
#
#            if data_by[i] != []:
#                histo( data_by[i], filename, maxn )
#            else:
#                print "WARNING: no data at instance",i

    else:

        data = parse( files )

        # an histogram for a given instance
        histo( data, file_match, opts.interactive )

