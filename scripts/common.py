import optparse, logging, sys

LEVELS = {'debug': logging.DEBUG,
          'info': logging.INFO,
          'warning': logging.WARNING,
          'error': logging.ERROR,
          'critical': logging.CRITICAL}

LOG_DEFAULT_FILENAME='notitle.log'

def parser(parser=optparse.OptionParser()):
    parser.add_option('-v', '--verbose', choices=LEVELS.keys(), default='info', help='set a verbose level')
    parser.add_option('-l', '--levels', action='store_true', default=False, help='list verbose levels')
    parser.add_option('-o', '--output', help='give an output filename for logging', default=LOG_DEFAULT_FILENAME)

    options, args = parser.parse_args()

    if options.levels: list_verbose_levels()

    logger(options.verbose, options.output)
    return options

def logger(level_name, filename=LOG_DEFAULT_FILENAME):
    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
        filename=filename, filemode='a'
        )

    console = logging.StreamHandler()
    console.setLevel(LEVELS.get(level_name, logging.NOTSET))
    console.setFormatter(logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s'))
    logging.getLogger('').addHandler(console)

def list_verbose_levels():
    print "Here's the verbose levels available:"
    for keys in LEVELS.keys():
        print "\t", keys
    sys.exit()
