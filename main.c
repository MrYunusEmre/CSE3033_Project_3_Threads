#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h> 
#include <semaphore.h>

struct args{
	int type;
	int index;
};


int checkDigit(char temp[]){
	int length = strlen(temp);
	int i = 0;

	for(i = 0 ; i < length; i++){
		if(isdigit(temp[i])){
			return 1;
		}
	}

	return 0;
}

int getAndCheckArguments(int argc , char *argv[], int *numPublisherType , int *numPublisherCount,int *numPackagerCount,
 																	int *numPublishingBook, int *numPackagerBook, int *bufferSize){
	//argc -> girilen argüman sayısını veriyor
	if(argc != 10){
		printf("Please check your arguments!\nPlease just write 0 , If you do not want to enter any threads!\n");
		return -1;
	}

	int i = 0;

	for(i = 1 ; i < argc-1 ; i++){

		int sc = -1;

		char temp[50];
		strcpy(temp,argv[i]);
		if(strcmp(argv[i],"-n") == 0){sc = 0;} // if option is -n
		else if(strcmp(argv[i],"-b") == 0){sc = 1;} // if option is -b
		else if(strcmp(argv[i],"-s") == 0){sc = 2;} // // if option is -s
		else if(!checkDigit(temp)){
			printf("Please check your arguments !\n");
			return -1;
		}


		switch(sc){
			case 0:{ // -n
			char temp1[10]; char temp2[10]; char temp3[10];
			strcpy(temp1,argv[i+1]); strcpy(temp2,argv[i+2]); strcpy(temp3,argv[i+3]); //This part indicates that
			if(!(checkDigit(temp1) && checkDigit(temp2) && checkDigit(temp3))){		   //you have to give 3 digit after -n option.
				printf("Please check your arguments !\n");
				return -1;
			}
			*numPublisherType = atoi(argv[i+1]); // this is for publisher type threads
			*numPublisherCount = atoi(argv[i+2]); // this is for publisher type threads count
			*numPackagerCount = atoi(argv[i+3]); // this is for packager threads count
				break;

			}

			case 1:{ // -b
			char temp1[10]; strcpy(temp1,argv[i+1]);
			if(!(checkDigit(temp1))){						//This part indicates that you have to give 1 digit after -b option
				printf("Please check your arguments !\n");
				return -1;
			}	
			*numPublishingBook = atoi(argv[i+1]); // this indicates how many book each publisher thread can publish
				break;
				}
				
			case 2:{ // -s
			char temp1[10]; char temp2[10];	
			strcpy(temp1,argv[i+1]); strcpy(temp2,argv[i+2]);
			if(!(checkDigit(temp1) && checkDigit(temp2))){
				printf("Please check your arguments !\n");
				return -1;
			}
			*numPackagerBook = atoi(argv[i+1]); // this indicates how many book each packager thread can package
			*bufferSize = atoi(argv[i+2]); // this indicates buffer size 
				break;
			}
			default:
				break;
		}
		
	}	

	return 0;


}

//--------------------------------------------------// Program bunun altında başlıyor 



void *publisher(void *Args){

	struct args *pArgs = (struct args *)Args;


	//printf("thread type : %d , thread count : %d is created!\n",pArgs->type,pArgs->index);
	pthread_exit((void*)Args);
}

void *packager(void *Args){
	struct args *pgArgs = (struct args *)Args;

	pthread_exit((void*)Args);
}


int main(int argc, char *argv[]){

	int numPublisherType = -1;
	int numPublisherCount = -1; //in total, there will be numPublisherType * numPublisherCount threads.
	int numPackagerCount = -1;
	int numPublishingBook = -1;
	int numPackagerBook = -1;
	int bufferSize = -1;

	//Argument Error handling 
	if(getAndCheckArguments(argc,argv,&numPublisherType,&numPublisherCount,&numPackagerCount,&numPublishingBook,&numPackagerBook,&bufferSize) == -1){
		return -1;
	}


	printf("pub type : %d , pub count : %d , pack count : %d , num book : %d , pack book num : %d , buffer size : %d\n",
			numPublisherType,numPublisherCount,numPackagerCount,numPublishingBook,numPackagerBook,bufferSize);


	//toplam publisher thread sayısı belli oldugu için fix size array olusturulabilir
	pthread_t publishers[numPublisherType * numPublisherCount];

	//packager sayısı bilindiğinden fix size array olusturulabilir
	pthread_t packagers[numPackagerCount];

	void * status;
	int rc;
	//Thread metodlarına en fazla 1 argument gönderebiliyoruz !!!


	int i = 0; int j = 0; int pIndex = 0; 

	for(i = 1; i <= numPublisherType; i++){
		for(j = 1; j <= numPublisherCount; j++){
			struct args pArgs;
			pArgs.type = i;
			pArgs.index = j;
			printf("MAIN : creating publisher thread type : %d , index : %d\n",i,j);
			rc = pthread_create(&(publishers[pIndex]),NULL,&publisher,&pArgs);
			if(rc){
				printf("ERRROOOOOOOOOR\n");
			}


			pIndex++;
		}
	}
	
	for(i = 0; i < numPackagerCount; i++){
		struct args pgArgs;
		pgArgs.type = -1;
		pgArgs.index = i+1;
		printf("MAIN : creating packager thread index : %d\n",i);
		rc = pthread_create(&(packagers[i]),NULL,&packager,&pgArgs);
		if(rc){
			printf("ERRROOOOOOOOOR\n");
		}
	}

	//Waiting part

	pIndex = 0;
	for(i = 1; i <= numPublisherType; i++){
		for(j = 1; j <= numPublisherCount; j++){

			rc = pthread_join(publishers[pIndex], &status);
	      if (rc) {
	         printf("ERROR; return code from pthread_join() is %d %d %d\n", rc, i , j);
	         exit(-1);
	         }
	      printf("Main: completed join with thread %ld having a status of %ld\n",pIndex,(long)status);			
			pIndex++;
		}
	}

	for(i = 0; i < numPackagerCount;i++){
		rc = pthread_join(packagers[i], &status);
	      if (rc) {
	         printf("ERROR; return code from pthread_join() is %d %d\n", rc, i );
	         exit(-1);
	         }
	      printf("Main: completed join with thread %ld having a status of %ld\n",i,(long)status);
		
	}



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
