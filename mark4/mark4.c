#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

const int MAX_BUFFER_SIZE = 5000;

int isCapital(char s) {
	if (s == 'Q' || s == 'W' || s == 'E' || s == 'R' || s == 'T' || s == 'Y' || s == 'U' || s == 'I' || s == 'O'
	|| s == 'P' || s == 'A' || s == 'D' || s == 'F' || s == 'G' || s == 'H' || s == 'K' || s == 'L' || s == 'Z'
	|| s == 'X' || s == 'C' || s == 'V' || s == 'B' || s == 'N' || s == 'M' || s == 'S' || s == 'J') {
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int fd;
	char buffer[MAX_BUFFER_SIZE];
	
	
	if ((fd = open(argv[1], O_RDONLY, 0666)) < 0) {
		printf("coudlnt open the file");
		exit(-1);
	}
	size_t read_bytes = read(fd, buffer, MAX_BUFFER_SIZE);
	buffer[read_bytes] = '\0';
	if(close(fd) < 0) {
		printf("coudlnt close the file");
		exit(-1);
	}
	
	for (int i = 1; i < sizeof(buffer); ++i) {
			if (isCapital(buffer[i])) {
				for (int j = sizeof(buffer) - 1; j > i; --j) {
					buffer[j] = buffer[j - 1];
				}
				buffer[i] = ' ';
				++i;
			}
	}
	size_t real_string_size = 0;
	for (int i = 0; i < sizeof(buffer); ++i) {
		if (buffer[i] == '\0') {
			break;
		}
		real_string_size++;
	}
	
	
	if ((fd = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
		printf("coudlnt open the file");
		exit(-1);
	}
	write(fd, buffer, real_string_size);
	if (close(fd) < 0) {
		printf("coudlnt close the file");
		exit(-1);
	}
	
	return 0;
}
