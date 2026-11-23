# Szyfrowanie RSA w C++

Szkolny program konsolowy dla 64-bitowego Windowsa. Wpisuje się zdanie
lub podaje nazwę pliku, a wynik zostaje zapisany do pliku.

## Uruchomienie

Rozpakuj folder i otwórz `rsa.exe`. Program ma gotową parę kluczy.
Do uruchomienia nie potrzeba kompilatora.

1. Wybierz **1**, aby szyfrować, albo **2**, aby odszyfrować.
2. Wybierz klawiaturę lub plik.
3. Wpisz dane albo nazwę pliku.
4. Wpisz nazwę wyniku lub zatwierdź domyślną Enterem.

Szyfrowanie domyślnie odczytuje `dane.txt` i zapisuje `wynik.txt`.
Odszyfrowanie odczytuje `wynik.txt` i zapisuje `odszyfrowane.txt`.

## Pliki

| Plik | Zawartość |
|---|---|
| `rsa.exe` | Gotowy program dla Windows x86-64 |
| `dane.txt` | Przykładowa wiadomość |
| `kod/rsa.cpp` | Kod źródłowy |
| `kod/Dokumentacja.txt` | Dokumentacja projektowa, techniczna i instrukcja |
| `Instrukcja.txt` | Krótka instrukcja obsługi |
| `Dla_prezentujacych.txt` | Wyjaśnienie RSA i kodu, scenariusz pokazu i pytania |

## Kompilacja

Z folderu projektu, przy zainstalowanym MinGW-w64:

```powershell
g++ -std=c++17 -O2 -static kod/rsa.cpp -o rsa.exe
```

## Charakter projektu

Wersja edukacyjna używa jawnej, stałej pary małych kluczy.
Służy do pokazania algorytmu i obsługi plików. Nie zapewnia ochrony
poufnych danych.
