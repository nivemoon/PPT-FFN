## Download

Latest binaries: [Releases](https://github.com/nivemoon/PPT-FFN/releases/latest)

# PPT-FFN
Global push-to-talk for default microphone on Windows (C, WinAPI).

(ru) «Утилита для PTT по боковым кнопкам мыши»
## Implementation details

- Language: **C** (plain C, without C++).
- Platform: Windows 10/11.
- APIs:
  - WinAPI (`windows.h`)
  - MMDevice API (`mmdeviceapi.h`)
  - `IAudioEndpointVolume` (`endpointvolume.h`) for mic mute/unmute
- Toolchain: **MinGW‑w64 gcc** (tested with x86_64, ucrt, posix, seh).

### Components

- `micctl.exe`  
  Small CLI tool that mutes or unmutes the default communications
  capture device using `IAudioEndpointVolume_SetMute`.

- `pttffn.exe`  
  Tray application with a low-level mouse hook (`WH_MOUSE_LL`) that:
  - listens for mouse XButton1/XButton2 press/release,
  - calls `micctl.exe 0` on press (unmute),
  - calls `micctl.exe 1` on release (mute),
  - runs as a background tray app (no taskbar / console window).

No Python, AutoHotkey or other scripting engines are required.
Everything is native C code compiled to standalone executables.

====================================================================

PTT FFN написан на чистом C с использованием Windows API и компилируется через MinGW‑w64. 
Никаких интерпретаторов (Python, AHK и т.п.) не требуется, 
только micctl.exe и pttffn.exe должны присутствовать в одной папке и управлять дефолтным микрофоном через системный COM‑интерфейс.

PTT FFN — Push-to-Talk (режим рации) для микрофона 
Набор из двух утилит для Windows:
	micctl.exe — включает/выключает микрофон на системном уровне.
	pttffn.exe — push-to-talk через боковые кнопки мыши, с иконкой в трее.
Работает глобально для всего: игр, Discord, браузера и т.д.

= Установка =
Распакуйте архив в любую папку, например:
C:\Tools\PTT_FFN

Убедитесь, что в папке лежат файлы:
	micctl.exe
	pttffn.exe
	pttffn.ico (если есть, отвечает за иконку в трее)

= Настройка микрофона =

1.	Откройте классическое окно звука:
Win + R → mmsys.cpl → Enter.
2.	Вкладка Запись.
3.	Найдите ваш микрофон, который хотите использовать для PTT 
4.	Нажмите ПКМ →	Использовать устройство связи по умолчанию
(зелёная иконка телефонной трубки).
Утилита micctl.exe управляет именно устройством связи по умолчанию.

= Запуск PTT =

1.	Запустите pttffn.exe двойным кликом.
Программа не имеет окна, только иконка в системном трее.
2. Управление из трея
Правый клик по иконке:
	PTT Button 4 
	(верхняя боковая кнопка мыши)
	PTT Button 5 
	(нижняя боковая кнопка мыши)
	Exit
	(выйти из программы)
Подсказка (tooltip) у иконки показывает, какая кнопка сейчас активна. 
По умолчанию указана клавиша XButton2 (lower) в качестве примера.

= Как работает PTT =
При зажатии выбранной боковой кнопки:
	разовое срабатывание micctl.exe → микрофон выключен
	удержание: pttffn.exe вызывает micctl.exe 0 → микрофон включён (unmute)
	отпускание: срабатвает micctl.exe 1 → микрофон выключен (mute)
Проверить можно так:
1.	В mmsys.cpl → Запись выберите микрофон → Свойства → вкладка Уровни.
2.	Держите это окно открытым.
3.	Зажмите боковую кнопку:
	иконка микрофона перестаёт быть перечёркнутой → звук включён.
4.	Отпустите:
	иконка снова перечёркивается → микрофон выключен.

= Заметки =
	Утилита не трогает клавиатуру/мышь в играх, не генерирует клик/нажатия — только переключает mute микрофона через системный API.
	Работает на Windows 10/11, где доступны mmdeviceapi и IAudioEndpointVolume.
	Если микрофон не переключается:
	проверьте, что выбран именно тот микрофон как устройство связи по умолчанию,
	проверьте, что никакой другой софт (Clownfish и т.п.) не перехватывает вход.

ПРИМЕЧАНИЕ: иногда производитель мышек часто именует боковые клавиши в обратном порядке. 
Возможно ваша кнопка 4, это наша кнопка 5. Пробуйте конкретно для себя.

= Удаление =
Чтобы всё отключить:
1.	Правый клик по иконке PTT FFN в трее → Exit.
2.	Закройте все программы, использующие микрофон.
3.	Удалите папку с micctl.exe и pttffn.exe, если больше не нужен PTT.
⁂

	Fifine A6 Neo в качестве иконки. 
1.	https://www.nirsoft.net/articles/mute_microphone_command_line.html   
2.	https://www.nirsoft.net/utils/sound_volume_view.html   
3.	https://www.tenforums.com/tutorials/128343-enable-disable-sound-output-device-windows.html    
4.	https://www.youtube.com/watch?v=kj-0UsbKi00 
5.	https://www.dell.com/support/contents/ru-az/article/product-support/self-support-knowledgebase/audio-and-speakers/microphone  
6.	https://sci.rambler.ru/kompyutery/54623719-chto-delat-esli-ne-rabotaet-mikrofon-na-windows-10-ili-11-poshagovaya-instruktsiya/  
