#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum{
	Blocksz = 512,
	Chkstart = 148,
	Chksz = 8,
};

struct posix_header{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
};

union block{
	struct posix_header h;
	char b[Blocksz];
};

int
main(int argc, char *argv[])
{
	FILE *f;
	int i;
	unsigned char *p;
	union block u;
	long sum = 0;

	if(argc != 2)
		errx(1, "I need a file");
	f = fopen(argv[1], "r+");
	if(f == NULL)
		errx(1, "can't open file");
	if(fread(&u, sizeof(u), 1, f) != 1)
		errx(1, "can't read header");
	p = (unsigned char *)&u;
 	for(i=0; i<Blocksz; i++){
		if(i >= Chkstart && i < Chkstart+Chksz){
			sum += ' ';
		}else{
			sum += p[i];
		}
	}
	fprintf(stderr, "old checksum: %s\n", u.h.chksum);
	snprintf(u.h.chksum, 8, "%lo",sum);
	fprintf(stderr, "current checksum: %s\n", u.h.chksum);
	fseek(f, 0, SEEK_SET);
	if(fwrite(&u, sizeof(u), 1, f) != 1)
		errx(1, "can't write header");
	fclose(f);
	exit(0);
}
