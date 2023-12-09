void kernel Convolution3D(global double* X, global double* K, global double* Y, int XSizeX, int XSizeY,int XSizeZ, int KSizeX, int KSizeY,int KSizeZ, int YSizeX, int YSizeY, int YSizeZ, int StrideX, int StrideY, int StrideZ, int PaddingX, int PaddingY,int PaddingZ)
{
    int gid_x = get_global_id(0);
    int gid_y = get_global_id(1);
	int gid_z = get_global_id(2);
    int threadCount_x = get_global_size(0);
    int threadCount_y = get_global_size(1);
	int threadCount_z = get_global_size(2);
    
    // Starting index of the output for this thread
    int StartingOutput_x = gid_x * (YSizeX / threadCount_x);
    int StartingOutput_y = gid_y * (YSizeY / threadCount_y);
	int StartingOutput_z = gid_z * (YSizeZ / threadCount_z);
    
    // Number of outputs to calculate for this thread
    int OutputsToCalculate_x = (YSizeX / threadCount_x);
    int OutputsToCalculate_y = (YSizeY / threadCount_y);
	int OutputsToCalculate_z = (YSizeZ / threadCount_z);
    
    if (gid_x == threadCount_x - 1) {
        OutputsToCalculate_x += YSizeX % threadCount_x;
    }
    if (gid_y == threadCount_y - 1) {
        OutputsToCalculate_y += YSizeY % threadCount_y;
    }
	if (gid_z == threadCount_z - 1) {
		OutputsToCalculate_z += YSizeZ % threadCount_z;
	}

	
	for (int x = StartingOutput_x; x < StartingOutput_x + OutputsToCalculate_x; x++) {
		for (int y = StartingOutput_y; y < StartingOutput_y + OutputsToCalculate_y; y++) {
			for(int z = StartingOutput_z; z < StartingOutput_z + OutputsToCalculate_z; z++){
				int index = z * YSizeX * YSizeY + y * YSizeX + x;
				Y[index] = 0;

				for (int k = 0; k < KSizeX; k++) {
					for (int l = 0; l < KSizeY; l++) {
						for (int m = 0; m < KSizeZ; m++) {
							int kernelIndex = m * KSizeX * KSizeY + l * KSizeX + k;
							
							// Calculate the input index based on the output and kernel positions
							int InputIndexX = x * StrideX - PaddingX + k;
							int InputIndexY = y * StrideY - PaddingY + l;
							int InputIndexZ = z * StrideZ - PaddingZ + m;
							
							// Check if the input index is within the bounds of the input data
							if (InputIndexX >= 0 && InputIndexX < XSizeX && InputIndexY >= 0 && InputIndexY < XSizeY) {
								int InputIndex = InputIndexZ * XSizeX * XSizeY + InputIndexY * XSizeX + InputIndexX;
								Y[index] += X[InputIndex] * K[kernelIndex];
							}
						}
					}
				}
			}
		}
	}
}


void kernel Convolution3DBackProp(global const double* X, global double* K, global double* ZPrime,global double* dY_dX, const int XSizeX, const int XSizeY, const int XSizeZ, const int KSizeX, const int KSizeY, const int KSizeZ, const int YSizeX, const int YSizeY, const int YSizeZ, const int StrideX, const int StrideY, const int StrideZ, const int PaddingX, const int PaddingY, const int PaddingZ)
{
    int gid_x = get_global_id(0);
    int gid_y = get_global_id(1);
	int gid_z = get_global_id(2);
    int threadCount_x = get_global_size(0);
    int threadCount_y = get_global_size(1);
	int threadCount_z = get_global_size(2);
    
    // Starting index of the output for this thread
    int StartingOutput_x = gid_x * (YSizeX / threadCount_x);
    int StartingOutput_y = gid_y * (YSizeY / threadCount_y);
	int StartingOutput_z = gid_z * (YSizeZ / threadCount_z);
    
    // Number of outputs to calculate for this thread
    int OutputsToCalculate_x = (YSizeX / threadCount_x);
    int OutputsToCalculate_y = (YSizeY / threadCount_y);
	int OutputsToCalculate_z = (YSizeZ / threadCount_z);
    
    if (gid_x == threadCount_x - 1) {
        OutputsToCalculate_x += YSizeX % threadCount_x;
    }
    if (gid_y == threadCount_y - 1) {
        OutputsToCalculate_y += YSizeY % threadCount_y;
    }
	if (gid_z == threadCount_z - 1) {
		OutputsToCalculate_z += YSizeZ % threadCount_z;
	}

    for (int x = StartingOutput_x; x < StartingOutput_x + OutputsToCalculate_x; x++) {
        for (int y = StartingOutput_y; y < StartingOutput_y + OutputsToCalculate_y; y++) {
			for (int z = StartingOutput_z; z < StartingOutput_z + OutputsToCalculate_z; z++) {
				for (int k = 0; k < KSizeX; k++) {
					for (int l = 0; l < KSizeY; l++) {
						for(int m=0; m< KSizeZ; m++){
							int kernelIndex = m * KSizeX * KSizeY + l * KSizeX + k;
							
							// Calculate the input index based on the output and kernel positions
							int InputIndexX = x * StrideX - PaddingX + k;
							int InputIndexY = y * StrideY - PaddingY + l;
							int InputIndexZ = z * StrideZ - PaddingZ + m;
							
							// Check if the input index is within the bounds of the input data
							if (InputIndexX >= 0 && InputIndexX < XSizeX && InputIndexY >= 0 && InputIndexY < XSizeY) {
								int InputIndex = InputIndexZ * XSizeX * XSizeY + InputIndexY * XSizeX + InputIndexX;
								dY_dX[InputIndex] += ZPrime[z * YSizeX * YSizeY + y * YSizeX + x] * K[kernelIndex];
								K[kernelIndex] += ZPrime[z * YSizeX * YSizeY + y * YSizeX + x] * X[InputIndex];
							}
						}
					}
				}
			}
        }
    }
}