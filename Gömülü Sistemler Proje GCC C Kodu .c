#define TERMINAL "/dev/ttyPS0"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>
#include <time.h>


// Make the SDK console work in the debugger
//#define printf(...) \
//fprintf(stdout,	VA_ARGS	); \
//fflush(stdout);
//typedef long long int u64;

char karakter[255];
int sayi1[255];
int sayi2[255];
int i;
int j;

int main()
{
	unsigned int gpio_size = 0x8000;
	off_t gpio_pbase = 0x41200000;
	long long *gpio64_vptr;
	int fdgpio;
	
	char *portname = TERMINAL;
	int fd;
	int wlen;
	char *xstr = "HESAPLAMAK ISTEDIGINIZ IKI DEGER GONDERIN: \n";
	int xlen = strlen(xstr);
fd = open (portname, O_RDWR | O_NOCTTY| O_SYNC);
if (fd < 0) {
	printf("Error opening %s: %s\n", portname, strerror(errno));
	return 1;
}
	wlen = write(fd, xstr, xlen); //PYNQ'den PC'ye bilgilendirme mesajı bastık.
	if (wlen != xlen){
		printf("Error from write: Xd, %d\n", wlen, errno);
	}

do{
	unsigned char buf[80];
	int rdlen;

	rdlen = read (fd, buf, sizeof(buf) -1); //Kullanıcının PC'den gönderdiği değerleri Array'e yazdık.
	if (rdlen > 0){

	buf[rdlen]=0;
	printf("Kullanicinin Gonderdigi Degerler: %s \n", buf);
	
    j = strlen(buf);
    int kontrol = 0;
	
    for (i = 0; i < j; i++) //Buf arayında aralarında tire "-" olacak şekilde verileri aldık.
    {
        if(buf[i]== '-'){ //Bu char "-" ile gönderilen iki faklı sayıya array'de ulaştık.
            kontrol = 1;
        } else{
            if(kontrol == 0){
                sayi1[i] = buf[i] - '0'; 
            }
            else {
                sayi2[i] = buf[i] - '0';
            }
        }
    }

    int valuefirst = sayi1[1] + sayi1[0]*10; //Char olarak gelen bu değerleri İnteger haline dönüştürdük.
    printf("Deger1:%d \n", valuefirst);
    int valuesecond = sayi2[4] + sayi2[3]*10;
    printf("Deger2:%d \n", valuesecond);
	
	if ((fdgpio = open("/dev/mem", O_RDWR | O_SYNC)) != -1) {

		gpio64_vptr = (long long *)mmap(NULL, gpio_size,PROT_READ|
		PROT_WRITE, MAP_SHARED, fdgpio, gpio_pbase); //MMAP Komutu ile GPİO'nun sanal adresine ulaştık.
		printf("MMAP Sanal Adress: %x\n", gpio64_vptr); //Bu sanal adres sayesinde GPİO giriş ve çıkışlarına veri yazdık.
		*(gpio64_vptr)  = valuefirst; //gpio 0.adresine kullanıcının gönderdiği 1. değeri gönderdik.
		*(gpio64_vptr + 1) = valuesecond; //gpio 8.adresine kullanıcının gönderdiği 2. değeri gönderdik.
	
		long sonuc = *(gpio64_vptr + 1); //gpio 8.adresinden PL tarafında hesaplanan değeri aldık.
		printf("Hesaplanan Deger: %d \n:" , sonuc);
		char *mtn = "HESAPLANAN DEGER: \n";
		int mtnlen = strlen(mtn);
		write(fd, mtn, mtnlen);
		
		char Gsonuc[20];
		sprintf(Gsonuc, "%d", sonuc);
		int Gsonuclen = strlen(Gsonuc);
		write(fd, Gsonuc, Gsonuclen); // Uart arayüzü ile PYNQ'ten PC'ye hesaplanan değeri gönderdik.
		close(fdgpio);
		}
	} else if (rdlen < 0) {
		printf("Error from read: %d: %s\n", rdlen, strerror (errno));
	}else { /* rdlen == 0 */
		printf("Timeout from read\n");
	}

	}while (1);
}