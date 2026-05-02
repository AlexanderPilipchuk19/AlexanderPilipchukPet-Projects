# Тестовые CSV-файлы для FFT Signal Lab

## 1. `test_detect_denoise_noisy_mixture.csv`

Назначение: проверить режимы `detect` и `denoise`.

Параметры сигнала:

- `fs = 8192 Hz`
- длительность: `2 s`
- число отсчётов: `16384`
- полезные частоты: `440 Hz`, `880 Hz`, `1230 Hz`
- добавлен белый гауссов шум с `sigma = 0.20`
- добавлена небольшая постоянная составляющая `DC = 0.08`

Рекомендуемый запуск для распознавания частот:

```bash
./dsp_lab detect \
    --input test_detect_denoise_noisy_mixture.csv \
    --fs 8192 \
    --nfft 16384 \
    --window hann \
    --top 8 \
    --floor 0.02
```

В результате среди главных пиков должны быть частоты около:

```text
440 Hz
880 Hz
1230 Hz
```

Рекомендуемый запуск для шумоподавления:

```bash
./dsp_lab denoise \
    --input test_detect_denoise_noisy_mixture.csv \
    --fs 8192 \
    --nfft 16384 \
    --keep-top 3 \
    --output clean_from_noisy_mixture.csv
```

Альтернативный вариант:

```bash
./dsp_lab denoise \
    --input test_detect_denoise_noisy_mixture.csv \
    --fs 8192 \
    --nfft 16384 \
    --threshold 0.05 \
    --output clean_from_noisy_mixture.csv
```

## 2. `test_reconstruct_bandlimited.csv`

Назначение: проверить режим `reconstruct`.

Параметры сигнала:

- `fs = 1024 Hz`
- длительность: `2 s`
- число отсчётов: `2048`
- полезные частоты: `32 Hz`, `64 Hz`, `120 Hz`
- сигнал чистый и ограниченный по спектру

Рекомендуемый запуск:

```bash
./dsp_lab reconstruct \
    --input test_reconstruct_bandlimited.csv \
    --fs 1024 \
    --factor 4 \
    --cutoff 150 \
    --output reconstructed_bandlimited.csv
```

Здесь условие для `cutoff` выполнено:

```text
f_max = 120 Hz
cutoff = 150 Hz
fs / 2 = 512 Hz

120 <= 150 < 512
```

После запуска выходной файл `reconstructed_bandlimited.csv` должен содержать более плотную временную сетку:

```text
fs_out = factor * fs = 4 * 1024 = 4096 Hz
```
