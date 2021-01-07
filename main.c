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
