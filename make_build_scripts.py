#!/usr/bin/env python

def flatten(l, ltypes=(list, tuple)):
    ltype = type(l)
    l = list(l)
    i = 0
    while i < len(l):
        while isinstance(l[i], ltypes):
            if not l[i]:
                l.pop(i)
                i -= 1
                break
            else:
                l[i:i + 1] = l[i]
        i += 1
    return ltype(l)


def binary_prod(options):
    import itertools
    o = options.keys()[0]
    args = ( o+"="+options[o][0], o+"="+options[o][1] )
    prods = args
    for i in range(1,len(options.keys())):
        o = options.keys()[i]
        args = ( o+"="+options[o][0], o+"="+options[o][1] )
        prods = list(itertools.product(prods, args ))

    return prods


def make_build( options ):
    build = {}
    for p in binary_prod(options):
        build_name = []
        build_def = []
        args = sorted(flatten(p))
        for arg in args:
            build_def.append( "-D"+arg )
            opt = arg.split("=")
            if not "off" in opt[1]:
                if "on" in opt[1]:
                    build_name.append( flags[opt[0]] )
                else:
                    build_name.append( opt[1].lower() )
        build[ "_".join(build_name) ] = " ".join(build_def)
    return build


def make_build_script( name, args ):
    import os, stat

    fname = "builds/build_"+name
    with open( fname, 'w') as fd:
        fd.write("""#!/usr/bin/env sh
mkdir -p %s
cd %s
cmake %s ../..
make
err=$?
cd ..
exit $err
""" % (name,name,args) )

    os.chmod( fname, stat.S_IRWXU | stat.S_IRWXG | stat.S_IROTH )


if __name__=="__main__":

    flags = {
            "CPT_WITH_OMP":"cpt-omp",
            "CPT_WITH_MPI":"cpt-mpi",
            "DAE_WITH_MPI":"dae-mpi",
            "DAE_WITH_OMP":"dae-omp",
            "BUILD_DAE_MO":"daemo"
            }

    options = { "CMAKE_BUILD_TYPE" : ("Debug", "Release") }

    for f in flags:
        options[f] = ("on", "off")

    build = make_build(options)
    for name in sorted(build.keys()):
        make_build_script( name, build[name] )

