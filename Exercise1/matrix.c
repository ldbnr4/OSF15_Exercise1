#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

	/* 
	 * PURPOSE: instantiates a new matrix with the given name, rows, cols 
	 * INPUTS:
	 *  new_matrix - the new matrix to be created
	 *	name - the name of the matrix limited to 50 characters 
	 *  rows - the number of rows the matrix
	 *  cols - the number of cols the matrix
	 * RETURN:
	 *  If no errors occurred during instantiation then true
	 *  else false for an error in the process.
	 *
	 */
bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {

	if( !name ){
		printf("New matrix name is NULL.\n");
		return false;
	} //TODO ERROR CHECK INCOMING PARAMETERS

	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

	/* 
	 * PURPOSE: frees the memory allocated for the matrix
	 * INPUT: 
	 *	m - the matrix to be destroyed
	 * RETURN:
	 *  
	 */
void destroy_matrix (Matrix_t** m) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	
	free((*m)->data);
	free(*m);
	*m = NULL;
}
	
	/* 
	 * PURPOSE: checks the equality of two matricies
	 * INPUT: 
	 *	a - the first matrix to compare
	 *	d - the second matrix to compare
	 * RETURN:
	 *  True - if no errors and the matricies are equal
	 *  Fasle - if there are errors with matricies a or b
	 */
bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	
	if (!a || !b || !a->data || !b->data) {
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

	/* 
	 * PURPOSE: makes a copy of the first matrix into the second
	 * INPUT: 
	 *	src - the matrix that is going to duplicated
	 *	dest - the matrix that will be a copy of the src
	 * RETURN:
	 *  True - if no errors and the src matrix is successfully copied into the dest matrix
	 *  Fasle - if there are errors with the src or dest matrix
	 */
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {

	if( !src || !dest ){
		return false;
	}
	//TODO ERROR CHECK INCOMING PARAMETERS

	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

	/* 
	 * PURPOSE: bit shifts the numbers in the given matrix in the given direction by the given amount of times
	 * INPUT: 
	 *	a - the matrix that will have it's contents bit shifted
	 *	direction - the direction the number will be shifted either a left or a right
	 *  shift - the amount of times the number in the matrix will be bit shifted
	 * RETURN:
	 *  True - if the contents of the matrix is successfully bit shfited
	 *  Fasle - if there are errors with matrix a
	 */
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a) {
		return false;
	}

	if (direction == 'l') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

	/* 
	 * PURPOSE: adds the contents of the two given matricies and stores them into a third matrix with the given name
	 * INPUT: 
	 *	a - the first matrix that will be added with the second matrix
	 *	b - the second matrix that will be added with the first matrix
	 *  c - the result matrix of a and b
	 * RETURN:
	 *  True - if the contents of the two matricies are successfully and stored into the third
	 *  Fasle - if there are errors with matrix a and b
	 */
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	if( !a || !b ){
		return false;
	}//TODO ERROR CHECK INCOMING PARAMETERS

	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

	/* 
	 * PURPOSE: displays the contents of the given matrix
	 * INPUT: 
	 *	m - the matrix whoes content will be printed
	 * RETURN:
	 *
	 */
void display_matrix (Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS


	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

	/* 
	 * PURPOSE: reads in a file with the given name and creates a matrix from it
	 * INPUT: 
	 *	matrix_input_filename - the name of the file that has the content for the new matrix to be created
	 *	m - the matrix that will be loaded with the contents of the read file
	 * RETURN:
	 *  True - if the new matrix has been created from the given file name
	 *  Fasle - if there are errors in the process
	 */
bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	if( !matrix_input_filename ){
		return false;
	}//TODO ERROR CHECK INCOMING PARAMETERS


	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);
	free(data);
	if (close(fd)) {
		return false;

	}
	return true;
}

	/* 
	 * PURPOSE: writes the contents of the given matrix to a file
	 * INPUT: 
	 *	matrix_output_filename - the name of the file that will be created from the given matrix
	 *	m - the matrix to be written to the file
	 * RETURN:
	 *  True - if the matrix has been successfully written to a file
	 *  Fasle - if there are errors in the process
	 */
bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	if( !matrix_output_filename || !m){
		return false;
	}//TODO ERROR CHECK INCOMING PARAMETERS

	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

	/* 
	 * PURPOSE: randomizes the numbers in the given matrix within a specific range
	 * INPUT: 
	 *	m - the matrix to have numbers randomized
	 *	start_range - the lowest number that will can be used in the matrix
	 *	end_range - the greatest number than can be used in the matrix
	 * RETURN:
	 *  True - if the matrix has been successfully randomized within the range
	 *  Fasle - if there are errors
	 */
bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	if( !m || (start_range > end_range) ){
		return false;
	}//TODO ERROR CHECK INCOMING PARAMETERS

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % (end_range + 1 - start_range) + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

	/* 
	 * PURPOSE: copies the given data into the given matrix
	 * INPUT: 
	 *	m - the matrix that will be loaded with the given data
	 *	data - the numbers that will be loaded into the matrix
	 * RETURN:
	 * 
	 */
void load_matrix (Matrix_t* m, unsigned int* data) {
	
	if( !m || !data ){
		printf("Null pointer to matrix or data");
		return;
	}//TODO ERROR CHECK INCOMING PARAMETERS
	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

	/* 
	 * PURPOSE: adds the given matrix to the given array
	 * INPUT: 
	 *	mats - the array of matricies
	 *	new_matrix - the matrix that will be added to the array
	 *	num_mats - the number of matricies
	 * RETURN:
	 *  pos - an int value of the next poition in the array after the matrix was added
	 */
unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	if( !new_matrix ){
		return 99;
	}//TODO ERROR CHECK INCOMING PARAMETERS
	
	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;
	return pos;
}
