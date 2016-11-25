#include<iostream>
#define BLKSIZE 8
using namespace std;

double pi = 3.141592653;

void getC(double c[], int len){
	for (int i = 0; i < 4*len; i++)
		c[i] = cos(pi *i/ 2.0 / double(len));
}


void DCT( int block[][BLKSIZE], double pixelmat[][BLKSIZE])
{
    /*
    discriminate cosine transform.

    Parameter:
        block : matrix contain origin data.
        DCTCoe : matrix that store DCT coefficient. It will be changed by calling this function.
    */
    int mheight = BLKSIZE;
    int mwidth = BLKSIZE;
	double *c, **pc, *tmp;
	tmp = new double[ mheight ]; 
	c = new double[ mheight * 4 ];
	getC( c, mheight );
	pc = new double*[ mheight ];		//malloc a 2-dimension array
	for ( int i = 0; i < mheight; i++ )	
		pc[i] = new double[ mwidth ];
	for ( int i = 0; i < mheight; i++ )
		for ( int j = 0; j < mwidth; j++ )
			pc[i][j] = block[i][j];

	for (int k = 0; k < mheight; k++){
		for (int u = 0; u < mheight; u++){
			tmp[u] = 0;
			for (int y = 0; y < mheight; y++)
				tmp[u] = tmp[u] + pc[k][y] * c[((2*y+1)*u) % (4*mheight)];
		}
		for (int y = 0; y < mheight; y++)
			pc[k][y] = tmp[y];
	}
	for (int y = 0; y < mheight; y++){
		for (int v = 0; v < mheight; v++){
			tmp[v] = 0;
			for (int x = 0; x < mheight; x++)
				tmp[v] = tmp[v] + pc[x][y] * c[((2*x+1)*v) % (4*mheight)];
		}
		for (int x = 0; x < mheight; x++)
			pc[x][y] = tmp[x];
	}
	for (int i = 0; i < mheight; i++)
		for (int j = 0; j < mwidth; j++){
			double a = pc[i][j];
			if (i == 0 && j == 0)	a = a / mheight;
			else if(i != 0 && j != 0)	a = a / mheight * 2;
			else a = a / mheight*sqrt(2);
			pixelmat[i][j] = a;
		}
}

void matmul(double td[8],double mat[8][8],double fd[8]){//矩阵相乘函数
	int i,j;//辅助变量
	double temp=0;//辅助变量
    int processeing_image_size = BLKSIZE;
	for (i=0;i<processeing_image_size;i++){
		temp=0;
		for (j=0;j<processeing_image_size;j++){
			temp=temp+td[j]*mat[i][j];
		}
		fd[i]=temp;
	}
}

void dct(int origin[8][8], double dct_r[8][8]){//DCT变换
	double dct_mat[8][8];//一维DCT变换矩阵
	double td[8];//时域（变换前）的数组
	double fd[8];//频域（变换后）的数组
	double dct_temp[8*8];//做一次一维DCT变换后的暂存矩阵
	double result[8*8];//二维DCT变换结果矩阵
	int i,j;//辅助变量
    int processeing_image_size = BLKSIZE;
	for (i=0;i<processeing_image_size;i++){//生成一维DCT变换矩阵直流部分
		dct_mat[0][i]=1/sqrt(2.0)/2.0;
	}
	for (i=1;i<processeing_image_size;i++){//生成一维DCT变换矩阵交流部分
		for (j=1;j<=processeing_image_size;j++){
			dct_mat[i][j-1]=cos(i*pi*(2*j-1)/(2*processeing_image_size))/2.0;
		}
	}
	for (i=0;i<processeing_image_size;i++){//一次一维DCT变换
		for (j=0;j<processeing_image_size;j++){
			td[j]=(double)(origin[i][j]);
		}
		matmul(td,dct_mat,fd);
		for (j=0;j<processeing_image_size;j++){
			dct_temp[i*processeing_image_size+j]=fd[j];
		}
	}
	for (j=0;j<processeing_image_size;j++){//第二次一维DCT变换
		for (i=0;i<processeing_image_size;i++){
			td[i]=dct_temp[i*processeing_image_size+j];
		}
		matmul(td,dct_mat,fd);
		i=0;
		for (i=0;i<processeing_image_size;i++){
			result[i*processeing_image_size+j]=fd[i];
		}
	}
	for (i=0;i<processeing_image_size;i++){//将DCT变换结果输出到所需数组中
		for (j=0;j<processeing_image_size;j++){
			dct_r[i][j]=result[i*processeing_image_size+j];
		}
	}
}