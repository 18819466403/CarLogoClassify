#pragma once
#include<iostream>
#include"image.h"
template<typename T>
class Matrix{
protected:
	T**array;
	int row;
	int column;
public:
	Matrix(int row_number, int column_number);
	Matrix(Image &image);	
	Matrix() {};
	virtual ~Matrix();
	virtual int getRow();
	virtual int getColumn();
	virtual T** getArray();
	virtual void setArray(T** matrix);
	virtual void init(int row_number, int column_number);
	virtual void put(int row_index, int column_index, const T &value);
	virtual const T& getElement(int row_index, int column_index);
	virtual Matrix<T>& getTranspose();
	virtual Matrix<T>& getInverse();
	virtual Matrix<T>& operator*(Matrix<T>& m);
	virtual T* operator[](int i);
	virtual Matrix<T>& operator=(Matrix<T>& right);
	virtual void  show();
	virtual double getDistance(Matrix<T>&back, int frontRow,int backRow); 
	virtual double getAbsolute(int row);
};

template<typename T> Matrix<T>::Matrix(int row_number, int column_number) {
	if (row_number > 0 && column_number > 0) {
		row = row_number;
		column = column_number;
	}
	else {
		row = 1;
		column =1;
	}
	array = new T*[row];
	for (int i = 0; i < row; i++) {
		array[i] = new T[column];
	}
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			array[i][j] = (T)(0);
}



template<typename T> void Matrix<T>::show() {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			std::cout << array[i][j] << ' ';
		}
		std::cout << '\n';
	}
}

template<typename T> Matrix<T>::Matrix(Image &image) {
	row = image.getImage().rows;
	column = image.getImage().cols;
	array = new T*[row];
	for (int i = 0; i < row; i++) {
		array[i] = new T[column];
	}
	for(int i=0;i<row;i++)
		for (int j = 0; j < column; j++) {
			array[i][j] = int(image.getImage().at<uchar>(i, j));
		}
}
template<typename T> Matrix<T>::~Matrix() {
	delete array;
}
template<typename T> void Matrix<T>::init(int row_number, int column_number) {
	if (row_number > 0 && column_number > 0) {
		row = row_number;
		column = column_number;
	}
	else {
		row = 1;
		column = 1;
	}
	array = new T*[row];
	for (int i = 0; i < row; i++) {
		array[i] = new T[column];
	}
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			array[i][j] = (T)(0);
}
template<typename T> int Matrix<T>::getRow() {
	return row;
}

template<typename T> int Matrix<T>::getColumn() {
	return column;
}
template<typename T> T** Matrix<T>::getArray() {
	return array;
}
template<typename T> void Matrix<T>::setArray(T** matrix) {
	for(int i=0;i<row;i++)
		for (int j = 0; j < column; j++)
		{
			array[i][j] = matrix[i][j];
		}
}
template<typename T> T* Matrix<T>::operator[](int i){
	return array[i];
}
template<typename T> Matrix<T>& Matrix<T>::operator=(Matrix<T>& right)
{
	for (int i = 0; i<row; i++)
		for (int j = 0; j < column; j++)
		{
			array[i][j] = right[i][j];
		}
	return *this;
}
template<typename T> double Matrix<T>::getDistance(Matrix<T>& back,int frontRow,int backRow)
{
	double sum = 0;
	for (int i = 0; i < column; i++) {
		sum += (array[frontRow][i] - back[backRow][i])*(array[frontRow][i] - back[backRow][i]);
	}
	sum = sqrt(sum);

	return sum;
}
template<typename T> double Matrix<T>::getAbsolute(int row) {
	double sum = 0;
	for (int i = 0; i < column; i++)
		sum += array[row][i]* array[row][i];
	sum = sqrt(sum);
	return sum;
}

template<typename T> void Matrix<T>::put(int row_index, int column_index, const T &value) {
	array[row_index][column_index] = value;
}

template<typename T> const T& Matrix<T>::getElement(int row_index, int column_index) {
	return array[row_index][column_index];
}

template<typename T> Matrix<T>& Matrix<T>::getTranspose() {
	Matrix<T> *transposesd_array = new Matrix<T>(column, row);
	for (int i = 0; i < column; i++)
		for (int j = 0; j < row; j++)
		{
			transposesd_array->put(i, j, array[j][i]);
		}
	return *transposesd_array;
}


template<typename T> Matrix<T>& Matrix<T>::operator*(Matrix<T> &m2) {
	if (column == m2.getRow()) {
		int r = row;
		int c = m2.getColumn();
		T temp = 0;
	
		Matrix<T> *mutiple = new Matrix<T>(r, c);
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				temp = 0;
				for (int k = 0; k < column; k++) {
					temp += array[i][k] * m2.getElement(k, j);
				}
				mutiple->put(i, j, temp);
			}
		}
		return *mutiple;
	}
}

template<typename T> Matrix<T>& Matrix<T>::getInverse() {

Matrix<T>*iversed_matrix = new Matrix<T>(row, column);
Matrix<T> b(row, 2 * row);
int i, j, k;
for (i = 0; i<row; i++)
{
	for (j = 0; j<row; j++)
	{
		b[i][j] = array[i][j];
	}
}
for (i = 0; i<row; i++)
{
	for (j = row; j<2*row; j++)
	{
		if (i == (j - row))
		{
			b[i][j] = 1;
		}
		else
		{
			b[i][j] = 0;
		}
	}
} for (i = 0; i<row; i++)
{
	if (b[i][i] == 0)
	{
		for (k = i; k<row; k++)
		{
			if (b[k][k] != 0)
			{
				for (int j = 0; j<2*row; j++)
				{
					double temp;
					temp = b[i][j];
					b[i][j] = b[k][j];
					b[k][j] = temp;
				}
				break;
			}
		}
		if (k == row)
		{
			cout << "¸Ã¾ØÕó²»¿ÉÄæ£¡" << endl;
		}
	}
	for (j = 2*row - 1; j >= i; j--)
	{
		b[i][j] /= b[i][i];
	}
	for (k = 0; k<row; k++)
	{
		if (k != i)
		{
			double temp = b[k][i];
			for (j = 0; j<2*row; j++)
			{
				b[k][j] -= temp*b[i][j];
			}
		}
	}
}
for (i = 0; i<row; i++)
{
	for (j = row; j<2*row; j++)
	{
		iversed_matrix->put(i, j - row, b[i][j]);
	}
}

return *iversed_matrix;

}