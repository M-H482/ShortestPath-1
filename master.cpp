#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include<mpi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {
#include <athread.h>

    void slave_setHall(void);
}

#define MAX_LEN 30
#define MAX_VERT 1000
#define MAX_PARA 1000
#define MAT_3D_0_LEN 100
#define MAT_3D_1_LEN 100
#define MAT_3D_2_LEN 100

double min(double x1, double x2) {
	if (x1 > x2) return x2;
	else return x1;
}

double getGrad(double ***GV, double x, double y, double z)
{
	double value;
	double x1, x2, y1, y2, z1, z2;
	if ((int)(x) != x && (int)(y) != y && (int)(z) != z)
	{
		x1 = (ceil(x) - x) * GV[(int)(x)][(int)(y)][(int)(z)] + (x - (int)(x)) * GV[(int)ceil(x)][(int)(y)][(int)(z)];
		x2 = (ceil(x) - x) * GV[(int)(x)][(int)ceil(y)][(int)(z)] + (x - (int)(x)) * GV[(int)ceil(x)][(int)ceil(y)][(int)(z)];
		z1 = (ceil(y) - y) * x1 + (y - (int)(y)) * x2;
		y1 = (ceil(x) - x) * GV[(int)(x)][(int)(y)][(int)ceil(z)] + (x - (int)(x)) * GV[(int)ceil(x)][(int)(y)][(int)ceil(z)];
		y2 = (ceil(x) - x) * GV[(int)(x)][(int)ceil(y)][(int)ceil(z)] + (x - (int)(x)) * GV[(int)ceil(x)][(int)ceil(y)][(int)ceil(z)];
		z2 = (ceil(y) - y) * y1 + (y - (int)(y)) * y2;
		value = (ceil(z) - z) * z1 + (z - (int)(z)) * z2;
	}
	else if ((int)(x) == x && (int)(y) != y && (int)(z) != z)
	{
		x1 = GV[(int)x][(int)(y)][(int)(z)];
		x2 = GV[(int)x][(int)ceil(y)][(int)(z)];
		z1 = (ceil(y) - y) * x1 + (y - (int)(y)) * x2;
		y1 = GV[(int)x][(int)(y)][(int)ceil(z)];
		y2 = GV[(int)x][(int)ceil(y)][(int)ceil(z)];
		z2 = (ceil(y) - y) * y1 + (y - (int)(y)) * y2;
		value = (ceil(z) - z) * z1 + (z - (int)(z)) * z2;
	}
	else if ((int)(x) != x && (int)(y) == y && (int)(z) != z)
	{
		x1 = GV[(int)(x)][(int)y][(int)(z)];
		x2 = GV[(int)ceil(x)][(int)y][(int)(z)];
		z1 = (ceil(x) - x) * x1 + (x - (int)(x)) * x2;
		y1 = GV[(int)(x)][(int)y][(int)ceil(z)];
		y2 = GV[(int)ceil(x)][(int)y][(int)ceil(z)];
		z2 = (ceil(y) - y) * y1 + (y - (int)(y)) * y2;
		value = (ceil(z) - z) * z1 + (z - (int)(z)) * z2;

	}
	else if ((int)(x) != x && (int)(y) != y && (int)(z) == z)
	{
		x1 = (ceil(x) - x) * GV[(int)(x)][(int)(y)][(int)z] + (x - (int)(x)) * GV[(int)ceil(x)][(int)(y)][(int)z];
		x2 = (ceil(x) - x) * GV[(int)(x)][(int)ceil(y)][(int)z] + (x - (int)(x)) * GV[(int)ceil(x)][(int)ceil(y)][(int)z];
		value = (ceil(y) - y) * x1 + (y - (int)(y)) * x2;
	}
	else if ((int)(x) != x && (int)(y) == y && (int)(z) == z)
	{
		value = (ceil(x) - x) * GV[(int)(x)][(int)y][(int)z] + (x - (int)(x)) * GV[(int)ceil(x)][(int)y][(int)z];
	}
	else if ((int)(x) == x && (int)(y) != y && (int)(z) == z)
	{
		value = (ceil(y) - y) * GV[(int)x][(int)(y)][(int)z] + (y - (int)(y)) * GV[(int)x][(int)ceil(y)][(int)z];
	}
	else if ((int)(x) == x && (int)(y) == y && (int)(z) != z)
	{
		value = (ceil(z) - z) * GV[(int)x][(int)y][(int)(z)] + (z - (int)(z)) * GV[(int)x][(int)y][(int)ceil(z)];
	}
	else
	{
		value = GV[(int)x][(int)y][(int)z];
	}

	return value;
}//���Բ�ֵ���������ݶ�ֵ

bool crossObstacle(double x1, double y1, double z1, double x2, double y2, double z2, int ***mat_3d, int c)
{

	if (x1 < x2)
	{
		for (int x = (int)floor(x1); x < (int)ceil(x2); x++)
		{
			int y = (int)round((x - x1) / (x2 - x1) * (y2 - y1) + y1);
			int z = (int)round((x - x1) / (x2 - x1) * (z2 - z1) + z1);
			if (y > 0 && y < MAT_3D_1_LEN && z > 0 && z < MAT_3D_2_LEN&&mat_3d[x][y][z]==0) return false;
		}//�ռ���ֱ�߷���              
	}
	else if (x1 == x2);
	else
	{
		for (int x = (int)floor(x2); x < (int)ceil(x1); x++)
		{
			int y = (int)round((x - x1) / (x2 - x1) * (y2 - y1) + y1);
			int z = (int)round((x - x1) / (x2 - x1) * (z2 - z1) + z1);
			if (y > 0 && y < MAT_3D_1_LEN && z > 0 && z < MAT_3D_2_LEN&&mat_3d[x][y][z]==0) return false;
		}//�ռ���ֱ�߷���          
	}
	if (y1 < y2)
	{
		for (int y = (int)floor(y1); y < (int)ceil(y2); y++)
		{
			int x = (int)round((y - y1) / (y2 - y1) * (x2 - x1) + x1);
			int z = (int)round((y - y1) / (y2 - y1) * (z2 - z1) + z1);
			if (y > 0 && y < MAT_3D_1_LEN && z > 0 && z < MAT_3D_2_LEN&&mat_3d[x][y][z]==0) return false;
		}//�ռ���ֱ�߷���              
	}
	else if (y1 == y2);
	else
	{
		for (int y = (int)floor(y2); y < (int)ceil(y1); y++)
		{
			int x = (int)round((y - y1) / (y2 - y1) * (x2 - x1) + x1);
			int z = (int)round((y - y1) / (y2 - y1) * (z2 - z1) + z1);
			if (y > 0 && y < MAT_3D_1_LEN && z > 0 && z < MAT_3D_2_LEN&&mat_3d[x][y][z]==0) return false;
		}//�ռ���ֱ�߷���            
	}
	if (z1 < z2)
	{
		for (int z = (int)floor(z1); z < (int)ceil(z2); z++)
		{
			int x = (int)round((z - z1) / (z2 - z1) * (x2 - x1) + x1);
			int y = (int)round((z - z1) / (z2 - z1) * (y2 - y1) + y1);
			if (y > 0 && y < MAT_3D_1_LEN && z > 0 && z < MAT_3D_2_LEN&&mat_3d[x][y][z]==0) return false;
		}//�ռ���ֱ�߷���              
	}
	else if (z1 == z2);
	else
	{
		for (int z = (int)floor(z2); z < (int)ceil(z1); z++)
		{
			int x = (int)round((z - z1) / (z2 - z1) * (x2 - x1) + x1);
			int y = (int)round((z - z1) / (z2 - z1) * (y2 - y1) + y1);
			if (y > 0 && y < MAT_3D_1_LEN && z > 0 && z < MAT_3D_2_LEN&&mat_3d[x][y][z]==0) return false;
		}//�ռ���ֱ�߷���               
	}
	return true;
}//�ж�����֮���Ƿ�ɼ��������������Ƿ񴩹��ϰ���

double findPath(int x1, int y1, int z1, int x2, int y2, int z2, int *vert, int ***mat_3d, int cons) {
	int change = 0;//�ж��Ƿ��������������
	int lastpop, nowpop = 0;//��¼�Ѿ���ɢ������
	int icount = 0;//��¼��ɢ�Ĵ���
	double dist = 0;//�������̾���

	double rat = 0.1;//Ȩֵ
	double speed = 0.5; //��������

	//double H[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 }; //��¼ԭ����ɢ˳��ľ���
	double ***H = NULL;
	H = (double***)malloc(sizeof(double**)*MAT_3D_0_LEN);
	for (int i = 0; i < MAT_3D_0_LEN; i++)
	{
		H[i] = (double**)malloc(sizeof(double*)*MAT_3D_1_LEN);
		for (int k = 0; k < MAT_3D_1_LEN; k++)
		{
			H[i][k] = (double*)malloc(sizeof(double)*MAT_3D_2_LEN);
		}
	}
	//double gradmat[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };
	double ***gradmat = NULL;
	gradmat = (double***)malloc(sizeof(double)*MAT_3D_0_LEN);
	for (int i = 0; i < MAT_3D_0_LEN; i++)
	{
		gradmat[i] = (double**)malloc(sizeof(double)*MAT_3D_1_LEN);
		for (int k = 0; k < MAT_3D_1_LEN; k++)
		{
			gradmat[i][k] = (double*)malloc(sizeof(double)*MAT_3D_2_LEN);
		}
	}

	
	//double gradmat_new[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };

	double ***gradmat_new = NULL;
	gradmat_new = (double***)malloc(sizeof(double)*MAT_3D_0_LEN);
	for (int i = 0; i < MAT_3D_0_LEN; i++)
	{
		gradmat_new[i] = (double**)malloc(sizeof(double)*MAT_3D_1_LEN);
		for (int k = 0; k < MAT_3D_1_LEN; k++)
		{
			gradmat_new[i][k] = (double*)malloc(sizeof(double)*MAT_3D_2_LEN);
		}
	}
	for (int i = 0; i < MAT_3D_0_LEN; i++) {
		for (int j = 0; j < MAT_3D_1_LEN; j++) {
			for (int k = 0; k < MAT_3D_2_LEN; k++) {
				gradmat[i][j][k] = 0;
				gradmat_new[i][j][k] = 0;
				H[i][j][k] = 0;
			}
		}
	}

	gradmat[x1][y1][z1] = 1;//��ʼ��Ϊ1
	//double xm[MAT_3D_0_LEN - 1][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };
	//double ym[MAT_3D_0_LEN][MAT_3D_1_LEN - 1][MAT_3D_2_LEN] = { 0 };
	//double zm[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN - 1] = { 0 };

	//time_t t = time(NULL);

	while (change < 20)
	{
		icount = icount + 1;//��¼��ɢ����
		//��ɢ����
		for (int i = 1; i < MAT_3D_0_LEN - 1; i++)
		{
			for (int j = 1; j < MAT_3D_1_LEN - 1; j++)
			{
				for (int k = 1; k < MAT_3D_2_LEN - 1; k++)
				{
					gradmat_new[i][j][k] = gradmat[i][j][k] * (1 + rat) + (gradmat[i - 1][j][k] + gradmat[i + 1][j][k] + gradmat[i][j - 1][k] + gradmat[i][j + 1][k] + gradmat[i][j][k - 1] + gradmat[i][j][k + 1]) * rat;
				}
			}
		}

		 //~~~~~~~~
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			for (int j = 0; j < MAT_3D_1_LEN; j++)
			{
				for (int k = 0; k < MAT_3D_2_LEN; k++)
				{
					gradmat[i][j][k] = gradmat_new[i][j][k] * mat_3d[i][j][k];
					if (gradmat[i][j][k] > 1 && H[i][j][k] == 0)
					{
						H[i][j][k] = icount;
					}
				}
			}
		}//�ϰ��ﴦ��ֹ��չ  ĳ�㴦ֵ����1ʱ����ǰ��ɢ������������ÿ����ֻ�ܱ�����һ��

		lastpop = nowpop;//��һ����ɢ������
		nowpop = 0;
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			for (int j = 0; j < MAT_3D_1_LEN; j++)
			{
				for (int k = 0; k < MAT_3D_2_LEN; k++)
				{
					if (gradmat[i][j][k] > 1)
					{
						nowpop = nowpop + 1;
					}

				}
			}
		}//��¼�ѱ�ɨ�������
		if (nowpop - lastpop < 1)
		{
			change = change + 1;
		}
	}//while*/   


	//printf("%d", t - time(NULL));


	if (gradmat[x2][y2][z2] == 0)
	{

		printf("�޷������յ�");
	}//�ж�������Ƿ���ͨ·
	else
	{
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			for (int j = 0; j < MAT_3D_1_LEN; j++)
			{
				for (int k = 0; k < MAT_3D_2_LEN; k++)
				{
					if (H[i][j][k] == 0)
					{
						H[i][j][k] = icount;
					}
				}
			}
		}//ʹ�ϰ��ﴦֵ������Χ���ݶ��½�Ѱ��·��ʱ���ƹ��ϰ���
		//double xgrad[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };//���������ϵ��ݶȾ���
		double ***xgrad = NULL;
		xgrad = (double***)malloc(sizeof(double)*MAT_3D_0_LEN);
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			xgrad[i] = (double**)malloc(sizeof(double)*MAT_3D_1_LEN);
			for (int k = 0; k < MAT_3D_1_LEN; k++)
			{
				xgrad[i][k] = (double*)malloc(sizeof(double)*MAT_3D_2_LEN);
			}
		}
		//double ygrad[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };
		double ***ygrad = NULL;
		ygrad = (double***)malloc(sizeof(double)*MAT_3D_0_LEN);
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			ygrad[i] = (double**)malloc(sizeof(double)*MAT_3D_1_LEN);
			for (int k = 0; k < MAT_3D_1_LEN; k++)
			{
				ygrad[i][k] = (double*)malloc(sizeof(double)*MAT_3D_2_LEN);
			}
		}
		//double zgrad[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };
		double ***zgrad = NULL;
		zgrad = (double***)malloc(sizeof(double)*MAT_3D_0_LEN);
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			zgrad[i] = (double**)malloc(sizeof(double)*MAT_3D_1_LEN);
			for (int k = 0; k < MAT_3D_1_LEN; k++)
			{
				zgrad[i][k] = (double*)malloc(sizeof(double)*MAT_3D_2_LEN);
			}
		}

		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			for (int j = 0; j < MAT_3D_1_LEN; j++)
			{
				for (int k = 0; k < MAT_3D_2_LEN; k++)
				{

					if (i == 0)
					{
						xgrad[i][j][k] = -(H[i + 1][j][k] - H[i][j][k]);
					}
					else if (i == MAT_3D_0_LEN - 1)
					{
						xgrad[i][j][k] = -(H[i][j][k] - H[i - 1][j][k]);
					}
					else
					{
						xgrad[i][j][k] = -((H[i + 1][j][k] - H[i - 1][j][k]) / 2);
					}//x����
					if (j == 0)
					{
						ygrad[i][j][k] = -(H[i][j + 1][k] - H[i][j][k]);
					}
					else if (j == MAT_3D_1_LEN - 1)
					{
						ygrad[i][j][k] = -(H[i][j][k] - H[i][j - 1][k]);
					}
					else
					{
						ygrad[i][j][k] = -((H[i][j + 1][k] - H[i][j - 1][k]) / 2);
					}//y����
					if (k == 0)
					{
						zgrad[i][j][k] = -(H[i][j][k + 1] - H[i][j][k]);
					}
					else if (k == MAT_3D_1_LEN - 1)
					{
						zgrad[i][j][k] = -(H[i][j][k] - H[i][j][k - 1]);
					}
					else
					{
						zgrad[i][j][k] = -((H[i][j][k + 1] - H[i][j][k - 1]) / 2);
					}//z����

				}
			}
		}//�������������ϵ��ݶȾ���,�������ݶ��½�������ȫȡ��
		//double r[MAT_3D_0_LEN * MAT_3D_1_LEN * MAT_3D_2_LEN] = { 0 };
		double *r = (double*)malloc(sizeof(double) * MAT_3D_0_LEN * MAT_3D_1_LEN * MAT_3D_2_LEN);
		//double c[MAT_3D_0_LEN * MAT_3D_1_LEN * MAT_3D_2_LEN] = { 0 };
		double *c = (double*)malloc(sizeof(double) * MAT_3D_0_LEN * MAT_3D_1_LEN * MAT_3D_2_LEN);
		//double h[MAT_3D_0_LEN * MAT_3D_1_LEN * MAT_3D_2_LEN] = { 0 };
		double *h = (double*)malloc(sizeof(double) * MAT_3D_0_LEN * MAT_3D_1_LEN * MAT_3D_2_LEN);
		double rmax = MAT_3D_0_LEN;
		double cmax = MAT_3D_1_LEN;
		double hmax = MAT_3D_2_LEN;

		r[0] = x2;
		c[0] = y2;
		h[0] = z2;
		int iter = 0;
		bool continflag = true;
		while (continflag)
		{
			double dr, dc, dh, dv;
			dr = getGrad(xgrad, r[iter], c[iter], h[iter]);
			dc = getGrad(ygrad, r[iter], c[iter], h[iter]);
			dh = getGrad(zgrad, r[iter], c[iter], h[iter]);
			dv = sqrt(dr * dr + dc * dc + dh * dh);//��Ϊ��λ����
			dr = speed * dr / dv;
			dc = speed * dc / dv;
			dh = speed * dh / dv;
			r[iter + 1] = r[iter] + dr;
			c[iter + 1] = c[iter] + dc;
			h[iter + 1] = h[iter] + dh;
			if (r[iter + 1] < 0)
			{
				r[iter + 1] = 0;
			}
			if (c[iter + 1] < 0)
			{
				c[iter + 1] = 0;
			}
			if (h[iter + 1] < 0)
			{
				h[iter + 1] = 0;
			}
			if (r[iter + 1] > rmax - 1)
			{
				r[iter + 1] = rmax - 1;
			}
			if (c[iter + 1] > cmax - 1)
			{
				c[iter + 1] = cmax - 1;
			}
			if (h[iter + 1] > hmax - 1)
			{
				h[iter + 1] = hmax - 1;
			}//����������������
			if (sqrt(pow((r[iter + 1] - x1), 2) + pow((c[iter + 1] - y1), 2) + pow((h[iter + 1] - z1), 2)) < 1)
			{
				r[iter + 1] = x1;
				c[iter + 1] = y1;
				h[iter + 1] = z1;
				continflag = false;
			}//������㹻����ֹͣ��                  
			iter = iter + 1;
		}//�ݶ��½���������
		//for (int i = iter; i >= 0; i--)
		//{
		//	printf("x����Ϊ��%f  y����Ϊ��%f  z����Ϊ��%f\n", r[i], c[i], h[i]);
		//}
		int infr_Length = 20;
		double infr[20] = { 0 };
		double infc[20] = { 0 };
		double infh[20] = { 0 };//��¼�յ������
		infr[0] = r[iter];
		infc[0] = c[iter];
		infh[0] = h[iter];
		int g = 0;//��¼�յ����                                       
		bool con = true;
		bool ju = true;
		//double test[MAT_3D_0_LEN][MAT_3D_1_LEN][MAT_3D_2_LEN] = { 0 };
		while (con)
		{
			ju = true;
			for (int i = 0; i <= iter; i++)
			{
				if (crossObstacle(r[i], c[i], h[i], r[iter], c[iter], h[iter], mat_3d, cons) && ju)
				{
					ju = false;
					++g;
					infr[g] = r[i];
					infc[g] = c[i];
					infh[g] = h[i];
					iter = i;
					if (i == 0)
					{
						con = false;
					}

				}

			}
		}//  ��¼�յ����ꣻ
		double s[MAX_VERT] = { 0 };//���ƹյ������
		for (int i = 1; i < infr_Length - 1; i++)
		{
			if (infr[i + 1] != 0 || infc[i + 1] != 0 || infh[i + 1] != 0)
			{
				double testx, testy, testz, minest;
				if (infh[i + 1] > infh[i - 1])
				{
					testz = fabs(sqrt(pow((infr[i] - infr[i - 1]), 2) + pow((infc[i] - infc[i - 1]), 2)) / (sqrt(pow((infr[i] - infr[i - 1]), 2) + pow((infc[i] - infc[i - 1]), 2)) + sqrt(pow((infr[i + 1] - infr[i]), 2) + pow((infc[i + 1] - infc[i]), 2))) * fabs(infh[i + 1] - infh[i - 1]) + infh[i - 1] - infh[i]);
				}
				else
				{
					testz = fabs(infh[i - 1] - sqrt(pow((infr[i] - infr[i - 1]), 2) + pow((infc[i] - infc[i - 1]), 2)) / (sqrt(pow((infr[i] - infr[i - 1]), 2) + pow((infc[i] - infc[i - 1]), 2)) + sqrt(pow((infr[i + 1] - infr[i]), 2) + pow((infc[i + 1] - infc[i]), 2))) * fabs(infh[i + 1] - infh[i - 1]) - infh[i]);
				}

				testx = fabs((infc[i] - infc[i + 1]) / (infc[i - 1] - infc[i + 1]) * (infr[i - 1] - infr[i + 1]) + infr[i + 1] - infr[i]);
				testy = fabs((infr[i] - infr[i + 1]) / (infr[i - 1] - infr[i + 1]) * (infc[i - 1] - infc[i + 1]) + infc[i + 1] - infc[i]);
				minest = min(min(testx, testy), testz);
				if (testz == minest)
				{
					for (int j = 0; j < MAX_VERT - 5; j++)
					{
						if (j % 6 == 0)
						{
							s[j] = sqrt(pow((infr[i] - vert[j]), 2) + pow((infc[i] - vert[j + 1]), 2));
							s[j + 2] = sqrt(pow((infr[i] - vert[j + 2]), 2) + pow((infc[i] - vert[j + 3]), 2));
						}
					}
					double mins = s[0];
					int index = 0;
					for (int k = 0; k < MAX_VERT; k++)
					{
						if (s[k] != 0)
						{
							if (s[k] == min(s[k], mins))
							{
								mins = s[k];
								index = k;
							}
						}
					}
					infr[i] = vert[index];
					infc[i] = vert[index + 1];
				}//���Ա��ƹ�
				else
				{

				}//���Ϸ��ƹ�
			}//����������յ�����м�Ĺյ�

		}//���յ���д����������
		double dist1[20] = { 0 };
		int dist1_Length = 20;
		for (int i = 0; i < infr_Length - 1; i++)
		{
			if (infr[i + 1] != 0 || infc[i + 1] != 0 || infh[i + 1] != 0)
			{
				dist1[i] = sqrt(pow((infr[i] - infr[i + 1]), 2) + pow((infc[i] - infc[i + 1]), 2) + pow((infh[i] - infh[i + 1]), 2));
			}
		}
		for (int i = 0; i < dist1_Length; i++)
		{
			printf("dist1[%d]=%f\n", i, dist1[i]);
		}
		for (int i = 0; i < infr_Length; i++)
		{
			dist = dist + dist1[i];
		}

		for (int i = 0; i < 20; i++)
		{
			//printf("�յ㣺x����Ϊ��%f\ty����Ϊ��%f\tz����Ϊ��%f\n", infr[i], infc[i], infh[i]);
		}
		//printf("����֮��ľ���Ϊ%f\n", dist);
		//system("pause");
	}

	return dist;
}

int main(int argc,char **argv)
{
	int rank,numprocs;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	int vert[MAX_VERT] = { 0 };
	int para[MAX_PARA] = { 0 };
	int vert_len = 0;
	int para_len = 0;

	if(rank==0){
		int *** mat_3d = NULL;
			char str[MAX_LEN] = { 0 };
		FILE* sr;
	
		int c = 0;
		mat_3d = (int***)malloc(sizeof(double)*MAT_3D_0_LEN);
		for (int i = 0; i < MAT_3D_0_LEN; i++)
		{
			mat_3d[i] = (int**)malloc(sizeof(double)*MAT_3D_1_LEN);
			for (int k = 0; k < MAT_3D_1_LEN; k++)
			{
				mat_3d[i][k] = (int*)malloc(sizeof(double)*MAT_3D_2_LEN);
			}
		}
		//memset(mat_3d,1,sizeof(mat_3d));
		sr = fopen("vert.txt", "r");
		if (sr == NULL) {//�����ļ�ʧ�����˳�
			puts("can not open file!");
			return 0;
		}

		fgets(str, MAX_LEN, sr);
	
		while (!feof(sr)) {
			fgets(str, MAX_LEN, sr);
			char * token = strtok(str, " ");
			while (token != NULL) {
				vert[vert_len++] = atoi(token);
				token = strtok(NULL, " ");
			}
		}
		fclose(sr);
		//��ȡ��ֱ�ϰ�ǽ������

		sr = fopen("para.txt", "r");
		if (sr == NULL) {//�����ļ�ʧ�����˳�
			puts("can not open file!");
			return 0;
		}

		fgets(str, MAX_LEN, sr);
	
		while (!feof(sr)) {
			fgets(str, MAX_LEN, sr);
			char * token = strtok(str, " ");
			while (token != NULL) {
				para[para_len++] = atoi(token);
				token = strtok(NULL, " ");
			}
		}

		fclose(sr);
		double end,start;
		//��ȡƽ���ϰ�ǽ������
	
		athread_init();
		athread_set_num_threads(nums);
		/*
		__real_athread_spawn((void *)func,0);	
		 */
		start=MPI_Wtime();
		for (int i = 0; i < vert_len - 5; i++)
		{
			if (i % 6 == 0)
			{
				int h;
				h = vert[i + 5] - vert[i + 4] + 1;//ǽ�ĸ߶�
				if (vert[i + 1] == vert[i + 3])
				{
					if (vert[i] < vert[i + 2])
					{
						for (int j = vert[i + 4]; j < h; j++)
						{
							for (int k = vert[i]; k <= vert[i + 2]; k++)
							{
								mat_3d[k][vert[i + 1]][j] = 1;
							}
						}
					}
					else
					{
						for (int j = vert[i + 4]; j < h; j++)
						{
							for (int k = vert[i + 2]; k <= vert[i]; k++)
							{
								mat_3d[k][vert[i + 1]][j] = 1;
							}
						}
					}
				}//��ֱ��Y����ϰ�ǽ��Ҫ��ȷ��ǽ�������Y��Z������ͬ
				else if (vert[i] == vert[i + 2])
				{
					if (vert[i + 1] < vert[i + 3])
					{
						for (int j = vert[i + 4]; j < h; j++)
						{
							for (int k = vert[i + 1]; k <= vert[i + 3]; k++)
							{
								mat_3d[vert[i]][k][j] = 1;
							}
						}
					}
					else
					{
						for (int j = vert[i + 4]; j < h; j++)
						{
							for (int k = vert[i + 3]; k <= vert[i + 1]; k++)
							{
								mat_3d[vert[i]][k][j] = 1;
							}
						}
					}
				}//��ֱ��X����ϰ�ǽ��Ҫ��ȷ��ǽ�������X��Z������ͬ
				else
				{
					double a = (double)(vert[i + 1] - vert[i + 3]) / (vert[i] - vert[i + 2]);
					double b = (double)(vert[i + 1] - a * vert[i]);
					if (fabs(vert[i + 1] - vert[i + 3]) <= fabs(vert[i] - vert[i + 2]))
					{
						if (vert[i] < vert[i + 2])
						{
							for (int j = vert[i + 4]; j < h; j++)
							{
								for (int k = vert[i]; k <= vert[i + 2]; k++)
								{
									mat_3d[k][(int)(a * k + b + 0.5)][j] = 1;
									mat_3d[k + 1][(int)(a * k + b + 0.5)][j] = 1;//��ֹ��ɢ���ϰ�ǽ

								}
							}
						}
						else
						{
							for (int j = vert[i + 4]; j < h; j++)
							{
								for (int k = vert[i + 2]; k <= vert[i]; k++)
								{
									mat_3d[k][(int)(a * k + b + 0.5)][j] = 1;
									mat_3d[k + 1][(int)(a * k + b + 0.5)][j] = 1;//��ֹ��ɢ���ϰ�ǽ

								}
							}
						}
					}
					else
					{
						if (vert[i + 1] < vert[i + 3])
						{
							for (int j = vert[i + 4]; j < h; j++)
							{
								for (int k = vert[i + 1]; k <= vert[i + 3]; k++)
								{
									mat_3d[(int)((k - b) / a + 0.5)][k][j] = 1;
									mat_3d[(int)((k - b) / a + 0.5)][k + 1][j] = 1;//��ֹ��ɢ���ϰ�ǽ

								}
							}
						}
						else
						{
							for (int j = vert[i + 4]; j < h; j++)
							{
								for (int k = vert[i + 3]; k <= vert[i + 1]; k++)
								{
									mat_3d[(int)((k - b) / a + 0.5)][k][j] = 1;
									mat_3d[(int)((k - b) / a + 0.5)][k + 1][j] = 1;//��ֹ��ɢ���ϰ�ǽ

								}
							}
						}
					}

				}//б���ϰ�ǽ��Ҫ��Z��ͬ
			}//�ϰ������ڴ���Ϊ1  


		}

		for (int i = 0; i < para_len - 5; i++)
		{
			if (para[i] == para[i + 2])
			{
				if (para[i + 1] < para[i + 3])
				{
					for (int k = para[i] - para[i + 5] / 2; k <= para[i] + para[i + 5] / 2; k++)
					{
						for (int j = para[i + 1]; j <= para[i + 3]; j++)
						{
							mat_3d[k][j][para[i + 4]] = 1;
						}
					}
				}
				else
				{
					for (int k = para[i] - para[i + 5] / 2; k <= para[i] + para[i + 5] / 2; k++)
					{
						for (int j = para[i + 3]; j <= para[i + 1]; j++)
						{
							mat_3d[k][j][para[i + 4]] = 1;
						}
					}

				}

			}
		}//ƽ���ڵ�����ϰ�ǽ ��ʱҪ�����������һ��

	}	
	for (int i = 0; i < MAT_3D_0_LEN; i++)
	{
		for (int j = 0; j < MAT_3D_1_LEN; j++)
		{
			for (int k = 0; k < MAT_3D_2_LEN; k++)
			{
				if (mat_3d[i][j][k] == 1)
				{
					mat_3d[i][j][k] = 0;
					c++;
				}
				else
				{

					mat_3d[i][j][k] = 1;

				}

			}
		}
	}//ʹ�ϰ��ﴦΪ0

	double dist = findPath(10, 5, 5, 15, 80, 5, vert, mat_3d, c);
	
	printf("dist = %lf", dist);
	end=MPI_Wtime();
	printf("\ttime:%lf\n");

	athread_halt();
	MPI_Finalize();
	//system("pause");
	
	return 0;
}
