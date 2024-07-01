#ifndef SYNCPLYAPI_H
#define SYNCPLYAPI_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <array>
#include <unordered_map>
#include<fstream>

#define VOXEL_SIZE 0.004
struct CartesianPoint
{
    double x, y, z, intensity;
    CartesianPoint & operator+=(const CartesianPoint &);
    CartesianPoint operator/(int) const;
};

class SyncPlyApi
{
public:
    SyncPlyApi();
    void SavePly(std::ofstream &fout_pc_name, const std::vector<CartesianPoint> points);
    std::vector<CartesianPoint> myXYZData;
    void downSamplePoint(int x,int y);
    std::vector<CartesianPoint> downsamplePointCloud(const std::vector<CartesianPoint> & pointCloud,int x, int y);
private:

};

#endif // SYNCPLYAPI_H
