#define _DEBUG


#ifdef DEBUG
#pragma OPENCL EXTENSION cl_amd_printf : enable
#endif
void kernel Forward(global double* X, int XSize, global double* W, int WSizeX, int WSizeY, global double* B, int BSize, global double* Z, int ZSize)
{
	int gid = get_global_id(0);
	int threads = get_global_size(0);
	//starting index for this thread
	int startingIndex = gid * (ZSize/threads);
	//number of z's to calculate
	int Zs_to_calculate = ZSize / threads;
	if(gid==threads-1)
	{
		Zs_to_calculate += ZSize % threads;
	}

	for(int i=startingIndex; i<startingIndex+Zs_to_calculate; i++)
	{
		#ifdef DEBUG
		printf("\n");
		#endif
		Z[i] = 0;
		for(int j=0; j<XSize-1; j++)
		{
			#ifdef DEBUG
			printf("X[%d] = %3.2f\n", j, X[j]);
			printf("W[%d] = %3.2f\n", j*WSizeX + i, W[j*WSizeX + i]);
			#endif
			Z[i] += X[j] * W[j*WSizeX + i];
		}
		#ifdef DEBUG
		printf("B[%d] = %3.2f\n", i, B[i]);
		#endif
		Z[i] += B[i];
		#ifdef DEBUG
		printf("Z[%d] = %3.2f\n", i, Z[i]);
		#endif
	}
}

void kernel Backward(global double* X, int XSize, global double* W, int WSizeX, int WSizeY, global double* B, int BSize, global double* ZPrime, int ZPrimeSize, global double* XPrime, int XPrimeSize, int GradientClipping, double GradientMax, double GradientMin)
{
	int gid=get_global_id(0);
	int threads = get_global_size(0);
	//starting index for this thread
	int startingIndex = gid * (ZPrimeSize/threads);
	//number of x's to calculate
	int Zs_to_calculate = ZPrimeSize / threads;
	if(gid==threads-1)
	{
		Zs_to_calculate += ZPrimeSize % threads;
	}

	for(int i=startingIndex; i<startingIndex+Zs_to_calculate; i++)
	{
		#ifdef DEBUG
		printf("\n");
		#endif
		for(int j=0; j<XSize-1; j++)
		{
			XPrime[j] += W[j*WSizeX + i] * ZPrime[i];
			W[j*WSizeX + i] += X[j] * ZPrime[i];
			if(GradientClipping==1)
			{
				if(fabs(XPrime[j]) > GradientMax)
				{
					XPrime[j] = GradientMax;
				}
				else if(fabs(XPrime[j]) < GradientMin)
				{
					XPrime[j] = GradientMin;
				}
				if(fabs(W[j*WSizeX + i]) > GradientMax)
				{
					W[j*WSizeX + i] = GradientMax;
				}
				else if(fabs(W[j*WSizeX + i]) < GradientMin)
				{
					W[j*WSizeX + i] = GradientMin;
				}
			}

			#ifdef DEBUG
			printf("XPrime[%d] = %3.2f\n", j, XPrime[j]);
			printf("W[%d] = %3.2f\n", j*WSizeX + i, W[j*WSizeX + i]);
			#endif
		}
		B[i] += ZPrime[i];
		if(GradientClipping==1)
		{
			if(fabs(B[i]) > GradientMax)
			{
				B[i] = GradientMax;
			}
			else if(fabs(B[i]) < GradientMin)
			{
				B[i] = GradientMin;
			}
		}
		#ifdef DEBUG
		printf("B[%d] = %3.2f\n", i, B[i]);
		#endif
	}
}