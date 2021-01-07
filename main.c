#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

	//argc -> girilen argüman sayısını veriyor

	printf("total arguman sayısı : %d\n",argc);

	int i = 0;
	int numPublisherType = -1;
	int numPublisherCount = -1; //in total, there will be numPublisherType * numPublisherCount threads.
	int numPackagerCount = -1;

	int numPublishingBook = -1;

	int numPackagerBook = -1;
	int bufferSize = -1;

	for(i = 1 ; i < argc-1 ; i++){

		int sc = -1;
		if(strcmp(argv[i],"-n") == 0){sc = 0;} // if option is -n
		else if(strcmp(argv[i],"-b") == 0){sc = 1;} // if option is -b
		else if(strcmp(argv[i],"-s") == 0){sc = 2;} // // if option is -s

		switch(sc){
			case 0:
			numPublisherType = atoi(argv[i+1]); // this is for publisher type threads
			numPublisherCount = atoi(argv[i+2]); // this is for publisher type threads count
			numPackagerCount = atoi(argv[i+3]); // this is for packager threads count
				break;
			case 1:
			numPublishingBook = atoi(argv[i+1]); // this indicates how many book each publisher thread can publish
				break;
			case 2:
			numPackagerBook = atoi(argv[i+1]); // this indicates how many book each packager thread can package
			bufferSize = atoi(argv[i+2]); // this indicates buffer size 
				break;
			default:
				break;
		}
		
	}

	printf("pub type : %d , pub count : %d , pack count : %d , num book : %d , pack book num : %d , buffer size : %d\n",
			numPublisherType,numPublisherCount,numPackagerCount,numPublishingBook,numPackagerBook,bufferSize);



	return 0;
}



// programda kitap yayınlayıcıları ve yayınlanan kitapları paketleyen packergerslar olacak

//command line üzerinden kaç tane publisher type olacagını , publisher sayısını ve packager sayısını gircez

//her publisher type sadece 1 tür kitap yayınlayacak. her yayıncının kaç tane kitap yayınlayacagı da command
//line üzerinden input olarak verilecek

//kitap ismi type ve kaçıncı kitap olusuna göre isimlendirilecek

//yeni kitap yayınlandıgında o kitabın türüyle ilgili olan threadler o türün buffer ı içine yerleştirecek
//Bu bufferların başlangıc uzunlukları da command linedan verilecek

//buffer size doldugu zaman yeni kitap koymak isteyen thread bu bufferın size ını 2ye katlayacak

//Eger istenilen rakamda kitap üretilmişse thread systemden cıkacak

//packager threadlerimz bu bufferlardan kitap alacak. packargerlar bütün bufferlardan kitap alabilir

//her packager thread istediği kadar kitap alabilecek. Bu sayı da command linedan verilecek

//bir paket hazırlandıgında , packager thread diğerini hazırlayacak

//packager kitap türünü rastgele seçecek ve sonra o kitabı kendi paketine koyacak (ne diyiy la burda)

//diyelim packagerın gittiği buffer listesi boş . bu durumda packager thread sisteme bakacak bu kitap türünü
//üreten thread var mı diye eğer varsa onu bekleyecek. Yoksa bir başka buffer a gidecek

//packager threadimiz eger bütün bufferları boş bulduysa ve sistemde publisher thread de yok ise
//Only i of j number of books could be packaged bu şekilde mesaj bastırp sistemi terk edecek
