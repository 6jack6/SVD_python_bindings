# SVD Python Bindings

Мини-проект демонстрирует реализацию алгоритма сингулярного разложения матрицы (SVD) на C++ и экспорт этой функции в Python при помощи [pybind11](https://pybind11.readthedocs.io).

## Структура проекта

```
src/               Весь C++-код (заголовки и реализации, включая pybind11-модуль)
examples/          Пример Python-скрипта
CMakeLists.txt     Конфигурация сборки
Makefile           Упрощённые команды сборки через cmake
```

## Требования

- CMake ≥ 3.16
- Компилятор с поддержкой C++17
- Python ≥ 3.8 с dev-пакетом
- Установленный `pybind11` (`pip install pybind11`)

## Быстрый старт (через venv)

Проект теперь полностью изолирован: все зависимости ставятся в виртуальное окружение.

```bash
# 1. Создаём и подготавливаем venv (по умолчанию ./venv)
./scripts/setup_venv.sh

# 2. Активируем окружение в текущем шелле
source .venv/bin/activate
```

После активации команда `python`/`python3` указывает на окружение, и нижеописанные команды можно запускать как обычно.

> Если хотите расположить окружение в другом месте, передайте путь первым аргументом: `./scripts/setup_venv.sh /tmp/my_env`.

## Сборка

```bash
# Сборка через Makefile (внутри активированного venv)
make

# Или напрямую через cmake
cmake -S . -B build
cmake --build build
```

После сборки расширение окажется в каталоге `build/svd/`.

> **Примечание.** `CMakeLists.txt` автоматически выбирает `python3` из PATH и находит `pybind11`, установленный через pip, поэтому дополнительные переменные окружения не нужны.

Установка модуля в активированное окружение Python выполняется командой (это опционально):

```bash
cmake --install build
```

## Использование из Python

```python
import numpy as np
import svd

A = np.array([[3.0, 1.0],
              [0.0, 2.0],
              [0.0, 0.0]])
U, S, V = svd.svd(A)
print("U =", U)
print("S =", S)
print("V =", V)
```

Функция `svd` принимает двумерный `numpy.ndarray` размера `m x n` в формате `float64` и возвращает кортеж `(U, S, V)`, где:

- `U` — матрица левых сингулярных векторов размера `m x n`;
- `S` — одномерный массив сингулярных значений (по убыванию);
- `V` — матрица правых сингулярных векторов размера `n x n`.

Параметры `tolerance` и `max_sweeps` позволяют контролировать точность и максимальное число проходов итеративного алгоритма Якоби.

## Пример

После сборки можно запустить пример без установки модуля:

```bash
PYTHONPATH=python python3 examples/simple_usage.py
```

`__init__.py` пакета автоматически добавит каталог `build/svd` в путь поиска модулей, поэтому расширение обнаружится и без установки.

## Тесты

Покрытие реализовано через pytest. Локально (после сборки) выполните:

```bash
PYTHONPATH=python python -m pytest tests
```

Те же тесты запускаются внутри docker-контейнера, поэтому достаточно собрать образ и выполнить:

```bash
docker build -t svd-wheel .
docker run --rm svd-wheel
```

Команда `docker run` вернёт код 0 только если оба теста пройдены.

## Сборка wheel-пакета

Для распространения проекта как python-пакета используется [scikit-build-core](https://scikit-build-core.readthedocs.io). Предварительно активируйте venv и установите `build`:

```bash
source .venv/bin/activate
python -m pip install build
python -m build
```

Готовые артефакты (`.whl` и `.tar.gz`) появятся в каталоге `dist/`. Установить их можно стандартной командой `python -m pip install dist/svd_python_bindings-0.1.0-py3-none-any.whl`.

## Docker

`Dockerfile` собирает wheel внутри образа Python 3.10 slim, затем устанавливает его в чистом окружении и выполняет sanity-check:

```bash
# Собрать образ со свежим wheel
docker build -t svd-wheel .

# Запустить проверку
docker run --rm svd-wheel
```

Чтобы получить wheel автоматически из сборки, используется отдельный экспортный этап (требуется BuildKit, по умолчанию он включён):

```bash
docker build --target wheel_export -o dist_from_docker .
```

После завершения команда поместит содержимое `/app/dist` (из docker-сборки) в локальный каталог `dist_from_docker`, откуда wheel можно устанавливать `pip install dist_from_docker/<wheel>.whl`.
