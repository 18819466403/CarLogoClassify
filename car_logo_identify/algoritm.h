#pragma once
#include<iostream>
#include<vector>
#include<time.h>
#define SWAP(a,b){a = a + b; b = a - b; a = a - b;}

//closet_pair function: two points
//#define _CLOSET_PAIR_POINT_ONE one;
//#define _CLOSET_PAIR_POINT_TWO two;

template<typename T>
struct s_point {
	T m_x;
	T m_y;
	s_point(T x, T y) :m_x(x), m_y(y) {};
};
template<typename T>
void swap(T a, T b) {
	T c;
	c = a;
	a = b;
	b = c;
}

template<typename T>
void set_mat(T ** &mat, size_t rows, size_t cols) {
	mat = new T*[rows];
	for (size_t i = 0; i < rows; i++)
		mat[i] = new T[cols];
}

template<typename T>
void convert_vec_to_mat(const T* vec, T **& mat, size_t rows, size_t cols) {
	size_t index = 0;
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			mat[i][j] = vec[index++];
		}
	}
}

//template<typename T>
//void convert_vec_to_mat (const std::vector<T> vec, T **& mat, size_t rows, size_t cols) {
//	size_t index = 0;
//	for (size_t i = 0; i < rows; i++) {
//		for (size_t j = 0; j < cols; j++) {
//			mat[i][j] = vec[index++];
//		}
//	}
//}

int find_min_index(int * vec,int n) {
	int index;
	for (int i = 0; i < n; i++) {
		if (vec[i] != 0) {
			index = i;
			break;
		}
	}
	
	for (int i = n-1; i >=0; i--) {
		if (vec[i]!=0 && vec[index] >= vec[i]) {
			index = i;
		}
	}
	return index;
}
int find_min(int * vec, int n) {
	int index;
	for (int i = 0; i < n; i++) {
		if (vec[i] != 0) {
			index = i;
			break;
		}
	}

	for (int i = n - 1; i >= 0; i--) {
		if (vec[i] != 0 && vec[index] >= vec[i]) {
			index = i;
		}
	}
	return vec[index];
}

int get(int **&mat, std::vector<int> &result, std::vector<int> &rows,std::vector<int> &min_index_row,std::vector<int> &min_row,int col) {
	for (int i = 0; i < result.size(); i++) {
		int min_index = find_min_index(mat[result[i]], col);
		int min = find_min(mat[result[i]], col);
		rows.push_back(result[i]);
		min_index_row.push_back(min_index);
		min_row.push_back(min);


	}
	int ind = 0;
	for (int j = 0; j < min_row.size(); j++) {
		if (min_row[ind] > min_row[j]) {
			ind = j;
		}
		
	}
	for (int i = 0; i < result.size(); i++) {
		if (min_index_row[ind] == result[i]) {
			mat[rows[ind]][min_index_row[ind]] = 0;
			mat[min_index_row[ind]][rows[ind]] = 0;
			rows.clear();
			min_index_row.clear();
			min_row.clear();
			ind = get(mat, result, rows, min_index_row, min_row,col);
			break;
		}
	}
	return ind;
}


void prim(int **& mat, std::vector<int> &result ,int row,int col) {
	if(0==result.size()) result.push_back(0);
	if (6 == result.size()) return;
	std::vector<int> rows;
    std::vector<int> min_index_row;
	std::vector<int> min_row;
	
	int ind = get(mat, result, rows, min_index_row, min_row,col);

	mat[rows[ind]][min_index_row[ind]] = 0;
	mat[min_index_row[ind]][rows[ind]] = 0;

	std::cout << "reslut vec:";
	for (int i = 0; i < result.size(); i++) {
		std::cout << result[i] << " ";
	}
	std::cout << "\nrestet:";
	std::cout << rows[ind] << "," << min_index_row[ind] << std::endl;
	std::cout << std::endl;
	std::cout << "current mat:" << std::endl;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++) {
			std::cout << mat[i][j] << " ";
		}
		std::cout << std::endl;
	}
	result.push_back(min_index_row[ind]);
	rows.clear();
	min_index_row.clear();
	min_row.clear();
	prim(mat, result,row,col);
}

//template<typename T>
//void swap(T* &vec, size_t i, size_t j) {
//	T temp = vec[i];
//	vec[i] = vec[j];
//	vec[j] = temp;
//}
//
//template<typename T>
//void swap(T i, T j) {
//	T temp = i;
//	i = j;
//	j = temp;
//}

template<typename T>
void quick_sort(T * vec, size_t n, size_t i, size_t j) {
	size_t temp = i;
	size_t index = j;
	std::cout << i << "------" << j << std::endl;
	T ref = vec[temp];
	if (i >=j) return;
	while (i < j) {
		while (i<j && vec[j] >= ref) j--;
		while (i<j && vec[i] < ref) i++;
		
		if (i != j) {
			std::cout << i << "<->" << j << std::endl;
			SWAP(vec[i], vec[j]);
			for (size_t k = 0; k < n; k++)
				std::cout << vec[k] << " ";
			std::cout << std::endl;
		}
	}
	if (temp != j)
	{
		std::cout << temp << "<->" << j << std::endl;
		//swap(vec, temp, j);
		SWAP(vec[temp], vec[j]);
	}
	for (size_t k = 0; k < n; k++)
		std::cout << vec[k] << " ";
	std::cout << std::endl;
	std::cout << std::endl;
	quick_sort(vec, n, temp, i);
	quick_sort(vec, n, j+1, index);
}

template<typename T>
void quick_sort(std::vector<T>&vec, size_t i, size_t j) {
	size_t n = vec.size();
	size_t temp = i;
	size_t index = j;
	//std::cout << i << "------" << j << std::endl;
	T ref = vec[temp];
	if (i >= j) return;
	while (i < j) {
		while (i<j && vec[j] >= ref) j--;
		while (i<j && vec[i] < ref) i++;

		if (i != j) {
			//std::cout << i << "<->" << j << std::endl;
			SWAP(vec[i], vec[j]);
			/*for (size_t k = 0; k < n; k++)
				std::cout << vec[k] << " ";
			std::cout << std::endl;*/
		}
	}
	if (temp != j)
	{
	//	std::cout << temp << "<->" << j << std::endl;
		//swap(vec, temp, j);
		SWAP(vec[temp], vec[j]);
	}
	/*for (size_t k = 0; k < n; k++)
		std::cout << vec[k] << " ";
	std::cout << std::endl;
	std::cout << std::endl;*/
	quick_sort(vec, temp, i);
	quick_sort(vec, j + 1, index);
}
template<typename T,typename M>
void quick_sort(std::vector<T>&vec, std::vector<M> &vec_follow, size_t i, size_t j) {
	size_t n = vec.size();
	size_t temp = i;
	size_t index = j;
	//std::cout << i << "------" << j << std::endl;
	T ref = vec[temp];
	if (i >= j) return;
	while (i < j) {
		while (i<j && vec[j] >= ref) j--;
		while (i<j && vec[i] < ref) i++;

		if (i != j) {
			//std::cout << i << "<->" << j << std::endl;
			SWAP(vec[i], vec[j]);
			SWAP(vec_follow[i], vec_follow[j]);
			/*for (size_t k = 0; k < n; k++)
			std::cout << vec[k] << " ";
			std::cout << std::endl;*/
		}
	}
	if (temp != j)
	{
		//	std::cout << temp << "<->" << j << std::endl;
		//swap(vec, temp, j);
		SWAP(vec[temp], vec[j]);
		SWAP(vec_follow[temp], vec_follow[j]);
	}
	/*for (size_t k = 0; k < n; k++)
	std::cout << vec[k] << " ";
	std::cout << std::endl;
	std::cout << std::endl;*/
	quick_sort(vec, vec_follow, temp, i);
	quick_sort(vec, vec_follow, j + 1, index);
}


template<typename T>
void quick_sort(std::vector<s_point<T>> &vec, size_t n, size_t i, size_t j) {
	size_t temp = i;
	size_t index = j;
	std::cout << i << "------" << j << std::endl;
	T ref = vec[temp].m_x;
	if (i >= j) return;
	while (i < j) {
		while (i<j && vec[j].m_x >= ref) j--;
		while (i<j && vec[i].m_x < ref) i++;

		if (i != j) {
			std::cout << i << "<->" << j << std::endl;
			SWAP(vec[i].m_x, vec[j].m_x);
			SWAP(vec[i].m_y, vec[j].m_y);
			for (size_t k = 0; k < n; k++)
				std::cout << vec[k].m_x << " ";
			std::cout << std::endl;
		}
	}
	if (temp != j)
	{
		std::cout << temp << "<->" << j << std::endl;
		//swap(vec, temp, j);
		SWAP(vec[temp].m_x, vec[j].m_x);
		SWAP(vec[temp].m_y, vec[j].m_y);
	}
	for (size_t k = 0; k < n; k++)
		std::cout << vec[k].m_x << " ";
	std::cout << std::endl;
	std::cout << std::endl;
	quick_sort(vec, n, temp, i);
	quick_sort(vec, n, j + 1, index);
}


template<typename T>
void divise(T *& vec , int l, int r, T *& result) {
	if (l < r)

	{	
		int mid = (l + r) / 2;		
		divise(vec, l, mid, result);
		divise(vec, mid + 1, r, result);	
		merge(vec, l, mid, r, result);
	}
	else
		return;
}

template<typename T>
void merge(T *& vec, int l,int mid, int r, T *& result) {
	int m = l;
	int n = mid + 1;
	int temp = l;
	while (m <= mid && n <= r) {
		if (vec[m] < vec[n])
			result[temp++] = vec[m++];
		else
			result[temp++] = vec[n++];
	}
	while (m <= mid)
		result[temp++] = vec[m++];
	while (n <= r)
		result[temp++] = vec[n++];
	temp = l;
	while (l <= r)
		vec[l++] = result[temp++];
}

template<typename T>
void merge_sort(T * vec, int n) {
	T * result = new T[n];
	divise(vec, 0, n-1, result);
}

template<typename T>
double get_point_distance(s_point<T> p1, s_point<T> p2) {
	return sqrt((double)(p1.m_x - p2.m_x)*(p1.m_x - p2.m_x) + (p1.m_y - p2.m_y)*(p1.m_y - p2.m_y));
}

template<typename T>
double divise(std::vector<s_point<T>> &vec, size_t i, size_t j) {
	size_t l = i;
	size_t r = j;
	std::cout << std::endl;
	for (size_t m = l; m <= r; m++)
		std::cout << vec[m].m_x << " ";
	if (i == j) return 0;
	if (j == i + 1) return get_point_distance(vec[i], vec[j]);
	size_t mid = (i + j) / 2;
	double d1 = divise(vec, l, mid);
	double d2 = divise(vec, mid + 1, r);

	double d;
	if (d1 >= d2) {
		d = d2;
	}
	else d = d1;
	if (d1 <= 0.00001) d = d2;
	if (d2 <= 0.00001) d = d1;

	std::cout << i << "---"<<j << std::endl;
	
	size_t l_d;
	size_t r_d;

	for (int m = mid; m >= (int)(l); m--) {
		if (d >= abs(vec[m].m_x - vec[mid].m_x)) l_d = m;
		else break;
	}
    
    for(int n =mid; n<= (int)(r); n++)
		if (d >= abs(vec[n].m_x - vec[mid].m_x)) r_d = n;
		else break;
    
	for(int i = l_d; i<=(int)(r_d);i++)
		for (int j = i; j <= (int)(r_d); j++) {
			if (get_point_distance(vec[i], vec[j]) < d && get_point_distance(vec[i], vec[j]) >0.00001) d = get_point_distance(vec[i], vec[j]);
		}
	std::cout << "d:" <<d<< '\n';
	return d;
}

template<typename T>
void closet_pair(std::vector<s_point<T>> & vec) {
	size_t size = vec.size();
	std::cout << "sort points:" << '\n';
	quick_sort(vec, vec.size(), 0, vec.size() - 1);
	for (size_t m = 0; m < size; m++)
		std::cout << vec[m].m_x << " ";
	std::cout << '\n' << "divise:" << '\n';
	std::cout<<"closet_distance:"<<divise(vec, 0, size - 1)<<'\n';
	
}

template<typename T>
bool is_in_vector(std::vector<T> & vec, T element) {
	for (int i = 0; i < vec.size(); i++) {
		if (element == vec[i]) {
			return true;
			break;
		}
	}
	return false;
}

template<typename T>
int deep_first_search_find_next(T **& mat, std::vector<int> & result, int row, int column, int current) {
	int next = 0;
	for (int i = 0; i < column; i++) {
		if (mat[current][i] != 0 && !is_in_vector(result,i)) {
			next = i;
			break;
		}
	}
	if (0 != next) return next;
	else
	{
		if (current >= 1)
			deep_first_search_find_next(mat, result, row, column, current - 1);
		else
			std::cerr << "There has isolated point!";
	}
	
}

template<typename T>
void deep_first_search(T **& mat, std::vector<int> & result, int row, int column) {
	if (0 == result.size()) {
		result.push_back(0);	
	}
	if (row == result.size()) return;
	int next = deep_first_search_find_next(mat, result, row, column, result[result.size()-1]);
	std::cout << result[result.size() - 1] << "--->" << next << std::endl;
	result.push_back(next);
	std::cout << "result" << std::endl;
	for (int i = 0; i < result.size(); i++)
		std::cout << result[i] << " ";
	deep_first_search(mat, result, 7, 7);
}

//template<typename T>
//int breadth_first_search_find_next(T **& mat, std::vector<int> & result, int row, int column, int current) {
//	int next = 0;
//	std::cout  << "current:"<<current << '\n';
//	for (int i = 0; i < column; i++) {
//		if (mat[current][i] != 0 && is_in_vector(result, i) && i< current) {
//			mat[current][i] = 0;
//			mat[i][current] = 0;
//			next = breadth_first_search_find_next(mat, result, row, column, i);
//			break;
//		}
//		if (mat[current][i] != 0 && !is_in_vector(result, i)) {
//			next = i;
//			break;
//		}
//	}
//	std::cout << "end" << '\n';
//	if (0 != next) return next;
//	else
//	{
//	
//		std::cerr << "There has isolated point!";
//	}
//}

template<typename T>
void breadth_first_search(T **& mat, std::vector<int> & result, int row, int column) {
	if (0 == result.size()) {
		result.push_back(0);
	}
	if (row == result.size()) return;
	//int next = breadth_first_search_find_next(mat, result, row, column, result[result.size() - 1]);
	for (int i = 0; i < result.size(); i++) {
		for (int j = 0; j < column; j++) {
			if (0 != mat[i][j] && !is_in_vector(result, j)) {
				std::cout << result[i] << "--->" <<j << std::endl;
				result.push_back(j);
			}
		}
	}
	std::cout << "result" << std::endl;
	for (int i = 0; i < result.size(); i++)
		std::cout << result[i] << " ";
	std::cout << std::endl;
}

size_t russian_peasant_multiplication(size_t n, size_t m, size_t bias =0) {
	if (1 == n) return m+bias;
	if (0 == n) return 0;
	if (0 == n % 2) {
		n = n / 2;
		m = m * 2;
	}
	else
	{
		n = (n - 1) / 2;
		bias += m;
		m = 2*m;
	}
	russian_peasant_multiplication(n, m, bias);
}

template<typename T>
void part_vec(std::vector<T> source, std::vector<T>& vec1, std::vector<T> &vec2, bool sort) {
	for (int i = 0; i < vec1.size(); i++) {
		srand((unsigned)time(NULL));
		int p = rand() % source.size();
		vec1[i]= source[p];
		std::vector<T>::iterator it = source.begin() + p;
		source.erase(it);
	}
	if (sort) {
		quick_sort(vec1, 0, vec1.size() - 1);
	}
	for (int i = 0; i < vec2.size(); i++) {
		vec2[i] = source[i];
	}

}