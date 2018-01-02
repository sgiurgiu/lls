#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <vector>
#include <set>
#include <array>
#include <string>
#include <stdexcept>
#include <tuple>


enum KeyboardModel : uint8_t {
        unknown = 0x00,
        g213,
        g410,
        g413,
        g610,
        g810,
        g910,
        gpro
};

enum class KeyAddressGroup : uint8_t {
        logo = 0x00,
        indicators,
        multimedia,
        gkeys,
        keys
};

enum class StartupMode : uint8_t {
         wave = 0x01,
         color
 };
 enum class NativeEffect : uint8_t {
         color = 0x01,
         breathing,
         cycle,
         hwave,
         vwave,
         cwave
 };
 enum class NativeEffectPart : uint8_t {
         all = 0xff,
         keys = 0x00,
         logo
 };
 enum class KeyGroup : uint8_t {
         logo = 0x00,
         indicators,
         multimedia,
         gkeys,
         fkeys,
         modifiers,
         functions,
         arrows,
         numeric,
         keys
 };
 enum class KeyCode : uint16_t { // 127 items

         logo = static_cast<uint8_t>(KeyAddressGroup::logo) << 8 | 0x01,
         logo2,

         backlight = static_cast<uint8_t>(KeyAddressGroup::indicators) << 8| 0x01,
         game, caps, scroll, num,

         next = static_cast<uint8_t>(KeyAddressGroup::multimedia) << 8 | 0xb5,
         prev, stop,
         play = static_cast<uint8_t>(KeyAddressGroup::multimedia) << 8 | 0xcd,
         mute = static_cast<uint8_t>(KeyAddressGroup::multimedia) << 8 | 0xe2,

         g1 = static_cast<uint8_t>(KeyAddressGroup::gkeys) << 8 | 0x01,
         g2, g3, g4, g5, g6, g7, g8, g9,

         a = static_cast<uint8_t>(KeyAddressGroup::keys) << 8 | 0x04,
         b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
         n1, n2, n3, n4, n5, n6, n7, n8, n9, n0,
         enter, esc, backspace, tab, space, minus, equal, open_bracket, close_bracket,
         backslash, dollar, semicolon, quote, tilde, comma, period, slash, caps_lock,
         f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
         print_screen, scroll_lock, pause_break, insert, home, page_up, del, end, page_down,
         arrow_right, arrow_left, arrow_bottom, arrow_top, num_lock, num_slash, num_asterisk,
         num_minus, num_plus, num_enter,
         num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9, num_0,
         num_dot, intl_backslash, menu,

         ctrl_left = static_cast<uint8_t>(KeyAddressGroup::keys) << 8 | 0xe0,
         shift_left, alt_left, win_left,
         ctrl_right, shift_right, alt_right, win_right

};
class KeyboardError : public std::runtime_error
{
public:
    KeyboardError(const char* msg):std::runtime_error(msg){}
    KeyboardError(const std::string& msg):std::runtime_error(msg){}
};
struct Key {
    Key(){}
    Key(KeyCode code,const std::string& name,uint32_t color):
    code(code),name(name),color(color){}
    KeyCode code;
    std::string name;
    uint32_t color = 0;
};
typedef std::vector<Key> Keys;
class Keyboard
{
public:
    Keyboard(unsigned short vendorId,unsigned short productId,
             const wchar_t* serialNumber, int interfaceNumber,
             const wchar_t* manufacturer, const wchar_t* product);
    ~Keyboard();
    Keyboard& operator=(Keyboard&& other);
    Keyboard(Keyboard&& other);
    Keyboard& operator=(const Keyboard& other) = delete;
    Keyboard(const Keyboard& other) = delete;
    static bool LoadAvailableKeyboards();
    static void DisposeKeyboards();
    static const Keyboard* const GetDefaultKeyboard();
    struct KeyboardLess
    {
      bool operator()(const Keyboard& k1, const Keyboard& k2)
      {
          return std::tie(k1.vendorId,k1.productId) < std::tie(k2.vendorId,k2.productId);
      }
    };
    static const std::set<Keyboard,KeyboardLess>* const GetKeyboards();
    
    void Product(const std::wstring&);
    std::wstring Product() const;
    void Manufacturer(const std::wstring&);
    std::wstring Manufacturer() const;
    unsigned short VendorId() const;
    unsigned short ProductId() const;
    std::wstring SerialNumber() const;
    std::wstring ToString() const;
    bool SetAllKeys(uint32_t color) const;
    bool Commit() const;
    Keys GetAllKeys() const;
private:
    typedef std::vector<unsigned char> byte_buffer_t;
    bool SetKeys(const Keys& keys) const;
    int Write(byte_buffer_t &data) const;
    byte_buffer_t getKeyGroupAddress(KeyAddressGroup keyAddressGroup) const;
    void WriteGroup(const Keys& keys,const byte_buffer_t& keyGroupAddress,size_t dataSize) const;
private:
    static std::vector<std::array<uint16_t,3>> SupportedKeyboards;
    static std::set<Keyboard,KeyboardLess> keyboards;
    
    const Keys keyGroupLogo = { {KeyCode::logo,"logo", 0}, {KeyCode::logo2,"logo2", 0 } };
    const Keys keyGroupIndicators = { { KeyCode::caps, "caps", 0 }, {KeyCode::num, "num", 0} , 
        {KeyCode::scroll, "scroll", 0 }, {KeyCode::game, "game", 0}, {KeyCode::backlight, "backlight" ,0} };
    
    const Keys keyGroupMultimedia = { {KeyCode::next, "next", 0}, {KeyCode::prev, "prev", 0} , 
        {KeyCode::stop, "stop", 0}, {KeyCode::play, "play", 0}, {KeyCode::mute,"mute",0} };
    const Keys keyGroupGKeys = { { KeyCode::g1, "g1", 0}, {KeyCode::g2, "g2", 0}, {KeyCode::g3,"g3",0} , 
        { KeyCode::g4, "g4",0}, {KeyCode::g5, "g5",0 }, {KeyCode::g6, "g6", 0}, {KeyCode::g7, "g7", 0}, 
        { KeyCode::g8, "g8" , 0} , {KeyCode::g9, "g9",0} };
    const Keys keyGroupFKeys = {
        {KeyCode::f1, "f1", 0}, {KeyCode::f2, "f2", 0}, {KeyCode::f3, "f3", 0}, {KeyCode::f4, "f4", 0}, 
        {KeyCode::f5, "f5", 0}, {KeyCode::f6, "f6", 0}, {KeyCode::f7, "f7", 0}, {KeyCode::f8, "f8", 0}, 
        {KeyCode::f9, "f9", 0}, {KeyCode::f10, "f10", 0}, {KeyCode::f11, "f11", 0}, {KeyCode::f12, "f12", 0}
    };
    const Keys keyGroupModifiers = {
        {KeyCode::shift_left, "shift left", 0}, {KeyCode::ctrl_left, "ctrl left", 0}, 
        {KeyCode::win_left, "win left", 0}, {KeyCode::alt_left, "alt left", 0}, {KeyCode::alt_right, "alt right", 0}, 
        {KeyCode::win_right, "win right", 0}, {KeyCode::ctrl_right, "ctrl right", 0}, {KeyCode::shift_right, "shift right", 0}, 
        {KeyCode::menu, "menu", 0} };
    const Keys keyGroupFunctions = {
        {KeyCode::esc, "esc", 0}, {KeyCode::print_screen, "print screen", 0}, {KeyCode::scroll_lock, "scroll lock", 0}, 
        {KeyCode::pause_break, "pause break", 0}, {KeyCode::insert, "insert", 0}, {KeyCode::del, "del", 0}, 
        {KeyCode::home, "home", 0}, {KeyCode::end, "end", 0}, {KeyCode::page_up, "page up", 0}, {KeyCode::page_down, "page down", 0}
    };
    const Keys keyGroupArrows = { 
        {KeyCode::arrow_top, "arrow top", 0}, {KeyCode::arrow_left, "arrow left", 0}, 
        {KeyCode::arrow_bottom, "arrow bottom", 0}, {KeyCode::arrow_right, "arrow right", 0} };
    const Keys keyGroupNumeric = {
        {KeyCode::num_1, "num 1", 0}, {KeyCode::num_2, "num 2", 0}, {KeyCode::num_3, "num 3", 0}, 
        {KeyCode::num_4, "num 4", 0}, {KeyCode::num_5, "num 5", 0}, {KeyCode::num_6, "num 6", 0}, 
        {KeyCode::num_7, "num 7", 0}, {KeyCode::num_8, "num 8", 0}, {KeyCode::num_9, "num 0", 0}, 
        {KeyCode::num_0, "num 0", 0}, {KeyCode::num_dot, "num .", 0}, {KeyCode::num_enter, "num enter", 0}, 
        {KeyCode::num_plus, "num +", 0}, {KeyCode::num_minus, "num -", 0},{KeyCode::num_asterisk, "num *", 0}, 
        {KeyCode::num_slash, "num /", 0}, {KeyCode::num_lock, "num lock", 0}
    };
    const Keys keyGroupKeys = {
        {KeyCode::a, "a", 0}, {KeyCode::b, "b", 0}, {KeyCode::c, "c", 0}, {KeyCode::d, "d", 0}, 
        {KeyCode::e, "e", 0}, {KeyCode::f, "f", 0}, {KeyCode::g, "g", 0}, {KeyCode::h, "h", 0}, 
        {KeyCode::i, "i", 0}, {KeyCode::j, "j", 0}, {KeyCode::k, "k", 0}, {KeyCode::l, "l", 0}, 
        {KeyCode::m, "m", 0}, {KeyCode::n, "n", 0}, {KeyCode::o, "o", 0}, {KeyCode::p, "p", 0}, 
        {KeyCode::q, "q", 0}, {KeyCode::r, "r", 0}, {KeyCode::s, "s", 0}, {KeyCode::t, "t", 0}, 
        {KeyCode::u, "u", 0}, {KeyCode::v, "v", 0}, {KeyCode::w, "w", 0}, {KeyCode::x, "x", 0}, 
        {KeyCode::y, "y", 0}, {KeyCode::z, "z", 0}, {KeyCode::n1, "1", 0}, {KeyCode::n2, "2", 0}, 
        {KeyCode::n3, "3", 0}, {KeyCode::n4, "4", 0}, {KeyCode::n5, "5", 0}, {KeyCode::n6, "6", 0}, 
        {KeyCode::n7, "7", 0}, {KeyCode::n8, "8", 0}, {KeyCode::n9, "9", 0}, {KeyCode::n0, "0", 0},
        {KeyCode::enter, "enter", 0}, {KeyCode::backspace, "backspace", 0}, {KeyCode::tab, "tab", 0}, 
        {KeyCode::space, "space", 0}, {KeyCode::minus, "-", 0}, {KeyCode::equal, "=", 0},
        {KeyCode::open_bracket, "[", 0}, {KeyCode::close_bracket, "]", 0}, {KeyCode::backslash, "\\", 0}, 
        {KeyCode::dollar, "$", 0}, {KeyCode::semicolon, ";", 0}, {KeyCode::quote, "'", 0}, {KeyCode::tilde, "~", 0},
        {KeyCode::comma, ",", 0}, {KeyCode::period, ".", 0}, {KeyCode::slash, "/", 0}, {KeyCode::caps_lock, "caps", 0}, 
        {KeyCode::intl_backslash, "<", 0}
    };
    
    unsigned short vendorId;
    unsigned short productId;
    int interfaceNumber;
    std::wstring serialNumber;
    std::wstring manufacturer;
    std::wstring product;
    KeyboardModel model;
    void* hid_handle;
};

#endif // KEYBOARD_H
