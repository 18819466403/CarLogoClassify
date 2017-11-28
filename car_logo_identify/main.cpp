#include"project.h"
using namespace rapidjson;
using namespace std;
void split_char(char *ch, vector<string>& vec, char c) {
	for (int i = 0, j = 0; i < strlen(ch); i++) {
		if (ch[i] == c) {
			string str_tmp(ch + j, i - j);
			vec.push_back(str_tmp);
			j = i + 1;
		}
	}
}
void generate_dir(string dir_path, bool MAKE_DIR_FLAG) {
	string dir = dir_path;
	if (_access(dir.c_str(), 0) == -1)
	{
		std::cout << dir << " is not existing" << std::endl;
		std::cout << "now make it" << std::endl;
#ifdef WIN32  
		int flag = _mkdir(dir.c_str());
#endif  
#ifdef linux   
		int flag = mkdir(dir.c_str(), 0777);
#endif  
		if (flag == 0)
		{
			std::cout << "make successfully" << std::endl;
		}
		else {
			std::cout << "make errorly" << std::endl;
		}
	}

	if (_access(dir.c_str(), 0) == 0 && !MAKE_DIR_FLAG)
	{
		std::cout << dir << " exists" << std::endl;
		std::cout << "now delete it" << std::endl;
		int flag = _rmdir(dir.c_str());
		if (flag == 0)
		{
			std::cout << "delete it successfully" << std::endl;
		}
		else {
			std::cout << "delete it errorly" << std::endl;
		}
	}


}

/**@ state:
       path_flag ==0 just return file name
	   path_flag ==1 return absolute path of all files

	   sub_dir ==0 without search sub_dir;
	   sub_dir ==1 search sub_dir;
*/
void getAllFiles(string path, vector<string>& files, int path_flag = 0, int sub_dir = 0) {
	//文件句柄
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR) && !sub_dir) {  //比较文件类型是否是文件夹
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					if (!path_flag)
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					else
						files.push_back(fileinfo.name);
					getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files, path_flag, sub_dir);
				}

			}
			else {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					if (path_flag)
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					else
						files.push_back(fileinfo.name);

				}
			}

		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
}

LPCWSTR string_to_LPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

int get_file_count(string dir_path) {
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;
	int iCount = 0;//统计文件数的变量

				   //m_strDir就是你要指定的路径
	hFind = FindFirstFile(string_to_LPCWSTR(dir_path + "/\*.*"), &dataFind);//找到路径中所有文件

																			//遍历路径中所有文件
	while (hFind != INVALID_HANDLE_VALUE&&bMoreFiles == TRUE)
	{
		if (dataFind.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)//判断是否是文件
		{
			iCount++;
		}
		bMoreFiles = FindNextFile(hFind, &dataFind);
	}
	FindClose(hFind);
	return iCount;
}

void WinToUnix(const char *strIn, const char *strOut) {
	FILE *fpIn;
	FILE *fpOut;

	fopen_s(&fpIn, strIn, "rb");
	fopen_s(&fpOut, strOut, "wb");
	if (fpIn == NULL || fpOut == NULL) return;

	char ch;
	while ((ch = fgetc(fpIn)) != EOF) {
		if (ch != '\r') {
			if (ch == ' ' && fgetc(fpIn)== ' '&& fgetc(fpIn) == ' '&& fgetc(fpIn) == ' ')
				ch = '\t';
			fputc(ch, fpOut);
		}
	}

	fclose(fpIn);
	fclose(fpOut);
}

void extract_train_json(string dir_path,string train_json_file ,bool _MAKE_FILE_flag) {
	FILE* fp = fopen(train_json_file.data(), "rb");
	char read_buffer[_BUFFER_SIZE];
	FileReadStream is(fp, read_buffer, sizeof(read_buffer));
	Document doc;
	doc.ParseStream<0>(is);
	fclose(fp);
	size_t count = doc.Size();
	if (doc.HasParseError())
	{
		printf("GetParseError %s\n", doc.GetParseError());
	}

	vector<vector<string>> label_id(count);
	vector<string> image_id(count);
	vector<vector<int>> bbox(count);
	for (int i = 0; i < count; i++) {
		Value& v1 = doc[i];
		image_id[i] = v1["image_id"].GetString();
		Value& v2 = v1["items"];
		for (int j = 0; j < v2.Size(); j++) {
			Value& v3 = v2[j];
			Value& v4 = v3["bbox"];
			Value& v5 = v3["label_id"];
			label_id[i].push_back(v5.GetString());
			bbox[i].push_back(v4[0].GetInt());
			bbox[i].push_back(v4[1].GetInt());
			bbox[i].push_back(v4[2].GetInt());
			bbox[i].push_back(v4[3].GetInt());
		}
	}

	generate_dir(dir_path, _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\JPEGImages", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\Annotations", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\ImageSets", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\ImageSets\\Main", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\ImageSets\\Layout", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\ImageSets\\Segmentation", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\SegmentationClass", _MAKE_DIR_RETAIN_EXIT_DIR);
	generate_dir(dir_path + "\\SegmentationObject", _MAKE_DIR_RETAIN_EXIT_DIR);

	//int min_x = abs(bbox[0][0] - bbox[0][2]);
	//int x_th = 0;
	//int y_th = 0;
	//for (int i = 1; i < doc.Size(); i++) {
	//	if (min_x > abs(bbox[i][0] - bbox[i][2])) {
	//		min_x = abs(bbox[i][0] - bbox[i][2]);
	//		x_th = i;
	//	}
	//}
	//int min_y = abs(bbox[0][1] - bbox[0][3]);
	//for (int i = 1; i < doc.Size(); i++) {
	//	if (min_y > abs(bbox[i][1] - bbox[i][3])) {
	//		min_y = abs(bbox[i][1] - bbox[i][3]);
	//		y_th = i;
	//	}
	//}
	//cout << min_x << " " << min_y << endl;
	//cout << " " << bbox[x_th][0] << " " << bbox[x_th][1] << " " << bbox[x_th][2] << " " << bbox[x_th][3] << endl;
	//cout << " " << bbox[y_th][0] << " " << bbox[y_th][1] << " " << bbox[y_th][3] << " " << bbox[y_th][3] << endl;


	std::cout << '\n' << "Generate xml flies..." << std::endl;
	for (size_t i = 0; i < count; i++) {
		string xml_file = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
			xml_file += "0";
		xml_file += to_string(i + 1);
		string picture_name = xml_file + ".jpg";
		xml_file += ".xml";
		
		TiXmlDocument doc;
		TiXmlElement* level_1 = new TiXmlElement("annotation");

		TiXmlElement* level_1_1 = new TiXmlElement("folder");
		level_1_1->LinkEndChild(new TiXmlText("VOC2007"));
		level_1->LinkEndChild(level_1_1);

		TiXmlElement* level_1_2 = new TiXmlElement("filename");
		level_1_2->LinkEndChild(new TiXmlText(picture_name.data()));
		level_1->LinkEndChild(level_1_2);

		TiXmlElement* level_1_3 = new TiXmlElement("source");
		TiXmlElement* level_1_3_1 = new TiXmlElement("database");
		level_1_3_1->LinkEndChild(new TiXmlText("The CCF2017 Database"));
		level_1_3->LinkEndChild(level_1_3_1);
		TiXmlElement* level_1_3_2 = new TiXmlElement("annotation");
		level_1_3_2->LinkEndChild(new TiXmlText("PASCAL CCF2017"));
		level_1_3->LinkEndChild(level_1_3_2);
		TiXmlElement* level_1_3_3 = new TiXmlElement("image");
		level_1_3_3->LinkEndChild(new TiXmlText("flickr"));
		level_1_3->LinkEndChild(level_1_3_3);



		TiXmlElement* level_1_3_4 = new TiXmlElement("flickrid");
		level_1_3_4->LinkEndChild(new TiXmlText("000000000"));
		level_1_3->LinkEndChild(level_1_3_4);
		level_1->LinkEndChild(level_1_3);

		TiXmlElement* level_1_4 = new TiXmlElement("owner");
		TiXmlElement* level_1_4_1 = new TiXmlElement("flickrid");
		level_1_4_1->LinkEndChild(new TiXmlText("?"));
		level_1_4->LinkEndChild(level_1_4_1);
		TiXmlElement* level_1_4_2 = new TiXmlElement("name");
		level_1_4_2->LinkEndChild(new TiXmlText("?"));
		level_1_4->LinkEndChild(level_1_4_2);
		level_1->LinkEndChild(level_1_4);

		Image img("K:\\参赛\\BDCI2017-gsum\\训练图像及标注json\\标注图像\\" + image_id[i]);
		img.loadImage(1);
		TiXmlElement* level_1_5 = new TiXmlElement("size");
		TiXmlElement* level_1_5_1 = new TiXmlElement("width");
		level_1_5_1->LinkEndChild(new TiXmlText(to_string(img.getImage().cols).data()));
		level_1_5->LinkEndChild(level_1_5_1);
		TiXmlElement* level_1_5_2 = new TiXmlElement("height");
		level_1_5_2->LinkEndChild(new TiXmlText(to_string(img.getImage().rows).data()));
		level_1_5->LinkEndChild(level_1_5_2);
		TiXmlElement* level_1_5_3 = new TiXmlElement("depth");
		level_1_5_3->LinkEndChild(new TiXmlText("3"));
		level_1_5->LinkEndChild(level_1_5_3);
		level_1->LinkEndChild(level_1_5);

		TiXmlElement* level_1_6 = new TiXmlElement("segmented");
		level_1_6->LinkEndChild(new TiXmlText("0"));
		level_1->LinkEndChild(level_1_6);

		for (size_t object_cout = 0; object_cout < label_id[i].size(); object_cout++) {
			TiXmlElement* level_1_7 = new TiXmlElement("object");
			TiXmlElement* level_1_7_1 = new TiXmlElement("name");
			level_1_7_1->LinkEndChild(new TiXmlText(label_id[i][object_cout].data()));
			level_1_7->LinkEndChild(level_1_7_1);
			TiXmlElement* level_1_7_2 = new TiXmlElement("pose");
			level_1_7_2->LinkEndChild(new TiXmlText("Unspecified"));
			level_1_7->LinkEndChild(level_1_7_2);
			TiXmlElement* level_1_7_3 = new TiXmlElement("truncated");
			level_1_7_3->LinkEndChild(new TiXmlText("0"));
			level_1_7->LinkEndChild(level_1_7_3);
			TiXmlElement* level_1_7_4 = new TiXmlElement("difficult");
			level_1_7_4->LinkEndChild(new TiXmlText("0"));
			level_1_7->LinkEndChild(level_1_7_4);
			TiXmlElement* level_1_7_5 = new TiXmlElement("bndbox");
			TiXmlElement* level_1_7_5_1 = new TiXmlElement("xmin");
			level_1_7_5_1->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_1);
			TiXmlElement* level_1_7_5_2 = new TiXmlElement("ymin");
			level_1_7_5_2->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 1]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_2);
			TiXmlElement* level_1_7_5_3 = new TiXmlElement("xmax");
			level_1_7_5_3->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 2]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_3);
			TiXmlElement* level_1_7_5_4 = new TiXmlElement("ymax");
			level_1_7_5_4->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 3]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_4);
			level_1_7->LinkEndChild(level_1_7_5);
			level_1->LinkEndChild(level_1_7);
		}
		doc.LinkEndChild(level_1);
		string file_path = "..\\Annotations\\" + xml_file;
		if (_access(file_path.c_str(), 0) == -1) {
			doc.SaveFile(file_path.data());
		}else{
			remove(file_path.data());
			doc.SaveFile(file_path.data());
		}

		string chage_file = dir_path + "\\Annotations\\" + xml_file;
		WinToUnix(file_path.data(), chage_file.data());
	}
	ofstream out;
	std::cout << "Generate txt file..." << '\n';

	vector<int> data_index;
	vector<int> trainval(count*_TRAINVAL_PERCENT);
	vector<int> test(count - trainval.size());
	vector<int> train(trainval.size()*_TRAIN_PERCENT);
	vector<int> val(trainval.size() - train.size());
	for (int i = 0; i < count; i++) {
		data_index.push_back(i + 1);
	}
	part_vec(data_index, test, trainval, true);
	part_vec(trainval, train, val, true);
	string trainval_file_name = dir_path + "\\ImageSets\\Main\\trainval.txt";
	string train_file_name = dir_path + "\\ImageSets\\Main\\train.txt";
	string val_file_name = dir_path + "\\ImageSets\\Main\\val.txt";
	string test_file_name = dir_path + "\\ImageSets\\Main\\test.txt";
	//std::cout << count<<" "<< count*_TRAINVAL_PERCENT <<" "<<trainval.size() << std::endl;

	out.open(trainval_file_name, ios::out);
	for (int i = 0; i < trainval.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(trainval[i]).length(); j++)
			str += "0";
		str += to_string(trainval[i]);
		out << str<<'\n';
	}
	out.close();
	out.open(train_file_name, ios::out);
	for (int i = 0; i < train.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(train[i]).length(); j++)
			str += "0";
		str += to_string(train[i]);
		out << str << '\n';
	}
	out.close();
	out.open(val_file_name, ios::out);
	for (int i = 0; i < val.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(val[i]).length(); j++)
			str += "0";
		str += to_string(val[i]);
		out << str << '\n';
	}
	out.close();
	out.open(test_file_name, ios::out);
	for (int i = 0; i < test.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(test[i]).length(); j++)
			str += "0";
		str += to_string(test[i]);
		out << str << '\n';
	}
	out.close();

	int length = label_id[0][0].length();
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type+1).length(); j++)
			str += "0";
		str += to_string(type+1);
		string file = dir_path + "\\ImageSets\\Main\\" + str + "_trainval.txt";
		std::cout << file << std::endl;
		out.open(file,ios::out);
		for (size_t i = 0; i < trainval.size(); i++) {
			int p = trainval[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p-1].size(); j++) {
				if (label_id[p-1][j] == str) {
					flag = 1;
				}
				
			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(trainval[i]).length(); j++)
				s += "0";
			s += to_string(trainval[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = dir_path + "\\ImageSets\\Main\\" + str + "_test.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < test.size(); i++) {
			int p = test[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(test[i]).length(); j++)
				s += "0";
			s += to_string(test[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = dir_path + "\\ImageSets\\Main\\" + str + "_train.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < train.size(); i++) {
			int p = train[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(train[i]).length(); j++)
				s += "0";
			s += to_string(train[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = dir_path + "\\ImageSets\\Main\\" + str + "_val.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < val.size(); i++) {
			int p = val[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(val[i]).length(); j++)
				s += "0";
			s += to_string(val[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}


	std::cout << "Generate jpegimages..." << std::endl;
	for (size_t i = 0; i < count; i++) {
		Image img("K:\\参赛\\BDCI2017-gsum\\训练图像及标注json\\标注图像\\" + image_id[i]);
		img.loadImage(1);
		//img.showImage(to_string(i));
		//std::cout << "K:\\参赛\\BDCI2017-gsum\\训练图像及标注json\\标注图像\\" + image_id[i] << std::endl;
		//Sleep(100);
		cv::String s ="";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i+1).length(); j++)
			s += "0";
		s += to_string(i+1);
		s += ".jpg";
		img.saveImage(dir_path + "\\JPEGImages\\"+s);
	}

	out.open("D:\\Program Files\\MATLAB\\MATLAB Production Server\\R2015a\\bin\\faster_rcnn-master\\image.txt", ios::out);

	for (size_t i = 0; i < count; i++) {
		out << image_id[i] << '\n';
	}
	out.close();

}

void generate_txt_file(string database_path) {
	int count = get_file_count(database_path + "\\Annotations");
	std::cout << count << std::endl;
	vector<vector<string>> label_id(count);
	vector<string> image_id(count);
	vector<vector<int>> bbox(count);
	for (int i = 0; i < count; i++) {
		string xml_file = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
			xml_file += "0";
		xml_file += to_string(i + 1);
		string picture_name = xml_file + ".jpg";
		xml_file += ".xml";
		string file_path = database_path + "\\Annotations\\" + xml_file;
		TiXmlDocument doc;
		if (!doc.LoadFile(file_path.data())) {
		std:; cerr << doc.ErrorDesc() << std::endl;
		}
		TiXmlElement * root = doc.RootElement();
		TiXmlElement *folder = root->FirstChildElement();
		TiXmlElement *filename = folder->NextSiblingElement();
		image_id[i] = filename->GetText();
		TiXmlElement *source = filename->NextSiblingElement();
		TiXmlElement *owner = source->NextSiblingElement();
		TiXmlElement *size = owner->NextSiblingElement();
		TiXmlElement *width = size->FirstChildElement();
		TiXmlElement *height = width->NextSiblingElement();
		TiXmlElement *depth = height->NextSiblingElement();
		TiXmlElement *segmented = size->NextSiblingElement();
		//cout << image_id[i] << endl;
		for (TiXmlElement * object = segmented->NextSiblingElement(); object != NULL; object = object->NextSiblingElement()) {
			TiXmlElement * name = object->FirstChildElement();
			label_id[i].push_back(name->GetText());
			//cout << label_id[i][0]<< endl;
			TiXmlElement * pose = name->NextSiblingElement();
			TiXmlElement * truncated = pose->NextSiblingElement();
			TiXmlElement * difficult = truncated->NextSiblingElement();
			TiXmlElement * bndbox = difficult->NextSiblingElement();
			TiXmlElement * xmin = bndbox->FirstChildElement();
			TiXmlElement * ymin = xmin->NextSiblingElement();
			TiXmlElement * xmax = ymin->NextSiblingElement();
			TiXmlElement * ymax = xmax->NextSiblingElement();
			bbox[i].push_back(stoi(xmin->GetText()));
			bbox[i].push_back(stoi(ymin->GetText()));
			bbox[i].push_back(stoi(xmax->GetText()));
			bbox[i].push_back(stoi(ymax->GetText()));
			//cout << bbox[i][0] << endl;
		}
	}

	std::cout << "Generate txt file..." << '\n';

	vector<int> data_index;
	vector<int> trainval(count*_TRAINVAL_PERCENT);
	vector<int> test(count - trainval.size());
	vector<int> train(trainval.size()*_TRAIN_PERCENT);
	vector<int> val(trainval.size() - train.size());
	for (int i = 0; i < count; i++) {
		data_index.push_back(i + 1);
	}
	part_vec(data_index, test, trainval, true);
	part_vec(trainval, train, val, true);
	string trainval_file_name = database_path + "\\ImageSets\\Main\\trainval.txt";
	string train_file_name = database_path + "\\ImageSets\\Main\\train.txt";
	string val_file_name = database_path + "\\ImageSets\\Main\\val.txt";
	string test_file_name = database_path + "\\ImageSets\\Main\\test.txt";
	//std::cout << count<<" "<< count*_TRAINVAL_PERCENT <<" "<<trainval.size() << std::endl;
	ofstream out;
	out.open(trainval_file_name, ios::out);
	for (int i = 0; i < trainval.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(trainval[i]).length(); j++)
			str += "0";
		str += to_string(trainval[i]);
		out << str << '\n';
	}
	out.close();
	out.open(train_file_name, ios::out);
	for (int i = 0; i < train.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(train[i]).length(); j++)
			str += "0";
		str += to_string(train[i]);
		out << str << '\n';
	}
	out.close();
	out.open(val_file_name, ios::out);
	for (int i = 0; i < val.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(val[i]).length(); j++)
			str += "0";
		str += to_string(val[i]);
		out << str << '\n';
	}
	out.close();
	out.open(test_file_name, ios::out);
	for (int i = 0; i < test.size(); i++)
	{
		string str = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(test[i]).length(); j++)
			str += "0";
		str += to_string(test[i]);
		out << str << '\n';
	}
	out.close();

	int length = label_id[0][0].length();
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = database_path + "\\ImageSets\\Main\\" + str + "_trainval.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < trainval.size(); i++) {
			int p = trainval[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(trainval[i]).length(); j++)
				s += "0";
			s += to_string(trainval[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = database_path + "\\ImageSets\\Main\\" + str + "_test.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < test.size(); i++) {
			int p = test[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(test[i]).length(); j++)
				s += "0";
			s += to_string(test[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = database_path + "\\ImageSets\\Main\\" + str + "_train.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < train.size(); i++) {
			int p = train[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(train[i]).length(); j++)
				s += "0";
			s += to_string(train[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
	for (size_t type = 0; type < _TYPE_COUNT; type++) {
		string str = "";
		for (size_t j = 0; j < length - to_string(type + 1).length(); j++)
			str += "0";
		str += to_string(type + 1);
		string file = database_path + "\\ImageSets\\Main\\" + str + "_val.txt";
		std::cout << file << std::endl;
		out.open(file, ios::out);
		for (size_t i = 0; i < val.size(); i++) {
			int p = val[i];
			size_t flag = 0;
			for (size_t j = 0; j < label_id[p - 1].size(); j++) {
				if (label_id[p - 1][j] == str) {
					flag = 1;
				}

			}
			string s = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(val[i]).length(); j++)
				s += "0";
			s += to_string(val[i]);
			if (1 == flag) {
				flag = 0;
				out << s << " " << 1 << '\n';
			}
			else
			{
				out << s << " " << -1 << '\n';
			}

		}
		out.close();
	}
}

void extract_image_id(string xml_path,string txt_path) {
	int count = get_file_count(xml_path);
	vector<string> image_id(count);
	for (int i = 0; i < count; i++) {
		string xml_file = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
			xml_file += "0";
		xml_file += to_string(i + 1);
		xml_file += ".xml";
		string file_path = xml_path + "\\" + xml_file;
		TiXmlDocument doc;
		if (!doc.LoadFile(file_path.data())) {
		std:; cerr << doc.ErrorDesc() << std::endl;
		}
		TiXmlElement * root = doc.RootElement();
		TiXmlElement *folder = root->FirstChildElement();
		TiXmlElement *filename = folder->NextSiblingElement();
		image_id[i] = filename->GetText();
	}
	ofstream out;
	out.open("tex_path", ios::out);
	for (size_t i = 0; i < count; i++) {
		out << image_id[i] << '\n';
	}
	out.close();
}

/**@state:

*/
void add_dataset(string database_path, string xml_path, string image_path, int nameFormat = 0) {
	int count = get_file_count(xml_path);
	vector<vector<string>> label_id(count);
	vector<string> image_id(count);
	vector<vector<int>> bbox(count);
	for (int i = 0; i < count; i++) {
		
		string file_path;
		if (nameFormat) {
			file_path = xml_path + "\\" + to_string(i + 1) + ".xml";
		}
		else
		{
			string xml_file = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
				xml_file += "0";
			xml_file += to_string(i + 1);
			xml_file += ".xml";
			file_path = xml_path + "\\" + xml_file;
		}
		std::cout << file_path << endl;
		TiXmlDocument doc;
		if (!doc.LoadFile(file_path.data())) {
		std:; cerr << doc.ErrorDesc() << std::endl;
		}
		TiXmlElement * root = doc.RootElement();
		TiXmlElement *folder = root->FirstChildElement();
		TiXmlElement *filename = folder->NextSiblingElement();
		image_id[i] = filename->GetText();
		TiXmlElement *path = filename->NextSiblingElement();
		TiXmlElement *source = path->NextSiblingElement();
		TiXmlElement *size = source->NextSiblingElement();
		TiXmlElement *width = size->FirstChildElement();
		TiXmlElement *height = width->NextSiblingElement();
		TiXmlElement *depth = height->NextSiblingElement();
		TiXmlElement *segmented = size->NextSiblingElement();
		//cout << image_id[i] << endl;
		for (TiXmlElement * object = segmented->NextSiblingElement(); object != NULL; object = object->NextSiblingElement()) {
			TiXmlElement * name = object->FirstChildElement();
			label_id[i].push_back(name->GetText());
			//cout << label_id[i][0]<< endl;
			TiXmlElement * pose = name->NextSiblingElement();
			TiXmlElement * truncated = pose->NextSiblingElement();
			TiXmlElement * difficult = truncated->NextSiblingElement();
			TiXmlElement * bndbox = difficult->NextSiblingElement();
			TiXmlElement * xmin = bndbox->FirstChildElement();
			TiXmlElement * ymin = xmin->NextSiblingElement();
			TiXmlElement * xmax = ymin->NextSiblingElement();
			TiXmlElement * ymax = xmax->NextSiblingElement();
			bbox[i].push_back(stoi(xmin->GetText()));
			bbox[i].push_back(stoi(ymin->GetText()));
			bbox[i].push_back(stoi(xmax->GetText()));
			bbox[i].push_back(stoi(ymax->GetText()));
			//cout << bbox[i][0] << endl;
		}	
	}

	string annotations_dir = database_path + "\\Annotations";
	int exit_file_count = get_file_count(annotations_dir);
	std::cout << exit_file_count << std::endl;
	std::cout << '\n' << "Generate xml flies..." << std::endl;

	for (size_t i = 0; i < count; i++) {
		string xml_file = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1 + exit_file_count).length(); j++)
			xml_file += "0";
		xml_file += to_string(i + 1 + exit_file_count);
		string picture_name = xml_file + ".jpg";
		xml_file += ".xml";

		TiXmlDocument doc;
		TiXmlElement* level_1 = new TiXmlElement("annotation");

		TiXmlElement* level_1_1 = new TiXmlElement("folder");
		level_1_1->LinkEndChild(new TiXmlText("VOC2007"));
		level_1->LinkEndChild(level_1_1);

		TiXmlElement* level_1_2 = new TiXmlElement("filename");
		level_1_2->LinkEndChild(new TiXmlText(picture_name.data()));
		level_1->LinkEndChild(level_1_2);

		TiXmlElement* level_1_3 = new TiXmlElement("source");
		TiXmlElement* level_1_3_1 = new TiXmlElement("database");
		level_1_3_1->LinkEndChild(new TiXmlText("The CCF2017 Database"));
		level_1_3->LinkEndChild(level_1_3_1);
		TiXmlElement* level_1_3_2 = new TiXmlElement("annotation");
		level_1_3_2->LinkEndChild(new TiXmlText("PASCAL CCF2017"));
		level_1_3->LinkEndChild(level_1_3_2);
		TiXmlElement* level_1_3_3 = new TiXmlElement("image");
		level_1_3_3->LinkEndChild(new TiXmlText("flickr"));
		level_1_3->LinkEndChild(level_1_3_3);



		TiXmlElement* level_1_3_4 = new TiXmlElement("flickrid");
		level_1_3_4->LinkEndChild(new TiXmlText("000000000"));
		level_1_3->LinkEndChild(level_1_3_4);
		level_1->LinkEndChild(level_1_3);

		TiXmlElement* level_1_4 = new TiXmlElement("owner");
		TiXmlElement* level_1_4_1 = new TiXmlElement("flickrid");
		level_1_4_1->LinkEndChild(new TiXmlText("?"));
		level_1_4->LinkEndChild(level_1_4_1);
		TiXmlElement* level_1_4_2 = new TiXmlElement("name");
		level_1_4_2->LinkEndChild(new TiXmlText("?"));
		level_1_4->LinkEndChild(level_1_4_2);
		level_1->LinkEndChild(level_1_4);

		string imagePath;
		if (nameFormat) {
			imagePath = image_path + "\\" + image_id[i] + ".jpg";
		}
		else
		{
			imagePath = image_path + "\\" + image_id[i];
		}
		Image img(imagePath);
		cout << imagePath << endl;
		TiXmlElement* level_1_5 = new TiXmlElement("size");
		TiXmlElement* level_1_5_1 = new TiXmlElement("width");
		level_1_5_1->LinkEndChild(new TiXmlText(to_string(img.getImage().cols).data()));
		level_1_5->LinkEndChild(level_1_5_1);
		TiXmlElement* level_1_5_2 = new TiXmlElement("height");
		level_1_5_2->LinkEndChild(new TiXmlText(to_string(img.getImage().rows).data()));
		level_1_5->LinkEndChild(level_1_5_2);
		TiXmlElement* level_1_5_3 = new TiXmlElement("depth");
		level_1_5_3->LinkEndChild(new TiXmlText("3"));
		level_1_5->LinkEndChild(level_1_5_3);
		level_1->LinkEndChild(level_1_5);

		TiXmlElement* level_1_6 = new TiXmlElement("segmented");
		level_1_6->LinkEndChild(new TiXmlText("0"));
		level_1->LinkEndChild(level_1_6);

		for (size_t object_cout = 0; object_cout < label_id[i].size(); object_cout++) {
			TiXmlElement* level_1_7 = new TiXmlElement("object");
			TiXmlElement* level_1_7_1 = new TiXmlElement("name");
			level_1_7_1->LinkEndChild(new TiXmlText(label_id[i][object_cout].data()));
			level_1_7->LinkEndChild(level_1_7_1);
			TiXmlElement* level_1_7_2 = new TiXmlElement("pose");
			level_1_7_2->LinkEndChild(new TiXmlText("Unspecified"));
			level_1_7->LinkEndChild(level_1_7_2);
			TiXmlElement* level_1_7_3 = new TiXmlElement("truncated");
			level_1_7_3->LinkEndChild(new TiXmlText("0"));
			level_1_7->LinkEndChild(level_1_7_3);
			TiXmlElement* level_1_7_4 = new TiXmlElement("difficult");
			level_1_7_4->LinkEndChild(new TiXmlText("0"));
			level_1_7->LinkEndChild(level_1_7_4);
			TiXmlElement* level_1_7_5 = new TiXmlElement("bndbox");
			TiXmlElement* level_1_7_5_1 = new TiXmlElement("xmin");
			level_1_7_5_1->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_1);
			TiXmlElement* level_1_7_5_2 = new TiXmlElement("ymin");
			level_1_7_5_2->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 1]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_2);
			TiXmlElement* level_1_7_5_3 = new TiXmlElement("xmax");
			level_1_7_5_3->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 2]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_3);
			TiXmlElement* level_1_7_5_4 = new TiXmlElement("ymax");
			level_1_7_5_4->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 3]).data()));
			level_1_7_5->LinkEndChild(level_1_7_5_4);
			level_1_7->LinkEndChild(level_1_7_5);
			level_1->LinkEndChild(level_1_7);
		}
		doc.LinkEndChild(level_1);
		string file_path = "..\\Annotations\\" + xml_file;
		if (_access(file_path.c_str(), 0) == -1) {
			doc.SaveFile(file_path.data());
		}
		else {
			remove(file_path.data());
			doc.SaveFile(file_path.data());
		}

		string chage_file = database_path + "\\Annotations\\" + xml_file;
		WinToUnix(file_path.data(), chage_file.data());
	}

	std::cout << "Generate jpegimages..." << std::endl;
	for (size_t i = 0; i < count; i++) {
		string imagePath;
		if (nameFormat) {
			imagePath = image_path + "\\" + image_id[i] + ".jpg";
		}
		else
		{
			imagePath = image_path + "\\" + image_id[i];
		}
		cv::Mat img = cv::imread(imagePath);
		//img.showImage(to_string(i));
		//std::cout << "K:\\参赛\\BDCI2017-gsum\\训练图像及标注json\\标注图像\\" + image_id[i] << std::endl;
		//Sleep(100);
		cv::String s = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1 + exit_file_count).length(); j++)
			s += "0";
		s += to_string(i + 1 + exit_file_count);
		s += ".jpg";
		cv::imwrite(database_path + "\\JPEGImages\\" + s,img);
	}
	generate_txt_file(database_path);
}

void generate_result_json(string result_txt_file_path, string result_json_file_path) {
	ifstream in;
	in.open(result_txt_file_path, ios::in);
	if (!in) {
		cerr << "File could not be open" << endl;
		abort();
	}
	vector<vector<string>> label_id;
	vector<string> image_id;
	vector<vector<int>> bbox;
	vector<vector<string>> scores;
	vector<string> str_line;
	vector<string> label;
	vector<int> box;
	vector<string> s;
	//char * img_id ="";
	char line[512];
	while (in.getline(line, 512, '\n')) {
		size_t null_char_num = 0;
		size_t obj_num = 0;
		for (size_t i = 0; line[i] != '\0'; i++) {
			if (line[i] == ' ') null_char_num++;
		}
		obj_num = null_char_num / 6;
		//std::cout << obj_num << " ";
		
		split_char(line, str_line, ' ');
		/*for (size_t i = 0; i < str_line.size(); i++) {
			std::cout << str_line[i] << " ";
		}*/
		image_id.push_back(str_line[0]);
		
		for (size_t i = 0; i < obj_num; i++) {
			label.push_back(str_line[i * 6 + 1]);
			box.push_back(atoi(str_line[i * 6 + 2].data()));
			box.push_back(atoi(str_line[i * 6 + 3].data()));
			box.push_back(atoi(str_line[i * 6 + 4].data()));
			box.push_back(atoi(str_line[i * 6 + 5].data()));
			s.push_back(str_line[i * 6 + 6]);
			
		}
		label_id.push_back(label);
		bbox.push_back(box);
		scores.push_back(s);
		//std::cout << std::endl;
		str_line.clear();
		label.clear();
		box.clear();
		s.clear();
	}	
	
	/*for (size_t i = 0; i < label_id.size(); i++) {
		for (size_t j = 0; j < label_id[i].size(); j++) {
		std::cout << label_id[i][j] << " ";
		}
		std::cout << std::endl;
	}*/
	in.close();

	Document doc;
	doc.SetArray();
	Document::AllocatorType& allocater = doc.GetAllocator();
	for (size_t j = 0; j < image_id.size(); j++) {
		//cout << j << endl;
		if(0 == label_id[j].size()) continue;
		Value obj_detail(kObjectType);
		obj_detail.AddMember("image_id", Value(StringRef(image_id[j].data())), allocater);
		obj_detail.AddMember("type", "A", allocater);
		Value item_array(kArrayType);
		for (size_t i = 0; i < label_id[j].size(); i++) {
			Value item(kObjectType);
			item.AddMember("label_id", Value(StringRef(label_id[j][i].data())), allocater);
			Value box(kArrayType);
			box.PushBack(bbox[j][i * 4], allocater);
			box.PushBack(bbox[j][i * 4 + 1], allocater);
			box.PushBack(bbox[j][i * 4 + 2], allocater);
			box.PushBack(bbox[j][i * 4 + 3], allocater);
			item.AddMember("bbox", box, allocater);
			item.AddMember("score", atof(scores[j][i].data()), allocater);
			item_array.PushBack(item, allocater);
		}
		obj_detail.AddMember("items", item_array, allocater);;
		doc.PushBack(obj_detail, allocater);
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	string str_json = buffer.GetString();
	FILE* json_file = fopen(result_json_file_path.data(), "w");
	if (json_file) {
		fputs(buffer.GetString(), json_file);
		fclose(json_file);
	}
}

void correct_format(string source_file, string output_file) {
	FILE *in;
	FILE *out;
	fopen_s(&in,source_file.data(),"rb");
	fopen_s(&out, output_file.data(), "wb");
	if (in == NULL || out == NULL) return;
	char c1;
	char c2;
	char c3;
	while ((c1 = fgetc(in)) != EOF) {
			if (c1 == '[') {
				if ((c2 = fgetc(in)) != EOF &&  c2 == ' ') {
					if ((c3 = fgetc(in)) != EOF &&  c3 == ']') {
						fputc(c1, out);
						fputc(c3, out);
					}
					else
					{
						fputc(c1, out);
						fputc(c2, out);
						fputc(c3, out);
					}
				}
				else
				{
					fputc(c1, out);
					fputc(c2, out);
				}
			}
			else 
			{
				fputc(c1, out);
			}
	}
	fclose(in);
	fclose(out);
}

void composite_data(string foreground_path, string backgroud_path, string result_xml, string result_image) {
	//string xml_path = dataset_path + "\\Annotations";
	//int count = get_file_count(xml_path);
	//vector<vector<int>> bbox(count);
	//for (int i = 0; i < count; i++) {
	//	string xml_file;
	//	for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
	//		xml_file += "0";
	//	xml_file += to_string(i + 1);
	//	xml_file += ".xml";
	//	string file_path = xml_path + "\\" + xml_file;
	//	TiXmlDocument doc;
	//	if (!doc.LoadFile(file_path.data())) {
	//	std:; cerr << doc.ErrorDesc() << std::endl;
	//	}
	//	TiXmlElement * root = doc.RootElement();
	//	TiXmlElement *folder = root->FirstChildElement();
	//	TiXmlElement *filename = folder->NextSiblingElement();
	//	TiXmlElement *source = filename->NextSiblingElement();
	//	TiXmlElement *owner = source->NextSiblingElement();
	//	TiXmlElement *size = owner->NextSiblingElement();
	//	TiXmlElement *segmented = size->NextSiblingElement();
	//	//cout << image_id[i] << endl;
	//	for (TiXmlElement * object = segmented->NextSiblingElement(); object != NULL; object = object->NextSiblingElement()) {
	//		TiXmlElement * name = object->FirstChildElement();
	//		//cout << label_id[i][0]<< endl;
	//		TiXmlElement * pose = name->NextSiblingElement();
	//		TiXmlElement * truncated = pose->NextSiblingElement();
	//		TiXmlElement * difficult = truncated->NextSiblingElement();
	//		TiXmlElement * bndbox = difficult->NextSiblingElement();
	//		TiXmlElement * xmin = bndbox->FirstChildElement();
	//		TiXmlElement * ymin = xmin->NextSiblingElement();
	//		TiXmlElement * xmax = ymin->NextSiblingElement();
	//		TiXmlElement * ymax = xmax->NextSiblingElement();
	//		bbox[i].push_back(stoi(xmin->GetText()));
	//		bbox[i].push_back(stoi(ymin->GetText()));
	//		bbox[i].push_back(stoi(xmax->GetText()));
	//		bbox[i].push_back(stoi(ymax->GetText()));
	//		//cout << bbox[i][0] << endl;
	//	}
	//}


	//size_t total_object = 0;
	//size_t r_min = 1000, r_max = 0, c_min = 1000, c_max = 0;
	//for (size_t i = 0; i < count; i++) {
	//	for (size_t j = 0; j < bbox[i].size()/4; j++) {
	//		total_object++;
	//		if (bbox[i][j * 4 + 2] - bbox[i][j * 4] < r_min) r_min = bbox[i][j * 4 + 2] - bbox[i][j * 4];
	//		if (bbox[i][j * 4 + 2] - bbox[i][j * 4] > r_max) r_max = bbox[i][j * 4 + 2] - bbox[i][j * 4];
	//		if (bbox[i][j * 4 + 3] - bbox[i][j * 4 + 1] < c_min) c_min = bbox[i][j * 4 + 3] - bbox[i][j * 4 + 1];
	//		if (bbox[i][j * 4 + 3] - bbox[i][j * 4 + 1] > c_max) c_max = bbox[i][j * 4 + 3] - bbox[i][j * 4 + 1];
	//	}
	//}

	//std::cout << "r_min = " << r_min << "\nr_max = " << r_max << "\nc_min = " << c_min << "\nc_max = " << c_max << '\n';

	///*size_t num = 5;
	//size_t interval = (r_max - r_min) / num;
	//vector<size_t> h_r((r_max - r_min )/interval + 1 );
	//vector<size_t> h_c((c_max - c_min )/ interval + 1);
	//vector<double_t> hist_r((r_max - r_min )/ interval + 1);
	//vector<double_t> hist_c((c_max - c_min )/ interval + 1);*/
	//
	//
	//size_t interval = 1;
	//vector<size_t> h_r((r_max - r_min) / interval + 1);
	//vector<size_t> h_c((c_max - c_min) / interval + 1);
	//vector<double_t> hist_r((r_max - r_min) / interval + 1);
	//vector<double_t> hist_c((c_max - c_min) / interval + 1);


	//for (size_t i = 0; i < count; i++) {
	//	for (size_t j = 0; j < bbox[i].size() / 4; j++) {
	//		h_r[(bbox[i][j * 4 + 2] - bbox[i][j * 4] - r_min)/ interval]++;
	//		h_c[(bbox[i][j * 4 + 3] - bbox[i][j * 4 + 1] - c_min)/ interval]++;
	//	}
	//}
	//double total = 0;
	//for (size_t i = 0; i < h_r.size(); i++) {
	//	hist_r[i] = h_r[i] * 1.0 / total_object;
	//	total += hist_r[i];
	//	std::cout << interval*i + r_min << "--"<< interval * i + r_min+ interval <<":"<< hist_r[i] <<" "<< total<< std::endl;
	//}
	/*for (size_t i = 0; i < h_c.size(); i++) {
		hist_c[i] = h_c[i] * 1.0 / total_object;
		std::cout << 5*i + c_min << "--" << 5 * i + c_min+5<<":"<< hist_c[i] << std::endl;
	}*/
	vector<vector<string>> label_id;
	vector<string> image_id;
	vector<vector<int>> bbox;
	vector<string> imageName;
	size_t sp = 0;
	getAllFiles(backgroud_path, imageName, 0, 0);
	size_t per_type_image = 10;
	size_t foreground_count = 30;
	std::vector<double> shear_angle = {0};
	std::vector<double> imrotate_angle_z = {0};
	std::vector<double> imrotate_angle_y = {0};
	std::vector<double> imrotate_angle_x = {0};
	std::vector<int> imsize = { 20,50};
	srand(time(NULL));
	int ada = 1;
	for (size_t i = 0; i < foreground_count; i++) {
		cout << "label:"<<i<<"  generate images..." << endl;
		string file_name;
		for (size_t j = 0; j < 4 - to_string(i + 1).length(); j++)
				file_name += "0";
		file_name += to_string(i + 1);
		string logo_path = foreground_path + "\\" + file_name + ".jpg";
		string mask_path = foreground_path + "\\" + file_name + "_mask.jpg";
		cv::Mat logo = cv::imread(logo_path);
		cv::Mat mask = cv::imread(mask_path);
		for (size_t m = 0; m < shear_angle.size(); m++) {
			cv::Mat logo_shear;
			cv::Mat mask_shear;
			Image::shear(logo, logo_shear, shear_angle[m]);
			Image::shear(mask, mask_shear, shear_angle[m]);
			for (size_t k = 0; k < imrotate_angle_z.size(); k++) {
				cv::Mat logo_imrotate_z;
				cv::Mat mask_imrotate_z;
				size_t random_size;
				Image::imrotate(logo_shear, logo_imrotate_z, imrotate_angle_z[k]);
				Image::imrotate(mask_shear, mask_imrotate_z, imrotate_angle_z[k]);
				for (size_t n = 0; n < imsize.size() - 1; n++) {
					cv::Mat logo_resized;
					cv::Mat mask_resized;
					cv::Mat imageROI;	
					
					random_size = rand() % (imsize[n+1] + 1 - imsize[n]) + imsize[n];
					size_t resize_row = random_size;
					size_t resize_col = resize_row *1.0 / logo.rows * logo.cols;
					resize(logo_imrotate_z, logo_resized, cv::Size(resize_col, resize_row), 0, 0);
					resize(mask_imrotate_z, mask_resized, cv::Size(resize_col, resize_row), 0, 0);
					bitwise_not(mask_resized, mask_resized);
					threshold(mask_resized, mask_resized, 30, 255, cv::THRESH_BINARY);
					for (size_t l = 0; l < per_type_image; l++) {
					    Sleep(sp);
						size_t random = rand() % imageName.size();
						int current = -1;
						for (size_t p = 0; p < image_id.size(); p++) {
							if (imageName[random] == image_id[p]) {
								current = p;
								break;
							}
						}
						cv::Mat image;
						if (-1 == current)
						{
							cv::Mat source = cv::imread(backgroud_path + "\\" + imageName[random]);
							image = source;
						}
						else
						{
							cv::Mat source = cv::imread(result_image + "\\" + imageName[random]);
							image = source;
						}
						
						size_t xmin = rand() % (image.rows - logo_resized.rows);
						size_t ymin = rand() % (image.cols - logo_resized.cols);
						size_t xmax = xmin + logo_resized.rows;
						size_t ymax = ymin + logo_resized.cols;
						imageROI = image(cv::Rect(ymin, xmin, logo_resized.cols, logo_resized.rows));
						logo_resized.copyTo(imageROI, mask_resized);
						std::vector<int> p(3);
						p[0] = CV_IMWRITE_JPEG_QUALITY;
						p[1] = 10;
						p[2] = 0;
						string savePath = result_image + "\\" + imageName[random];
						//cout << savePath << endl;
						cv::imwrite(savePath, image);
						/*cv::imshow("result", image);
						cv::waitKey(0);*/
						
						if (-1 == current) {
							std::vector<string> id;
							id.push_back(file_name);
							label_id.push_back(id);

							image_id.push_back(imageName[random]);

							std::vector<int> box;
							box.push_back(ymin);
							box.push_back(xmin);
							box.push_back(ymax);
							box.push_back(xmax);
							bbox.push_back(box);
						}
						else
						{
							label_id[current].push_back(file_name);
							bbox[current].push_back(ymin);
							bbox[current].push_back(xmin);
							bbox[current].push_back(ymax);
							bbox[current].push_back(xmax);
							//cout << "dasdsa   " <<label_id[current].size()<< endl;

						}

						
					}
					}
					
				}
			}
		for (size_t m = 0; m < imrotate_angle_y.size(); m++) {
			cv::Mat logo_y;
			cv::Mat mask_y;
			Image::imrotate(logo, logo_y, imrotate_angle_y[m],1);
			Image::imrotate(mask, mask_y, imrotate_angle_y[m],1);
			for (size_t k = 0; k < imrotate_angle_z.size(); k++) {
				cv::Mat logo_imrotate_z;
				cv::Mat mask_imrotate_z;
				size_t random_size;
				Image::imrotate(logo_y, logo_imrotate_z, imrotate_angle_z[k]);
				Image::imrotate(mask_y, mask_imrotate_z, imrotate_angle_z[k]);
				for (size_t n = 0; n < imsize.size() - 1; n++) {
					cv::Mat logo_resized;
					cv::Mat mask_resized;
					cv::Mat imageROI;
					
					random_size = rand() % (imsize[n + 1] + 1 - imsize[n]) + imsize[n];
					size_t resize_row = random_size;
					size_t resize_col = resize_row *1.0 / logo.rows * logo.cols;
					resize(logo_imrotate_z, logo_resized, cv::Size(resize_col, resize_row), 0, 0);
					resize(mask_imrotate_z, mask_resized, cv::Size(resize_col, resize_row), 0, 0);
					bitwise_not(mask_resized, mask_resized);
					threshold(mask_resized, mask_resized, 30, 255, cv::THRESH_BINARY);
					for (size_t l = 0; l < per_type_image; l++) {
						Sleep(sp);
						
						size_t random = rand() % imageName.size();
						int current = -1;
						for (size_t p = 0; p < image_id.size(); p++) {
							if (imageName[random] == image_id[p]) {
								current = p;
								break;
							}
						}
						cv::Mat image;
						if (-1 == current)
						{
							cv::Mat source = cv::imread(backgroud_path + "\\" + imageName[random]);
							image = source;
						}
						else
						{
							cv::Mat source = cv::imread(result_image + "\\" + imageName[random]);
							image = source;
						}
						size_t xmin = rand() % (image.rows - logo_resized.rows);
						size_t ymin = rand() % (image.cols - logo_resized.cols);
						size_t xmax = xmin + logo_resized.rows;
						size_t ymax = ymin + logo_resized.cols;
						imageROI = image(cv::Rect(ymin, xmin, logo_resized.cols, logo_resized.rows));
						logo_resized.copyTo(imageROI, mask_resized);
						std::vector<int> p(3);
						p[0] = CV_IMWRITE_JPEG_QUALITY;
						p[1] = 10;
						p[2] = 0;
						string savePath = result_image + "\\" + imageName[random];
						cv::imwrite(savePath, image);
						/*cv::imshow("result", image);
						cv::waitKey(0);*/
						
						if (-1 == current) {
							std::vector<string> id;
							id.push_back(file_name);
							label_id.push_back(id);

							image_id.push_back(imageName[random]);

							std::vector<int> box;
							box.push_back(ymin);
							box.push_back(xmin);
							box.push_back(ymax);
							box.push_back(xmax);
							bbox.push_back(box);
						}
						else
						{
							label_id[current].push_back(file_name);
							bbox[current].push_back(ymin);
							bbox[current].push_back(xmin);
							bbox[current].push_back(ymax);
							bbox[current].push_back(xmax);
							//cout << "dasdsa   " <<label_id[current].size()<< endl;

						}


					}
				}

			}
		}
		for (size_t m = 0; m < imrotate_angle_x.size(); m++) {
			cv::Mat logo_x;
			cv::Mat mask_x;
			Image::imrotate(logo, logo_x, imrotate_angle_x[m], 1);
			Image::imrotate(mask, mask_x, imrotate_angle_x[m], 1);
			for (size_t k = 0; k < imrotate_angle_z.size(); k++) {
				cv::Mat logo_imrotate_z;
				cv::Mat mask_imrotate_z;
				size_t random_size;
				Image::imrotate(logo_x, logo_imrotate_z, imrotate_angle_z[k]);
				Image::imrotate(mask_x, mask_imrotate_z, imrotate_angle_z[k]);
				for (size_t n = 0; n < imsize.size() - 1; n++) {
					cv::Mat logo_resized;
					cv::Mat mask_resized;
					cv::Mat imageROI;
					
					random_size = rand() % (imsize[n + 1] + 1 - imsize[n]) + imsize[n];
					size_t resize_row = random_size;
					size_t resize_col = resize_row *1.0 / logo.rows * logo.cols;
					resize(logo_imrotate_z, logo_resized, cv::Size(resize_col, resize_row), 0, 0);
					resize(mask_imrotate_z, mask_resized, cv::Size(resize_col, resize_row), 0, 0);
					bitwise_not(mask_resized, mask_resized);
					threshold(mask_resized, mask_resized, 30, 255, cv::THRESH_BINARY);
					for (size_t l = 0; l < per_type_image; l++) {
						Sleep(sp);
						
						size_t random = rand() % imageName.size();
						int current = -1;
						for (size_t p = 0; p < image_id.size(); p++) {
							if (imageName[random] == image_id[p]) {
								current = p;
								break;
							}
						}
						cv::Mat image;
						if (-1 == current)
						{
							cv::Mat source = cv::imread(backgroud_path + "\\" + imageName[random]);
							image = source;
						}
						else
						{
							cv::Mat source = cv::imread(result_image + "\\" + imageName[random]);
							image = source;
						}
						size_t xmin = rand() % (image.rows - logo_resized.rows);
						size_t ymin = rand() % (image.cols - logo_resized.cols);
						size_t xmax = xmin + logo_resized.rows;
						size_t ymax = ymin + logo_resized.cols;
						imageROI = image(cv::Rect(ymin, xmin, logo_resized.cols, logo_resized.rows));
						logo_resized.copyTo(imageROI, mask_resized);
						std::vector<int> p(3);
						p[0] = CV_IMWRITE_JPEG_QUALITY;
						p[1] = 10;
						p[2] = 0;
						string savePath = result_image + "\\" + imageName[random];
						cv::imwrite(savePath, image);
						/*cv::imshow("result", image);
						cv::waitKey(0);*/
						
						if (-1 == current) {
							std::vector<string> id;
							id.push_back(file_name);
							label_id.push_back(id);

							image_id.push_back(imageName[random]);

							std::vector<int> box;
							box.push_back(ymin);
							box.push_back(xmin);
							box.push_back(ymax);
							box.push_back(xmax);
							bbox.push_back(box);
						}
						else
						{
							label_id[current].push_back(file_name);
							bbox[current].push_back(ymin);
							bbox[current].push_back(xmin);
							bbox[current].push_back(ymax);
							bbox[current].push_back(xmax);
							//cout << "dasdsa   " <<label_id[current].size()<< endl;

						}


					}
				}

			}
		}
			
		}

	cout << "generarate xml..." << endl;
	for (size_t i = 0; i < image_id.size(); i++) {
			string xml_file = "";
			for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
				xml_file += "0";
			xml_file += to_string(i + 1);
			xml_file += ".xml";
			string picture_name = image_id[i];
			
			cout << i + 1 << ":" << picture_name << endl;
			TiXmlDocument doc;
			TiXmlElement* level_1 = new TiXmlElement("annotation");

			TiXmlElement* level_1_1 = new TiXmlElement("folder");
			level_1_1->LinkEndChild(new TiXmlText("VOC2007"));
			level_1->LinkEndChild(level_1_1);

			TiXmlElement* level_1_2 = new TiXmlElement("filename");
			level_1_2->LinkEndChild(new TiXmlText(picture_name.data()));
			level_1->LinkEndChild(level_1_2);

			TiXmlElement* level_1_3 = new TiXmlElement("source");
			TiXmlElement* level_1_3_1 = new TiXmlElement("database");
			level_1_3_1->LinkEndChild(new TiXmlText("The CCF2017 Database"));
			level_1_3->LinkEndChild(level_1_3_1);
			TiXmlElement* level_1_3_2 = new TiXmlElement("annotation");
			level_1_3_2->LinkEndChild(new TiXmlText("PASCAL CCF2017"));
			level_1_3->LinkEndChild(level_1_3_2);
			TiXmlElement* level_1_3_3 = new TiXmlElement("image");
			level_1_3_3->LinkEndChild(new TiXmlText("flickr"));
			level_1_3->LinkEndChild(level_1_3_3);



			TiXmlElement* level_1_3_4 = new TiXmlElement("flickrid");
			level_1_3_4->LinkEndChild(new TiXmlText("000000000"));
			level_1_3->LinkEndChild(level_1_3_4);
			level_1->LinkEndChild(level_1_3);

			TiXmlElement* level_1_4 = new TiXmlElement("owner");
			TiXmlElement* level_1_4_1 = new TiXmlElement("flickrid");
			level_1_4_1->LinkEndChild(new TiXmlText("?"));
			level_1_4->LinkEndChild(level_1_4_1);
			TiXmlElement* level_1_4_2 = new TiXmlElement("name");
			level_1_4_2->LinkEndChild(new TiXmlText("?"));
			level_1_4->LinkEndChild(level_1_4_2);
			level_1->LinkEndChild(level_1_4);

			Image img(backgroud_path+"\\"+imageName[i]);
			img.loadImage(1);
			TiXmlElement* level_1_5 = new TiXmlElement("size");
			TiXmlElement* level_1_5_1 = new TiXmlElement("width");
			level_1_5_1->LinkEndChild(new TiXmlText(to_string(img.getImage().cols).data()));
			level_1_5->LinkEndChild(level_1_5_1);
			TiXmlElement* level_1_5_2 = new TiXmlElement("height");
			level_1_5_2->LinkEndChild(new TiXmlText(to_string(img.getImage().rows).data()));
			level_1_5->LinkEndChild(level_1_5_2);
			TiXmlElement* level_1_5_3 = new TiXmlElement("depth");
			level_1_5_3->LinkEndChild(new TiXmlText("3"));
			level_1_5->LinkEndChild(level_1_5_3);
			level_1->LinkEndChild(level_1_5);

			TiXmlElement* level_1_6 = new TiXmlElement("segmented");
			level_1_6->LinkEndChild(new TiXmlText("0"));
			level_1->LinkEndChild(level_1_6);

			for (size_t object_cout = 0; object_cout < label_id[i].size(); object_cout++) {
				TiXmlElement* level_1_7 = new TiXmlElement("object");
				TiXmlElement* level_1_7_1 = new TiXmlElement("name");
				level_1_7_1->LinkEndChild(new TiXmlText(label_id[i][object_cout].data()));
				level_1_7->LinkEndChild(level_1_7_1);
				TiXmlElement* level_1_7_2 = new TiXmlElement("pose");
				level_1_7_2->LinkEndChild(new TiXmlText("Unspecified"));
				level_1_7->LinkEndChild(level_1_7_2);
				TiXmlElement* level_1_7_3 = new TiXmlElement("truncated");
				level_1_7_3->LinkEndChild(new TiXmlText("0"));
				level_1_7->LinkEndChild(level_1_7_3);
				TiXmlElement* level_1_7_4 = new TiXmlElement("difficult");
				level_1_7_4->LinkEndChild(new TiXmlText("0"));
				level_1_7->LinkEndChild(level_1_7_4);
				TiXmlElement* level_1_7_5 = new TiXmlElement("bndbox");
				TiXmlElement* level_1_7_5_1 = new TiXmlElement("xmin");
				level_1_7_5_1->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4]).data()));
				level_1_7_5->LinkEndChild(level_1_7_5_1);
				TiXmlElement* level_1_7_5_2 = new TiXmlElement("ymin");
				level_1_7_5_2->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 1]).data()));
				level_1_7_5->LinkEndChild(level_1_7_5_2);
				TiXmlElement* level_1_7_5_3 = new TiXmlElement("xmax");
				level_1_7_5_3->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 2]).data()));
				level_1_7_5->LinkEndChild(level_1_7_5_3);
				TiXmlElement* level_1_7_5_4 = new TiXmlElement("ymax");
				level_1_7_5_4->LinkEndChild(new TiXmlText(to_string(bbox[i][object_cout * 4 + 3]).data()));
				level_1_7_5->LinkEndChild(level_1_7_5_4);
				level_1_7->LinkEndChild(level_1_7_5);
				level_1->LinkEndChild(level_1_7);
			}
			doc.LinkEndChild(level_1);
			string file_path = "E:\\compsiteImages\\xml\\"+ xml_file;
			if (_access(file_path.c_str(), 0) == -1) {
				doc.SaveFile(file_path.data());
			}
			else {
				remove(file_path.data());
				doc.SaveFile(file_path.data());
			}

		}

}

void cal_mAP(string result_file, string xml_path) {
	int count = get_file_count(xml_path);
//	std::cout << "count:" <<count << std::endl;
	vector<vector<int>> label_id(count);
	vector<string> image_id(count);
	vector<vector<int>> bbox(count);
	vector<size_t> rows(count);
	vector<size_t> cols(count);
	//std::cout << xml_path << std::endl;
	for (int i = 0; i < count; i++) {
		string xml_file = "";
		for (size_t j = 0; j < _NANE_LENGTH - to_string(i + 1).length(); j++)
			xml_file += "0";
		xml_file += to_string(i + 1);
		xml_file += ".xml";
		string file_path = xml_path + "\\" + xml_file;
		TiXmlDocument doc;
		if (!doc.LoadFile(file_path.data())) {
		std:; cerr << doc.ErrorDesc() << std::endl;
		}
	//	std::wcout << i << std::endl;
		TiXmlElement * root = doc.RootElement();
		TiXmlElement *folder = root->FirstChildElement();
		TiXmlElement *filename = folder->NextSiblingElement();
		image_id[i] = filename->GetText();
		TiXmlElement *source = filename->NextSiblingElement();
		TiXmlElement *owner = source->NextSiblingElement();
		TiXmlElement *size = owner->NextSiblingElement();
		TiXmlElement *width = size->FirstChildElement();
		rows[i] = atoi(width->GetText());
		TiXmlElement *height = width->NextSiblingElement();
		cols[i] = atoi(height->GetText());
		TiXmlElement *depth = height->NextSiblingElement();
		TiXmlElement *segmented = size->NextSiblingElement();
	//	cout << image_id[i] << endl;
		for (TiXmlElement * object = segmented->NextSiblingElement(); object != NULL; object = object->NextSiblingElement()) {
			TiXmlElement * name = object->FirstChildElement();
			label_id[i].push_back(atoi(name->GetText()));
	//		cout << label_id[i][0]<< endl;
			TiXmlElement * pose = name->NextSiblingElement();
			TiXmlElement * truncated = pose->NextSiblingElement();
			TiXmlElement * difficult = truncated->NextSiblingElement();
			TiXmlElement * bndbox = difficult->NextSiblingElement();
			TiXmlElement * xmin = bndbox->FirstChildElement();
			TiXmlElement * ymin = xmin->NextSiblingElement();
			TiXmlElement * xmax = ymin->NextSiblingElement();
			TiXmlElement * ymax = xmax->NextSiblingElement();
			bbox[i].push_back(stoi(xmin->GetText()));
			bbox[i].push_back(stoi(ymin->GetText()));
			bbox[i].push_back(stoi(xmax->GetText()));
			bbox[i].push_back(stoi(ymax->GetText()));
		//	cout << bbox[i][0] << endl;
		}
	}
	//std::cout << result_file << endl;
	ifstream in;
	in.open(result_file, ios::in);
	if (!in) {
		cerr << "File could not be open" << endl;
		abort();
	}
	std::cout << result_file << std::endl;
	vector<vector<int>> rlabel_id;
	vector<string> rimage_id;
	vector<vector<int>> rbbox;
	vector<vector<float>> scores;
	vector<string> str_line;
	vector<int> label;
	vector<int> box;
	vector<float> s;
	//char * img_id ="";
	char line[512];
	int A = 0;
	while (in.getline(line, 512, '\n')) {
		A++;
		size_t null_char_num = 0;
		size_t obj_num = 0;
		for (size_t i = 0; line[i] != '\0'; i++) {
			if (line[i] == ' ') null_char_num++;
		}
		obj_num = null_char_num / 6;
	//	std::cout << obj_num << " ";

		split_char(line, str_line, ' ');
		/*for (size_t i = 0; i < str_line.size(); i++) {
		std::cout << str_line[i] << " ";
		}*/
		rimage_id.push_back(str_line[0]);

		for (size_t i = 0; i < obj_num; i++) {
			label.push_back(atoi(str_line[i * 6 + 1].data()));
			box.push_back(atoi(str_line[i * 6 + 2].data()));
			box.push_back(atoi(str_line[i * 6 + 3].data()));
			box.push_back(atoi(str_line[i * 6 + 4].data()));
			box.push_back(atoi(str_line[i * 6 + 5].data()));
			s.push_back(atof(str_line[i * 6 + 6].data()));

		}
		rlabel_id.push_back(label);
		rbbox.push_back(box);
		scores.push_back(s);
		str_line.clear();
		label.clear();
		box.clear();
		s.clear();
	}

	/*for (size_t i = 0; i < label_id.size(); i++) {
	for (size_t j = 0; j < label_id[i].size(); j++) {
	std::cout << label_id[i][j] << " ";
	}
	std::cout << std::endl;
	}*/
	in.close();

	std::cout << "cal.." << std::endl;
	vector<vector<int>> tf_type(30);
	vector<vector<float>>score_type(30);
	//count = 53;
	for (size_t n = 0; n < count; n++) {
		//std::cout << n+1<<":"<< std::endl;
		for (size_t m = 0; m < rlabel_id[n].size(); m++) {
			int flag = 0;
			int s = 0;
			for (size_t k = 0; k < label_id[n].size(); k++) {
				if (rlabel_id[n][m] == label_id[n][k]) {
					flag = 1;
					s = k;
				}
			}
			if (1 == flag) {
				vector<vector<int>> result(rows[n]);
				for (size_t i = 0; i < result.size(); i++) {
					result[i].resize(cols[n]);
					for (size_t j = 0; j < result[i].size(); j++) {
						if (rbbox[n][m * 4] <= i && i <= rbbox[n][m * 4 + 2] && rbbox[n][m * 4 + 1] <= j && j <= rbbox[n][m * 4 + 3])
						{
							result[i][j] = rlabel_id[n][m];
						}
						else
							result[i][j] = 0;
					}
					
				}
		//		std::cout <<rlabel_id[n][m]<<" "<< rbbox[n][m * 4] <<" "<< rbbox[n][m * 4 + 1] <<" "<< rbbox[n][m * 4 + 2] <<" "<< rbbox[n][m * 4 + 3]<< std::endl;
				vector<vector<int>> ground_truth(rows[n]);
				for (size_t i = 0; i < ground_truth.size(); i++) {
					ground_truth[i].resize(cols[n]);
					for (size_t j = 0; j < ground_truth[i].size(); j++) {
						if (bbox[n][s * 4] <= i && i <= bbox[n][s * 4 + 2] && bbox[n][s * 4 + 1] <= j && j <= bbox[n][s * 4 + 3])
							ground_truth[i][j] = label_id[n][s];
						else
							ground_truth[i][j] = 0;
					}
				}
			//	std::cout <<label_id[n][s] <<" "<< bbox[n][s * 4]<<" "<< bbox[n][s * 4 + 1] <<" "<< bbox[n][s * 4 + 2]<<" "<< bbox[n][s * 4 + 3]<< std::endl;
				int union_set = 0;
				int inter_set = 0;
				const int id = label_id[n][s];
				float IOU = 0;
				for (size_t i = 0; i < rows[n]; i++) {
					for (size_t j = 0; j < cols[n]; j++) {
						if (ground_truth[i][j] == id && result[i][j] == id) inter_set++;
						if (id == ground_truth[i][j] || id == result[i][j]) union_set++;
					}
				}
				IOU = 1.0*inter_set / union_set;
			//	std::cout << "IOU:" << IOU << std::endl;
				if (IOU >= 0.5) 
					tf_type[rlabel_id[n][m]-1].push_back(1);
				else
					tf_type[rlabel_id[n][m]-1].push_back(0);
				score_type[rlabel_id[n][m]-1].push_back(scores[n][m]);
			}
			else
			{
				tf_type[rlabel_id[n][m]-1].push_back(0);
				score_type[rlabel_id[n][m]-1].push_back(scores[n][m]);
			}
			
		}
	}

	std::cout << "P@K..." << std::endl;
	vector<vector<float>> pk(30);
	vector<float> AP(30);
	float mAP = 0;
	for (size_t i = 0; i < 30; i++)
	{
		if(score_type[i].size()>=2)
		quick_sort(score_type[i], tf_type[i], 0, score_type[i].size() - 1);
	}
	for (size_t i = 0; i < 30; i++)
	{
		for (size_t j = 0; j < score_type[i].size(); j++) {
			size_t right = 0;
			for (size_t m = 0; m < j + 1; m++) {
				if (1 == tf_type[i][m]) right++;
			}
		//	cout << right*1.0 / (j + 1) << " " << endl;
			pk[i].push_back(right*1.0 / (j+1));
		}
	}
	/*for (size_t i = 0; i < 30; i++)
	{
		cout << i << ": ";
		for (size_t j = 0; j < score_type[i].size(); j++){
			cout << pk[i][j] << " ";
		}
	cout << endl;
	}*/
	float tmp = 0;
	for (size_t i = 0; i < 30; i++) {
		AP[i] = 0;
		for (size_t j = 0; j < score_type[i].size(); j++) {
			tmp += pk[i][j];
		}
	//	cout << tmp << " ";
		if(score_type[i].size()!=0)
		AP[i] = tmp * 1.0 / score_type[i].size();
		tmp = 0;
		cout << "AP[" << i << "]=" << AP[i] << endl;
	}
	tmp = 0;

	for (size_t i = 0; i < 30; i++)
		tmp += AP[i];
	mAP = tmp / 30;
	std::cout << "mAp:" << mAP << std::endl;
}

int main() {

	string dataset_path = "E:\\VOC2007";

	int decide = 3;

	if (1 == decide) { //generate dataset with source train data given by CCF 
		extract_train_json(dataset_path, "train.json", _MAKE_FILE_RETAIN_EXIT_FILE);
	}
	if (2 == decide) { //add dataset to an existent dataset 
		add_dataset(dataset_path, "C:\\Users\\YOUNA\\Desktop\\xml", "C:\\Users\\YOUNA\\Desktop\\NEW", 1);
	}
	if (3 == decide) { //generate a new dataset with your data and CCF train data
		
		extract_train_json(dataset_path, "train.json", _MAKE_FILE_RETAIN_EXIT_FILE);
		add_dataset(dataset_path, "E:\\compsiteImages\\xml", "E:\\compsiteImages\\image");
		add_dataset(dataset_path, "C:\\Users\\YOUNA\\Desktop\\xml", "C:\\Users\\YOUNA\\Desktop\\NEW", 1);
	}
	if (4 == decide) { //covert your result to the format submitted
		generate_result_json("F:\\result.txt", "F:\\result.json");
	}

	if (5 == decide) {//modify your result format, but this step is meaningless 
		correct_format(".\\result.json", "C:\\Users\\YOUNA\\Desktop\\result.json");
	}
	if (6 == decide) {//calculate the mAp , but with special situation
		cal_mAP("D:\\Program Files\\MATLAB\\MATLAB Production Server\\R2015a\\bin\\faster_rcnn-master\\result.txt", "E:\\VOC2007\\Annotations");
	}
	if (7 == decide) {//generate composited data
		composite_data("K:\\参赛\\BDCI2017-gsum\\LOGO图像\\logo","E:\\背景图像\\CCF 背景底图\\car_images_","E:\\compsiteImages\\xml","E:\\compsiteImages\\image");
	}	
	
}