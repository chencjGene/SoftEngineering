import numpy as np

'''
This file contain several function that get data from data source.

all function return a tuple. The first one is data and the second one is label.
'''

def linearData( size = (100,2) ):
    '''
    a artifical datasets that seperates by line y + 0.3 * x + 0.2 
    '''
    data = 100 * np.random.random(size) - 50
    label = (np.dot( data, np.array([1,0.3]) ) + 0.2 ) > 0 
    label = label * 2 - 1
    return data,label
