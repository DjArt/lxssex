typedef void* DisplayHandle;

DisplayHandle CreateWindow();
bool GetWindowSize(DisplayHandle display, int* x, int* y);
bool SetWindowSize(DisplayHandle display, int x, int y);
bool SetWindowTitle(DisplayHandle display, char* string);
bool SetIcon(DisplayHandle display, void* buffer, int size);
bool CloseWindow(DisplayHandle display);
