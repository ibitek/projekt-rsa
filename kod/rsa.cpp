// Szkolny projekt RSA — program konsolowy w C++
//
// Jak czytać program: zaczynamy w main() w sekcji 7. Program wyświetla menu,
// wczytuje dane, szyfruje je lub odszyfrowuje i zapisuje wynik do pliku.

// 1. Biblioteki, limity danych i klucze =================================
// Gotowa para kluczy pozwala prześledzić te same obliczenia na lekcji.
// Oba klucze używają liczby 3233. Wykładnik 7 szyfruje, a 1783 odszyfrowuje.
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

// 2. Obsługa konsoli ==================================================
// Wczytujemy cały wiersz. Przy pytaniu z menu zamieniamy go na liczbę
// i sprawdzamy zakres, np. od 0 do 3. Wartość -1 oznacza błąd odczytu
// lub niepoprawny wybór.
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

// 3. Obliczenia RSA ===================================================
// Obie operacje liczą resztę z dzielenia potęgi przez n. Po każdym
// mnożeniu od razu bierzemy resztę, dzięki czemu liczby pozostają małe.
// Np. 65 do potęgi 7 liczymy z potęg 4, 2 i 1, bo 7 = 4 + 2 + 1.
int potegaModulo(int podstawa, int wykladnik, int n) {
    int wartosc = 1;
    while (wykladnik > 0) {
        if (wykladnik % 2 == 1) wartosc = wartosc * podstawa % n;
        podstawa = podstawa * podstawa % n;
        wykladnik /= 2;
    }
    return wartosc;
}

// 4. Wczytywanie wiadomości lub szyfrogramu ===========================
// Dane można wpisać ręcznie albo odczytać z pliku. Sam Enter wybiera
// dane.txt do szyfrowania lub wynik.txt do odszyfrowania.
// Plik czytamy bajt po bajcie, żeby zachować też spacje i końce linii.
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

// 5. Szyfrowanie i odszyfrowanie ======================================
// Szyfrujemy każdy bajt osobno. Dla litery A kod bajtu to 65.
// Z kluczem publicznym otrzymujemy 65 do potęgi 7 modulo 3233, czyli 1317.
// Zapisujemy kolejne wyniki jako liczby rozdzielone spacjami.
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

// Odszyfrowanie czyta liczby oddzielone odstępami i używa klucza prywatnego.
// 1317 do potęgi 1783 modulo 3233 daje 65, czyli znowu literę A.
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

// 6. Zapis wyniku ====================================================
// Można wpisać własną nazwę pliku albo zatwierdzić domyślną Enterem.
// Jeśli plik już istnieje, program pyta, czy można go nadpisać.
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

// 7. Start programu i menu ===========================================
// Na Windowsie ustawiamy UTF-8 i szukamy plików obok programu.
// Menu powtarza się, dopóki użytkownik nie wybierze 0.
// Opcje 1 i 2 wczytują dane, przetwarzają je i zapisują wynik.
// Opcja 3 pokazuje gotowe klucze używane w obliczeniach.
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
