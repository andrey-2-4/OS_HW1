# OS_HW1
# ОС ИДЗ №1 ВАРИАНТ №30
БПИ216 Дюгай Андрей Владимирович

30. Разработать программу, которая ищет в ASCII-строке слова, начинающиеся с заглавной буквы и формирует из них новую
строку, в которой слова разделяются пробелами. Слова состоят из
букв. Все остальные символы являются разделителями слов.

В каждой программе для записи и чтения из файлов используются read и write, размеры буфферов для записи/чтения 5000 байт, задание имен входного и выходного файла происходит через командную строку (./a.out inputi outputi)

markX - папка с прогрммой на языке C на X баллов (markX.c), в папке помимо файла программы есть 5 файлов с вводом (inputi) и 5 с выводом (outputi), а также исполняем файл (a.out)

В папке на 8 баллов вместо mark8 есть 2 файла (т.к. этого требует задание на 8 баллов):
mark8_file_manager.c (a.out) - программа 1, которая читает из файла на вход, передает программе 2, получает от программы 2, пишет в файл на выход
mark8_handler.c (b.out) - программа 2, которая получает от программы 1, обрабатывает, передает программе 1

Общице процессы и связи между ними:
![photo_5465245773223545284_y](https://user-images.githubusercontent.com/61345502/221410298-4548095c-fe05-411d-86a2-dae8a9aedac0.jpg)
В программах на 4 и 5 баллов первый процесс читатет из файла input, передает процессу 2, который обрабатыает данные, передает обработанные данные процессу 3, который уже записывает обработанные данные в файл output. Отличие лишь в том, что в программе на 4 баллов используются неименованные каналы, а в программе на 5 баллов используются именованные.

В программах на 6 и 7 баллов происходит то же, что и в программах на 4 и 5 баллов кроме того, что данные от процесса 2 не идут к процессу 3, а идут к процессу 1, который уже записывает обработанные данный в файл. Аналогично в программе на 6 баллов используются неименованные каналы, а в программе на 7 баллов используются именованные.

Программа на 8 баллов работает аналогично программе на 7 баллов, но процессы уже НЕродственные.

Ожидаемая оценка 8 :D
