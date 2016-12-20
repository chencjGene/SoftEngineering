import numpy as np

def Dot( x1, x2 ):
    '''
    using linear kernel.
    '''
    assert len(x1.shape) == 1
    assert len(x2.shape) == 1
    return np.dot(x1, x2)
