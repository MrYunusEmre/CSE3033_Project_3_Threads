#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h> 
#include <semaphore.h>


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


//Global Variables
int numPublisherType = -1;
int numPublisherCount = -1; //in total, there will be numPublisherType * numPublisherCount threads.
int numPackagerCount = -1;
int numPublishingBook = -1;
int numPackagerBook = -1;
int bufferSize = -1;


typedef struct args{
	int type;
	int index;
	int bookCount;
}args;

//publisher part
struct publisherBufferList{ // this struct will hold books for each type 
	int bookIndex;
	char bookName[50];
	struct publisherBufferList *nextPtr;
};

typedef struct publisherBufferList PublisherBufferList;
typedef PublisherBufferList *PublisherBufferPtr;

struct publisherTypeList{ // this struct will hold publisher type list
	int pType;
	int pIndex;
	int bufSize; // this is the buffer size for each type of publisher buffer
	int threadCount;
	sem_t semaphore_queue;
	struct publisherBufferList *bufferPtr; // this will hold the buffer for corresponding type
	struct publisherTypeList *nextPtr;
};

typedef struct publisherTypeList PublisherTypeList;
typedef PublisherTypeList *PublisherTypePtr;





//packager part
struct packagerBufferList{
	char bookName[50];
	struct packagerBufferList *nextPtr;
};

typedef struct packagerBufferList PackagerBufferList;
typedef PackagerBufferList *PackagerBufferListPtr;

struct packagerList{
	int packIndex;
	int packSize;
	struct packagerBufferList *bufferPtr; // this will hold the buffer for corresponding packager
	struct packagerList *nextPtr;
};

typedef struct packagerList PackagerList;
typedef PackagerList *PackagerListPtr;




// Insert to the publisher type list
void insertPublisherType(PublisherTypePtr *sPtr,int pType,int pIndex,int bufSize,int threadCount){

	PublisherTypePtr newPtr = malloc(sizeof(PublisherTypeList));

	if(newPtr != NULL){
		newPtr->pType = pType;
		newPtr->pIndex = pIndex;
		newPtr->bufSize = bufSize;
		newPtr->threadCount = threadCount;
		sem_init(&newPtr->semaphore_queue , 0 , 1);

		PublisherBufferPtr bufferStartPtr = NULL;


		newPtr->bufferPtr = bufferStartPtr;
		newPtr->nextPtr = NULL;

		PublisherTypePtr previousPtr = NULL;
		PublisherTypePtr currentPtr = *sPtr;

		while(currentPtr != NULL){
			previousPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		if(previousPtr == NULL){
			newPtr->nextPtr = *sPtr;
			*sPtr = newPtr;
		}
		else{
			previousPtr->nextPtr = newPtr;
			newPtr->nextPtr = currentPtr;
		}
	}
	else{
		printf("There is no memory available.\n");
	}
}

// Insert to the publisher buffer list
void insertPublisherBuffer(PublisherBufferPtr *sPtr,int bookIndex,char bookName[]){

	PublisherBufferPtr newPtr = malloc(sizeof(PublisherBufferList));

	if(newPtr != NULL){

		newPtr->bookIndex = bookIndex;
		strcpy(newPtr->bookName,bookName);
		newPtr->nextPtr = NULL;

		PublisherBufferPtr previousPtr = NULL;
		PublisherBufferPtr currentPtr = *sPtr;

		while(currentPtr != NULL){
			previousPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		if(previousPtr == NULL){
			newPtr->nextPtr = *sPtr;
			*sPtr = newPtr;
		}
		else{
			previousPtr->nextPtr = newPtr;
			newPtr->nextPtr = currentPtr;
		}
	}
	else{
		printf("There is no memory available.\n");
	}
}

// Insert to the packager list
void insertPackagerList(PackagerListPtr *sPtr,int packIndex,int packSize, char bookName[]){

	PackagerListPtr newPtr = malloc(sizeof(PackagerList));

	if(newPtr != NULL){
		newPtr->packIndex = packIndex;
		newPtr->packSize = packSize;
		newPtr->nextPtr = NULL;

		PackagerBufferListPtr pBufferPtr = NULL;
		newPtr->bufferPtr = NULL;
		

		PackagerListPtr previousPtr = NULL;
		PackagerListPtr currentPtr = *sPtr;

		while(currentPtr != NULL){
			previousPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}	

		if(previousPtr == NULL){
			newPtr->nextPtr = *sPtr;
			*sPtr = newPtr;
		}
		else{
			previousPtr->nextPtr = newPtr;
			newPtr->nextPtr = currentPtr;
		}
	}
	else{
		printf("There is no memory available.\n");
	}
}

void insertPackagerBufferList(PackagerBufferListPtr *sPtr , char bookName[]){

	PackagerBufferListPtr newPtr = malloc(sizeof(PackagerBufferList));

	if(newPtr != NULL){

		strcpy(newPtr->bookName,bookName);
		newPtr->nextPtr = NULL;

		PackagerBufferListPtr previousPtr = NULL;
		PackagerBufferListPtr currentPtr = *sPtr;

		while(currentPtr != NULL){
			previousPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		if(previousPtr == NULL){
			newPtr->nextPtr = *sPtr;
			*sPtr = newPtr;
		}
		else{
			previousPtr->nextPtr = newPtr;
			newPtr->nextPtr = currentPtr;
		}
	}
	else{
		printf("There is no memory available.\n");
	}

}


int isEmptyType(PublisherTypePtr sPtr){return sPtr == NULL;}
int isEmptyPackage(PackagerListPtr sPtr){return sPtr == NULL;}
int isEmptyBuffer(PublisherBufferPtr sPtr){return sPtr == NULL;}


void printPackagerList(PackagerListPtr currentPtr){

	if(isEmptyPackage(currentPtr)){
		puts("List is empty");
	}
	else{
		while(currentPtr != NULL){
			printf("packIndex : %d packSize : %d\n",currentPtr->packIndex , currentPtr->packSize);
			currentPtr = currentPtr->nextPtr;
		}
	}
}

void printBufferList(PublisherBufferPtr currentPtr){

	if(isEmptyBuffer(currentPtr)){
		puts("List is empty");
	}
	else{
		while(currentPtr != NULL){
			printf("Book Index : %d , Book Name : %s\n",currentPtr->bookIndex,currentPtr->bookName);
			currentPtr = currentPtr->nextPtr;
		}
	}
}

void printPubTypeList(PublisherTypePtr currentPtr){

	if(isEmptyType(currentPtr)){
		puts("List is empty");
	}
	else{
		while(currentPtr != NULL){
			printf("pType : %d bufSize : %d\n",currentPtr->pType,currentPtr->bufSize);
			printBufferList(currentPtr->bufferPtr);
			currentPtr = currentPtr->nextPtr;
		}
	}
}


void initiliazeBuffer(PublisherTypePtr *sPtr , int typeIndex){

	PublisherTypePtr tempPtr = *sPtr;

	while(tempPtr != NULL){
		if(tempPtr->pType == typeIndex){
			break;
		}
		tempPtr = tempPtr -> nextPtr;
	}

	int i = 0;
	for(i = 0; i < tempPtr->bufSize; i++){
		insertPublisherBuffer(&(tempPtr->bufferPtr) , -1 , "" );
	}

}

void initiliazePackagerBuffer(PackagerListPtr *sPtr, int index){

	PackagerListPtr tempPtr = *sPtr;

	while(tempPtr != NULL){
		if(tempPtr->packIndex == index){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	int i=0 ;
	for(i=0 ; i<numPackagerBook ; i++){
		insertPackagerBufferList(&(tempPtr->bufferPtr) , "");
	}

}

//GLOBAL VARIABLES 

PublisherTypePtr publisherStartPtr = NULL;

PackagerListPtr packagerStartPtr = NULL;


int getPublishedBookSize(int type){ // this method will give us the number of books in the buffer

	PublisherTypePtr tempPtr = publisherStartPtr;

	while(tempPtr != NULL){ // bütün type listesini gezecek ve dogru type ı bulacak
		if(tempPtr->pType == type){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	PublisherBufferPtr tempBuffer = tempPtr->bufferPtr;

	int count = 0;
	while(tempBuffer != NULL){
		if(tempBuffer->bookIndex != -1 && tempBuffer->bookIndex != -2){ //kitap isminin uzunlugundan node içinde kitap var mı yok mu anlayabilrz
			count++;
		}

		tempBuffer = tempBuffer->nextPtr;
	}

	return count;
}

int getNodeNumberInBuffer(int type){

	PublisherTypePtr tempPtr = publisherStartPtr;

	while(tempPtr != NULL){ // bütün type listesini gezecek ve dogru type ı bulacak
		if(tempPtr->pType == type){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	PublisherBufferPtr tempBuffer = tempPtr->bufferPtr;

	int count = 0;
	while(tempBuffer != NULL){
		count++;

		tempBuffer = tempBuffer->nextPtr;
	}

	return count;

}

void resizeBuffer(int type){

	PublisherTypePtr tempPtr = publisherStartPtr;

	while(tempPtr != NULL){ // bütün type listesini gezecek ve dogru type ı bulacak
		if(tempPtr->pType == type){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	int previousSize = tempPtr->bufSize;
	tempPtr->bufSize = tempPtr->bufSize * 2; //size is doubled here

	int i = 0;

	for(i = 0 ; i < previousSize ; i++){ // bir önceki size kadar yeni node üretecek
		insertPublisherBuffer(&(tempPtr->bufferPtr) , -1 , "" ); //creating new nodes 
	}


}

void insertToBuffer(int index , char bookName[], PublisherTypePtr *tempPtr ){

	PublisherBufferPtr tempBuffer = (*tempPtr)->bufferPtr;

	while(tempBuffer->bookIndex != -1 && tempBuffer->bookIndex != -2){
		tempBuffer = tempBuffer->nextPtr;
	}

	tempBuffer->bookIndex = index;
	strcpy(tempBuffer->bookName,bookName);

}


void publishBook(int type, int bookIndex , int index){ // dogru type ı bulup onun bufferına eklemek lazım


	PublisherTypePtr tempPtr = publisherStartPtr;

	while(tempPtr != NULL){ // bütün type listesini gezecek ve dogru type ı bulacak
		if(tempPtr->pType == type){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	char buf[12];
	snprintf(buf, 12, "Book%d_%d", type ,tempPtr->pIndex+1);

	printf("Publisher %d of type %d \t%s is published and put into the buffer %d.\n",index,type , buf,type);

	insertToBuffer(tempPtr->pIndex+1,buf , &tempPtr);

	tempPtr->pIndex = tempPtr->pIndex + 1 ;
	

}


void *publisher(void *Args){

	struct args *pArgs = (struct args *)Args;
	int type = pArgs->type;
	int index = pArgs->index;

	PublisherTypePtr tempPtr = publisherStartPtr;

	while(tempPtr != NULL){

		if(type == tempPtr->pType) break;

		tempPtr = tempPtr->nextPtr;
	}



//	printf("\nI am thread %d and I am in the semaphore queue now - type : %d , index : %d\n",(int)pthread_self(),type,index);

	sem_wait(&(tempPtr->semaphore_queue));

	//for döngüsü olacak her publisherın yayınlayabileceği kadar dönecek
	int i;

//	printf("publisherda type : %d , index : %d thread içeri girdi -> %d\n",type, index , (int)pthread_self());

	for(i = 1; i <= numPublishingBook; i++){

		if(getPublishedBookSize(type) == getNodeNumberInBuffer(type) ){
			printf("Publisher %d of type %d \tBuffer is full. Resizing the buffer.\n",index,type);
			resizeBuffer(type);
		}

		publishBook(type, i , index);

		if(i == numPublishingBook){
			tempPtr->threadCount -=  1;
			printf("Publisher %d of type %d \tFinished publishing %d books. Exiting the system.\n",index,type,numPublishingBook);
		}
	}


//	printf("I am existing now. (My ID: %d ) type : %d index : %d\n",(int)pthread_self() , type , index);
	sem_post(&(tempPtr->semaphore_queue));

}

//

void packageBook(int type, int index){

	PublisherTypePtr tempPtr = publisherStartPtr;
	while(tempPtr != NULL){
		if(tempPtr->pType == type){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	PackagerListPtr packagerTempPtr = packagerStartPtr;
	while(packagerTempPtr != NULL){
		if(packagerTempPtr->packIndex == index){
			break;
		}
		packagerTempPtr = packagerTempPtr->nextPtr ;
	}

	PackagerBufferListPtr bufferPtr = packagerTempPtr->bufferPtr ;
	


	PublisherBufferPtr tempPublisherBuffer = tempPtr->bufferPtr;

	while(tempPublisherBuffer != NULL){      ///dogru kitabı buldk
 
		if(tempPublisherBuffer->bookIndex != -1 && tempPublisherBuffer->bookIndex != -2){
			break;
		}

		tempPublisherBuffer = tempPublisherBuffer->nextPtr;
	}
	

	while(bufferPtr != NULL){               ///bufferda dogru yeri buldk
		if(strlen(bufferPtr->bookName) < 2){
			break;
		}
		bufferPtr = bufferPtr->nextPtr;
	}

	if(tempPublisherBuffer == NULL) printf("NULLLLLL\n");
	if(bufferPtr == NULL) printf("NULLLLLLLLLLLLLLLLLLLLLLLLL\n");


	printf("%s kitabını paketledik\n",tempPublisherBuffer->bookName);
	strcpy(bufferPtr->bookName , tempPublisherBuffer->bookName); //pack buffer içine kitabı koyduk

	packagerTempPtr->packSize -= 1;

	strcpy(tempPublisherBuffer->bookName , ""); //publisher listte bufferı silmis gibi yaptık
	tempPublisherBuffer->bookIndex = -2;


}

int checkAllThreads(){

	int control = 0 ;
	PublisherTypePtr tempPtr = publisherStartPtr;
	while(tempPtr != NULL){
		if(tempPtr->threadCount > 0)		control = 1 ;
		tempPtr = tempPtr->nextPtr;
	}
	if(control == 0)		return 1 ;			// eger sistemde thread kalmamışsa 1 dönecek
	else 					return 0 ;			// varsa 0

}

int checkPublisherThread(int type){

	PublisherTypePtr tempPtr = publisherStartPtr;
	while(tempPtr != NULL){
		if(tempPtr->pType == type){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	if(tempPtr->threadCount != 0)		return 1;
	else 								return 0 ;

}

int checkPackageSize(int index){

	PackagerListPtr tempPtr = packagerStartPtr;

	while(tempPtr != NULL){
		if(tempPtr->packIndex == index){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	if(tempPtr->packSize == 0){
		return 0;
	}
	else{
		return 1;
	}

}

void printAndResetPackBuffer(int index){

	PackagerListPtr tempPtr = packagerStartPtr;

	while(tempPtr != NULL){
		if(tempPtr->packIndex == index){
			break;
		}
		tempPtr = tempPtr->nextPtr;
	}

	PackagerBufferListPtr bufferPtr = tempPtr->bufferPtr ;

	printf("Packager %d\tFinished preparing one package. The package contains: ",index);
	while(bufferPtr != NULL){
		printf("%s   " , bufferPtr->bookName);
		strcpy(bufferPtr->bookName,""); //silmek icin
		bufferPtr = bufferPtr->nextPtr;
	}
	printf("\n");


	tempPtr->packSize = numPackagerBook; //


}



sem_t semaphore_queue_packager;

void *packager(void *Args){

	struct args *pArgs = (struct args *)Args;
	int index = pArgs->index;

	while(1){
	//	sem_wait(&semaphore_queue_packager);
		// sistemde thread kalmıs mı ona bakıyoruz , kalmamıssa 1 dönecek
		int randomType = rand() % numPublisherType + 1 ;
		//printf("random : %d\n",randomType);	
		if(getPublishedBookSize(randomType) > 0){		// eger bu typedan sistemde kitap varsa >0 dönüyor
			packageBook(randomType,index); // her seferinde tek kitap packagelayacak


			if(checkPackageSize(index) == 0){
				printAndResetPackBuffer(index);
			}


			printf("type : %d getpublished book : %d\n",randomType,getPublishedBookSize(randomType));
		}
		else if(checkPublisherThread(randomType)){			// eger random gelen typedan yoksa baska bir type secene kadar continu
			printf("Sistemde thread vaaarrrr\n");
			continue ;
		}
		else if(checkAllThreads()){						// sistemde hic thread var mı buna bakıoruz 
			printf("There are no publishers left in the system.");
			pthread_exit(NULL);	
		}

	//	sem_post(&semaphore_queue_packager);

	}

	
}


int main(int argc, char *argv[]){


	//Argument Error handling 
	if(getAndCheckArguments(argc,argv,&numPublisherType,&numPublisherCount,&numPackagerCount,&numPublishingBook,&numPackagerBook,&bufferSize) == -1){
		return -1;
	}



	//toplam publisher thread sayısı belli oldugu için fix size array olusturulabilir
	pthread_t publishers[numPublisherType * numPublisherCount];

	//packager sayısı bilindiğinden fix size array olusturulabilir
	pthread_t packagers[numPackagerCount];

	sem_init (&semaphore_queue_packager,0,1); 

	void * status;
	int rc;
	//Thread metodlarına en fazla 1 argument gönderebiliyoruz !!!

	int i = 0; int j = 0; int pIndex = -1; 


	for(i = 1; i <= numPublisherType; i++){

		//Adding into publisher type list 
		insertPublisherType(&publisherStartPtr , i , 0 , bufferSize,numPublisherCount); //type index bufsize

		initiliazeBuffer(&publisherStartPtr , i); // buffer nodes are initiliazed

		//semaphore array

		for(j = 1; j <= numPublisherCount; j++){

			pIndex++;

			args *pArgs = (args*)malloc(sizeof(args)); //.............//
			pArgs->type = i;
			pArgs->index = j;
			pArgs->bookCount = numPublishingBook;

	//		printf("MAIN : creating publisher thread type : %d , index : %d\n",i,j);
			rc = pthread_create(&(publishers[pIndex]),NULL,&publisher,(void *)pArgs);
			if(rc){
				printf("ERRROOOOOOOOOR\n");
			}

			
		}
	}

//	printPubTypeList(publisherStartPtr);
	
	for(i = 0; i < numPackagerCount; i++){
		//Adding into packager list
		insertPackagerList(&packagerStartPtr , i+1 , numPackagerBook , NULL); // index , size book name
		initiliazePackagerBuffer(&packagerStartPtr,i+1);
		args *pgArgs = (args*)malloc(sizeof(args)); //.............//
		pgArgs->type = -1;
		pgArgs->index = i+1;
//		printf("MAIN : creating packager thread index : %d\n",i);
		rc = pthread_create(&(packagers[i]),NULL,&packager,(void *)pgArgs);
		if(rc){
			printf("ERRROOOOOOOOOR\n");
		}
	}

//	printPackagerList(packagerStartPtr);

	//Waiting part -- bura sayesinde threadler görevini tamamen bitirdi mi bunu anlıyoruz

	pIndex = 0;
	for(i = 1; i <= numPublisherType; i++){
		for(j = 1; j <= numPublisherCount; j++){

			rc = pthread_join(publishers[pIndex], &status);
	      if (rc) {
	         printf("ERROR; return code from pthread_join() is %d %d %d\n", rc, i , j);
	         exit(-1);
	         }
	  //    printf("Main: completed join with thread %ld having a status of %ld\n",pIndex,(long)status);			
			pIndex++;
		}
	}

	for(i = 0; i < numPackagerCount;i++){
		rc = pthread_join(packagers[i], &status);
	      if (rc) {
	         printf("ERROR; return code from pthread_join() is %d %d\n", rc, i );
	         exit(-1);
	         }
	 //     printf("Main: completed join with thread %ld having a status of %ld\n",i,(long)status);
		
	}


	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);


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
