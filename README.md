# EAN Project - metoda Newtona-Raphsona drugiego rzędu

Projekt jest graficzną aplikacją w C++ do numerycznego wyznaczania pierwiastków równań nieliniowych o postaci: `f(x) = 0`

Aplikacja korzysta z biblioteki Raylib do interfejsu użytkownika, z MPFR/GMP oraz lokalnej implementacji arytmetyki przedziałowej. Funkcja rozwiązywana przez program jest ładowana z biblioteki dynamicznej `.so`, dlatego można zmieniać równanie bez przebudowy głównej aplikacji.

Domyślna biblioteka funkcji:

```text
./build/funcs/func.so
```

## Tryby pracy

Program udostępnia trzy tryby wybierane przyciskami w oknie aplikacji:

| Tryb | Dane wejściowe | Opis |
| --- | --- | --- |
| `Real numbers` | liczba rzeczywista | Metoda Newtona-Raphsona drugiego rzędu dla punktu startowego `x0`. |
| `Interval for real numbers` | liczba rzeczywista | Punkt `x0` jest zamieniany na punktowy przedział i dalej liczony metodą przedziałową. |
| `Interval for interval numbers` | przedział | Metoda przedziałowa uruchamiana bezpośrednio dla zadanego przedziału. |

Pola `Epsilon` i `Max iterations` pozwalają zmienić tolerancję oraz limit iteracji. Domyślnie:

| Parametr | Wartość |
| --- | ---: |
| `epsilon` | `1e-15` |
| `MAX_ITER` | `100` |

Okno aplikacji ma rozmiar `1400 x 700` i odświeża się z docelową szybkością `60 FPS`.

## Budowanie i uruchamianie

Wymagane biblioteki systemowe:

```text
raylib, mpfr, gmp, dl
```

Budowanie projektu:

```bash
make
```

Uruchomienie z domyślną funkcją:

```bash
./build/main
```

Uruchomienie z wybraną biblioteką funkcji:

```bash
./build/main ./build/funcs/func1.so
```

`Makefile` buduje aplikację oraz przykładowe biblioteki:

| Plik źródłowy | Biblioteka | Funkcja |
| --- | --- | --- |
| `funcs/func.cpp` | `build/funcs/func.so` | `f(x) = x^2 - 2` |
| `funcs/func1.cpp` | `build/funcs/func1.so` | `f(x) = x^3 - x - 2` |
| `funcs/func2.cpp` | `build/funcs/func2.so` | `f(x) = cos(x) - x` |
| `funcs/func3.cpp` | `build/funcs/func3.so` | `f(x) = sin(x)^2 + 0.5sin(x) - 0.5` |

## Metoda

Program implementuje metodę Newtona-Raphsona drugiego rzędu. Dla punktu `x_n` używane jest rozwinięcie:


$$
f(x_n) + f'(x_n)(x - x_n) + \frac{1}{2}f''(x_n)(x - x_n)^2 = 0
$$

Po podstawieniu `h = x - x_n` otrzymujemy równanie kwadratowe:


$$
\frac{1}{2} f''(x_n)h^2 + f'(x_n)h + f(x_n) = 0
$$

Kolejne przybliżenie jest wyznaczane z kandydatów:

$$
x_{n+1}
= x_n -
\frac{
f'(x_n) \pm \sqrt{(f'(x_n))^2 - 2f(x_n)f''(x_n)}
}{
f''(x_n)
}
$$

Dla liczb rzeczywistych program wybiera kandydata dającego mniejszy krok względem aktualnego punktu. Jeżeli `f''(x)` jest bliska zeru albo wyróżnik jest ujemny, obliczenia są przerywane z komunikatem błędu.

Dla przedziałów te same operacje są wykonywane na obiektach `Interval<long double>`. W każdej iteracji program:

1. zapamiętuje poprzedni przedział,
2. liczy środek przedziału,
3. oblicza `f(m)`, `f'(m)` oraz `f''(X)`,
4. buduje dwa kandydaty drugiego rzędu,
5. wybiera kandydata bliższego środkowi,
6. sprawdza bezwzględny i względny warunek stopu.

Warunki stopu dla przedziałów sprawdzają zmianę obu końców przedziału:

```text
|a_{k+1} - a_k| < epsilon
|b_{k+1} - b_k| < epsilon
```

oraz wersję względną z przeskalowaniem przez `max(|x|, 1)`.

## Główne funkcje

Deklaracje procedur obliczeniowych znajdują się w `math/Newton.h`, a implementacje w `math/Newton.cpp`.

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

Uruchamia metodę punktową dla rzeczywistego punktu startowego `x0`.

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

Uruchamia metodę przedziałową dla przedziału startowego `x0`.

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

Tworzy punktowy przedział z liczby `x0` i wykonuje obliczenia metodą przedziałową.

Typy funkcji są zdefiniowane w `common/Types.h`:

```cpp
using RealFn = std::function<long double(long double)>;
using IntervalFn = std::function<Interval<long double>(Interval<long double>)>;

typedef long double (*FnPtr)(long double);
typedef void (*IVFnPtr)(long double, long double, long double*, long double*);
```

## Format danych wejściowych

W trybie punktowym należy wpisać liczbę rzeczywistą:

```text
1.5
```

W trybie przedziałowym należy wpisać przedział domknięty:

```text
[1, 2]
```

Spacje w zapisie przedziału są ignorowane. Błędny format wejścia powoduje wyczyszczenie pola wynikowego i wyświetlenie:

```text
Error: wrong input format
```

## Wyniki

Wyniki są wypisywane w komponencie `OutputBox`.

Dla trybu `Real numbers` program pokazuje kolejne iteracje i znaleziony pierwiastek:

```text
Iter  1: x=...  step=...
Iter  2: x=...  step=...
--- Roots: ---
Root: ...
```

Dla trybów przedziałowych program pokazuje kolejne przedziały, ich szerokość i krok:

```text
Iter  1: [a, b] w=... step=...
Iter  2: [a, b] w=... step=...
--- Interval root: ---
[a, b] w=...
```

Po przekroczeniu limitu iteracji wypisywany jest aktualny przedział:

```text
Max iterations reached.
Current interval: [a, b]
```

## Biblioteki funkcji

Biblioteka dynamiczna z funkcją musi eksportować sześć symboli C:

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
| `f_iv` | funkcja przedziałowa |
| `df_iv` | pierwsza pochodna funkcji przedziałowej |
| `ddf_iv` | druga pochodna funkcji przedziałowej |

Funkcje przedziałowe przyjmują końce przedziału `a`, `b`, a wynik zwracają przez wskaźniki `ra`, `rb`.

Ładowanie biblioteki odbywa się w `loadFunctions` z `math/IntervalUtils.cpp` przez `dlopen` i `dlsym`. Jeżeli biblioteka podana w argumencie programu nie zostanie załadowana, aplikacja próbuje wrócić do `./build/funcs/func.so`.

## Przykłady

### Liczby rzeczywiste

Dla domyślnej funkcji:

$$
f(x) = x^2 - 2
$$

uruchom aplikację:

```bash
make
./build/main ./build/funcs/func.so
```

Wybierz `Real numbers` i wpisz:

```text
1.5
```

Oczekiwany wynik to przybliżenie:

```text
Root: 1.414213562373095...
```

### Przedział

Dla tej samej funkcji wybierz `Interval for interval numbers` i wpisz:

```text
[1, 2]
```

Program powinien znaleźć przedział otaczający dodatni pierwiastek:

```text
--- Interval root: ---
[1.41421356237309..., 1.41421356237310...] w=...
```

Dokładne końce przedziału mogą zależeć od zaokrągleń i kolejnych iteracji.
