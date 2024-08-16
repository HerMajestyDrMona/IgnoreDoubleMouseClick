# IgnoreDoubleMouseClick
A program allowing you to programmatically workaround the double mouse clicking issue when your mouse switch is damaged on Windows.

## [Download Binary (IgnoreDoubleMouseClick.exe)](https://github.com/HerMajestyDrMona/IgnoreDoubleMouseClick/releases/)
(IgnoreDoubleMouseClick.exe is a 64bit static executable)

## [View the source code](https://github.com/HerMajestyDrMona/IgnoreDoubleMouseClick/blob/main/IgnoreDoubleMouseClick/IgnoreDoubleMouseClick.cpp)

## What if it doesn't work?
If the program does not work for you, please try to run it as administrator. This way Hooking the `WH_MOUSE_LL` should success.

## What options are available in Ignore Double Mouse Click?
It's possible to configure the program by creating a configuration file at the same location as `IgnoreDoubleMouseClick.exe`. The configuration file should be named: `IgnoreDoubleMouseClick.txt`.

You can include the following variables to your `IgnoreDoubleMouseClick.txt` file:

- Show Console Window?
  - 0 = Don't show the program window with the output and run it in the background.
  - 1 = Show the program console window.
```
ShowConsoleWindow=0
```

- After how many milliseconds the next mouse click should be allowed?
```
IgnoreNextMouseClickTimeMilliseconds=80
```

- Monitor Left Mouse Click?
  - 0 = No, don't monitor the Left Mouse Button clicks.
  - 1 = Yes, prevent too fast double mouse clicking of the Left Mouse Button.
```
MonitorLeftMouseClick=1
```

- Monitor Right Mouse Click?
  - 0 = No, don't monitor the Right Mouse Button clicks.
  - 1 = Yes, prevent too fast double mouse clicking of the Right Mouse Button.

```
MonitorRightMouseClick=1
```

- Should the program run automatically on system startup? 
  - 0 = No
  - 1 = Yes
```
AutomaticallyRunThisProgramOnStartup=0
```
