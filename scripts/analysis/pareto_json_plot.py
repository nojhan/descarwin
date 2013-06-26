#!/usr/bin/env python

import sys
import json
import matplotlib.pyplot as plt

data = json.load( open(sys.argv[1]))

x=[]
y=[]
for sol in data:
    x.append( sol["objective_makespan"] )
    y.append( sol["objective_cost"] )


plt.xlabel('Makespan')
plt.ylabel('Cost')
plt.margins(0.5)
plt.plot(x,y, 'o')
plt.show()

