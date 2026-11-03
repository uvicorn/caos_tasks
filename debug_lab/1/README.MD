# Auth


Запуск:
```
make all
./authent
```

Запускать ваш патч в `overload.c`
```
make overload
LD_PRELOAD=./overload.so ./authent
```

Вам должно вывести "Authentication successful". Патчить код запрещено, пишем только в overload.c