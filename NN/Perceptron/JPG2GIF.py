import ImageGrab # from PIL  
import time  
import string  
from PIL import Image, ImageChops  
from PIL.GifImagePlugin import getheader, getdata  
import os  
    
def intToBin(i):  
      
      
    i1 = i % 256  
    i2 = int( i/256)  
    
    return chr(i1) + chr(i2)  
def getheaderAnim(im):  
      
    bb = "GIF89a"  
    bb += intToBin(im.size[0])  
    bb += intToBin(im.size[1])  
    bb += "\x87\x00\x00"    
    return bb  
def getAppExt(loops=0):  

  
    bb = "\x21\xFF\x0B"  # application extension  
    bb += "NETSCAPE2.0"  
    bb += "\x03\x01"  
    if loops == 0:  
        loops = 2**16-1  
    bb += intToBin(loops)  
    bb += '\x00'  # end  
    return bb  
      
      
def getGraphicsControlExt(duration=0.1):  
      
    bb = '\x21\xF9\x04'  
    bb += '\x08'  # no transparancy  
    bb += intToBin( int(duration*100) ) # in 100th of seconds  
    bb += '\x00'  # no transparant color  
    bb += '\x00'  # end  
    return bb  
      
      
      
def _writeGifToFile(fp, images, durations, loops):  
     
      
      
    frames = 0  
    previous = None  
    for im in images:  
        if not previous:  
            #   
            #   
            palette = getheader(im)[1]  #  
            data = getdata(im)  
            imdes, data = data[0], data[1:]              
            header = getheaderAnim(im)  
            appext = getAppExt(loops)  
            graphext = getGraphicsControlExt(durations[0])  
                 
            #   
            fp.write(header)  
            fp.write(palette)  
            fp.write(appext)  
                
            #   
            fp.write(graphext)  
            fp.write(imdes)  
            for d in data:  
                fp.write(d)  
                
        else:  
            #             
            data = getdata(im)   
            imdes, data = data[0], data[1:]         
            graphext = getGraphicsControlExt(durations[frames])  
                  
            #   
            fp.write(graphext)  
            fp.write(imdes)  
            for d in data:  
                fp.write(d)     
        #  
        previous = im.copy()          
        frames = frames + 1  
   
    fp.write(";")  #   
    return frames  
     
def writeGif(filename, images, duration=0.1, loops=0, dither=1):  




    images2 = []  
    #   
    for im in images:  
           
        if isinstance(im,Image.Image): #  
            images2.append( im.convert('P',dither=dither) )  
                
        elif np and isinstance(im, np.ndarray): # 
            if im.dtype == np.uint8:  
                pass  
            elif im.dtype in [np.float32, np.float64]:  
                im = (im*255).astype(np.uint8)  
            else:  
                im = im.astype(np.uint8)  

            if len(im.shape)==3 and im.shape[2]==3:  
                im = Image.fromarray(im,'RGB').convert('P',dither=dither)  
            elif len(im.shape)==2:  
                im = Image.fromarray(im,'L').convert('P',dither=dither)  
            else:  
                raise ValueError("wrong image format")  
            images2.append(im)  
                
        else:  
            raise ValueError("unknown image format")  
          

    durations = [duration for im in images2]  

    fp = open(filename, 'wb')  

    try:  
        n = _writeGifToFile(fp, images2, durations, loops)  
    finally:  
        fp.close()  
    return n  
      
#################################  

def images2gif( images, giffile, durations=0.05, loops = 1):  
    seq = []  
    for i in range(len(images)):  
        im = Image.open(images[i])  
        background = Image.new('RGB', im.size, (255,255,255))  
        background.paste(im, (0,0))  
        seq.append(background)  
    frames = writeGif( giffile, seq, durations, loops)  
    print frames, 'images has been merged to', giffile  
      

def gif2images( filename, distDir = '.', type = 'bmp' ):  
    if not os.path.exists(distDir):  
        os.mkdir(distDir)  
    print 'spliting', filename,  
    im  = Image.open( filename )  
    im.seek(0)  # skip to the second frame  
    cnt = 0  
    type = string.lower(type)  
    mode = 'RGB'  # image modea  
    if type == 'bmp' or type == 'png':  
        mode = 'P'    # image mode   
    im.convert(mode).save(distDir+'/%d.'%cnt+type )  
    cnt = cnt+1  
    try:  
        while 1:  
            im.seek(im.tell()+1)  
            im.convert(mode).save(distDir+'/%d.'%cnt+type)  
            cnt = cnt+1  
    except EOFError:  
        pass # end of sequence  
    white = (255,255,255)  
    preIm = Image.open(distDir+'/%d.'%0+type).convert('RGB')  
    size = preIm.size  
    prePixs = preIm.load()  
    for k in range (1,cnt):  
        print '.',  
        im = Image.open(distDir+'/%d.'%k+type).convert('RGB')  
        pixs = im.load()  
        for i in range(size[0]):  
            for j in range(size[1]):  
                if pixs[i,j] == white:  
                    pixs[i,j] = prePixs[i,j]  
        preIm = im  
        prePixs = preIm.load()  
        im.convert(mode).save(distDir+'/%d.'%k+type)  
    print '\n', filename, 'has been splited to directory: [',distDir,']'  
    return cnt    
      
    ##############################################################  
if __name__ == '__main__':  
    #frames = gif2images('source.gif',distDir='Converged',type='jpg')  
    images = []  
    for i in range(500):
        try:
            images.append('tmp/%d.jpg'%i)  
        except:
            None
    images2gif(images,'save.gif', durations = 0.05)  
