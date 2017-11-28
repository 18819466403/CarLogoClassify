#pragma once
#include"matrix.h"
#include<iostream>
using namespace std;
void locate(Matrix<int> &matrix, int* &region) {
	region = new int[2];
	if (region == NULL)
	{
		cout << "faiure" << endl;
		return;
	}

	int row = matrix.getRow();
	int column = matrix.getColumn();

	int *average = new int[row];
	for (int i = 0; i < row; i++) {
		average[i] = 0;
	}
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++) {
			average[i] += matrix[i][j];

		}
		average[i] = average[i] / column;
	}
	int r1 = row - 1, r2 = row-1;
	for (int i = 0; i < row; i++) {
		if (average[i] > average[r1] && i>9 && i<100) r1 = i;
	}
	for (int i = 0; i < row; i++) {
		if (average[i] == 0) {
			r2 = i;
			break;
		}
	}
	region[0] = r1;
	region[1] = r2;

}

void quickSort(int *list, int *weight, int m, int n) {
	int i, j, k, t;

	if (m < n) {
		i = m; j = n + 1; k = list[m];
		while (i < j) {
			for (i = i + 1; i < n; i++)
				if (list[i] >= k)
					break;
			for (j = j - 1; j > m; j--)
				if (list[j] <= k)
					break;
			if (i < j)
			{
				t = list[i];
				list[i] = list[j];
				list[j] = t;

				t = weight[i];
				weight[i] = weight[j];
				weight[j] = t;

			}
		}
		t = list[m];
		list[m] = list[j];
		list[j] = t;

		t = weight[m];
		weight[m] = weight[j];
		weight[j] = t;

		quickSort(list, weight, m, j - 1);
		quickSort(list, weight, i, n);
	}
}
void quickSort(int *list, int m, int n) {
	int i, j, k, t;

	if (m < n) {
		i = m; j = n + 1; k = list[m];
		while (i < j) {
			for (i = i + 1; i < n; i++)
				if (list[i] >= k)
					break;
			for (j = j - 1; j > m; j--)
				if (list[j] <= k)
					break;
			if (i < j)
			{
				t = list[i];
				list[i] = list[j];
				list[j] = t;

				

			}
		}
		t = list[m];
		list[m] = list[j];
		list[j] = t;


		quickSort(list, m, j - 1);
		quickSort(list, i, n);
	}
}
void AWMF(Matrix<int> &array, Matrix<int>&result, int M, double g, int w0) {
	int start_tag = M / 2;
	double *distance = new double[M*M];
	int dis_tag = 0;
	for (int i = 0; i<M; i++)
		for (int j = 0; j < M; j++) {
			distance[dis_tag] = sqrt((i - start_tag)*(i - start_tag) + (j - start_tag)*(j - start_tag));
			//	cout << "distance[" << dis_tag << "]=" << distance[dis_tag] << '\n';
			dis_tag++;
		}
	for (int i = 0; i < array.getRow(); i++)
		for (int j = 0; j < array.getColumn(); j++)
			result[i][j] = array[i][j];
	for (int i = start_tag; i<array.getRow() - start_tag; i++)
		for (int j = start_tag; j < array.getColumn() - start_tag; j++) {
			int select = 0;
			int row_index = 0;
			int col_index = 0;
			int window_index = 0;
			int *weight = new int[M*M];
			int *window = new int[M*M];
			int total_weight = 0;
			int pixel_number = M*M;
			double total_gray = 0;
			double mean = 0;
			double var = 0;
			double total_var = 0;
			for (int m = i - start_tag; m <= i + start_tag; m++)
				for (int n = j - start_tag; n <= j + start_tag; n++) {
					total_gray += array[m][n];
					window[window_index] = array[m][n];
					//		cout << window[window_index] << '\t';
					window_index++;
				}
			mean = total_gray / pixel_number;
			for (int m = i - start_tag; m <= i + start_tag; m++)
				for (int n = j - start_tag; n <= j + start_tag; n++) {
					total_var += (array[m][n] - mean)*(array[m][n] - mean);
				}
			var = total_var / pixel_number;
			//	cout << endl;

			for (int q = 0; q < M*M; q++) {
				weight[q] = (w0 - (g*distance[q] * var / mean) + 0.5);
				if (weight[q] < 0) weight[q] = 0;

				//		cout  << weight[q] << '\t';
				total_weight += weight[q];

			}
			//	cout <<'\n'<< "mean:" << mean << '\n';
			//	cout << "var:" << var << '\n';
			quickSort(window, weight, 0, M*M - 1);
			//	for (int q = 0; q < M*M; q++) {
			//		cout << window[q] << '\t';
			//		
			//	}
			//	cout << endl;
			//	for (int q = 0; q < M*M; q++) {
			//		cout <<weight[q] << '\t';

			//	}
			////	cout << endl;
			////	cout << "total weith:" << total_weight << '\n';
			int temp = 0;
			for (int i = 0; i < M*M; i++) {
				temp += weight[i];
				if (temp >= (total_weight + 1) / 2)
				{
					select = i;
					break;
				}
			}

			result[i][j] = window[select];
			//	cout <<array[i][j]<<'\t'<< result[i][j] << '\n';
		}

	delete[]distance;
}

void expand(Matrix<int>& matrix) {
	Matrix<int> temp(matrix.getRow(), matrix.getColumn());
	for (int i = 0; i < matrix.getRow(); i++)
		for (int j = 0; j < matrix.getColumn(); j++)
			temp[i][j] = matrix[i][j];

	for(int i=1;i<matrix.getRow()-1;i++)
		for (int j = 1; j < matrix.getColumn()-1; j++)
		{
			int max = 0;
			for(int m=i-1;m<i+1;m++)
				for (int n = j - 1; n < j + 1; n++) {
					if (temp[m][n] > max) {
						max = temp[m][n];
					}
				}
			matrix[i][j] = max;
		}
}

void etch(Matrix<int>& matrix) {
	Matrix<int> temp(matrix.getRow(), matrix.getColumn());
	for (int i = 0; i < matrix.getRow(); i++)
		for (int j = 0; j < matrix.getColumn(); j++)
			temp[i][j] = matrix[i][j];

	for (int i = 1; i<matrix.getRow() - 1; i++)
		for (int j = 1; j < matrix.getColumn() - 1; j++)
		{
			int min = 256;
			for (int m = i - 1; m<i + 1; m++)
				for (int n = j - 1; n < j + 1; n++) {
					if (temp[m][n] < min) {
						min = temp[m][n];
					}
				}
			matrix[i][j] = min;
		}
}

void open(Matrix<int>&matrix) {
	expand(matrix);
	etch(matrix);
}

void close(Matrix<int>&matrix) {
	etch(matrix);
	expand(matrix);
}

void morphology(Matrix<int>&matrix) {
	close(matrix);
	open(matrix);


}
void compensation(Matrix<int>&matrix) {
	int gln = 128;
	int pixel_number = matrix.getRow()*matrix.getColumn();
	int *temp = new int[pixel_number];
	int index = 0;
	for(int i=0;i<matrix.getRow();i++)
		for (int j = 0; j < matrix.getColumn(); j++) {
			temp[index++] = matrix[i][j];
		}
	quickSort(temp, 0, pixel_number - 1);
	int top20percentage = pixel_number / 5;
	int total = 0;
	int average = 0;
	for (int i = pixel_number - 1; i >= pixel_number - top20percentage; i--) {
		total += temp[i];
	}
	average = total / top20percentage;
	int delta = gln - average;
	
	for (int i = 0; i < matrix.getRow(); i++) {
		for (int j = 0; j < matrix.getColumn(); j++) {
			if (matrix[i][j] - delta > 255)
				matrix[i][j] = 255;
			else
			{
				if (matrix[i][j] - delta < 0)
					matrix[i][j] = 0;
				else
				{
					matrix[i][j] -= delta;
				}
			}
		}
	}

	delete[]temp;
}
