# EAN Project - metoda Newtona-Raphsona drugiego rzedu

Projekt jest graficzna aplikacja w C++ do numerycznego wyznaczania pierwiastkow rownan nieliniowych:

```text
f(x) = 0
```

Aplikacja korzysta z biblioteki Raylib do interfejsu uzytkownika, z MPFR/GMP oraz lokalnej implementacji arytmetyki przedzialowej. Funkcja rozwiazywana przez program jest ladowana z biblioteki dynamicznej `.so`, dlatego mozna zmieniac rownanie bez przebudowy glownej aplikacji.

Domyslna biblioteka funkcji:

```text
./build/funcs/func.so
```

## Tryby pracy

Program udostepnia trzy tryby wybierane przyciskami w oknie aplikacji:

| Tryb | Dane wejsciowe | Opis |
| --- | --- | --- |
| `Real numbers` | liczba rzeczywista, np. `1.5` | Metoda Newtona-Raphsona drugiego rzedu dla punktu startowego `x0`. |
| `Interval for real numbers` | liczba rzeczywista, np. `1.5` | Punkt `x0` jest zamieniany na punktowy przedzial i dalej liczony metoda przedzialowa. |
| `Interval for interval numbers` | przedzial, np. `[1, 2]` | Metoda przedzialowa uruchamiana bezposrednio dla zadanego przedzialu. |

Pola `Epsilon` i `Max iterations` pozwalaja zmienic tolerancje oraz limit iteracji. Domyslnie:

| Parametr | Wartosc |
| --- | ---: |
| `epsilon` | `1e-15` |
| `MAX_ITER` | `100` |

Okno aplikacji ma rozmiar `1300 x 700` i odswieza sie z docelowa szybkoscia `60 FPS`.

## Budowanie i uruchamianie

Wymagane biblioteki systemowe:

```text
raylib, mpfr, gmp, dl
```

Budowanie projektu:

```bash
make
```

Uruchomienie z domyslna funkcja:

```bash
./build/main
```

Uruchomienie z wybrana biblioteka funkcji:

```bash
./build/main ./build/funcs/func1.so
```

`Makefile` buduje aplikacje oraz przykladowe biblioteki:

| Plik zrodlowy | Biblioteka | Funkcja |
| --- | --- | --- |
| `funcs/func.cpp` | `build/funcs/func.so` | `f(x) = x^2 - 2` |
| `funcs/func1.cpp` | `build/funcs/func1.so` | `f(x) = x^3 - x - 2` |
| `funcs/func2.cpp` | `build/funcs/func2.so` | `f(x) = cos(x) - x` |
| `funcs/func3.cpp` | `build/funcs/func3.so` | `f(x) = sin(x)^2 + 0.5sin(x) - 0.5` |

Czyszczenie katalogu budowania:

```bash
make clean
```

## Metoda

Program implementuje metode Newtona-Raphsona drugiego rzedu. Dla punktu `x_n` uzywane jest rozwiniecie:

$$
f(x_n) + f'(x_n)(x - x_n) + 1/2 f''(x_n)(x - x_n)^2 = 0
$$

Po podstawieniu `h = x - x_n` otrzymujemy rownanie kwadratowe:

$$
1/2 f''(x_n)h^2 + f'(x_n)h + f(x_n) = 0
$$

Kolejne przyblizenie jest wyznaczane z kandydatow:

$$
x_{n+1} = x_n - (f'(x_n) +/- sqrt((f'(x_n))^2 - 2f(x_n)f''(x_n))) / f''(x_n)
$$

Dla liczb rzeczywistych program wybiera kandydata dajacego mniejszy krok wzgledem aktualnego punktu. Jezeli `f''(x)` jest bliskie zeru albo wyroznik jest ujemny, obliczenia sa przerywane z komunikatem bledu.

Dla przedzialow te same operacje sa wykonywane na obiektach `Interval<long double>`. W kazdej iteracji program:

1. zapamietuje poprzedni przedzial,
2. liczy srodek przedzialu,
3. oblicza `f(m)`, `f'(m)` oraz `f''(X)`,
4. buduje dwa kandydaty drugiego rzedu,
5. wybiera kandydata blizszego srodkowi,
6. sprawdza bezwzgledny i wzgledny warunek stopu.

Warunki stopu dla przedzialow sprawdzaja zmiane obu koncow przedzialu:

```text
|a_{k+1} - a_k| < epsilon
|b_{k+1} - b_k| < epsilon
```

oraz wersje wzgledna z przeskalowaniem przez `max(|x|, 1)`.

## Glowne funkcje

Deklaracje procedur obliczeniowych znajduja sie w `math/Newton.h`, a implementacje w `math/Newton.cpp`.

```cpp
void runNewtonRaphsonReal(
    long double x0,
    RealFn f,
    RealFn df,
    RealFn ddf,
    OutputBox& out,
    int MAX_ITER,
    long double epsilon
);
```

Uruchamia metode punktowa dla rzeczywistego punktu startowego `x0`.

```cpp
void runNewtonRaphsonInterval(
    Interval<long double> x0,
    IntervalFn f,
    IntervalFn df,
    IntervalFn ddf,
    OutputBox& out,
    int MAX_ITER,
    long double epsilon
);
```

Uruchamia metode przedzialowa dla przedzialu startowego `x0`.

```cpp
void runNewtonRaphsonFromPoint(
    long double x0,
    IntervalFn f,
    IntervalFn df,
    IntervalFn ddf,
    OutputBox& out,
    int MAX_ITER,
    long double epsilon
);
```

Tworzy punktowy przedzial z liczby `x0` i wykonuje obliczenia metoda przedzialowa.

Typy funkcji sa zdefiniowane w `common/Types.h`:

```cpp
using RealFn = std::function<long double(long double)>;
using IntervalFn = std::function<Interval<long double>(Interval<long double>)>;

typedef long double (*FnPtr)(long double);
typedef void (*IVFnPtr)(long double, long double, long double*, long double*);
```

## Format danych wejsciowych

W trybie punktowym nalezy wpisac liczbe rzeczywista:

```text
1.5
```

W trybie przedzialowym nalezy wpisac przedzial domkniety:

```text
[1, 2]
```

Spacje w zapisie przedzialu sa ignorowane. Bledny format wejscia powoduje wyczyszczenie pola wynikowego i wyswietlenie:

```text
Error: wrong input format
```

## Wyniki

Wyniki sa wypisywane w komponencie `OutputBox`.

Dla trybu `Real numbers` program pokazuje kolejne iteracje i znaleziony pierwiastek:

```text
Iter  1: x=...  step=...
Iter  2: x=...  step=...
--- Roots: ---
Root: ...
```

Dla trybow przedzialowych program pokazuje kolejne przedzialy, ich szerokosc i krok:

```text
Iter  1: [a, b] w=... step=...
Iter  2: [a, b] w=... step=...
--- Interval root: ---
[a, b] w=...
```

Po przekroczeniu limitu iteracji wypisywany jest aktualny przedzial:

```text
Max iterations reached.
Current interval: [a, b]
```

## Biblioteki funkcji

Biblioteka dynamiczna z funkcja musi eksportowac szesc symboli C:

```cpp
extern "C" {
    long double f(long double x);
    long double df(long double x);
    long double ddf(long double x);

    void f_iv(long double a, long double b, long double* ra, long double* rb);
    void df_iv(long double a, long double b, long double* ra, long double* rb);
    void ddf_iv(long double a, long double b, long double* ra, long double* rb);
}
```

Znaczenie symboli:

| Symbol | Znaczenie |
| --- | --- |
| `f` | funkcja rzeczywista |
| `df` | pierwsza pochodna funkcji rzeczywistej |
| `ddf` | druga pochodna funkcji rzeczywistej |
| `f_iv` | przedzialowa ocena funkcji na `[a, b]` |
| `df_iv` | przedzialowa ocena pierwszej pochodnej na `[a, b]` |
| `ddf_iv` | przedzialowa ocena drugiej pochodnej na `[a, b]` |

Funkcje przedzialowe przyjmuja konce przedzialu `a`, `b`, a wynik zwracaja przez wskazniki `ra`, `rb`.

Ladowanie biblioteki odbywa sie w `loadFunctions` z `math/IntervalUtils.cpp` przez `dlopen` i `dlsym`. Jezeli biblioteka podana w argumencie programu nie zostanie zaladowana, aplikacja probuje wrocic do `./build/funcs/func.so`.

## Przyklady

### Liczby rzeczywiste

Dla domyslnej funkcji:

$$
f(x) = x^2 - 2
$$

uruchom aplikacje:

```bash
make
./build/main ./build/funcs/func.so
```

Wybierz `Real numbers` i wpisz:

```text
1.5
```

Oczekiwany wynik to przyblizenie:

```text
Root: 1.414213562373095...
```

### Przedzial

Dla tej samej funkcji wybierz `Interval for interval numbers` i wpisz:

```text
[1, 2]
```

Program powinien zwezac przedzial do otoczenia dodatniego pierwiastka:

```text
--- Interval root: ---
[1.41421356237309..., 1.41421356237310...] w=...
```

Dokladne konce przedzialu moga zalezec od zaokraglen i kolejnych iteracji.
