### Python
# Лабораторная работа №7

Необходимо реализовать клиентское консольное приложение для получения информации о погоде.

### Требования

* По команде `current %city%` выводить актуальную информацию о погоде в городе `%city%`
* По команде `forecast %city% %days%` выводить прогноз погоды в городе `%city%` на ближайшие `%days%` дней. Прогноз погоды должен содержать максимальные и минимальные значения температуры в диапазонах 00:00-05:00, 06:00-11:00, 12:00-17:00, 18:00-23:00
* Команда `setunit %unit%` устанавливает единицы измерения температуры. Доступные значения (c, f, celsius, fahrenheit без учета регистра)
* Команда `getunit` выводит выбранную единицу измерения
* Единицы измерения по умолчанию: цельсий
* По команде `exit` завершать выполнение программы
* Остальные команды должны сообщать об ошибке и выводить список доступных команд
* Координаты города по его имени получать с [nominatim](https://nominatim.org/release-docs/develop/api/Search/)
* Погоду по координатам получать с [open meteo](https://open-meteo.com/en/docs#api-documentation)
* Для обращения к API использовать библиотеку requests
* Программа должна сообщать пользователю о некорректных входных параметрах

### Предлагаемый интерфейс программы

```
>>> current Zelenograd
Current temp is -4.1C
>>> forecast Zelenograd 3
2022-11-22 00:00-05:00 min: -4.4C    max: -3.1C
2022-11-22 06:00-11:00 min: -2.4C    max: -0.4C
2022-11-22 12:00-17:00 min: -0.2C    max:  0.1C
2022-11-22 18:00-23:00 min:  0.1C    max:  0.1C
2022-11-23 00:00-05:00 min:  0.0C    max:  0.1C
2022-11-23 06:00-11:00 min: -0.3C    max: -0.0C
2022-11-23 12:00-17:00 min: -1.9C    max: -0.5C
2022-11-23 18:00-23:00 min: -3.2C    max: -2.2C
2022-11-24 00:00-05:00 min: -5.0C    max: -3.4C
2022-11-24 06:00-11:00 min: -5.2C    max: -4.4C
2022-11-24 12:00-17:00 min: -5.4C    max: -4.5C
2022-11-24 18:00-23:00 min: -5.9C    max: -5.4C
>>> setunit f
>>> current Zelenograd
Current temp is 24.6F
>>> getunit
fahrenheit
>>> quit
'quit' command is unknown
Available commands:
        current %city%
        forecast %city% %days%
        setunit %units%
        getunit
        exit
>>> exit
```

### Полезные ссылки
* [Requests homepage](https://requests.readthedocs.io/en/latest/)
* [Про requests](https://realpython.com/api-integration-in-python/)
* [Туториал по requests](https://www.digitalocean.com/community/tutorials/how-to-get-started-with-the-requests-library-in-python-ru)
* [Datetime](https://docs.python.org/3/library/datetime.html)
* [Groupby](https://docs.python.org/3/library/itertools.html#itertools.groupby)
* [Pattern matching](https://habr.com/ru/company/yandex_praktikum/blog/547902/)