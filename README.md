# Мои лабораторные работы по C++

Этот репозиторий содержит решения лабораторных работ по C++ с первого курса ИТМО.

## Содержание

| Лабораторная                            | Ссылка                                                                                                                                                                                                                                                                                                     |
|:----------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Модель песчаной кучи (Sandpile Model)   | [Перейти](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Abelian-Sandpile-Model-To-BMP-File)                                                                                                                                                                                                           |
| Парсер аргументов командной строки      | [Перейти](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Argument-Parser)                                                                                                                                                                                                                              |
| Поиск маршрутов (Yandex API + HTTP)     | [Перейти](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Search_for_intercity_tickets_Yandex_Maps_API)                                                                                                                                                                                                                                                                                                |
| STL: Адаптеры и ленивые вычисления      | [Перейти](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Pipeline_implementation)                                                                                                                                                                                                                      |
| Шедулер задач (TTaskScheduler)          | [Перейти](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Scheduler_Pattern_Implementation)                                                                                                                                                                                                             |

---

## Краткое описание

*   **[Lab 3](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Abelian-Sandpile-Model-To-BMP-File) — Sandpile Model:** Реализация клеточного автомата «модель песчаной кучи». Программа сохраняет итерации состояния сетки в формате `.bmp`, используя ручную реализацию работы с форматом (без сторонних библиотек).
*   **[Lab 4](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Argument-Parser) — ArgParser:** Проектирование библиотеки для удобного парсинга аргументов командной строки. Реализована поддержка строковых, целочисленных и булевых аргументов, протестирована с помощью GoogleTest.
*   **[Lab 6](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Search_for_intercity_tickets_Yandex_Maps_API) — Route Searcher:** Консольное приложение для поиска маршрутов между городами с использованием Yandex API. Реализовано кэширование результатов, работа с HTTP-запросами через библиотеку `cpr` и парсинг JSON.
*   **[Lab 8](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Pipeline_implementation) — Adapters & Streams:** Разработка библиотеки адаптеров (Dir, Filter, Transform, AggregateByKey и др.) для потоковой обработки данных в стиле функционального программирования.
*   **[Lab 9](https://github.com/Shapka11/Cpp-Itmo-Hub/tree/main/Scheduler_Pattern_Implementation) — Task Scheduler:** Создание системы планирования задач (`TTaskScheduler`), представляющей вычисления в виде графа зависимостей. Поддерживает работу с Future-результатами и отложенное выполнение вычислений.
