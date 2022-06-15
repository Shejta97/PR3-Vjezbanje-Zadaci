#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

const char * crt = "\n----------------------------------------------------\n";
char _indeks[9] = "IB150000";

struct Datum {
	int * _dan, *_mjesec, *_godina;
	void Unos(int d, int m, int g) {
		_dan = new int(d);
		_mjesec = new int(m);
		_godina = new int(g);
	}
	void Ispis() { cout << *_dan << "/" << *_mjesec << "/" << *_godina << endl; }
	void Dealociraj() {
		delete _dan; delete _mjesec; delete _godina;
		_dan = _mjesec = _godina = nullptr;
	}

};

struct Izostanak {
	char * _razlogIzostanka;
	Datum _datumIzostanka;
	int _brojSati;
	bool _opravdano;//pravdanje zahtijeva da se evidentira i razlog izostanka

	void Unos(Datum datumIzostanka, int brojSati) {
		_datumIzostanka.Unos(*datumIzostanka._dan, *datumIzostanka._mjesec, *datumIzostanka._godina);
		_brojSati = brojSati;
		_opravdano = false;
		_razlogIzostanka = new char;
		_razlogIzostanka[0] = '\0';
	}

	void Dealociraj() { delete[] _razlogIzostanka; _razlogIzostanka = nullptr; }

	void Ispis() { _datumIzostanka.Ispis(); cout << _razlogIzostanka << " " << _razlogIzostanka; }

	void Opravdaj(const char * razlogIzostanka) {
		if (_razlogIzostanka != nullptr)
			delete[] _razlogIzostanka;
		int vel = strlen(razlogIzostanka) + 1;
		_razlogIzostanka = new char[vel];
		strcpy_s(_razlogIzostanka, vel, razlogIzostanka);
		_opravdano = true;
	}
};

struct Student {
	//format broja indeksa: IB150051
	//osigurati automatsko generisanje broja indeksa pocevsi od IB150001
	char _brojIndeksa[9];
	char * _imePrezime;
	Izostanak * _izostanci;
	int _brojIzostanaka;

	//Potpisi funkcija trebaju odgovarati onima u main-u
	//1. Unos
	void Unos(const char *brojIndeksa, const char *imePrezime)
	{
		strcpy_s(_brojIndeksa, 9, brojIndeksa);

		_imePrezime = new char[strlen(imePrezime) + 1];
		strcpy_s(_imePrezime, strlen(imePrezime) + 1, imePrezime);

		_izostanci = nullptr;
		_brojIzostanaka = 0;
	}

	//2. Dealociraj

	void Dealociraj() {
		delete[] _imePrezime; _imePrezime = nullptr;
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			_izostanci[i].Dealociraj();
		}
		delete[] _izostanci; _izostanci = nullptr;
	}

	/*3. DodajIzostanak - dodaje novi izostanak u listu izostanaka.	Onemoguciti ponavljanje izostanaka na nacin da se uvecava samo broj sati ukoliko korisnik (profesor) vise puta pokusa dodati izostanak na isti dan.*/

	void DodajIzostanak(Izostanak izostanak) 
	{
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			if (*_izostanci[i]._datumIzostanka._dan == *izostanak._datumIzostanka._dan && *_izostanci[i]._datumIzostanka._mjesec == *izostanak._datumIzostanka._mjesec && *_izostanci[i]._datumIzostanka._godina == *izostanak._datumIzostanka._godina) {
				_izostanci[i]._brojSati = _izostanci[i]._brojSati + izostanak._brojSati;
				return;
			}
		}

		Izostanak *temp = new Izostanak[_brojIzostanaka + 1];
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			temp[i].Unos(_izostanci[i]._datumIzostanka, _izostanci[i]._brojSati);
			temp[i]._opravdano = _izostanci[i]._opravdano;
			if (_izostanci[i]._opravdano)
				temp[i].Opravdaj(_izostanci[i]._razlogIzostanka);
			_izostanci[i].Dealociraj();
		}
		if (_izostanci != nullptr) {
			delete[] _izostanci;
		}

		temp[_brojIzostanaka].Unos(izostanak._datumIzostanka, izostanak._brojSati);
		if (izostanak._opravdano) {
			temp[_brojIzostanaka].Opravdaj(izostanak._razlogIzostanka);
		}
		_izostanci = temp;
		_brojIzostanaka++;
	}

	//4. Ispis - ispisuje izostanke u formatu prikazanom na slici (nije obavezno da ispis bude identican, ali je svakako pozeljno). Prilikom svakog ispisa, izostanke je potrebno spasiti u tekstualni fajl sa istim nazivom kao i broj indeksa.

	void Ispis() {
		cout << "Broj indeksa -> " << _brojIndeksa << endl;
		cout << "Broj izostanaka -> " << _brojIzostanaka << endl;
		cout << "Opravdani: " << endl;
		cout << crt;
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			if (_izostanci[i]._opravdano) {
				cout << "Datum ->";
				_izostanci[i]._datumIzostanka.Ispis();
				cout << "Broj sati -> " << _izostanci[i]._brojSati << endl;
				cout << "Razlog-> " << _izostanci[i]._razlogIzostanka << endl;
				cout << crt;
			}
		}
		cout << "Neopravdani: " << endl;
		cout << crt;
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			if (!_izostanci[i]._opravdano) {
				cout << "Datum -> ";
				_izostanci[i]._datumIzostanka.Ispis();
				cout << "Broj sati -> " << _izostanci[i]._brojSati << endl;
				cout << crt;
			}
		}
		char naziv_fajla[13];
		strcpy_s(naziv_fajla, 13, _brojIndeksa);
		strcat_s(naziv_fajla, 13, ".txt");
		ofstream upis_u_fajl(naziv_fajla);
		upis_u_fajl << "Broj indeksa-> " << _brojIndeksa << endl << "Broj izostanaka-> " << _brojIzostanaka << endl << "Opravdani: " << crt;
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			if (_izostanci[i]._opravdano) {
				upis_u_fajl << "Datum-> " << *_izostanci[i]._datumIzostanka._dan << "/" << *_izostanci[i]._datumIzostanka._mjesec << "/" << *_izostanci[i]._datumIzostanka._godina << endl << "Broj sati -> " << _izostanci[i]._brojSati << endl << "Razlog -> " << _izostanci[i]._razlogIzostanka << endl << crt;
			}
		}
		upis_u_fajl << "Neopravdani: " << endl;
		upis_u_fajl << crt;
		for (int i = 0; i < _brojIzostanaka; i++)
		{
			if (!_izostanci[i]._opravdano)
			{
				upis_u_fajl << "Datum -> " << *_izostanci[i]._datumIzostanka._dan << "/" << *_izostanci[i]._datumIzostanka._mjesec << "/" << *_izostanci[i]._datumIzostanka._godina << endl << "Broj sati-> " << _izostanci[i]._brojSati << endl << crt;
			}
		}
		upis_u_fajl.close();
	}
	

	//5. BinarnaPretraga - koristeci binarnu pretragu, na osnovu datuma, pronalazi i vraca pokazivac na izostanak. Ukoliko trazeni izostanak ne postoji funkcija vraca nullptr.

	bool PoredjenjeDatuma(Datum d1, Datum d2)
	{
		//funkcija vraca
		//0 -> ako su datumi jednaki
		//1 -> ako je d1 mladji od d2
		//-1 -> ako je d1 stariji od d2
		
		if (*d1._godina == *d2._godina && *d1._mjesec == *d2._mjesec && *d1._dan == *d2._dan)
			return 0;
		if (*d1._godina < *d2._godina)
			return 1;
		else if (*d1._godina > *d2._godina)
			return -1;
		else {
			if (*d1._mjesec < *d2._mjesec)
				return 1;
			else if (*d1._mjesec > *d2._mjesec)
				return -1;
			else {
				if (*d1._dan < *d2._dan)
					return 1;
				else
					return -1;
			}
		}
	}

	Izostanak *BinarnaPretraga(Datum d)
	{
		//sortiranje izostanaka
		for (int i = 0; i < _brojIzostanaka-1; i++)
		{
			for (int j = i+1; j < _brojIzostanaka; j++)
			{
				if (PoredjenjeDatuma(_izostanci[i]._datumIzostanka, _izostanci[j]._datumIzostanka) == 1) {
					Izostanak temp;
					temp.Unos(_izostanci[i]._datumIzostanka, _izostanci[i]._brojSati);
					if (_izostanci[i]._opravdano)
						temp.Opravdaj(_izostanci[i]._razlogIzostanka);
					_izostanci[i].Dealociraj();
					_izostanci[i].Unos(_izostanci[j]._datumIzostanka, _izostanci[j]._brojSati);
					if (_izostanci[j]._opravdano)
						_izostanci[i].Opravdaj(_izostanci[j]._razlogIzostanka);
					_izostanci[j].Dealociraj();
					_izostanci[j].Unos(temp._datumIzostanka, temp._brojSati);
					if (temp._opravdano)
						_izostanci[j].Opravdaj(temp._razlogIzostanka);
					temp.Dealociraj();
				}
			}
		}

		//binarna pretraga
		int prvi = 0, zadnji = _brojIzostanaka;
		while (prvi <= zadnji)
		{
			int srednji = (prvi + zadnji) / 2;
			if (PoredjenjeDatuma(d, _izostanci[srednji]._datumIzostanka) == 0)
				return _izostanci + srednji;
			if (PoredjenjeDatuma(d, _izostanci[srednji]._datumIzostanka) == 1)
				prvi = srednji + 1;
			if (PoredjenjeDatuma(d, _izostanci[srednji]._datumIzostanka) == -1)
				zadnji = srednji - 1;
		}
		return nullptr;
	}
};

//1. PronadjiNajveciNeopravdaniIzostanak - rekurzivna funkcija koja vraca pokazivac na neopravdani izostanak koji ima najveci broj sati

Izostanak *PronadjiNajveciNeopravdaniIzostanak(Student *s, int brojac = 0, Izostanak * i = nullptr)
{
	if (brojac == s->_brojIzostanaka)
		return i;
	if (i == nullptr)
		return PronadjiNajveciNeopravdaniIzostanak(s, brojac + 1, &s->_izostanci[brojac]);
	if (s->_izostanci[brojac]._brojSati > i->_brojSati && s->_izostanci[brojac]._opravdano == false)
		return PronadjiNajveciNeopravdaniIzostanak(s, brojac + 1, &s->_izostanci[brojac]);
	return PronadjiNajveciNeopravdaniIzostanak(s, brojac + 1, i);
}
//2. GenerisiSljedeciBrojIndeksa - generise i vraca naredni broj indeksa

char *GenerisiSljedeciBrojIndeksa()
{
	int i = 7; // ib160000
	while (i > 1)
	{
		char broj_za_uvecat = _indeks[i];  // 16000[0]
		int broj = atoi(&broj_za_uvecat);  //      &0 
		broj = broj + 1; // 0 +	1 = 1
		if (broj < 10) { 
			char pretvori_u_karakter[2]; 
			_itoa_s(broj, pretvori_u_karakter,2 , 10);
			_indeks[i] = pretvori_u_karakter[0];
			break;
		}
		_indeks[i] = '0';
		i--;
	}
	return _indeks;
}

void main() {

	Datum jucer, prije5Dana, prije10Dana;
	jucer.Unos(12, 6, 2016);
	prije5Dana.Unos(8, 6, 2016);
	prije10Dana.Unos(3, 6, 2016);


	Student denis;
	denis.Unos(GenerisiSljedeciBrojIndeksa(), "Denis Music");

	Izostanak izostanakJucer, izostanakPrije5Dana, izostanakPrije10Dana;

	izostanakJucer.Unos(jucer, 5);
	denis.DodajIzostanak(izostanakJucer);

	izostanakPrije5Dana.Unos(prije5Dana, 3);
	izostanakPrije5Dana.Opravdaj("Odsutan zbog bolesti - gripa");
	denis.DodajIzostanak(izostanakPrije5Dana);
	izostanakPrije5Dana.Unos(prije5Dana, 2);
	denis.DodajIzostanak(izostanakPrije5Dana);

	izostanakPrije10Dana.Unos(prije10Dana, 1);
	denis.DodajIzostanak(izostanakPrije10Dana);
	izostanakPrije10Dana.Unos(prije10Dana, 1);
	denis.DodajIzostanak(izostanakPrije10Dana);

	denis.Ispis();
	Izostanak * p = denis.BinarnaPretraga(jucer);
	cout << "Binarnom pretragom pronadjen izostanak -> ";
	p->Ispis();
	denis.Ispis();
	Izostanak * p2 = PronadjiNajveciNeopravdaniIzostanak(&denis);
	cout << "Rekurzivno pronadjen najveci neopravdani izostanak -> ";
	p2->Ispis();
	cout << endl;
	//dealocirati zauzetu memoriju
	jucer.Dealociraj();
	prije5Dana.Dealociraj();
	prije10Dana.Dealociraj();
	izostanakJucer.Dealociraj();
	izostanakPrije5Dana.Dealociraj();
	izostanakPrije10Dana.Dealociraj();
	denis.Dealociraj();


	system("pause>0");
}