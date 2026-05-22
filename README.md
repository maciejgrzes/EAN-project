# Dokumentacja projektu: rozwiązywanie równań nieliniowych metodą Newtona-Raphsona drugiego rzędu

## 1. Zastosowanie

Program służy do numerycznego wyznaczania pierwiastków równań nieliniowych postaci:

$$
f(x) = 0
$$

Projekt udostępnia graficzny interfejs użytkownika napisany z użyciem biblioteki Raylib oraz trzy tryby pracy:

1. `Real numbers` - metoda dla liczb rzeczywistych, startująca z punktu `x0`.
2. `Interval for real numbers` - metoda startująca z punktu rzeczywistego, która po znalezieniu przybliżenia buduje przedział kandydujący wokół przybliżenia i uruchamia wersję przedziałową.
3. `Interval for interval numbers` - metoda bezpośrednio dla danych przedziałowych, np. `[1, 2]`.

Funkcja badana jest ładowana z biblioteki dynamicznej `.so`, dzięki czemu można łatwo podmieniać rozwiązywane równanie bez przebudowy głównej aplikacji. W kodzie jako ścieżka domyślna wpisana jest biblioteka:

```text
./build/funcs/func.so
```

W katalogu `funcs/` znajdują się trzy przykładowe funkcje:

| Plik | Funkcja |
| --- | --- |
| `func.cpp` | $f(x) = x^2 - 2$ |
| `func1.cpp` | $f(x) = x^3 - x - 2$ |
| `func2.cpp` | $f(x) = \cos(x) - x$ |

## 2. Opis metody

Program implementuje metodę Newtona-Raphsona drugiego rzędu:

$$
f(x_n) + f'(x_n)(x - x_n) + \frac{1}{2}f''(x_n)(x - x_n)^2 = 0
$$

otrzymuje się równanie kwadratowe względem kroku `h = x - x_n`:

$$
\frac{1}{2} f''(x_n)h^2 + f'(x_n)h + f(x_n) = 0
$$

Po rozwiązaniu tego równania kolejne przybliżenie ma postać:

$$
x_{n+1}
= x_n -
\frac{
f'(x_n) \pm \sqrt{(f'(x_n))^2 - 2f(x_n)f''(x_n)}
}{
f''(x_n)
}
$$

Program oblicza dwa możliwe kandydaty i wybiera ten, który daje mniejszy krok względem bieżącego punktu. Jeżeli wyróżnik jest ujemny, implementacja przechodzi awaryjnie do klasycznej metody Newtona:

$$
x_{n+1} = x_n - \frac{f(x_n)}{f'(x_n)}
$$

Dla przedziałów stosowana jest analogiczna idea, ale działania wykonywane są na obiektach `Interval<mpreal>`. Przedział jest odrzucany, jeżeli $0 \notin f(X)$. Jeżeli pochodne zawierają zero albo kandydaci metody drugiego rzędu nie dają poprawnego zawężenia, przedział jest dzielony na dwie części. Końcowym wynikiem są przedziały o szerokości mniejszej niż `1e-15`. Przedziały otrzymane po osiągnięciu limitu podziału są wypisywane osobno jako przedziały kandydujące.

## 3. Wywołanie procedury/funkcji

Główne procedury obliczeniowe znajdują się w plikach `math/Newton.h` i `math/Newton.cpp`.

```cpp
void runNewtonRaphsonReal(
    long double x0,
    RealFn f,
    RealFn df,
    RealFn ddf,
    OutputBox& out,
    const int MAX_ITER
);
```

Procedura uruchamia metodę Newtona drugiego rzędu dla punktu rzeczywistego `x0`.

```cpp
void runNewtonRaphsonInterval(
    Interval<mpreal> x0,
    IntervalFn f,
    IntervalFn df,
    IntervalFn ddf,
    OutputBox& out,
    const int MAX_ITER,
    const int MAX_DEPTH
);
```

Procedura uruchamia metodę dla przedziału startowego `x0`.

```cpp
void runNewtonRaphsonFromPoint(
    long double x0,
    RealFn f,
    RealFn df,
    RealFn ddf,
    IntervalFn fi,
    IntervalFn dfi,
    IntervalFn ddfi,
    OutputBox& out,
    const int MAX_ITER,
    const int MAX_DEPTH
);
```

Procedura najpierw wykonuje iteracje dla liczb rzeczywistych, następnie szuka przedziału kandydującego, dla którego ocena przedziałowa funkcji zawiera zero, i przekazuje go do `runNewtonRaphsonInterval`.

Z poziomu użytkownika program uruchamia się poleceniem:

```bash
make
./build/main ./build/funcs/func.so
```

Można też wskazać inną bibliotekę funkcji:

```bash
./build/main ./build/funcs/func1.so
```

## 4. Dane do funkcji

W zależności od trybu pracy użytkownik podaje w polu tekstowym:

| Tryb | Format wejścia | Przykład |
| --- | --- | --- |
| `Real numbers` | liczba rzeczywista | `1.5` |
| `Interval for real numbers` | liczba rzeczywista | `1.5` |
| `Interval for interval numbers` | przedział domknięty | `[1, 2]` |

W przypadku trybu przedziałowego wejście jest parsowane przez funkcję:

```cpp
Interval<mpreal> ParseInterval(const string& input);
```

Funkcja akceptuje zapis `[a, b]` oraz pojedynczą liczbę. Spacje są ignorowane. Błędny format wejścia powoduje wyświetlenie komunikatu:

```text
Error: wrong input format
```

## 5. Wynik

Wyniki są wypisywane w polu `OutputBox`. Dla trybu rzeczywistego program pokazuje kolejne iteracje:

```text
Iter  1: x=...  step=...
Iter  2: x=...  step=...
--- Roots: ---
Root: ...
```

Dla trybu przedziałowego program pokazuje proces zawężania lub dzielenia przedziałów. Na końcu wypisywana jest także osobna sekcja dla przedziałów zatrzymanych przez limit podziału:

```text
d=0 Iter  1: [a, b] w=...
Bisect (f'=0) [a, b] depth=...
Split candidates [a, b] depth=...
--- Roots: n ---
Root: [a, b]
--- Depth limited intervals: n ---
Depth-limited candidate: [a, b]
```

Wynikiem końcowym jest:
1. punktowe przybliżenie pierwiastka w trybie rzeczywistym,
2. zero lub więcej wąskich przedziałów wynikowych w trybie przedziałowym oraz ewentualne przedziały kandydujące zatrzymane przez limit podziału.

Pole wynikowe automatycznie przewija się do najnowszych komunikatów, dzięki czemu przy większej liczbie iteracji widoczny jest końcowy wynik.

## 6. Inne parametry

Najważniejsze stałe sterujące obliczeniami są zdefiniowane bezpośrednio w implementacji:

| Parametr | Wartość | Znaczenie |
| --- | ---: | --- |
| tolerancja kroku | `1e-15` | bezwzględny i względny warunek zakończenia iteracji rzeczywistej |
| tolerancja szerokości | `1e-15` | warunek uznania przedziału za wynikowy |
| minimalna wartość pochodnej drugiej w iteracjach punktowych | `1e-20` | zabezpieczenie przed dzieleniem przez wartość bliską zeru |
| początkowe `delta` w trybie mieszanym | `1e-10` | promień pierwszego przedziału budowanego wokół przybliżenia |

Aplikacja graficzna działa w oknie `1300 x 700` pikseli i odświeża się z docelową szybkością `60 FPS`.

Dla iteracji rzeczywistych stosowane są dwa warunki stopu:

$$
|x_{k+1} - x_k| < \varepsilon
$$

oraz:

$$
\frac{|x_{k+1} - x_k|}{\max(|x_{k+1}|, 1)} < \varepsilon
$$

gdzie w programie przyjęto $\varepsilon = 10^{-15}$.

## 7. Typy parametrów

Typy funkcji są zdefiniowane w pliku `common/Types.h`.

```cpp
using RealFn = std::function<long double(long double)>;
using IntervalFn = std::function<Interval<mpreal>(Interval<mpreal>)>;

typedef long double (*FnPtr)(long double);
typedef void (*IVFnPtr)(long double, long double, long double*, long double*);
```

Znaczenie typów:

| Typ | Znaczenie |
| --- | --- |
| `long double` | typ liczbowy dla obliczeń punktowych |
| `mpreal` | typ wieloprecyzyjny z biblioteki MPFR C++ |
| `Interval<mpreal>` | przedział o końcach typu `mpreal` |
| `RealFn` | funkcja rzeczywista przyjmująca i zwracająca `long double` |
| `IntervalFn` | funkcja przedziałowa przyjmująca i zwracająca `Interval<mpreal>` |
| `OutputBox&` | referencja do komponentu GUI, do którego dopisywane są komunikaty |

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

Funkcje `f_iv`, `df_iv` i `ddf_iv` przyjmują końce przedziału `[a, b]`, a wynik zwracają przez wskaźniki `ra` i `rb`.

## 8. Identyfikatory nielokalne

Procedury Newtona korzystają z następujących identyfikatorów i elementów spoza własnego zakresu lokalnego:

| Identyfikator | Lokalizacja | Rola |
| --- | --- | --- |
| `Interval<mpreal>` | `libs/Interval.h` | reprezentacja przedziałów |
| `mpreal` | `libs/mpreal.h` | liczby wieloprecyzyjne MPFR |
| `ContainsZero` | `math/IntervalUtils.h` | sprawdza, czy przedział zawiera zero |
| `ISqrt` | `libs/Interval.h` | pierwiastek z przedziału |
| `IntRead` | `libs/Interval.h` | wczytywanie liczby jako przedziału z poprawnym zaokrągleniem |
| `IEndsToStrings` | `libs/Interval.h` | konwersja końców przedziału na napisy |
| `FormatInterval` | `math/Newton.cpp` | pomocnicze formatowanie przedziału do wypisania |
| `BisectWithLog` | `math/Newton.cpp` | podział przedziału na dwie części z dopisaniem komunikatu |
| `StoreOrQueue` | `math/Newton.cpp` | zapisanie wąskiego przedziału jako wyniku albo dodanie go do kolejki |
| `OutputBox::Add` | `ui/OutputBox.cpp` | dopisywanie komunikatów do okna wyników |
| `OutputBox::Clear` | `ui/OutputBox.cpp` | czyszczenie okna wyników |
| `loadFunctions` | `math/IntervalUtils.cpp` | ładowanie funkcji z biblioteki dynamicznej przez `dlopen` i `dlsym` |
| `ParseInterval` | `math/IntervalUtils.cpp` | zamiana tekstu użytkownika na `Interval<mpreal>` |

Projekt zależy od bibliotek systemowych:

```text
raylib, mpfr, gmp, dl
```

Są one linkowane w pliku `Makefile`:

```make
LIBS := -lmpfr -lgmp -lraylib -ldl
```

## 9. Kod procedury/funkcji

Poniżej przedstawiono najważniejsze fragmenty algorytmu. Pełny kod znajduje się w pliku `math/Newton.cpp`.

### 9.1. Metoda dla liczb rzeczywistych

```cpp
void runNewtonRaphsonReal(long double x0, RealFn f, RealFn df, RealFn ddf, OutputBox& out, const int MAX_ITER) {
    out.Clear();
    long double x = x0;

    for (int i = 0; i < MAX_ITER; i++) {
        long double fx   = f(x);
        long double dfx  = df(x);
        long double ddfx = ddf(x);

        if (fabsl(ddfx) < 1e-20L) {
            out.Add("Error: f''(x) = 0, cannot apply NR2");
            return;
        }

        long double disc = dfx*dfx - 2.0L*fx*ddfx;

        long double xn;
        if (disc < 0.0L) {
            out.Add("  disc<0 at iter " + to_string(i+1) + ", using Newton fallback");
            if (fabsl(dfx) < 1e-20L) { out.Add("Error: f'(x)=0 too"); return; }
            xn = x - fx / dfx;
        } else {
            long double sp = sqrtl(disc);
            long double x1 = x - (dfx - sp) / ddfx;
            long double x2 = x - (dfx + sp) / ddfx;
            xn = (fabsl(x2 - x) > fabsl(x1 - x)) ? x1 : x2;
        }

        long double step = fabsl(xn - x);

        ostringstream ss;
        ss << "Iter " << setw(2) << i + 1
           << ": x=" << fixed << setprecision(10) << xn
           << "  step=" << scientific << setprecision(2) << step;
        out.Add(ss.str());

        x = xn;

        if (step < 1e-15L || step / max(fabsl(x), 1.0L) < 1e-15L) {
            ostringstream root;
            root << fixed << setprecision(20) << x;
            out.Add("--- Roots: ---");
            out.Add("Root: " + root.str());
            return;
        }
    }
    out.Add("Max iterations reached.");
}
```

### 9.2. Metoda dla przedziałów

Wersja przedziałowa działa na kolejce przedziałów. Każdy element kolejki przechowuje przedział oraz głębokość jego podziału:

```cpp
struct SubInterval {
    Interval<mpreal> x;
    int depth;
};
```

Dla każdego przedziału program:

1. sprawdza, czy $0 \in f(X)$,
2. oblicza środek przedziału,
3. tworzy kandydatów metody Newtona drugiego rzędu,
4. przecina kandydatów z aktualnym przedziałem,
5. zapisuje wąskie przedziały jako wyniki albo dodaje kolejne przedziały do dalszego przetwarzania,
6. stosuje bisekcję, gdy pochodne zawierają zero albo nie da się jednoznacznie zawęzić przedziału.

Powtarzalne operacje zostały wydzielone do funkcji pomocniczych:

| Funkcja pomocnicza | Zadanie |
| --- | --- |
| `FormatInterval` | tworzy tekstową postać przedziału |
| `PushBisection` | dodaje do kolejki dwa podprzedziały |
| `BisectWithLog` | wykonuje bisekcję i wypisuje powód podziału |
| `StoreOrQueue` | zapisuje kandydata jako wynik albo dodaje go do kolejki |

Najważniejszy fragment:

```cpp
Interval<mpreal> disc = dfm * dfm - two * fm * ddfx;

if (disc.b < mpreal(0)) {
    Interval<mpreal> xn = m - fm / dfm;
    mpreal na = max(xn.a, x.a);
    mpreal nb = min(xn.b, x.b);

    if (na > nb) {
        BisectWithLog(queue, x, mid_val, depth, "disc<0", out);
        done = true;
        break;
    }

    x = Interval<mpreal>(na, nb);
    continue;
}
```

## 10. Przykłady

### 10.1. Przykład dla liczb rzeczywistych

Funkcja z pliku `func.cpp`:

$$
f(x) = x^2 - 2
$$

Uruchomienie:

```bash
make
./build/main ./build/funcs/func.so
```

W programie należy wybrać tryb `Real numbers` i wpisać na przykład:

```text
1.5
```

Oczekiwany wynik to przybliżenie:

```text
Root: 1.414213562373095...
```

czyli $\sqrt{2}$.

### 10.2. Przykład dla danych przedziałowych

Dla tej samej funkcji:

$$
f(x) = x^2 - 2
$$

należy wybrać tryb `Interval for interval numbers` i wpisać (przy czym ważne jest aby pierwiastek znajdował się w przedziale):

```text
[1, 2]
```

Program powinien zawężać przedział do otoczenia pierwiastka dodatniego:

```text
Root: [1.41421356237309..., 1.41421356237310...]
```

Dokładne końce przedziału mogą zależeć od zaokrągleń i kolejności zawężania.

### 10.3. Przykład z inną funkcją

Funkcja z pliku `func1.cpp`:

$$
f(x) = x^3 - x - 2
$$

Uruchomienie:

```bash
./build/main ./build/funcs/func1.so
```

Dane wejściowe w trybie `Real numbers` na przykład:

```text
1.5
```

Oczekiwany pierwiastek rzeczywisty:

```text
Root: 1.521379706804567...
```

### 10.4. Przykład z funkcją trygonometryczną

Funkcja z pliku `func2.cpp`:

$$
f(x) = \cos(x) - x
$$

Uruchomienie:

```bash
./build/main ./build/funcs/func2.so
```

Dane wejściowe:

```text
0.7
```

Oczekiwany pierwiastek:

```text
Root: 0.739085133215160...
```

### 10.5. Przykład błędu formatu wejścia

W trybie `Interval for interval numbers` wejście:

```text
[1 2]
```

nie zawiera przecinka rozdzielającego końce przedziału. Program przechwytuje wyjątek parsera i wypisuje:

```text
Error: wrong input format
```

### 10.6. Przykład błędu ładowania funkcji

Jeżeli program zostanie uruchomiony z nieistniejącą biblioteką:

```bash
./build/main ./build/funcs/brak.so
```

aplikacja wypisze komunikat o błędzie ładowania i spróbuje wrócić do domyślnej biblioteki:

```text
ERROR: cannot load: ./build/funcs/brak.so
Falling back to: ./build/funcs/func.so
```

## 11. Struktura projektu

```text
.
├── Makefile
├── main.cpp
├── common/
│   └── Types.h
├── funcs/
│   ├── func.cpp
│   ├── func1.cpp
│   └── func2.cpp
├── libs/
│   ├── Interval.h
│   └── mpreal.h
├── math/
│   ├── IntervalUtils.cpp
│   ├── IntervalUtils.h
│   ├── Newton.cpp
│   └── Newton.h
└── ui/
    ├── Button.cpp
    ├── Button.h
    ├── InputBox.cpp
    ├── InputBox.h
    ├── OutputBox.cpp
    └── OutputBox.h
```

Najważniejsze moduły:

| Moduł | Opis |
| --- | --- |
| `main.cpp` | obsługa okna, przycisków, pola wejścia i wyboru trybu |
| `math/Newton.cpp` | implementacja metod Newtona |
| `math/IntervalUtils.cpp` | parser przedziałów i ładowanie funkcji z `.so` |
| `funcs/*.cpp` | przykładowe funkcje testowe |
| `ui/*.cpp` | komponenty interfejsu graficznego |
| `libs/Interval.h` | arytmetyka przedziałowa |

## 12. Kompilacja i uruchomienie

Do kompilacji wymagane są biblioteki:

```text
raylib, MPFR, GMP, dl
```

Budowanie projektu:

```bash
make
```

Uruchomienie z funkcją domyślną:

```bash
./build/main
```

Uruchomienie z wybraną funkcją:

```bash
./build/main ./ścieżka/do/pliku/funkcja.so
```

Czyszczenie plików wynikowych:

```bash
make clean
```
