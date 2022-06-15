#include<iostream>
#include<memory>
using namespace std;
//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

enum VrstaObaveze { Seminarski, Parcijalni1, Parcijalni2, Integralni, Prakticni };
const char *VrstaObavezeChar[] = { "Seminarski", "Parcijalni1", "Parcijalni2", "Integralni", "Prakticni" };

struct Datum {
	int * _dan, *_mjesec, *_godina;
	void Unos(int d, int m, int g) {
		_dan = new int(d);
		_mjesec = new int(m);
		_godina = new int(g);
	}
	void Ispis() {
		cout << *_dan << "/" << *_mjesec << "/" << *_godina << endl;
	}
	void Dealociraj() { delete _dan; delete _mjesec; delete _godina; }

	int getDani() { return *_dan + *_mjesec * 30 + *_godina * 365; }
};

struct ObavezeNaPredmetu {
	shared_ptr<VrstaObaveze> _vrstaObaveze;
	Datum _datumIzvrsenja;
	char * _napomena;
	int _ocjena; // 5 - 10 
	void Unos(VrstaObaveze vrsta, Datum datum, int ocjena,const char * napomena) {
		_vrstaObaveze = make_shared<VrstaObaveze>(vrsta);
		_datumIzvrsenja.Unos(*datum._dan, *datum._mjesec, *datum._godina);
		_ocjena = ocjena;
		_napomena = new char[strlen(napomena) + 1];
		strcpy_s(_napomena, strlen(napomena) + 1, napomena);
	}
	void Ispis() {
		cout <<VrstaObavezeChar[*_vrstaObaveze] << " " << _ocjena << " " << _napomena;
		_datumIzvrsenja.Ispis();
		cout << endl;
	}
	void Dealociraj() {
		_datumIzvrsenja.Dealociraj();
		delete[] _napomena; _napomena = nullptr;
	}
};

const char * _kljucneRijeci[15] = { NULL }; /*da bi se odredjena obaveza na predmetu smatrala uspjesno izvrsenom, u napomeni (objekta ObavezeNaPredmetu) ne smije stajati niti jedna od rijeci koja se nalazi u nizu _kljucneRijeci*/

struct PolozeniPredmet {
	Datum _datumPolaganja;//datum koji ce se evidentirati kao datum kada je predmet polozen tj. kada je formirana konacna ocjena
	char * _nazivPredmeta;
	ObavezeNaPredmetu * _listaIzvrsenihObaveza;
	int _trenutnoIzvrsenihObaveza;
	int _minimalanBrojDana; //odnosi se na minimalan broj dana koji mora proci od izvrsenja pojedinih obaveza na predmetu (npr. 7 dana)
	int _konacnaOcjena; //formira se na osnovu ocjena izvrsenih obaveza

	void Unos(const char * naziv, int minimalanBrojDana) {
		_nazivPredmeta = new char[strlen(naziv) + 1];
		strcpy_s(_nazivPredmeta, strlen(naziv) + 1, naziv);
		_listaIzvrsenihObaveza = nullptr;
		_trenutnoIzvrsenihObaveza = 0;
		_minimalanBrojDana = minimalanBrojDana;
		_konacnaOcjena = 0;
	}

	/*Na osnovu vrijednosti primljenih parametara osigurati dodavanje novoizvrsene obaveze na predmetu. Potrebno je onemoguciti dodavanje identicnih obaveza, a izmedju izvrsenja pojedinih obaveza mora proci najmanje onoliko dana koliko je definisano vrijednoscu atributa _minimalanBrojDana. Identicna vrsta obaveze se moze dodati samo u slucaju da je prethodno dodana obaveza (identicne vrste) imala ocjenu 5 ili je u napomeni sadrzavala neku od zabranjenih rijeci (definisanih nizom _kljucneRijeci).*/
	bool DodajIzvrsenuObavezu(VrstaObaveze vrsta,Datum datum, int ocjena, const char *napomena)
	{
		bool postoji_kljucna_rijec = false;
		//provjera da li postoji vec ta obaveza i da li je ocjena veca od 5
		for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
		{
			//provjera da li je izmedju posljednje izvresene obaveze i trenutno koje dodajemo proslo min broj dana
			if (abs(datum.getDani() - _listaIzvrsenihObaveza[i]._datumIzvrsenja.getDani()) < _minimalanBrojDana)
				return false;
			// nadjena je ista obaveza
			if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == vrsta) {
				//provjera da li je ocjena > 5
				if (_listaIzvrsenihObaveza[i]._ocjena > 5)
					return false; //ako jeste izbacujemo iz funkcije jer je dodavanje onemoguceno
				//provjera da li postoji kljucna rijec u napomeni
				for (int j = 0; j < 15; j++)
				{
					if (_kljucneRijeci[j] != nullptr && strstr(_listaIzvrsenihObaveza[i]._napomena, _kljucneRijeci[j]) != nullptr)
						postoji_kljucna_rijec = true;
				}
				//u slucaju da ne postoji izbacujemo iz funkcije jer je dodavanje onemoguceno
				if (!postoji_kljucna_rijec)
					return false;
				//resetujemo "postoji_kljucna_rijec" na "false" da bi mogli iducu "_listaIzvrsenihObaveza" provjeravati
				postoji_kljucna_rijec = false;
			}
		}
		//tek ako parametri zadovolje sve uslove mozemo napraviti unos obaveze

		ObavezeNaPredmetu *temp = new ObavezeNaPredmetu[_trenutnoIzvrsenihObaveza + 1];
		for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
		{
			temp[i].Unos(*_listaIzvrsenihObaveza[i]._vrstaObaveze, _listaIzvrsenihObaveza[i]._datumIzvrsenja, _listaIzvrsenihObaveza[i]._ocjena, _listaIzvrsenihObaveza[i]._napomena);
			_listaIzvrsenihObaveza[i].Dealociraj();
		}
		if (_listaIzvrsenihObaveza != nullptr)
			delete[] _listaIzvrsenihObaveza;
		temp[_trenutnoIzvrsenihObaveza++].Unos(vrsta, datum, ocjena, napomena);
		_listaIzvrsenihObaveza = temp;
		return true;
	}

	/*Konacna ocjene predstavlja prosjecnu ocjenu na predmetu (prosjecnu ocjenu izvrsenih obaveza koje ispunjavaju uslove za uspjesno polaganje predmeta),a za njeno formiranje student mora posjedovati polozen integralni ili dva parijcalna ispita. Ukoliko je ispit polozen putem parcijalnih ispita, student takodjer mora imati pozitivno (ocjenom vecom od 5) ocijenjena najmanje dva seminarska i jedan prakticni rad. Takodjer, datum polaganja drugog parcijalnog ispita ne smije biti raniji (manji) od datuma polaganja prvog parcijalnog ispita. Konacna ocjena ne smije biti formirana u slucaju da napomena bilo koje odbaveze (koja je ucestvovala u formiranju konacne ocjene) sadrzi neku od zabranjenih rijeci (niz _kljucneRijeci). U slucaju da neki od navedenih uslova nije zadovoljen konacna ocjena treba biti postavljena na vrijednost 5. Ukoliko su ispunjeni svi uslovi, osim formiranja konacne ocjene, datum polaganja je potrebno postaviti na vrijednost datuma posljednje izvrsene obaveze sa najvecom ocjenom.
	*/
	int FormirajKonacnuOcjenu()
	{
		bool integralni, parcijalni1, parcijalni2, prakticni;
		integralni = parcijalni1 = parcijalni2 = prakticni = false;
		int seminarski = 0;
		int suma_ocjena = 0;
		Datum datum_ocjene;
		datum_ocjene.Unos(0, 0, 0);
		int najveca_ocjena = 0;

		for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
		{
			bool postoji_kljucna_rijec = false;
			// provjera da li obaveza sadrzi neku kljucnu rijec
			for (int j = 0; j < 15; j++)
			{
				if (_kljucneRijeci[j] != nullptr && strstr(_listaIzvrsenihObaveza[i]._napomena, _kljucneRijeci[j]) != nullptr) {
					postoji_kljucna_rijec = true;
				}
			}
			if (!postoji_kljucna_rijec) {
				if(*_listaIzvrsenihObaveza[i]._vrstaObaveze == Integralni && _listaIzvrsenihObaveza[i]._ocjena>5)
				{
					integralni = true;
					suma_ocjena = _listaIzvrsenihObaveza[i]._ocjena;
					break; // prekid petlje radi formiranja ocjene na osnovu integralnog ispita
				}
				if(*_listaIzvrsenihObaveza[i]._vrstaObaveze==Parcijalni1 && _listaIzvrsenihObaveza[i]._ocjena>5)
				{
					parcijalni1 = true;
					suma_ocjena += _listaIzvrsenihObaveza[i]._ocjena;
				}
				if(*_listaIzvrsenihObaveza[i]._vrstaObaveze==Parcijalni2 && _listaIzvrsenihObaveza[i]._ocjena>5)
				{
					parcijalni2 = true;
					suma_ocjena += _listaIzvrsenihObaveza[i]._ocjena;
				}
				if(*_listaIzvrsenihObaveza[i]._vrstaObaveze==Prakticni && _listaIzvrsenihObaveza[i]._ocjena>5)
				{
					prakticni = true;
					suma_ocjena += _listaIzvrsenihObaveza[i]._ocjena;
				}
				if(*_listaIzvrsenihObaveza[i]._vrstaObaveze==Seminarski && _listaIzvrsenihObaveza[i]._ocjena>5)
				{
					seminarski++;
					suma_ocjena += _listaIzvrsenihObaveza[i]._ocjena;
				}
				if (_listaIzvrsenihObaveza[i]._ocjena > najveca_ocjena && _listaIzvrsenihObaveza[i]._datumIzvrsenja.getDani() > datum_ocjene.getDani())
				{
					datum_ocjene.Unos(*_listaIzvrsenihObaveza[i]._datumIzvrsenja._dan, *_listaIzvrsenihObaveza[i]._datumIzvrsenja._mjesec, *_listaIzvrsenihObaveza[i]._datumIzvrsenja._godina);
				}
			}
		}
		// ako je polozeni predmet integralni formiramo konacnu ocjenu
		if (integralni) {
			_konacnaOcjena = suma_ocjena;
		}
		else if (parcijalni1 && parcijalni2 && prakticni && seminarski >= 2) {
			_konacnaOcjena = (float)(suma_ocjena) / (3 + seminarski);
		}
		else {
			_konacnaOcjena = 5;
		}
		// postavljanje datuma konacne ocjene
		_datumPolaganja.Unos(*datum_ocjene._dan, *datum_ocjene._mjesec, *datum_ocjene._godina);
		datum_ocjene.Dealociraj(); // mozemo obrisati pomocni datum koji nam je sluzio da pronadjemo odg datum na osnovu kog se formira konacna ocjena
		return _konacnaOcjena;

	}

	void Dealociraj() {
		_datumPolaganja.Dealociraj();
		delete[] _nazivPredmeta; _nazivPredmeta = nullptr;
		for (size_t i = 0; i < _trenutnoIzvrsenihObaveza; i++)
			_listaIzvrsenihObaveza[i].Dealociraj();
		delete[]_listaIzvrsenihObaveza;
		_listaIzvrsenihObaveza = nullptr;
	}
};


/*Rekurzivna funkcija treba vratiti prosjecnu ocjenu uspjesno izvrsenih obaveze na predmetu odredjene vrste u definisanom periodu. Izbjegavati koristenje globalnih varijabli*/
float PretragaRekurzivno(PolozeniPredmet p, VrstaObaveze vrsta,Datum OD,Datum DO, int brojacObaveza, int prosjek, int brojacObavezaKojiUlazeUProsjek=0)
{
	if (brojacObaveza >= p._trenutnoIzvrsenihObaveza)
		return (float)prosjek / brojacObavezaKojiUlazeUProsjek;
	// pronadjena je ta obaveza
	if (*p._listaIzvrsenihObaveza[brojacObaveza]._vrstaObaveze == vrsta) {
		// provjeravamo da li je unutar zadanog okvira datuma
		if (OD.getDani() <= p._listaIzvrsenihObaveza[brojacObaveza]._datumIzvrsenja.getDani() && DO.getDani() >= p._listaIzvrsenihObaveza[brojacObaveza]._datumIzvrsenja.getDani()) {
			// pozivamo ponovo funckiju povecamo brojac obaveza i proslijedimo prosjek uvecan za ocjenu ove obaveze i uvecamo brojac obaveza koji ulaze u prosjek tj da znamo koliko ima obaveza koje trazimo
			//return PretragaRekurzivno(p, vrsta, OD, DO, brojacObaveza + 1, prosjek + p._listaIzvrsenihObaveza[brojacObaveza]._ocjena, brojacObavezaKojiUlazeUProsjek + 1);

			brojacObaveza++;
			prosjek += p._listaIzvrsenihObaveza[brojacObaveza]._ocjena;
			brojacObavezaKojiUlazeUProsjek++;
		}
	}
	// u slucaju da nije ta obaveza samo povecamo brojac obaveza za provjeru iduce obaveze
	return PretragaRekurzivno(p, vrsta, OD, DO, brojacObaveza + 1, prosjek, brojacObavezaKojiUlazeUProsjek);
}

int GetBrojObavezaIznadProsjeka(PolozeniPredmet p, float prosjecnaOcjena)
{
	auto broj_obaveza_iznad_prosjeka = [p, prosjecnaOcjena]() {
		int brojac = 0;
		for (int i = 0; i < p._trenutnoIzvrsenihObaveza; i++)
		{
			if (p._listaIzvrsenihObaveza[i]._ocjena >= prosjecnaOcjena) {
				brojac++;
			}
		}
		return brojac;
	};
	return broj_obaveza_iznad_prosjeka();
}

void main() {
	_kljucneRijeci[0] = "prepisivao";
	_kljucneRijeci[1] = "ometao";
	_kljucneRijeci[2] = "nije dostavio";

	Datum datumSeminarski1, datumSeminarski2, datumParcijalni1, datumParcijalni2, datumIntegralni;
	datumSeminarski1.Unos(10, 6, 2017);
	datumSeminarski2.Unos(18, 6, 2017);
	datumParcijalni1.Unos(22, 6, 2017);
	datumParcijalni2.Unos(30, 6, 2017);
	datumIntegralni.Unos(24, 6, 2017);

	PolozeniPredmet prII;
	//naziv predmeta, minimalan broj dana
	prII.Unos("Programiranje II", 7);

	if (prII.DodajIzvrsenuObavezu(Parcijalni1, datumParcijalni1, 6, "uslovno polozen ispit"))
		cout << "Parcijalni 1...dodan" << endl;
	if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 5, "previse gresaka, a tokom ispita ometao druge studente"))
		cout << "Parcijalni 2...dodan" << endl;
	if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski1, 8, "implementirani svi algoritmi"))
		cout << "Seminarski1...dodan" << endl;
	if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski2, 6, "rad slican kao kod studenta IB150388"))
		cout << "Seminarski2...dodan" << endl;

	cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;

	datumParcijalni2.Unos(22, 7, 2017);
	if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 6, ""))
		cout << "Parcijalni 2...dodan" << endl;
	cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;
	Datum OD, DO; OD.Unos(1, 1, 2017); DO.Unos(1, 7, 2017);

	/*Rekurzivna funkcija treba vratiti prosjecnu ocjenu uspjesno izvrsenih obaveze na predmetu odredjene vrste u definisanom periodu. Izbjegavati koristenje globalnih varijabli*/
	float prosjek = PretragaRekurzivno(prII, Seminarski, OD, DO, 0, 0);
	cout << "Prosjecna ocjena izvrsenih obaveza je: " << prosjek << endl;

	/*Koristeci lambda-u, unutar funkcije GetBrojObavezaIznadProsjeka pronaci broj izvrsenih obaveza koje imaju istu ili vecu ocjenu od prosjecne*/
	int iznadProsjeka = GetBrojObavezaIznadProsjeka(prII, prosjek);
	cout << "Broj izvrsenih obaveza iznad prosjeka je: " << iznadProsjeka << endl;

	datumParcijalni1.Dealociraj();
	datumParcijalni2.Dealociraj();
	datumSeminarski1.Dealociraj();
	datumSeminarski2.Dealociraj();
	datumIntegralni.Dealociraj();
	prII.Dealociraj();

	system("pause>0");
}