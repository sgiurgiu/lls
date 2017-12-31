#include "keyboard.h"
#include "utils.h"
#include <iostream>
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
bool Keyboard::SetKeys(KeyValueArray keyValues) const
{
    if (keyValues.empty()) return false;
    
    bool retval = true;

    std::vector<std::vector<KeyValue>> SortedKeys = {
            {}, // Logo AddressGroup
            {}, // Indicators AddressGroup
            {}, // Multimedia AddressGroup
            {}, // GKeys AddressGroup
            {} // Keys AddressGroup
    };

    for (uint8_t i = 0; i < keyValues.size(); i++) {
            switch(static_cast<KeyAddressGroup>(static_cast<uint16_t>(keyValues[i].key) >> 8 )) {
                    case KeyAddressGroup::logo:
                            switch (model) {
                                    case KeyboardModel::g610:
                                    case KeyboardModel::g810:
                                    case KeyboardModel::gpro:
                                            if (SortedKeys[0].size() <= 1 && keyValues[i].key == Key::logo)
                                                    SortedKeys[0].push_back(keyValues[i]);
                                            break;
                                    case KeyboardModel::g910:
                                            if (SortedKeys[0].size() <= 2) SortedKeys[0].push_back(keyValues[i]);
                                            break;
                                    default:
                                            break;
                            }
                            break;
                    case KeyAddressGroup::indicators:
                            if (SortedKeys[1].size() <= 5) SortedKeys[1].push_back(keyValues[i]);
                            break;
            case KeyAddressGroup::multimedia:
                    switch (model) {
                            case KeyboardModel::g610:
                            case KeyboardModel::g810:
                            case KeyboardModel::gpro:
                                    if (SortedKeys[2].size() <= 5) SortedKeys[2].push_back(keyValues[i]);
                                    break;
                            default:
                                    break;
                    }
                    break;
            case KeyAddressGroup::gkeys:
                    switch (model) {
                            case KeyboardModel::g910:
                                    if (SortedKeys[3].size() <= 9) SortedKeys[3].push_back(keyValues[i]);
                                    break;
                            default:
                                    break;
                    }
                    break;
            case KeyAddressGroup::keys:
                    switch (model) {
                            case KeyboardModel::g610:
                            case KeyboardModel::g810:
                            case KeyboardModel::g910:
                            case KeyboardModel::gpro:
                                    if (SortedKeys[4].size() <= 120) SortedKeys[4].push_back(keyValues[i]);
                                    break;
                            case KeyboardModel::g410:
                                    if (SortedKeys[4].size() <= 120)
                                            if (keyValues[i].key < Key::num_lock ||
                                                keyValues[i].key > Key::num_dot)
                                                    SortedKeys[4].push_back(keyValues[i]);
                                    break;
                            default:
                                    break;
                    }
                    break;
            }
    }
    for (uint8_t kag = 0; kag < 5; kag++) {
             if (SortedKeys[kag].size() > 0) {
                     uint8_t gi = 0;
                     while (gi < SortedKeys[kag].size()) {

                             size_t data_size = 0;
                             byte_buffer_t data = {};

                             switch (kag) {
                                     case 0:
                                             data_size = 20;
                                             data = getKeyGroupAddress(KeyAddressGroup::logo);
                                             break;
                                     case 1:
                                             data_size = 64;
                                             data = getKeyGroupAddress(KeyAddressGroup::indicators);
                                             break;
                                     case 2:
                                             data_size = 64;
                                             data = getKeyGroupAddress(KeyAddressGroup::multimedia);
                                             break;
                                     case 3:
                                             data_size = 64;
                                             data = getKeyGroupAddress(KeyAddressGroup::gkeys);
                                             break;
                                     case 4:
                                             data_size = 64;
                                             data = getKeyGroupAddress(KeyAddressGroup::keys);
                                             break;
                             }

                             const uint8_t maxKeyCount = (data_size - 8) / 4;

                             if (data.size() > 0) {
                                for (uint8_t i = 0; i < maxKeyCount; i++) {
                                    if (gi + i < SortedKeys[kag].size()) {
                                            data.push_back(static_cast<uint8_t>(
                                                    static_cast<uint16_t>(SortedKeys[kag][gi+i].key) & 0x00ff));
                                            data.push_back(SortedKeys[kag][gi+i].color.red);
                                            data.push_back(SortedKeys[kag][gi+i].color.green);
                                            data.push_back(SortedKeys[kag][gi+i].color.blue);
                                    }
                                }

                                data.resize(data_size, 0x00);
                                write(data);
                             }

                             gi = gi + maxKeyCount;
                     }

             }
     }
     return retval;    
}

bool Keyboard::commit() const
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
        return write(data);
}

int Keyboard::write(byte_buffer_t &data) const
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


bool Keyboard::SetAllKeys(const Color& color) const
{
    KeyValueArray keyValues;
    bool retval = false;
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
        addKeyToArray(keyGroupLogo,keyValues,color);
        addKeyToArray(keyGroupIndicators,keyValues,color);
        addKeyToArray(keyGroupMultimedia,keyValues,color);
        addKeyToArray(keyGroupGKeys,keyValues,color);
        addKeyToArray(keyGroupFKeys,keyValues,color);
        addKeyToArray(keyGroupFunctions,keyValues,color);
        addKeyToArray(keyGroupArrows,keyValues,color);
        addKeyToArray(keyGroupNumeric,keyValues,color);
        addKeyToArray(keyGroupModifiers,keyValues,color);
        addKeyToArray(keyGroupKeys,keyValues,color);
        retval = SetKeys(keyValues);
    default:
        retval = false;
    }
    return retval;
}
void Keyboard::addKeyToArray(const KeyArray& list, KeyValueArray& toArray, const Color& color) const
{
    for(auto key:list)
    {
        toArray.push_back({key,color});
    }
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
