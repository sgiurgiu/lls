#include "keyboard.h"
#include <hidapi/hidapi.h>
#include <iostream>

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
                   const wchar_t* serialNumber, const wchar_t* manufacturer, const wchar_t* product):
    hid_handle(nullptr),vendorId(vendorId),productId(productId)
{
    std::cout << "Keyboard constructed " << vendorId <<":"<<productId <<std::endl;
    if(serialNumber != nullptr) this->serialNumber = std::wstring(serialNumber);
    if(manufacturer != nullptr) this->manufacturer = std::wstring(manufacturer);
    if(product != nullptr) this->product = std::wstring(product);
    hid_handle = (void*) hid_open(vendorId,productId,serialNumber);
    if(!hid_handle)
    {
        throw KeyboardError("Are udev permissions correct? Unable to open keyboard "+std::to_string(vendorId)+":"+std::to_string(productId));
    }
}
Keyboard& Keyboard::operator=(Keyboard&& other)
{
    hid_handle = other.hid_handle;
    other.hid_handle = nullptr;
    vendorId = std::move(other.vendorId);
    productId = std::move(other.productId);
    serialNumber = std::move(other.serialNumber);
    manufacturer = std::move(other.manufacturer);
    product = std::move(other.product);
    return *this;
}
Keyboard::Keyboard(Keyboard&& other):hid_handle(other.hid_handle),vendorId(std::move(other.vendorId)),
    productId(std::move(other.productId)),serialNumber(std::move(other.serialNumber)),
    manufacturer(std::move(other.manufacturer)),product(std::move(other.product))
{
    other.hid_handle = nullptr;
}

Keyboard::~Keyboard()
{    
    if(hid_handle)
    {
        std::cout << "Keyboard destroyed "<< vendorId <<":"<<productId  <<std::endl;
        hid_close((hid_device*)hid_handle);    
    }
    hid_handle = nullptr;
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
            +std::to_wstring(vendorId)+L", product ID: "+std::to_wstring(productId);
}

bool Keyboard::SetAllKeys(uint8_t red, uint8_t green, uint8_t blue) const
{
    KeyValueArray keyValues;
   
    switch (currentDevice.model) {
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
        for (uint8_t i = 0; i < keyGroupLogo.size(); i++) keyValues.push_back({keyGroupLogo[i], color});
        for (uint8_t i = 0; i < keyGroupIndicators.size(); i++) keyValues.push_back({keyGroupIndicators[i], color});
        for (uint8_t i = 0; i < keyGroupMultimedia.size(); i++) keyValues.push_back({keyGroupMultimedia[i], color});
        for (uint8_t i = 0; i < keyGroupGKeys.size(); i++) keyValues.push_back({keyGroupGKeys[i], color});
        for (uint8_t i = 0; i < keyGroupFKeys.size(); i++) keyValues.push_back({keyGroupFKeys[i], color});
        for (uint8_t i = 0; i < keyGroupFunctions.size(); i++) keyValues.push_back({keyGroupFunctions[i], color});
        for (uint8_t i = 0; i < keyGroupArrows.size(); i++) keyValues.push_back({keyGroupArrows[i], color});
        for (uint8_t i = 0; i < keyGroupNumeric.size(); i++) keyValues.push_back({keyGroupNumeric[i], color});
        for (uint8_t i = 0; i < keyGroupModifiers.size(); i++) keyValues.push_back({keyGroupModifiers[i], color});
        for (uint8_t i = 0; i < keyGroupKeys.size(); i++) keyValues.push_back({keyGroupKeys[i], color});
        return setKeys(keyValues);
    default:
        return false;
    }
    return false;
    
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
        for (size_t i = 0; i < SupportedKeyboards.size(); i++) 
        {
            if (dev->vendor_id == SupportedKeyboards[i][0] && 
                    dev->product_id == SupportedKeyboards[i][1]) 
            {
                try
                {
                    keyboards.emplace(dev->vendor_id,dev->product_id,dev->serial_number,
                                      dev->manufacturer_string,dev->product_string);
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
    return !keyboards.empty();
}
