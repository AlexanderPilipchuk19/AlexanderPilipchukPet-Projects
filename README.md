# AlexanderPilipchukPet-Projects
В этом репозитории собраны мои pet-проекты и авторские учебные материалы по математике.

Основной фокус репозитория - проекты и материалы, в которых математические идеи переходят в конкретные алгоритмы.

## Содержание репозитория

```text
.
├── signal-processing-lab/   # pet-проект по обработке сигнала
├── lectures/                # мои авторские лекции по математике
├── signalresearch/          # моё исследования методов интерполяции сигналов для будущего проекта
└── README.md
```

## Сборка и запуск
1. signal-processing-lab/

сборка: g++ -std=c++20 -Wall -Wextra -Wpedantic -Iinclude \
    src/main.cpp src/cli.cpp src/signal.cpp src/generator.cpp \
    src/fft.cpp src/window.cpp src/spectrum.cpp src/reconstruction.cpp \
    -o dsp_lab

запуск: прототипы есть в отчёте

2. signalresearch 

сборка: g++ -std=c++20 -O2 -Wall -Wextra -pedantic main.cpp -o signal_research

запуск: ./signal_research 