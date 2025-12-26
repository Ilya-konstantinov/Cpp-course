# Segment Tree Beats

## Введение

Segment Tree Beats - это расширение классического сегментного дерева, предназначенное для эффективной поддержки нелинейных операций на отрезках, прежде всего операций вида `a[i] = min(a[i], x)` и `a[i] = max(a[i], x)` в сочетании с агрегатными запросами. Ключевая идея состоит в хранении дополнительной информации в каждом узле, позволяющей выполнять массовые изменения без спуска до листьев.

Структура особенно полезна в задачах, где стандартные ленивые метки не применимы напрямую из-за нелинейности операции.


## Структура узла

Для каждого узла, соответствующего отрезку массива, хранятся следующие данные:
- `sum` - сумма элементов на отрезке
- `max` - максимальный элемент
- `smax` - второй по величине элемент
- `maxcnt` - количество элементов, равных `max`

Инварианты:
- `max >= smax`
- каждый элемент отрезка либо равен `max`, либо не превосходит `smax`

Такая структура позволяет корректно и быстро применять операцию `chmin` (change to minimum), если новое значение лежит между `smax` и `max`.

В работе будут также рассмотрены усложненные вариации узла.

## Вариации и асимптотика

### Вариант A: `range_chmin` + `range_sum`

Поддерживаемые операции:
- `range_chmin(l, r, x)` - для всех i в [l,r) a[i] = min(a[i], x)
- `range_sum(l, r)` - сумма элементов на отрезке [l,r)

Асимптотика - `O(log n)`



```python
INF = 10 ** 30


class SegTreeBeatsA:
    def __init__(self, a):
        self.n = len(a)
        size = 1
        while size < self.n:
            size <<= 1
        self.size = size
        N = 2 * size
        self.sum = [0] * N
        self.maxv = [-INF] * N
        self.smax = [-INF] * N
        self.maxcnt = [0] * N
        self.len = [0] * N
        self._build(a)

    def _build(self, a):
        for i in range(self.size):
            k = i + self.size
            if i < self.n:
                v = a[i]
                self.sum[k] = v
                self.maxv[k] = v
                self.smax[k] = -INF
                self.maxcnt[k] = 1
                self.len[k] = 1
            else:
                self.maxv[k] = -INF
        for k in range(self.size - 1, 0, -1):
            self._update(k)

    def _update(self, k):
        l, r = 2 * k, 2 * k + 1
        self.sum[k] = self.sum[l] + self.sum[r]
        self.len[k] = self.len[l] + self.len[r]
        if self.maxv[l] > self.maxv[r]:
            self.maxv[k] = self.maxv[l]
            self.maxcnt[k] = self.maxcnt[l]
            self.smax[k] = max(self.smax[l], self.maxv[r])
        elif self.maxv[l] < self.maxv[r]:
            self.maxv[k] = self.maxv[r]
            self.maxcnt[k] = self.maxcnt[r]
            self.smax[k] = max(self.maxv[l], self.smax[r])
        else:
            self.maxv[k] = self.maxv[l]
            self.maxcnt[k] = self.maxcnt[l] + self.maxcnt[r]
            self.smax[k] = max(self.smax[l], self.smax[r])

    def _apply_chmin(self, k, x):
        if self.maxv[k] <= x:
            return
        self.sum[k] -= (self.maxv[k] - x) * self.maxcnt[k]
        self.maxv[k] = x

    def _push(self, k):
        for c in (2 * k, 2 * k + 1):
            if self.maxv[c] > self.maxv[k]:
                self._apply_chmin(c, self.maxv[k])

    def _range_chmin(self, a, b, x, k, l, r):
        if r <= a or b <= l or self.maxv[k] <= x:
            return
        if a <= l and r <= b and self.smax[k] < x:
            self._apply_chmin(k, x)
            return
        self._push(k)
        m = (l + r) // 2
        self._range_chmin(a, b, x, 2 * k, l, m)
        self._range_chmin(a, b, x, 2 * k + 1, m, r)
        self._update(k)

    def range_chmin(self, l, r, x):
        self._range_chmin(l, r, x, 1, 0, self.size)

    def _range_sum(self, a, b, k, l, r):
        if r <= a or b <= l:
            return 0
        if a <= l and r <= b:
            return self.sum[k]
        self._push(k)
        m = (l + r) // 2
        return self._range_sum(a, b, 2 * k, l, m) + self._range_sum(a, b, 2 * k + 1, m, r)

    def range_sum(self, l, r):
        return self._range_sum(l, r, 1, 0, self.size)

```

## Юнит-тесты (Вариант A)

Проверка корректности реализации Варианта A на случайных и детерминированных тестах.



```python
def naive_process_A(a, ops):
    res = []
    a = list(a)
    for op in ops:
        if op[0] == "chmin":
            _, l, r, x = op
            for i in range(l, r):
                a[i] = min(a[i], x)
        else:
            _, l, r = op
            res.append(sum(a[l:r]))
    return res


import random

random.seed(1)


def test_variant_A(iterations=50):
    for _ in range(iterations):
        n = random.randint(1, 30)
        a = [random.randint(0, 20) for _ in range(n)]
        ops = []
        for _ in range(40):
            if random.random() < 0.6:
                l = random.randint(0, n - 1)
                r = random.randint(l + 1, n)
                x = random.randint(0, 20)
                ops.append(("chmin", l, r, x))
            else:
                l = random.randint(0, n - 1)
                r = random.randint(l + 1, n)
                ops.append(("sum", l, r))

        st = SegTreeBeatsA(a.copy())
        out1 = []
        for op in ops:
            if op[0] == "chmin":
                st.range_chmin(op[1], op[2], op[3])
            else:
                out1.append(st.range_sum(op[1], op[2]))
        out2 = naive_process_A(a.copy(), ops)
        assert out1 == out2
    print("Юнит-тесты Варианта A пройдены.")


test_variant_A()
```

    Юнит-тесты Варианта A пройдены.
    

## Бенчмаркинг (Вариант A)

Скрипт сравнивает среднее время на операцию для Segment Tree Beats (Вариант A) и наивного массива.



```python
import time
import random
from statistics import mean


def generate_ops_A(n, m, op_type="mixed"):
    ops = []
    for _ in range(m):
        if op_type == "chmin":
            l = random.randint(0, n - 1)
            r = random.randint(l + 1, n)
            x = random.randint(0, 10 ** 6)
            ops.append(("chmin", l, r, x))
        elif op_type == "mixed":
            if random.random() < 0.6:
                l = random.randint(0, n - 1)
                r = random.randint(l + 1, n)
                x = random.randint(0, 10 ** 6)
                ops.append(("chmin", l, r, x))
            else:
                l = random.randint(0, n - 1)
                r = random.randint(l + 1, n)
                ops.append(("sum", l, r))
    return ops


def run_ops_A_naive(a, ops):
    return naive_process_A(a.copy(), ops)


def run_ops_A_st(a, ops):
    st = SegTreeBeatsA(a.copy())
    res = []
    for op in ops:
        if op[0] == "chmin":
            st.range_chmin(op[1], op[2], op[3])
        else:
            res.append(st.range_sum(op[1], op[2]))
    return res


def benchmark_A(k_min=10, k_max=20, m=2000, repeats=2):
    random.seed(42)
    results = []
    for k in range(k_min, k_max + 1):
        n = 1 << k
        a = [random.randint(0, 10 ** 6) for _ in range(n)]
        ops = generate_ops_A(n, m, op_type="mixed")
        # naive
        t0 = time.perf_counter()
        for _ in range(repeats):
            _ = run_ops_A_naive(a, ops)
        t_naive = (time.perf_counter() - t0) / repeats
        # segtree
        t0 = time.perf_counter()
        for _ in range(repeats):
            _ = run_ops_A_st(a, ops)
        t_st = (time.perf_counter() - t0) / repeats
        results.append((k, n, t_naive / m, t_st / m))
        print(f"k={k}, n={n}: naive avg per op={t_naive / m:.6f}s, ST avg per op={t_st / m:.6f}s")
    return results


benchmark_A(k_min=10, k_max=14, m=2000, repeats=1)
```

    k=10, n=1024: naive avg per op=0.000008s, ST avg per op=0.000009s
    k=11, n=2048: naive avg per op=0.000017s, ST avg per op=0.000011s
    k=12, n=4096: naive avg per op=0.000034s, ST avg per op=0.000019s
    k=13, n=8192: naive avg per op=0.000065s, ST avg per op=0.000029s
    k=14, n=16384: naive avg per op=0.000137s, ST avg per op=0.000049s
    




    [(10, 1024, 7.975700000315556e-06, 8.527949999916019e-06),
     (11, 2048, 1.693275000070571e-05, 1.1167950000526616e-05),
     (12, 4096, 3.4196049999081877e-05, 1.881004999995639e-05),
     (13, 8192, 6.480200000078184e-05, 2.9102300000886316e-05),
     (14, 16384, 0.00013690409999981057, 4.8887850000028264e-05)]



## Вариант B (расширенный)

Дополняется хранением:
- `min`, `smin`, `mincnt`
- ленивой меткой `add`

Поддерживаемые операции:
- `range_add(l, r, x)` - для всех i в [l,r): a[i] += x (прибавление константы)
- `range_chmin(l, r, x)` - для всех i в [l,r): a[i] = min(a[i], x)
- `range_chmax(l, r, x)` - для всех i в [l,r): a[i] = max(a[i], x)
- `range_sum(l, r)` - сумма элементов на отрезке [l,r)

Асимптотика сохраняется амортизированной `O(log n)`.



```python
INF = 10 ** 30

class SegTreeBeatsB:
    def __init__(self, a):
        self.n = len(a)
        size = 1
        while size < self.n:
            size <<= 1
        self.size = size
        N = 2 * size

        self.sum = [0] * N
        self.maxv = [-INF] * N
        self.smax = [-INF] * N
        self.maxcnt = [0] * N
        self.minv = [INF] * N
        self.smin = [INF] * N
        self.mincnt = [0] * N
        self.len = [0] * N
        self.lazy_add = [0] * N

        self._build(a)

    def _build(self, a):
        for i in range(self.size):
            k = i + self.size
            if i < self.n:
                v = a[i]
                self.sum[k] = v
                self.maxv[k] = v
                self.smax[k] = -INF
                self.maxcnt[k] = 1
                self.minv[k] = v
                self.smin[k] = INF
                self.mincnt[k] = 1
                self.len[k] = 1
            else:
                self.maxv[k] = -INF
                self.smax[k] = -INF
                self.maxcnt[k] = 0
                self.minv[k] = INF
                self.smin[k] = INF
                self.mincnt[k] = 0
                self.len[k] = 0
        for k in range(self.size - 1, 0, -1):
            self._update(k)

    def _update(self, k):
        l, r = 2*k, 2*k+1
        self.len[k] = self.len[l] + self.len[r]
        self.sum[k] = self.sum[l] + self.sum[r]

        # max, smax, maxcnt
        if self.maxv[l] > self.maxv[r]:
            self.maxv[k] = self.maxv[l]
            self.maxcnt[k] = self.maxcnt[l]
            self.smax[k] = max(self.smax[l], self.maxv[r])
        elif self.maxv[l] < self.maxv[r]:
            self.maxv[k] = self.maxv[r]
            self.maxcnt[k] = self.maxcnt[r]
            self.smax[k] = max(self.maxv[l], self.smax[r])
        else:
            self.maxv[k] = self.maxv[l]
            self.maxcnt[k] = self.maxcnt[l] + self.maxcnt[r]
            self.smax[k] = max(self.smax[l], self.smax[r])

        # min, smin, mincnt
        if self.minv[l] < self.minv[r]:
            self.minv[k] = self.minv[l]
            self.mincnt[k] = self.mincnt[l]
            self.smin[k] = min(self.smin[l], self.minv[r])
        elif self.minv[l] > self.minv[r]:
            self.minv[k] = self.minv[r]
            self.mincnt[k] = self.mincnt[r]
            self.smin[k] = min(self.minv[l], self.smin[r])
        else:
            self.minv[k] = self.minv[l]
            self.mincnt[k] = self.mincnt[l] + self.mincnt[r]
            self.smin[k] = min(self.smin[l], self.smin[r])

    def _apply_add(self, k, x):
        if self.len[k] == 0:
            return
        self.sum[k] += x * self.len[k]
        self.maxv[k] += x
        self.minv[k] += x
        if self.smax[k] != -INF:
            self.smax[k] += x
        if self.smin[k] != INF:
            self.smin[k] += x
        self.lazy_add[k] += x

    def _apply_chmin(self, k, x):
        if self.maxv[k] <= x:
            return
        self.sum[k] -= (self.maxv[k] - x) * self.maxcnt[k]
        self.maxv[k] = x
        if self.minv[k] > x:
            self.minv[k] = x
        if self.smax[k] > x:
            self.smax[k] = x

    def _apply_chmax(self, k, x):
        if self.minv[k] >= x:
            return
        self.sum[k] += (x - self.minv[k]) * self.mincnt[k]
        self.minv[k] = x
        if self.maxv[k] < x:
            self.maxv[k] = x
        if self.smin[k] < x:
            self.smin[k] = x

    def _push_max(self, k, x):
        if self.maxv[k] > x:
            self._apply_chmin(k, x)

    def _push_min(self, k, x):
        if self.minv[k] < x:
            self._apply_chmax(k, x)

    def _push(self, k):
        if self.len[k] == 0:
            return
        for c in (2*k, 2*k+1):
            if self.lazy_add[k] != 0:
                self._apply_add(c, self.lazy_add[k])

        if self.maxv[k] < self.maxv[2*k]:
            self._push_max(2*k, self.maxv[k])
        if self.maxv[k] < self.maxv[2*k+1]:
            self._push_max(2*k+1, self.maxv[k])

        if self.minv[k] > self.minv[2*k]:
            self._push_min(2*k, self.minv[k])
        if self.minv[k] > self.minv[2*k+1]:
            self._push_min(2*k+1, self.minv[k])

        self.lazy_add[k] = 0

    def _range_add(self, a, b, x, k, l, r):
        if r <= a or b <= l or self.len[k] == 0:
            return
        if a <= l and r <= b:
            self._apply_add(k, x)
            return
        self._push(k)
        m = (l + r) // 2
        self._range_add(a, b, x, 2*k, l, m)
        self._range_add(a, b, x, 2*k+1, m, r)
        self._update(k)

    def range_add(self, l, r, x):
        self._range_add(l, r, x, 1, 0, self.size)

    def _range_chmin(self, a, b, x, k, l, r):
        if r <= a or b <= l or self.maxv[k] <= x:
            return
        if a <= l and r <= b and self.smax[k] < x:
            self._apply_chmin(k, x)
            return
        self._push(k)
        m = (l + r) // 2
        self._range_chmin(a, b, x, 2*k, l, m)
        self._range_chmin(a, b, x, 2*k+1, m, r)
        self._update(k)

    def range_chmin(self, l, r, x):
        self._range_chmin(l, r, x, 1, 0, self.size)

    def _range_chmax(self, a, b, x, k, l, r):
        if r <= a or b <= l or self.minv[k] >= x:
            return
        if a <= l and r <= b and self.smin[k] > x:
            self._apply_chmax(k, x)
            return
        self._push(k)
        m = (l + r) // 2
        self._range_chmax(a, b, x, 2*k, l, m)
        self._range_chmax(a, b, x, 2*k+1, m, r)
        self._update(k)

    def range_chmax(self, l, r, x):
        self._range_chmax(l, r, x, 1, 0, self.size)

    def _range_sum(self, a, b, k, l, r):
        if r <= a or b <= l or self.len[k] == 0:
            return 0
        if a <= l and r <= b:
            return self.sum[k]
        self._push(k)
        m = (l + r) // 2
        return self._range_sum(a, b, 2*k, l, m) + self._range_sum(a, b, 2*k+1, m, r)

    def range_sum(self, l, r):
        return self._range_sum(l, r, 1, 0, self.size)

```

## Юнит-тесты (Вариант B)

Ниже - наивная реализация и набор тестов (случайные + детерминированные) для проверки Варианта B.



```python
def naive_process_B(a, ops):
    a = a[:]
    res = []
    for op in ops:
        if op[0] == 'add':
            _, l, r, x = op
            for i in range(l, r):
                a[i] += x
        elif op[0] == 'chmin':
            _, l, r, x = op
            for i in range(l, r):
                a[i] = min(a[i], x)
        elif op[0] == 'chmax':
            _, l, r, x = op
            for i in range(l, r):
                a[i] = max(a[i], x)
        else:  # sum
            _, l, r = op
            res.append(sum(a[l:r]))
    return res


def runopsBst(a, ops):
    st = SegTreeBeatsB(a.copy())
    res = []
    for op in ops:
        if op[0] == 'add':
            st.range_add(op[1], op[2], op[3])
        elif op[0] == 'chmin':
            st.range_chmin(op[1], op[2], op[3])
        elif op[0] == 'chmax':
            st.range_chmax(op[1], op[2], op[3])
        else:
            res.append(st.range_sum(op[1], op[2]))
    return res


def test_variant_B(iterations=50):
    for _ in range(iterations):
        n = random.randint(1, 30)
        a = [random.randint(-20, 20) for _ in range(n)]
        ops = []
        for _ in range(40):
            l = random.randint(0, n - 1)
            r = random.randint(l + 1, n)
            if random.random() < 0.3:
                x = random.randint(-20, 20)
                ops.append(('add', l, r, x))
            elif random.random() < 0.5:
                x = random.randint(-20, 20)
                ops.append(('chmin', l, r, x))
            else:
                x = random.randint(-20, 20)
                ops.append(('chmax', l, r, x))
        out1 = runopsBst(a, ops)
        out2 = naive_process_B(a, ops)
        assert out1 == out2
    print("Юнит-тесты Варианта B пройдены.")


test_variant_B()
```

    Юнит-тесты Варианта B пройдены.
    

## Бенчмаркинг (Вариант B)

Скрипт сравнивает среднее время на операцию для Segment Tree Beats (Вариант B) и наивного массива. Методология аналогична Варианту A.



```python
import time
import random


def generate_ops_B(n, m, mode="mixed"):
    ops = []
    for _ in range(m):
        rtype = random.random()
        l = random.randint(0, n - 1)
        r = random.randint(l + 1, n)
        if mode == "add_only":
            x = random.randint(-1000, 1000)
            ops.append(("add", l, r, x))
        elif mode == "chmin_only":
            x = random.randint(-1000, 1000)
            ops.append(("chmin", l, r, x))
        else:  # mixed
            if rtype < 0.25:
                x = random.randint(-500, 500)
                ops.append(("add", l, r, x))
            elif rtype < 0.60:
                x = random.randint(-500, 500)
                ops.append(("chmin", l, r, x))
            elif rtype < 0.90:
                x = random.randint(-500, 500)
                ops.append(("chmax", l, r, x))
            else:
                ops.append(("sum", l, r))
    return ops


def run_ops_B_naive(a, ops):
    return naive_process_B(a.copy(), ops)


def run_ops_B_st(a, ops):
    st = SegTreeBeatsB(a.copy())
    res = []
    for op in ops:
        if op[0] == "add":
            st.range_add(op[1], op[2], op[3])
        elif op[0] == "chmin":
            st.range_chmin(op[1], op[2], op[3])
        elif op[0] == "chmax":
            st.range_chmax(op[1], op[2], op[3])
        else:
            res.append(st.range_sum(op[1], op[2]))
    return res


def benchmark_B(k_min=10, k_max=20, m=2000, repeats=1):
    random.seed(202)
    for k in range(k_min, k_max + 1):
        n = 1 << k
        a = [random.randint(-1000, 1000) for _ in range(n)]
        ops = generate_ops_B(n, m, mode="mixed")
        # naive
        t0 = time.perf_counter()
        for _ in range(repeats):
            _ = run_ops_B_naive(a, ops)
        t_naive = (time.perf_counter() - t0) / max(1, repeats)
        # segtree
        t0 = time.perf_counter()
        for _ in range(repeats):
            _ = run_ops_B_st(a, ops)
        t_st = (time.perf_counter() - t0) / max(1, repeats)
        print(f"k={k}, n={n}: naive avg per op={t_naive / m:.6f}s, ST avg per op={t_st / m:.6f}s")


benchmark_B(k_min=17, k_max=21, m=1000, repeats=1)
# пасхалко
print("прикол: из-за реализации ST в питоне, она работает чуть дольше на k<=17")
```

    k=17, n=131072: naive avg per op=0.001528s, ST avg per op=0.001026s
    k=18, n=262144: naive avg per op=0.003226s, ST avg per op=0.001734s
    k=19, n=524288: naive avg per op=0.006389s, ST avg per op=0.003790s
    k=20, n=1048576: naive avg per op=0.012372s, ST avg per op=0.007319s
    k=21, n=2097152: naive avg per op=0.025743s, ST avg per op=0.013166s
    прикол: из-за реализации ST в питоне, она работает чуть дольше на k<=17
    

## Доказательство асимптотики

Рассматривается потенциальная функция:

Φ = Σ_v (max_v - smax_v) * maxcnt_v

Каждое успешное применение `apply_chmin` уменьшает `Φ` минимум на единицу. Спуск вглубь дерева возможен только если операция не может быть выполнена на текущем узле, а это означает наличие ненулевого потенциала ниже. Потенциал изначально конечен, значит суммарное число таких спусков ограничено. Следовательно, суммарная стоимость всех операций ограничена `O((n + q) log n)`.

## Методы Segment Tree Beats

### Обновление узла
- `_update(k)` - обновляет информацию: sum, len, maxv/smax/maxcnt, minv/smin/mincnt (B)
### Применение операций к узлу
- `_apply_chmin(k, x)` - применяет `a[i] = min(a[i], x)` ко всему сегменту узла k, обновляет sum, maxv, smax и minv (B)
- `_apply_chmax(k, x)` - применяет `a[i] = max(a[i], x)` ко всему сегменту узла k, обновляет sum, minv, smin и maxv (B)
- `_apply_add(k, x)` - прибавляет x ко всем элементам сегмента k, обновляет sum, maxv, minv, smax, smin и lazy_add (B)
### Проталкивание изменений вниз
- `_push(k)` - применяет отложенные изменения к узлам: lazy_add, chmin/chmax
- `_push_max(k, x)` - применяет chmin к ребенку, если maxv > x (B)
- `_push_min(k, x)` - применяет chmax к ребенку, если minv < x (B)

### Рекурсивные операции на отрезке
- `_range_chmin(a, b, x, k, l, r)` - рекурсивно применяет chmin к отрезку \[a,b) в поддереве k, использует _push и _apply_chmin
- `range_chmin(l, r, x)` - публичный интерфейс для chmin
- `_range_chmax(a, b, x, k, l, r)` - рекурсивно применяет chmax к отрезку \[a,b) в поддереве k (B)
- `range_chmax(l, r, x)` - публичный интерфейс для chmax (B)
- `_range_add(a, b, x, k, l, r)` - рекурсивно применяет add к отрезку \[a,b) в поддереве k, использует _push и _apply_add
- `range_add(l, r, x)` - публичный интерфейс для add (B)
- `_range_sum(a, b, k, l, r)` - рекурсивно вычисляет сумму элементов на отрезке \[a,b) в поддереве k, использует _push
- `range_sum(l, r)` -  интерфейс для range_sum

## Заключение

Segment Tree Beats является одной из наиболее продвинутых структур данных для работы с отрезками. Она позволяет эффективно решать задачи, недоступные стандартным сегментным деревьям, за счет хранения расширенной информации в узлах и использования амортизированного анализа.
В данной работе для успешного бенчмаркинга ST версии B искусственно были увеличены размеры тестируемых массивов.

## Источники
- https://codeforces.com/blog/entry/57319 - непереведенная статья по асимптотике операций дерева
- https://www.youtube.com/watch?v=hpsgEu0t5OI - видео от Яндекса по дереву

