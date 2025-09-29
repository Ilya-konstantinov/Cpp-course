## Установка Clang++-18
На Линуксе слишком много пакетных менеджеров, чтобы расписывать каждый
Приведу пример установки на Fedora
```
sudo dnf install clang18
```
Проверьте установку командой `clang++-18 --version`
Все окей если у вас что-то такое первой строчкой:
`Homebrew clang version 18.1.8`
! Важно чтобы бинарник был доступен по команде `clang++-18`

## Настройка VSCode
#### 1. Установите эти расширение в VSCode

![[.img/c_c++_extension.png]]
![[.img/codelldb.png]]

#### 2. Создайте папку .vscode в вашей рабочей директории
#### 3. Создайте в этой папке tasks.json и вставьте конфиг снизу
```
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: clang++ build active file",
            "command": "clang++-18",
            "args": [
                "-fcolor-diagnostics",
                "-fansi-escape-codes",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}" // тут вы можете изменять имя выходного файла
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ],
    "version": "2.0.0"
}
```
#### 4. Создайте в той же папке launch.json и вставьте конфиг снизу
```
{
    "version": "0.2.0",
    "configurations": [
      {
        "type": "lldb",
        "request": "launch",
        "name": "Launch",
        "program": "${fileDirname}/${fileBasenameNoExtension}",
        "args": [],
        "cwd": "${workspaceFolder}"
      }
    ]
}
```
#### 5. Запускайте ваш код по кнопке f5, ввод и вывод идет в терминале который появится при запуске