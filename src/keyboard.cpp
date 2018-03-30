#include "keyboard.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <hidapi/hidapi.h>

std::vector<std::array<uint16_t,3>> Keyboard::SupportedKeyboards = {
        { 0x46d, 0xc336, (u_int16_t)KeyboardModel::g213 },
        { 0x46d, 0xc330, (u_int16_t)KeyboardModel::g410 },
        { 0x46d, 0xc33a, (u_int16_t)KeyboardModel::g413 },
        { 0x46d, 0xc333, (u_int16_t)KeyboardModel::g610 },
        { 0x46d, 0xc338, (u_int16_t)KeyboardModel::g610 },
        { 0x46d, 0xc331, (u_int16_t)KeyboardModel::g810 },
        { 0x46d, 0xc337, (u_int16_t)KeyboardModel::g810 },
        { 0x46d, 0xc32b, (u_int16_t)KeyboardModel::g910 },
        { 0x46d, 0xc335, (u_int16_t)KeyboardModel::g910 },
        { 0x46d, 0xc339, (u_int16_t)KeyboardModel::gpro }
};

std::set<Keyboard,Keyboard::KeyboardLess> Keyboard::keyboards;

Keyboard::Keyboard(unsigned short vendorId,unsigned short productId,
                   const wchar_t* serialNumber, int interfaceNumber,
                   const wchar_t* manufacturer, const wchar_t* product):
    vendorId(vendorId),productId(productId),interfaceNumber(interfaceNumber),hid_handle(nullptr)
{
    std::cout << "Keyboard constructed " << vendorId <<":"<<productId <<std::endl;
    
    hid_handle = (void*)hid_open(vendorId,productId,serialNumber);
    if(!hid_handle)
    {
        throw KeyboardError("Are udev permissions correct? Unable to open keyboard "+std::to_string(vendorId)+":"+std::to_string(productId));
    }

    if(serialNumber != nullptr) this->serialNumber = std::wstring(serialNumber);
    if(manufacturer != nullptr) this->manufacturer = std::wstring(manufacturer);
    if(product != nullptr) this->product = std::wstring(product);
    
    for (const auto& supportedKeyboard : SupportedKeyboards) 
    {
        if (vendorId == supportedKeyboard[0] && 
                productId == supportedKeyboard[1]) 
        {
            model = (KeyboardModel)supportedKeyboard[2];
            break;
        }
    }
}
Keyboard::~Keyboard()
{
    if(hid_handle)
    {
        hid_close((hid_device*)hid_handle);
        hid_handle = nullptr;
    }
}
Keyboard & Keyboard::operator=(Keyboard && other)
{
    hid_handle = other.hid_handle;
    vendorId = std::move(other.vendorId);
    productId = std::move(other.productId);
    interfaceNumber = std::move(other.interfaceNumber);
    serialNumber = std::move(other.serialNumber);
    manufacturer = std::move(other.manufacturer);
    product = std::move(other.product);
    model = std::move(other.model);
    other.hid_handle = nullptr;
    return *this;
}

Keyboard::Keyboard(Keyboard && other):vendorId(std::move(other.vendorId)),productId(std::move(other.productId)),
    interfaceNumber(std::move(other.interfaceNumber)),serialNumber(std::move(other.serialNumber)),
    manufacturer(std::move(other.manufacturer)),product(std::move(other.product)),model(std::move(other.model)),
    hid_handle(other.hid_handle)
{
    other.hid_handle = nullptr;
}

void Keyboard::DisposeKeyboards()
{
    keyboards.clear();
    hid_exit();
}
void Keyboard::Product(const std::wstring& product)
{
    this->product = product;
}
std::wstring Keyboard::Product() const
{
    return product;
}
void Keyboard::Manufacturer(const std::wstring& manufacturer)
{
    this->manufacturer = manufacturer;
}
std::wstring Keyboard::Manufacturer() const
{
    return manufacturer;
}
unsigned short Keyboard::VendorId() const
{
    return vendorId;
}
unsigned short Keyboard::ProductId() const
{
    return productId;
}
std::wstring Keyboard::SerialNumber() const
{
    return serialNumber;
}
std::wstring  Keyboard::ToString() const
{
    return L"Manufacturer: "+manufacturer+L", Product: "+product+L", vendor ID:"
            +std::to_wstring(vendorId)+L", product ID: "+std::to_wstring(productId)
            +L", interface: "+std::to_wstring(interfaceNumber);
}
bool Keyboard::SetKeys(const Keys& keys) const
{
    if (keys.empty()) return false;
    bool retval = true;

    Keys logoGroup,indicatorsGroup,multimediaGroup,gkeysGroup,keysGroup;

    for (const auto& key : keys) {
        switch(static_cast<KeyAddressGroup>(static_cast<uint16_t>(key.code) >> 8 )) {
        case KeyAddressGroup::logo:
            switch (model) 
            {
                case KeyboardModel::g610:
                case KeyboardModel::g810:
                case KeyboardModel::gpro:
                    if (logoGroup.size() <= 1 && key.code == KeyCode::logo)
                        logoGroup.push_back(key);
                    break;
                case KeyboardModel::g910:
                    if (logoGroup.size() <= 2) logoGroup.push_back(key);
                    break;
                default:
                        break;
            }
            break;
        case KeyAddressGroup::indicators:
            if (indicatorsGroup.size() <= 5) indicatorsGroup.push_back(key);
            break;
        case KeyAddressGroup::multimedia:
            switch (model) 
            {
                case KeyboardModel::g610:
                case KeyboardModel::g810:
                case KeyboardModel::gpro:
                    if (multimediaGroup.size() <= 5) multimediaGroup.push_back(key);
                    break;
                default:
                        break;
            }
            break;
        case KeyAddressGroup::gkeys:
            switch (model) 
            {
                case KeyboardModel::g910:
                    if (gkeysGroup.size() <= 9) gkeysGroup.push_back(key);
                    break;
                default:
                    break;
            }
            break;
        case KeyAddressGroup::keys:
            switch (model) 
            {
                case KeyboardModel::g610:
                case KeyboardModel::g810:
                case KeyboardModel::g910:
                case KeyboardModel::gpro:
                    if (keysGroup.size() <= 120) keysGroup.push_back(key);
                    break;
                case KeyboardModel::g410:
                    if (keysGroup.size() <= 120 && 
                        (
                            key.code < KeyCode::num_lock || key.code > KeyCode::num_dot)
                        )
                    {
                        keysGroup.push_back(key);
                    }
                    break;
                default:
                        break;
            }
            break;
        }
    }
    
    WriteGroup(logoGroup,getKeyGroupAddress(KeyAddressGroup::logo),20);
    WriteGroup(indicatorsGroup,getKeyGroupAddress(KeyAddressGroup::indicators),64);
    WriteGroup(multimediaGroup,getKeyGroupAddress(KeyAddressGroup::multimedia),64);
    WriteGroup(gkeysGroup,getKeyGroupAddress(KeyAddressGroup::gkeys),64);
    WriteGroup(keysGroup,getKeyGroupAddress(KeyAddressGroup::keys),64);
    
    return retval;
}

void Keyboard::WriteGroup(const Keys& keys,const byte_buffer_t& keyGroupAddress,size_t dataSize) const
{
    const uint8_t maxKeyCount = (dataSize - 8) / 4;
    uint8_t gi = 0;
    while (gi < keys.size()) 
    {
        byte_buffer_t data = keyGroupAddress;
        if (data.size() > 0) 
        {
            for (uint8_t i = 0; i < maxKeyCount; i++) 
            {
                if (gi + i < keys.size()) 
                {
                    data.push_back(static_cast<uint8_t>(static_cast<uint16_t>(keys[gi+i].code) & 0x00ff));
                    data.push_back((keys[gi+i].color & 0xff0000) >> 16);
                    data.push_back((keys[gi+i].color & 0xff00) >> 8 );
                    data.push_back((keys[gi+i].color & 0xff));
                }
            }

            data.resize(dataSize, 0x00);
            Write(data);
        }
        gi += maxKeyCount;
    }
}


bool Keyboard::Commit() const
{
    byte_buffer_t data;
    switch (model) {
            case KeyboardModel::g213:
            case KeyboardModel::g413:
                    return true; // Keyboard is non-transactional
            case KeyboardModel::g410:
            case KeyboardModel::g610:
            case KeyboardModel::g810:
            case KeyboardModel::gpro:
                    data = { 0x11, 0xff, 0x0c, 0x5a };
                    break;
            case KeyboardModel::g910:
                    data = { 0x11, 0xff, 0x0f, 0x5d };
                    break;
            default:
                    return false;
    }
    data.resize(20, 0x00);
    return Write(data);
}

int Keyboard::Write(byte_buffer_t &data) const
{
    if(data.empty()) return -1;

    Utils::Sleep(1); // needs 1ms sleep otherwise data gets all mangled in memory
    data.insert(data.begin(),0x00);
    int totalWritten = 0;
    int written = -1;
    unsigned char* dataPtr = data.data();
    size_t size = data.size() ;
    while (totalWritten < size && 
        (written = hid_write((hid_device*)hid_handle, dataPtr + totalWritten, size - totalWritten)) > 0) 
    {
        totalWritten += written;
    }
    return totalWritten;
}
Keyboard::byte_buffer_t Keyboard::getKeyGroupAddress(KeyAddressGroup keyAddressGroup) const
{
    switch (model) {
    case KeyboardModel::g213:
    case KeyboardModel::g413:
        break; // Device doesn't support per-key setting
    case KeyboardModel::g410:
    case KeyboardModel::g610:
    case KeyboardModel::g810:
    case KeyboardModel::gpro:
        switch (keyAddressGroup) {
        case KeyAddressGroup::logo:
            return { 0x11, 0xff, 0x0c, 0x3a, 0x00, 0x10, 0x00, 0x01 };
        case KeyAddressGroup::indicators:
            return { 0x12, 0xff, 0x0c, 0x3a, 0x00, 0x40, 0x00, 0x05 };
        case KeyAddressGroup::gkeys:
            return {};
        case KeyAddressGroup::multimedia:
            return { 0x12, 0xff, 0x0c, 0x3a, 0x00, 0x02, 0x00, 0x05 };
        case KeyAddressGroup::keys:
            return { 0x12, 0xff, 0x0c, 0x3a, 0x00, 0x01, 0x00, 0x0e };
        }
        break;
    case KeyboardModel::g910:
        switch (keyAddressGroup) {
        case KeyAddressGroup::logo:
            return { 0x11, 0xff, 0x0f, 0x3a, 0x00, 0x10, 0x00, 0x02 };
        case KeyAddressGroup::indicators:
            return { 0x12, 0xff, 0x0c, 0x3a, 0x00, 0x40, 0x00, 0x05 };
        case KeyAddressGroup::gkeys:
            return { 0x12, 0xff, 0x0f, 0x3e, 0x00, 0x04, 0x00, 0x09 };
        case KeyAddressGroup::multimedia:
            return {};
        case KeyAddressGroup::keys:
            return { 0x12, 0xff, 0x0f, 0x3d, 0x00, 0x01, 0x00, 0x0e };
        }
        break;
    default:
        break;
    }
    return {};
}


bool Keyboard::SetAllKeys(uint32_t color) const
{
    auto makeKeyValue = [&color](Key key) -> Key {
        return {key.code, key.name, color};
    };
    Keys keys;
    switch (model) {
    case KeyboardModel::g213:
        //for (uint8_t rIndex=0x01; rIndex <= 0x05; rIndex++) if (! setRegion(rIndex, color)) return false;
        return true;
    case KeyboardModel::g413:
        //setNativeEffect(NativeEffect::color, NativeEffectPart::keys, 0, color);
        return true;
    case KeyboardModel::g410:
    case KeyboardModel::g610:
    case KeyboardModel::g810:
    case KeyboardModel::g910:
    case KeyboardModel::gpro:
        std::transform(keyGroupLogo.begin(),keyGroupLogo.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupIndicators.begin(),keyGroupIndicators.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupMultimedia.begin(),keyGroupMultimedia.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupGKeys.begin(),keyGroupGKeys.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupFKeys.begin(),keyGroupFKeys.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupFunctions.begin(),keyGroupFunctions.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupArrows.begin(),keyGroupArrows.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupNumeric.begin(),keyGroupNumeric.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupModifiers.begin(),keyGroupModifiers.end(),std::back_inserter(keys),makeKeyValue);
        std::transform(keyGroupKeys.begin(),keyGroupKeys.end(),std::back_inserter(keys),makeKeyValue);
        return SetKeys(keys);
    default:
        return false;
    }
}

Keys Keyboard::GetAllKeys() const
{
    Keys allKeys;
    std::copy(keyGroupLogo.begin(),keyGroupLogo.end(),std::back_inserter(allKeys));
    std::copy(keyGroupIndicators.begin(),keyGroupIndicators.end(),std::back_inserter(allKeys));
    std::copy(keyGroupMultimedia.begin(),keyGroupMultimedia.end(),std::back_inserter(allKeys));
    std::copy(keyGroupGKeys.begin(),keyGroupGKeys.end(),std::back_inserter(allKeys));
    std::copy(keyGroupFKeys.begin(),keyGroupFKeys.end(),std::back_inserter(allKeys));
    std::copy(keyGroupFunctions.begin(),keyGroupFunctions.end(),std::back_inserter(allKeys));
    std::copy(keyGroupArrows.begin(),keyGroupArrows.end(),std::back_inserter(allKeys));
    std::copy(keyGroupNumeric.begin(),keyGroupNumeric.end(),std::back_inserter(allKeys));
    std::copy(keyGroupModifiers.begin(),keyGroupModifiers.end(),std::back_inserter(allKeys));
    std::copy(keyGroupKeys.begin(),keyGroupKeys.end(),std::back_inserter(allKeys));
    return allKeys;
}

KeysMatrix Keyboard::GetKeysMatrix() const
{
    KeysMatrix matrix;
    Key none = {KeyCode::none,"",0};
    
    switch(model)
    {
        case KeyboardModel::g810:
        {
            Keys row1;
            row1.push_back({KeyCode::logo,"logo", 0});
            for(int i=0;i<9;i++) row1.push_back(none);
            row1.push_back({KeyCode::num,"num", 0});
            row1.push_back({KeyCode::caps,"caps", 0});
            row1.push_back({KeyCode::scroll,"scroll", 0});
            row1.push_back(none);
            row1.push_back({KeyCode::game,"game", 0});
            row1.push_back(none);
            row1.push_back({KeyCode::backlight,"backlight", 0});
            row1.push_back({KeyCode::mute,"mute", 0});
            for(int i=0;i<3;i++) row1.push_back(none);
            matrix.push_back(row1);
            Keys row2;
            row2.push_back({KeyCode::esc,"esc", 0});
            row2.push_back(none);
            row2.insert(row2.end(),keyGroupFKeys.begin(),keyGroupFKeys.end());
            row2.insert(row2.end(),keyGroupFunctions.begin()+1,keyGroupFunctions.begin()+4);
            row2.push_back({KeyCode::play, "play", 0});
            row2.push_back({KeyCode::stop, "stop", 0});
            row2.push_back({KeyCode::prev, "prev", 0});
            row2.push_back({KeyCode::next, "next", 0});
            matrix.push_back(row2);
            Keys row3;
            row3.push_back({KeyCode::tilde, "~", 0});
            row3.insert(row3.end(),keyGroupKeys.begin()+27,keyGroupKeys.begin()+37);
            row3.push_back({KeyCode::minus, "-", 0});
            row3.push_back({KeyCode::equal, "=", 0});
            row3.push_back({KeyCode::backspace, "backspace", 0});
            row3.push_back({KeyCode::insert, "insert", 0});
            row3.push_back({KeyCode::home, "home", 0});
            row3.push_back({KeyCode::page_up, "page up", 0});
            row3.push_back({KeyCode::num_lock, "num lock", 0});
            row3.push_back({KeyCode::num_slash, "num /", 0});
            row3.push_back({KeyCode::num_asterisk, "num *", 0});
            row3.push_back({KeyCode::num_minus, "num -", 0});
            matrix.push_back(row3);
            Keys row4;
            row4.push_back({KeyCode::tab, "tab", 0});
            
            
            
        }
            break;
        default:
            std::cerr <<"GetKeysMatrix not supported on this keyboard model"<<std::endl;
            break;
    }
    
    return matrix;
}

const Keyboard* const Keyboard::GetDefaultKeyboard()
{
    if(!keyboards.empty()) return &(*keyboards.begin());
    return NULL;
}
const std::set<Keyboard, Keyboard::KeyboardLess>* const Keyboard::GetKeyboards()
{
    return &keyboards;
}

bool Keyboard::LoadAvailableKeyboards()
{
    if (hid_init() < 0) return false;

    struct hid_device_info *devs, *dev;
    devs = hid_enumerate(0x0, 0x0);
    dev = devs;
    while (dev) 
    {
        for (const auto& supportedKeyboard : SupportedKeyboards) 
        {
            if (dev->vendor_id == supportedKeyboard[0] && 
                    dev->product_id == supportedKeyboard[1]) 
            {
                try
                {
                    keyboards.emplace(dev->vendor_id,dev->product_id,dev->serial_number,
                                      dev->interface_number,dev->manufacturer_string,dev->product_string);
                }
                catch(const KeyboardError& ex)
                {
                    std::cerr << ex.what() << std::endl;
                }
                break;
            }
        }
        dev = dev->next;
    }
    
    hid_free_enumeration(devs);
    hid_exit();
    return !keyboards.empty();
    
}
