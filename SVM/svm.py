import numpy as np
import matplotlib.pyplot as plt
import matplotlib

def Dot( x1, x2 ):
    assert len(x1.shape) == 1
    assert len(x2.shape) == 1
    return np.dot(x1, x2)

def linearData( size = (100,2) ):
    data = 100 * np.random.random(size) - 50
    label = (np.dot( data, np.array([1,0.3]) ) + 0.2 ) > 0 
    label = label * 2 - 1
    return data,label

class svm(object):

    def __init__(self, kernelMethod=Dot, dataSource=linearData,eps = 1e-3, tol = 1e-3, C = 1 ):
        '''
        
        '''
        self.kernelMethod = kernelMethod
        self.eps = eps
        self.dataSource = dataSource
        self.tol = tol
        self.C = C
    def _init_w(self, size = 2 , force = False):
        '''
        initialize w.
        '''
        if hasattr(self,'w') and force==False:
            raise ValueError(' w already exists. If you want to re-initialize w, please set force True.')
        self.w = 0.2 * np.random.random(size)
    
    def _init_b(self, force=False):
        '''
        initialize b.
        '''
        if hasattr(self,'b') and force == False:
            raise ValueError('b already exists, If you want to re-initialize b, please set force True.')
        self.b = 0.2 * np.random.random(1)
        
    def _init_lm(self, size = 100 , force=False):
        '''
        initialize lagrange multipliers.
        '''
        if hasattr(self,'lm') and force == False:
            raise ValueError('lm already exists, If you want to re-initialize lm, please set force True.')
        self.lm = np.zeros(size)
        
    def getData(self):
        '''
        
        '''
        self.data, self.label = self.dataSource()
        
    def getLM(self,i):
        return self.lm[i]
    
    def target(self, i):
        return self.label[i]
    
    def output(self, i):
        #print(self.w.shape, self.data.shape,i)
        
        #a = np.dot(self.w,self.data[i])
        #return ( a  + self.b )[0]
        if hasattr(self,'E') == False:
            self.E = np.dot(self.data, self.w) - self.b
        return self.E[i]

    def point(self, i):
        return self.data[i]
        
    def getLH(self, alpha1, alpha2, y1, y2):
        if y1 * y2 < 0 :
            L = max(0, alpha2 - alpha1)
            H = min(self.C, self.C + alpha2 - alpha1 )
        else:
            L = max(0, alpha2 + alpha1 - self.C)
            H = min(self.C, alpha1 + alpha2 )
        return L,H

    
    def getLHobj(alpha1, alpha2, E1, E2, y1, y2, L, H, k11, k12, k22, s):
        f1 = y1 * ( E1 + self.b ) - alpha1 * k11 - s * alpha2 * k12
        f2 = y2 * ( E2 + self.b ) - s * alpha1 * k12 - alpha2 * k22
        L1 = alpha1 + s * ( alpha2 - L )
        H1 = alpha1 + s * ( alpha2 - H )
        phi_L = L1 * f1 + L * f2 + L1 * L1 * k11 / 2.0 + L * L * k22 / 2.0 + s * L * L1 * k12
        phi_H = H1 * f1 + H * f2 + H1 * H1 * k11 / 2.0 + H * H * k22 / 2.0 + s * H * H1 * k12
        
        return phi_L, phi_H
       
    def notAtBound(self, alpha):
        if alpha > 0 and alpha < self.C :
            return True
        else:
            return False
    def notOnBound(self, alpha):
        if abs(alpha - 0)<self.tol and abs(alpha - self.C) < self.tol :
            return True
        else:
            return False
             
    def updateThreshold(self, alpha1, alpha2, E1, E2, y1, y2, k11, k12, k22, a1, a2):
        b1 = E1 + y1 * ( a1 - alpha1 ) * k11 + y2 * ( a2 - alpha2 ) * k12 + self.b
        b2 = E2 + y1 * ( a1 - alpha1 ) * k12 + y2 * ( a2 - alpha2 ) * k22 + self.b
        if self.notAtBound(alpha1) and self.notAtBound(alpha2) :
            print(1)
            self.b = b1  
        elif not self.notAtBound(alpha1) and not self.notAtBound(alpha2) :
            print(2)
            self.b = ( b1 + b2 ) / 2.0
        elif not self.notAtBound(alpha1) and self.notAtBound(alpha2):
            print(3)
            self.b = b2
        else:
            self.b = b1
            
    def updateWeight(self, alpha1, alpha2, y1, y2, i1, i2, a1, a2):
    
        self.w = self.w + y1 * ( a1 - alpha1 ) * self.point(i1) + y2 * ( a2 - alpha2 ) * self.point(i2)
    
    def updateErrorCache(self):
        
        self.E = np.dot( self.data, self.w) - self.b
        
        
    def updateLM(self,i1, i2, a1, a2):
        self.lm[i1] = a1
        self.lm[i2] = a2
        
    def takeStep(self,i1,i2):
        '''
        
        '''
        #if select the same data point, it changes nothing.
        if ( i1 == i2 ):
            return 0
        alpha1 = self.getLM(i1)
        alpha2 = self.getLM(i2)
        y1 = self.target(i1)
        y2 = self.target(i2)
        E1 = self.output(i1) - y1
        E2 = self.output(i2) - y2
        s = y1 * y2
        L,H = self.getLH(alpha1, alpha2, y1, y2)
        #if 
        if ( L == H):
            return 0
        k11 = self.kernelMethod(self.point(i1),self.point(i1))
        k12 = self.kernelMethod(self.point(i1),self.point(i2))
        k22 = self.kernelMethod(self.point(i2),self.point(i2))
        eta = k11 + k22 - 2 * k12
        if ( eta > 0 ):
            a2 = alpha2 + y2 * ( E1 - E2 ) / eta
            if ( a2 < L ):
                a2 = L
            elif( a2 > H ):
                a2 = H
        else:
            Lobj,Hobj = getLHObj(alpha1, alpha2, E1, E2, y1, y2, L, H, k11, k12, k22, s)
            if ( Lobj < Hobj - self.eps ):
                a2 = L
            elif( Lobj > Hobj + self.eps):
                a2 = H
            else:
                a2 = alpha2
        if ( abs(a2 - alpha2) < self.eps * (a2 + alpha2 + self.eps) ):
            return 0
        a1 = alpha1 + s * ( alpha2 - a2)
        self.updateThreshold(alpha1, alpha2, E1, E2, y1, y2, k11, k12, k22, a1, a2)
        self.updateWeight(alpha1, alpha2, y1, y2, i1, i2, a1, a2)
        self.updateErrorCache()
        self.updateLM(i1,i2,a1,a2)
        print(self.b)
        return 1
        
    def numOfNonBound(self):    
        return ( ( self.lm != 0 ) * (self.lm != self.C ) > 0 ).sum()
        
    def secondChoiceHeuristic(self,i):
        if self.output(i) > 0:
            return self.E.argmin()
        else:
            return self.E.argmax()
    
    def randomOrder(self):
        a = np.array(range(len(self.lm)))
        np.random.shuffle(a)
        return a
        
    def randomNonBoundOrder(self):
        a = self.randomOrder()
        b = [i for i in a if self.notOnBound(i)]
        return b
    
    def NonBoundOrder(self):
        a = np.array(range(len(self.lm)))
        b = [i for i in a if self.notOnBound(i)]
        return b
   
    
    def examineExample(self,i2):
        y2 = self.target(i2)
        alpha2 = self.getLM(i2)
        #print(self.output(i2))
        E2 = self.output(i2) - y2
        r2 = E2 * y2
        #print(r2)
        if ( ( r2 < -self.tol and alpha2 < self.C) ) or ( ( r2 > self.tol and alpha2 > 0) ) :
            if ( self.numOfNonBound() > 1 ):
                i1 = self.secondChoiceHeuristic(i2)
                if ( self.takeStep(i1,i2) ):
                    return 1
            for i1 in self.randomNonBoundOrder():
                if self.takeStep(i1,i2):
                    return 1
            for i1 in self.randomOrder():
                if self.takeStep(i1,i2):
                    return 1
        return 0

    def train(self):
        self._init_w()
        self._init_b()
        self._init_lm()
        self.getData()
        #self.Visualize(f=2)
        numChanged = 0
        examineAll = 1
        while( numChanged > 0 or examineAll ):
            numChanged = 0
            if examineAll:
                for I in range(len(self.lm)):
                    numChanged += self.examineExample(I)
            else:
                #print(self.NonBoundOrder())
                for I in self.NonBoundOrder():
                    numChanged += self.examineExample(I)
            if examineAll == 1 :
                examineAll = 0
            elif numChanged == 0 :
                examineAll = 1
                
    def evaluate(self):
        return ( ( ( np.dot(self.data,self.w) - self.b ).reshape(100) > 0 ) != (self.label > 0 ) ) .sum()
        
    def Visualize(self,f = 1, saveImage = True):
        fg = plt.figure(f)
        idx_1 = self.label==1
        p1 = plt.scatter(self.data[idx_1,1], self.data[idx_1,0],marker='x', color = 'm', label = '1',s=30)
        idx_2 = self.label==-1
        p2 = plt.scatter(self.data[idx_2,1], self.data[idx_2,0],marker='+', color = 'c', label = '2',s=50)
        p3 = plt.plot([50,-50],[-0.3*50-0.2,0.3*50-0.2 ],color = 'b')
        if f != 1:
            p4 = plt.plot([50,-50],[(-50*self.w[1]+self.b)/self.w[0],(50*self.w[1]+self.b)/self.w[0]],color = 'r')
        if saveImage == True:
            plt.savefig(str(f)+'.jpg',format='jpg')
        else:
            plt.show()        
if __name__ == '__main__':
    s =svm(C=10000)
    s.train()
    print s.evaluate()
    print(s.w/s.w[0],s.b/s.w[0])
    s.Visualize(f=3,saveImage=False)
