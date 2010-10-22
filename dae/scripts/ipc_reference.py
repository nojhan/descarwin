import os
import sys

def recursive_search(directory, filename ):
    files = []
    for file in os.listdir(directory):
        node = os.path.join(directory, file)
        if os.path.isfile(node):
            if os.path.basename(node) == filename:
                files.append(node)

        # recursively access file names in subdirectories
        elif os.path.isdir(node):
            files.extend( recursive_search(node, filename) )

    return files


def grep_values( files, pattern="Value:", mark = ":", index=-1, type=float ):

    values = {}
    
    for filename in files:

        file = open( filename )

        for line in file:
            if pattern in line:
                list = line.split( mark )
                value = type( list[ index ] )
                values[filename] = value

        file.close()

    return values


def generate_data( values ):
    data = {}
    refs = {}
    for path in values:
        (dot,solver,domain,instance,file) = path.split("/")
        value = values[path]
        #print solver,domain,instance,value
        
        # if first time we see this domain
        if not data.has_key(domain):
            # create the dico
            data[domain] = { solver:{instance:value} }
        else:
            if not data[domain].has_key(solver):
                # add a new key for this solver
                data[domain][solver] = {instance:value}
            else:
                data[domain][solver][instance] = value

        if not refs.has_key(domain):
            # create a list of values for each instance
            refs[domain] = { instance:(value,solver) }

        else:
            # do not consider zero values
            if not refs[domain].has_key(instance) and value != 0:
                refs[domain][instance] = (value,solver)

            # if value is lesser than
            elif value < refs[domain][instance][0] and value != 0:
                refs[domain][instance] = (value,solver)

            # if equal values
            elif value == refs[domain][instance][0]:
                # concatenate solvers
                refs[domain][instance] = (value, solver+"/"+refs[domain][instance][1] )

            else:
                pass

    return (data,refs)



def fill_in( data, refs, instances_nb = 31 ):

    # data[domain][solver][instance] = value
    for domain,solvers in data.items():
        for solver,instances in solvers.items():
            # browse on numeric index
            # IPC starts at 01
            for i in range( 1, instances_nb ):
                # we do not have this instance
                instance = str(i).zfill(2)
                if not instances.has_key( instance ):
                    # put zero
                    instances[instance] = 0


    # refs[domain][instance] = (value,solver)
    for domain,instances in refs.items():
        for i in range( 1, instances_nb ):
            instance = str(i).zfill(2)
            if not instances.has_key( instance ):
                # put zero and no solver
                instances[instance] = ( 0, "NONE" )

    return (data,refs)





if __name__=="__main__":

    print "Search for data files..."
    files = recursive_search( ".", "plan.soln.1.val" )

    print "Search for values in files..."
    values = grep_values( files, "Value", ":", -1, float )
    
    print "Generate data structures..."
    # data[domain][solver][instance] = value
    # refs[domain][instance] = (value,solver)
    data,refs = generate_data( values )

    print "Fill in data with zero for missing instances..."
    instances_nb = 31
    data,refs = fill_in( data, refs, instances_nb )
    
    print "Write CSV files..."
    for domain in data:
        #print domain
        csvname = "%s_%s.csv" % (sys.argv[1], domain)
        f = open( csvname, 'w' )
        for solver in data[domain]:
            f.write(solver)
            #for instance in data[domain][solver]:
            for i in range( 1, instances_nb ):
                instance = str(i).zfill(2)
                f.write( ";" + str( data[domain][solver][instance] ) )
            f.write('\n')
        f.close()


    print "Write CSV references files..."
    for domain in refs:

        csvname = "%s_%s_references.tsv" % (sys.argv[1], domain)
        f = open( csvname, 'w' )
        f.write( "# References values for %s %s\n" % (sys.argv[1], domain) )
        f.write( "# Value\t\tSolver\n" )

        #for instance in refs[domain]:
        for i in range( 1, instances_nb ):
            instance = str(i).zfill(2)
            f.write( "%f\t%s\n" % ( refs[domain][instance][0], refs[domain][instance][1]  ) )

        f.close()

