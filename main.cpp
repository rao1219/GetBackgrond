#pragma comment(linker, "/STACK:102400000,102400000")

#include <iostream>  
#include <cmath>
#include <fstream>
#include <io.h>
#include <cstring>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  


#define LIMIT 8
using namespace cv;
using namespace std;


int dir[][2] = { 1, 0, 0, 1, -1, 0, 0, -1,1,1, -1,1, 1,-1, -1,-1 };

class GetBackground
{
public:
	String img_name;
	String res_dir;
	int label;
	int x_bound, y_bound;
	int count;
	bool **vis;
	Mat img, channel;
	GetBackground(){}
	GetBackground(String name,String wkdir,int la)
	{
		label = la;
		res_dir = wkdir;
		img_name = name;
		img = imread(name);
		channel = Mat(img.rows, img.cols, CV_8UC3, Scalar(255, 255, 255));
		x_bound = img.rows, y_bound = img.cols;
		count = 0;
		vis = new bool*[x_bound];
		for (int i = 0; i < x_bound; i++)
		{
			vis[i] = new bool[y_bound];
			for (int j = 0; j < y_bound; j++)
			{
				vis[i][j] = false;
			}
		}

		std::cout << x_bound << "\t" << y_bound << std::endl;
		//fill(vis, vis + x_bound*y_bound, (bool)0);
		//memset(vis, 0, sizeof(bool)*x_bound*y_bound);
		//memset(vis, 0, sizeof(vis));
		//process();
		//cout << vis[0][0] << endl;
	}


	float getDistance(Vec3b p1, Vec3b p2)
	{
		return abs(p1.val[0] - p2.val[0])
			+ abs(p1.val[1] - p2.val[1])
			+ abs(p1.val[2] - p2.val[2]);
	}
	float getAbsolutDistance(Vec3b p)
	{
		Vec3b white;
		white.val[0] = 0, white.val[1] = 0, white.val[2] = 0;
		return getDistance(p, white);
	}
	bool check_bound(int cx, int cy)
	{
		//cout << x_bound << "\t" << y_bound << endl;
		//cout << cx << "\t" << cy << endl;
		return (cx >= 0 && cx < x_bound&&cy < y_bound&&cy >= 0);
	}

	void dfs(int x, int y)
	{
		channel.at<Vec3b>(x, y)[0] = 0;
		channel.at<Vec3b>(x, y)[1] = 0;
		vis[x][y] = 1;
		for (int i = 0; i < 8; i++)
		{
			int cx = x + dir[i][0];
			int cy = y + dir[i][1];
			if (!check_bound(cx, cy) || vis[cx][cy]) continue;
			Vec3b currP = img.at<Vec3b>(x, y), surrP = img.at<Vec3b>(cx, cy);
			double dist = getDistance(currP, surrP);
			if (dist > LIMIT) continue;
			count++;
			dfs(cx, cy);
		}
	}

	void process()
	{
		dfs(0, 0);
		//for (int i = 0; i < img.rows; i++)
		//{
		//	for (int j = 0; j < img.cols - 1; j++)
		//	{
		//		Vec3b p = img.at<Vec3b>(i, j);
		//		Vec3b p2 = img.at<Vec3b>(i, j + 1);

		//		//std::cout << p<<"\t"<<p2 << std::endl;
		//		//std::cout << getAbsolutDistance(p) << std::endl;
		//		if (getAbsolutDistance(p)>LIMIT || (getAbsolutDistance(p2)>LIMIT&&getDistance(p, p2)<30))
		//		{
		//			count++;
		//			channel.at<Vec3b>(i, j)[0] = 0;
		//			channel.at<Vec3b>(i, j)[1] = 0;
		//		}
		//		//getchar();
		//	}
		//}

		double resultRate = count*1.0 / (img.rows*img.cols);
		std::cout << count << std::endl;
		std::cout << resultRate << std::endl;

		std::fstream out;
		out.open(res_dir+"\\result.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		out << resultRate << std::endl;
		out.close();
		imwrite(res_dir + "\\" + std::to_string(label)+".png", channel);

	}
	
	void present()
	{
		imshow("origin", img);
		imshow("after", channel);
		//imwrite("out.png", channel);
		waitKey(1000000);
	}

};

void getFiles(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int main()
{
	
	String name, wkdir;
	String myclass = "bandage_1";
	int _size = 49;
	for (int i = 1; i <= _size; i++)
	{
		//int a; cin >> a;
		name = "flow_viz\\", wkdir = "result\\";
		name += myclass, wkdir += myclass;
		name += "\\frame_00";

		if (i < 10)
			name += "0";
		name += std::to_string(i);
		name += ".png";
		std::cout << name << std::endl;

		GetBackground tmp = GetBackground(name, wkdir, i);
		tmp.process();
		//tmp.present();

	}

	return 0;
}

//int main()
//{
//	vector<string> files;
//	char * oripath = ".\\flow_viz\\";
//	getFiles(oripath, files);
//	int size = files.size();
//	cout << size << endl;
//	int a; cin >> a;
//	for (int j = 0; j < size; j++)
//	{
//		String name, wkdir;
//		vector<string> frames;
//		String myclass = files.at(j);
//		getFiles(oripath+myclass+"\\", frames);
//		int _size = frames.size();
//		for (int i = 1; i <= _size; i++)
//		{
//			//int a; cin >> a;
//			name = "flow_viz\\", wkdir = "result\\";
//			name += myclass, wkdir += myclass;
//			name += "\\frame_00";
//
//			if (i < 10)
//				name += "0";
//			name += std::to_string(i);
//			name += ".png";
//			std::cout << name << std::endl;
//
//			GetBackground tmp = GetBackground(name, wkdir, i);
//			tmp.process();
//			//tmp.present();
//
//		}
//
//	}
//	
//	return 0;
//}