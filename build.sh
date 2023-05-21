uic src/app.ui -o ui_app.h
cmake -G "MinGW Makefiles" -S . -B build
cmake --build build
