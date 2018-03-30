Logitech Lights Script
======================

LLS is a JavaScript interpreter that provides an API for Logitech keyboards' leds to be controlled via scripts.

Example:
```javascript
var keyboard = GetDefaultKeyboard();
if(keyboard != null) {
    //sets all keys to red
    keyboard.SetAllKeysColor(255,0,0);    
}
```
Will have this effect:
[![Set all keys to red](https://img.youtube.com/vi/cRtkRXbmhZg/0.jpg)](https://www.youtube.com/watch?v=cRtkRXbmhZg) 

While something like:
```javascript
var keyboard = GetDefaultKeyboard();
if(keyboard != null)
{
    for (var i=0;i<20;i++)
    {
        keyboard.SetAllKeysColor(i*10,i*10,i*10);    
        sleep(100);
    }    
}
```
Will have this effect:
[![Set all keys to red](https://img.youtube.com/vi/ClRO3pC96i8/0.jpg)](https://www.youtube.com/watch?v=ClRO3pC96i8) 

### Available APIs

```javascript
//Returns all the supported keyboards found (as an array)
var keyboards = GetKeyboards();

// Returns the first supported keyboard found or null if none
var keyboard = GetDefaultKeyboard() 

//The Keyboard object
//Returns a string representation of the keyboard, for example
//Manufacturer: Logitech, Product: Gaming Keyboard G810, vendor ID:1133, product ID: 49975, interface: 0
var str = keyboard.ToString(); 
//Set all keys to the specified RGB color
//r,g and b are integers between 0 and 255 or 0x00 and 0xFF
keyboard.SetAllKeysColor(r,g,b);
//Returns an array of keys of the keyboard. Each key object is of the format:
//{"name":name,"code":code,"color":color}
var keys = keyboard.GetKeys(); 

//Sets the specified keys to the specified color.
keyboard.UpdateKeys(keys,r,g,b);

//Sets the specified key to the specified color.
keyboard.UpdateKey(key,r,g,b);
```

In addition to the keyboard related APIs, there are a few more utility APIs available:
```javascript
//returns the current LLS version, e.g. '0.1'
version();
//prints to stdout the provided message
log(msg);
//includes the specified script
require(script);
include(script);

//sleeps for the specified number of milliseconds
sleep(ms);


```

### Building the application
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

