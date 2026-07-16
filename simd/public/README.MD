# Задача: SIMD LiDAR Filter (Branchless SoA)

**Тема:** SIMD, Data-Oriented Design, Branch Prediction.

## Контекст
Представьте, что вы разрабатываете софт для беспилотного автомобиля. На крыше машины установлен сенсор LiDAR, который 100 раз в секунду генерирует "облако точек" — массив из миллионов 3D-координат окружающего мира. 

Ваша задача — написать фильтр, который мгновенно находит индексы точек, попавших в заданную "опасную зону" (Bounding Box) прямо по курсу движения автомобиля, чтобы передать их дальше в алгоритм экстренного торможения.

Наивное решение будет плохим в силу branch missprediction и обработки каждой точки отдельно.

## Условие задачи

Вам необходимо реализовать функцию `filter_points_simd` в файле `filter_simd.c`, используя C-интринсики для **AVX2** (`<immintrin.h>`).

### Формат данных: SoA (Structure of Arrays)
Вместо классического массива структур (AoS), где данные лежат как `[XYZ] [XYZ] [XYZ]`, на вход подаются три независимых выровненных массива: `xs`, `ys` и `zs`. 
Это классический паттерн **Data-Oriented Design**, который позволяет загружать данные в векторные регистры идеально ровными блоками по 8 `float` за одну инструкцию `_mm256_loadu_ps`.

### Сигнатура функции:
```c
size_t filter_points_simd(
    const float* xs, const float* ys, const float* zs, 
    size_t count, 
    BoundingBox box, 
    uint32_t* out_indices
);
```

**Что нужно сделать:**

Переписать на SIMD функцию:
```c
size_t filter_points_naive(
    const float* xs, const float* ys, const float* zs, 
    size_t count, BoundingBox box, uint32_t* out_indices) 
{
    size_t out_count = 0;
    for (size_t i = 0; i < count; i++) {
        if (xs[i] >= box.x_min && xs[i] <= box.x_max &&
            ys[i] >= box.y_min && ys[i] <= box.y_max &&
            zs[i] >= box.z_min && zs[i] <= box.z_max) {
            out_indices[out_count++] = i;
        }
    }
    return out_count;
}
```

## Ограничения и требования
* **Strictly Branchless:** Внутри основного SIMD-цикла **не должно быть ни одного условного оператора (`if`, `? :`)**. Вся фильтрация должна делаться через битовые маски.
* **Производительность:** Ваш код должен работать **как минимум в 3.5 раза быстрее**, чем оптимизированный компилятором C-код.

## Build & test
```bash
make test
```

Пример вывода:
```text
[*] Starting SIMD LiDAR Filter Test...
[1] Running Naive version...
[2] Running SIMD version...

--- RESULTS ---
Points found: 18451 (Naive), 18451 (SIMD)
[+] Correctness check: PASSED
Naive Time: 12.45 ms
SIMD Time:  2.90 ms
Speedup:    4.29x
[+] Performance check: PASSED
```

## Что почитать
* [Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)
* [Data-Oriented Design: AoS vs SoA](https://en.wikipedia.org/wiki/AoS_and_SoA)
Допом:
* [improving performance with simd intrinsics in three use cases](https://stackoverflow.blog/2020/07/08/improving-performance-with-simd-intrinsics-in-three-use-cases/)
* [Designing a SIMD Algorithm from Scratch](https://mcyoung.xyz/2023/11/27/simd-base64/)

Удачи в оптимизации! 🚀
