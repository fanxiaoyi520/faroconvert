#include "syncplyapi.h"


CartesianPoint & CartesianPoint::operator+=(const CartesianPoint & p) {
	x += p.x;
	y += p.y;
	z += p.z;
	intensity += p.intensity;

	return *this;
}

CartesianPoint CartesianPoint::operator/(int count) const {

	auto d = (float)count;

	CartesianPoint cp{ x / d, y / d, z / d, intensity / d };

	return cp;
}

SyncPlyApi::SyncPlyApi()
{

}

void SyncPlyApi::downSamplePoint(int x, int y) {
	this->myXYZData = downsamplePointCloud(this->myXYZData, x, y);
}

typedef unsigned long long int key_data_type;
std::vector<CartesianPoint> SyncPlyApi::downsamplePointCloud(const std::vector<CartesianPoint> & pointCloud, int x, int y) {

	std::unordered_map<key_data_type, std::pair<CartesianPoint, int>> voxels_map;

	int y_bin_max = int((2 * x) / VOXEL_SIZE);
	int z_bin_max = int((2 * y) / VOXEL_SIZE);

	int y_binary_len = 1;
	int z_binary_len = 1;

	while (y_bin_max >>= 1) {
		y_binary_len++;
	}

	while (z_bin_max >>= 1) {
		z_binary_len++;
	}

	int x_move_binary = y_binary_len + z_binary_len;
	int y_move_binary = y_binary_len;

	for (const auto& point : pointCloud) {
		if (point.x <= (float)-x || point.x >= (float)x ||
			point.y <= (float)-x || point.y >= (float)x ||
			point.z <= (float)-y || point.z >= (float)y) {
			continue;
		}

		key_data_type x_bin = int((point.x + (float)x) / VOXEL_SIZE);
		key_data_type y_bin = int((point.y + (float)x) / VOXEL_SIZE);
		key_data_type z_bin = int((point.z + (float)y) / VOXEL_SIZE);

		x_bin <<= x_move_binary;
		y_bin <<= y_move_binary;

		auto key = (x_bin | y_bin | z_bin);

		auto it = voxels_map.find(key);
		//未找到元素会返回 end()
		if (it == voxels_map.end()) {
			//新添加point元素
			voxels_map[key] = std::pair<CartesianPoint, int>(point, 1);
		}
		else {
			//已存在元素 累加
			it->second.first += point;
			it->second.second++;
		}
	}

	std::vector<CartesianPoint> downPts;
	downPts.reserve(voxels_map.size());

	for (auto const & voxel_points : voxels_map) {
		downPts.push_back(voxel_points.second.first / voxel_points.second.second);
	}
	return downPts;
}

void SyncPlyApi::SavePly(std::ofstream &fout_pc_name, const std::vector<CartesianPoint> points) {
	int points_len = points.size();
	fout_pc_name << "ply" << std::endl;
	fout_pc_name << "format ascii 1.0" << std::endl;
	fout_pc_name << "element vertex " << points_len << std::endl;
	fout_pc_name << "property float x" << std::endl;
	fout_pc_name << "property float y" << std::endl;
	fout_pc_name << "property float z" << std::endl;
	fout_pc_name << "property float intensity" << std::endl;
	fout_pc_name << "element face 0" << std::endl;
	fout_pc_name << "property list uchar int vertex_index" << std::endl;
	fout_pc_name << "end_header" << std::endl;

	for (int n = 0; n < points_len; n++) {
		fout_pc_name << points[n].x << " " << points[n].y << " " << points[n].z << " "
			<< points[n].intensity << "\n";
	}
	fout_pc_name.close();
}
