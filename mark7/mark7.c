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

int isLower(char s) {
	if (s == 'q' || s == 'w' || s == 'e' || s == 'r' || s == 't' || s == 'y' || s == 'u' || s == 'i' || s == 'o'
	|| s == 'p' || s == 'a' || s == 'd' || s == 'f' || s == 'g' || s == 'h' || s == 'k' || s == 'l' || s == 'z'
	|| s == 'x' || s == 'c' || s == 'v' || s == 'b' || s == 'n' || s == 'm' || s == 's' || s == 'j') {
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int fd_read_write; // для чтения из файла argv[1] и записи в argv[2]
	size_t real_string_size = 0; // чтобы понимать какой актуальный размер, т.к. строку мы немного меняем по заданию
	
	int    fd12, result12; // для передачи от процесса 1 к процессу 2
        char    name12[] = "aaa.fifo";
        // для передачи от процесса 2 к процессу 1
        int    fd21;
        char    name21[] = "bbb.fifo";
        
        mknod(name12, S_IFIFO | 0666, 0);
        mknod(name21, S_IFIFO | 0666, 0);
  	(void)umask(0);
	// пытаемся создать процесс 2
	result12 = fork();
	if(result12 < 0) { // если не удалось создать ппроцесс 2
 		printf("Can\'t fork child\n");
 		exit(-1);
	} else if (result12 > 0) { /* процесс 1 */
		char buffer1[MAX_BUFFER_SIZE]; // буффер для процесса 1
		// процесс 1 читает из файла argv[1]
		if ((fd_read_write = open(argv[1], O_RDONLY, 0666)) < 0) {
			printf("coudlnt open the file");
			exit(-1);
		}
		size_t read_bytes = read(fd_read_write, buffer1, MAX_BUFFER_SIZE);
		buffer1[read_bytes] = '\0';
		if(close(fd_read_write) < 0) {
			printf("coudlnt close the file");
			exit(-1);
		}
		// процесс 1 пишет в aaa.fifo
		if ((fd12 = open(name12, O_WRONLY, 0666)) < 0) {
			printf("coudlnt open the FIFO");
			exit(-1);
		}
		write(fd12, buffer1, MAX_BUFFER_SIZE);
		if (close(fd12) < 0) {
			printf("coudlnt close the FIFO");
			exit(-1);
		}
		// процесс 1 читеат из bbb.fifo
		if((fd21 = open(name21, O_RDONLY)) < 0) {
			printf("Can\'t open FIFO for reading\n");
			exit(-1);
		}
		read(fd21, buffer1, MAX_BUFFER_SIZE);
		if(close(fd21) < 0) {
			printf("child: Can\'t close FIFO\n"); exit(-1);
		}
		// процесс 1 пишет в argv[2]
		if ((fd_read_write = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
			printf("coudlnt open the file");
			exit(-1);
		}
		// подсчет реальной длины строки
		for (int i = 0; i < sizeof(buffer1); ++i) {
			if (buffer1[i] == '\0') {
				real_string_size = i;
				break;
			}
		}
		write(fd_read_write, buffer1, real_string_size);
		if (close(fd_read_write) < 0) {
			printf("coudlnt close the file");
			exit(-1);
		}
	} else { /* процесс 2 */
		char buffer2[MAX_BUFFER_SIZE]; // буффер для процесса 2
		// процесс 2 читеат из aaa.fifo
		if((fd12 = open(name12, O_RDONLY)) < 0) {
			printf("Can\'t open FIFO for reading\n");
			exit(-1);
	        }
	        read(fd12, buffer2, MAX_BUFFER_SIZE);
	        if(close(fd12) < 0) {
			printf("child: Can\'t close FIFO\n"); exit(-1);
	        }
		// процесс 2 обрабатывает строку
		for (int i = 0; i < sizeof(buffer2); ++i) {
			if (buffer2[i] == '\0') {
				real_string_size = i;
				break;
			}
			if (!isLower(buffer2[i]) && !isCapital(buffer2[i])) {
				for (int j = i; j < sizeof(buffer2) - 1; ++j) {
					buffer2[j] = buffer2[j + 1];
				}
				--i;
			} else if (isCapital(buffer2[i])) {
				while (isLower(buffer2[i + 1])) {
					++i;
				}
				++i;
				for (int j = sizeof(buffer2) - 1; j > i; --j) {
					buffer2[j] = buffer2[j - 1];
				}
				buffer2[i] = ' ';
			} else {
				for (int j = i; j < sizeof(buffer2) - 1; ++j) {
					buffer2[j] = buffer2[j + 1];
				}
				--i;
			}
			// printf("%s", buffer2); проверка того, как работает обработка
		}
		// процесс 2 пишет в bbb.fifo
		if ((fd21 = open(name21, O_WRONLY, 0666)) < 0) {
			printf("coudlnt open the FIFO");
			exit(-1);
		}
		write(fd21, buffer2, MAX_BUFFER_SIZE);
		if (close(fd21) < 0) {
			printf("coudlnt close the FIFO");
			exit(-1);
		}
	}
	return 0;
}
