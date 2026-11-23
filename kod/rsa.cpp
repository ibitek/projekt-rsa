// Edukacyjny RSA. Standardowe naglowki C++ oraz systemowa obsluga konsoli Windows.
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

constexpr int LIMIT_TEKSTU = 10000;
constexpr int LIMIT_SZYFRU = LIMIT_TEKSTU * 5; // cztery cyfry i spacja na bajt
// Stala para demonstracyjna: p=61, q=53, phi=3120; 7*1783 mod 3120 = 1.
constexpr int MODUL = 3233;
constexpr int PUBLICZNY = 7;
constexpr int PRYWATNY = 1783;
char dane[LIMIT_SZYFRU + 1], wynik[LIMIT_SZYFRU + 1];

bool koniecWejscia = false;

void wypisz(const char* tekst) {
    while (*tekst) putchar(*tekst++);
    fflush(nullptr);
}

// Czyta caly wiersz i usuwa koniec linii. Zwraca -1, gdy przekroczono limit.
int wiersz(char* tekst, int limit) {
    int ile = 0, znak;
    while ((znak = getchar()) != EOF && znak != '\n') {
        if (ile < limit) tekst[ile] = static_cast<char>(znak);
        if (ile <= limit) ++ile;
    }
    koniecWejscia = znak == EOF && ile == 0;
    if (ile > limit) return -1;
    tekst[ile] = '\0';
    return ile;
}

int liczba(const char* pytanie, int minimum, int maksimum) {
    wypisz(pytanie);
    char tekst[32], *koniec;
    if (wiersz(tekst, 31) <= 0) return -1;
    long wartosc = strtol(tekst, &koniec, 10);
    if (*koniec || wartosc < minimum || wartosc > maksimum) return -1;
    return static_cast<int>(wartosc);
}

int potegaModulo(int podstawa, int wykladnik, int n) {
    int wartosc = 1;
    while (wykladnik > 0) {
        if (wykladnik % 2 == 1) wartosc = wartosc * podstawa % n;
        podstawa = podstawa * podstawa % n;
        wykladnik /= 2;
    }
    return wartosc;
}

int pobierzDane(int limit, bool szyfrowanie) {
    int zrodlo = liczba("Dane: 1 - klawiatura, 2 - plik: ", 1, 2);
    if (zrodlo == 1) {
        wypisz("Wpisz dane w jednym wierszu: ");
        return wiersz(dane, limit);
    }
    if (zrodlo != 2) return -1;
    char nazwa[256];
    wypisz(szyfrowanie ? "Nazwa pliku [Enter = dane.txt]: " : "Nazwa pliku [Enter = wynik.txt]: ");
    int rozmiar = wiersz(nazwa, 255);
    if (rozmiar < 0) return -1;
    const char* wejscie = rozmiar ? nazwa : (szyfrowanie ? "dane.txt" : "wynik.txt");
    FILE* plik = fopen(wejscie, "rb");
    if (!plik) {
        puts("Nie mozna otworzyc wskazanego pliku.");
        return -1;
    }
    int ile = 0, znak;
    while (ile <= limit && (znak = fgetc(plik)) != EOF)
        dane[ile++] = static_cast<char>(znak);
    bool blad = ferror(plik) || ile > limit;
    fclose(plik);
    return blad ? -1 : ile;
}

int szyfruj(int ile, int e, int n) {
    int dlugosc = 0;
    for (int i = 0; i < ile; ++i) {
        int bajt = static_cast<unsigned char>(dane[i]);
        int blok = potegaModulo(bajt, e, n);
        dlugosc += sprintf(wynik + dlugosc, "%d ", blok);
    }
    return dlugosc;
}

bool odstep(char znak) {
    return znak == ' ' || (znak >= '\t' && znak <= '\r');
}

int odszyfruj(int ile, int d, int n) {
    int i = 0, dlugosc = 0;
    while (i < ile) {
        if (odstep(dane[i])) {
            ++i;
            continue;
        }
        int blok = 0;
        while (i < ile && !odstep(dane[i])) {
            if (dane[i] < '0' || dane[i] > '9') return -1;
            blok = blok * 10 + (dane[i++] - '0');
            if (blok >= n) return -1;
        }
        int bajt = potegaModulo(blok, d, n);
        if (bajt > 255) return -1;
        wynik[dlugosc++] = static_cast<char>(bajt);
    }
    return dlugosc;
}

void zapisz(int dlugosc, bool szyfrowanie) {
    char nazwa[256];
    wypisz(szyfrowanie ? "Plik wynikowy [Enter = wynik.txt]: " : "Plik wynikowy [Enter = odszyfrowane.txt]: ");
    int rozmiar = wiersz(nazwa, 255);
    if (rozmiar < 0) { puts("Zbyt dluga nazwa."); return; }
    const char* wyjscie = rozmiar ? nazwa : (szyfrowanie ? "wynik.txt" : "odszyfrowane.txt");
    FILE* plik = fopen(wyjscie, "rb");
    if (plik) {
        fclose(plik);
        if (liczba("Plik istnieje. Nadpisac? 1 - tak, 0 - nie: ", 0, 1) != 1) {
            puts("Anulowano zapis.");
            return;
        }
    }
    plik = fopen(wyjscie, "wb");
    if (!plik) { puts("Nie mozna utworzyc pliku."); return; }
    bool zapisano = fwrite(wynik, 1, dlugosc, plik) == static_cast<unsigned>(dlugosc);
    if (fclose(plik) != 0 || !zapisano) puts("Blad zapisu pliku.");
    else { wypisz("Zapisano: "); puts(wyjscie); }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    // Pliki wzgledne znajduja sie obok EXE, takze przy starcie ze skrotu.
    static wchar_t sciezka[32768];
    unsigned dlugoscSciezki = GetModuleFileNameW(nullptr, sciezka, 32768);
    if (dlugoscSciezki > 0 && dlugoscSciezki < 32768) {
        for (int i = static_cast<int>(dlugoscSciezki) - 1; i >= 0; --i) {
            if (sciezka[i] == L'\\') {
                sciezka[i] = 0;
                SetCurrentDirectoryW(sciezka);
                break;
            }
        }
    }
#endif
    puts("RSA - szyfrowanie wiadomosci");
    puts("Program korzysta z gotowej, szkolnej pary kluczy RSA.");
    while (true) {
        int opcja = liczba("\n1 - szyfruj\n2 - odszyfruj\n3 - pokaz klucze\n0 - koniec\nWybor: ", 0, 3);
        if (opcja == 0 || koniecWejscia) break;
        if (opcja < 0) { puts("Wybierz opcje 0-3."); continue; }
        if (opcja == 3) {
            printf("Publiczny (n, e): %d %d\n", MODUL, PUBLICZNY);
            printf("Prywatny (n, d): %d %d\n", MODUL, PRYWATNY);
            continue;
        }
        bool szyfrowanie = opcja == 1;
        int ile = pobierzDane(szyfrowanie ? LIMIT_TEKSTU : LIMIT_SZYFRU, szyfrowanie);
        if (ile <= 0) { puts("Sprawdz zrodlo, zawartosc i rozmiar danych."); continue; }
        int dlugosc = szyfrowanie ? szyfruj(ile, PUBLICZNY, MODUL) : odszyfruj(ile, PRYWATNY, MODUL);
        if (dlugosc <= 0) { puts("Bledny szyfrogram."); continue; }
        zapisz(dlugosc, szyfrowanie);
    }
    return 0;
}
