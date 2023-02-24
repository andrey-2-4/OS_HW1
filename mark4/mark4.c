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
	int fd_read_write; // для чтения из файла argv[1] и записи в argv[2]
	size_t real_string_size = 0; // чтобы понимать какой актуальный размер, т.к. строку мы немного меняем по заданию
	
	int    fd12[2], result12; // для передачи от процесса 1 к процессу 2
	// открываем канал для процесса 1 и 2
	if(pipe(fd12) < 0) {
		printf("Can\'t open pipe\n");
		exit(-1);
	}
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
		// процесс 1 пишет в канал
		if(close(fd12[0]) < 0){ // закрываем чтение, т.к. процесс 1 только пишет (канал 1 - 2)
  			printf("parent: Can\'t close reading side of pipe\n"); exit(-1);
		}
		write(fd12[1], buffer1, MAX_BUFFER_SIZE);
		if(close(fd12[1]) < 0) { // закончили писать, закрываем (канал 1 - 2)
			printf("parent: Can\'t close writing side of pipe\n");
			exit(-1);
		}
	} else { /* процесс 2 */
		char buffer2[MAX_BUFFER_SIZE]; // буффер для процесса 2
		// процесс 2 читеат из канала
		if(close(fd12[1]) < 0){ // закрываем писание, т.к. процесс 2 только читает (канал 1 - 2)
			printf("child: Can\'t close writing side of pipe\n"); exit(-1);
		}
		read(fd12[0], buffer2, MAX_BUFFER_SIZE);
		if(close(fd12[0]) < 0){ // закончили читать, закрываем (канал 1 - 2)
			printf("child: Can\'t close reading side of pipe\n"); exit(-1);
		}
		// процесс 2 обрабатывает строку
		for (int i = 1; i < sizeof(buffer2); ++i) {
			if (isCapital(buffer2[i])) {
				for (int j = sizeof(buffer2) - 1; j > i; --j) {
					buffer2[j] = buffer2[j - 1];
				}
				buffer2[i] = ' ';
				++i;
			}
		}
		for (int i = 0; i < sizeof(buffer2); ++i) {
			if (buffer2[i] == '\0') {
				break;
			}
			real_string_size++;
		}
		
		int    fd23[2], result23; // для передачи от процесса 2 к процессу 3
		// открываем канал для процесса 2 и 3
		if(pipe(fd23) < 0) {
			printf("Can\'t open pipe\n");
			exit(-1);
		}
		// пытаемся создать процесс 2
		result23 = fork();
		if(result23 < 0) { // если не удалось создать ппроцесс 3
	 		printf("Can\'t fork child\n");
	 		exit(-1);
		} else if (result23 > 0) { /*процесс 2*/
			// процесс 2 пишет в канал
			if(close(fd23[0]) < 0){ // закрываем чтение, т.к. процесс 1 только пишет (канал 2 - 3)
	  			printf("parent: Can\'t close reading side of pipe\n"); exit(-1);
			}
			write(fd23[1], buffer2, MAX_BUFFER_SIZE);
			if(close(fd23[1]) < 0) { // закончили писать, закрываем (канал 1 - 2)
				printf("parent: Can\'t close writing side of pipe\n");
				exit(-1);
			}
		} else { /*процесс 3*/
			char buffer3[MAX_BUFFER_SIZE]; // буффер для процесса 3
			// процесс 3 читеат из канала
			if(close(fd23[1]) < 0){ // закрываем писание, т.к. процесс 3 только читает (канал 2 - 3)
				printf("child: Can\'t close writing side of pipe\n"); exit(-1);
			}
			read(fd23[0], buffer3, MAX_BUFFER_SIZE);
			if(close(fd23[0]) < 0){ // закончили читать, закрываем (канал 1 - 2)
				printf("child: Can\'t close reading side of pipe\n"); exit(-1);
			}
			// процесс 3 пишет в argv[2]
			if ((fd_read_write = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
				printf("coudlnt open the file");
				exit(-1);
			}
			write(fd_read_write, buffer3, real_string_size);
			if (close(fd_read_write) < 0) {
				printf("coudlnt close the file");
				exit(-1);
			}
		}
	}
	return 0;
}
