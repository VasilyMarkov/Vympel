#### Настройка Raspberry PI OS
1. Обновить пакеты
```bash
sudo apt update && apt upgrade
```
2. Включить SSH

#### Установка всех зависимостей
```bash
sudo apt install cmake
```

```bash
sudo apt install qtbase6-dev
sudo apt install qt6-connectivity-dev
```

```bash
sudo apt-get install -y libopencv-dev
```
Проект камеры libcamera:
```
https://github.com/kbarni/LCCV
```
Filters repo:
```
https://github.com/berndporr/iir1
```

#### Сборка и запуск
```bash
./bld
./run
```
#### Сборка и запуск тестовых данных
1.В файле app.cpp

```C++
core_ = std::make_unique<Core>(std::make_shared<app::CameraProcessingModule>());
```
Поменять на:
```C++
core_ = std::make_unique<Core>(std::make_shared<app::TestProcessUnit>());
```

2. В `bld` скрипте поменять `release` на `not_ble`. 
3. В `conf/config.json` в поле `"files":"videoFile"` прописать путь к видео конденсации и испарения.
4. Собрать и запустить проект.


