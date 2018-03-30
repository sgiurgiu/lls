Logitech Lights Script
======================

LLS is a JavaScript interpreter that provides a way for the supported Logitech keyboards' leds to be controlled via JavaScript.

Example:
```javascript
var keyboard = GetDefaultKeyboard();
if(keyboard != null) {
    //sets all keys to red
    keyboard.SetAllKeysColor(255,0,0);    
}
```


Building the application
* Prerequisites:
    * C++17 compatible compiler (gcc, clang)
    * cmake
    * hidapi-hidraw library
    * v8 library
* Get the code, configure and build    
```
    git clone lls
    cd lls
    mkdir build & cd build
    cmake ../
    make
    sudo make install
```
Run with `lls <path to script>`

