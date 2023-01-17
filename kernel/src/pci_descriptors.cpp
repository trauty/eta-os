#include <stdint.h>

#include "c_str.h"

namespace PCI
{
    const char* device_classes[]
    {
        "Unclassified",
        "Mass Storage Controller",
        "Network Controller",
        "Display Controller",
        "Multimedia Controller",
        "Memory Controller",
        "Bridge Device",
        "Simple Communication Controller",
        "Base System Peripheral",
        "Input Device Controller",
        "Docking Station",
        "Processor",
        "Serial Bus Controller",
        "Wireless Controller",
        "Intelligent Controller",
        "Satellite Communication Controller",
        "Encryption Controller",
        "Signal Processing Controller",
        "Processing Accelerator",
        "Non Essential Instrumentation"
    };

    const char* get_vendor_name(uint16_t vendor_id)
    {
        switch (vendor_id)
        {
            case 0x8086:
                return "Intel Corporation";
            case 0x1022:
                return "AMD";
            case 0x10de:
                return "NVIDIA Corporation";
        }
        
        return to_hstring(vendor_id);
    }

    const char* get_device_name(uint16_t vendor_id, uint16_t device_id)
    {
        switch (vendor_id)
        {
            case 0x8086:        // for Intel devices
                switch (device_id)
                {
                    case 0x29C0:
                        return "Express DRAM Controller";
                    case 0x2918:
                        return "LPC Interface Controller";
                    case 0x2922:
                        return "6 port SATA Controller [AHCI mode]";
                    case 0x2930:
                        return "SMBus Controller";
                }
        }
        
        return to_hstring(device_id);
    }

    const char* mass_storage_controller_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
            case 0x00:
                return "SCSI Bus Controller";
            case 0x01:
                return "IDE Controller";
            case 0x02:
                return "Floppy Disk Controller";
            case 0x03:
                return "IPI Bus Controller";
            case 0x04:
                return "RAID Controller";
            case 0x05:
                return "ATA Controller";
            case 0x06:
                return "Serial ATA";
            case 0x07:
                return "Serial Attached SCSI";
            case 0x08:
                return "Non-Volatile Memory Controller";
            case 0x80:
                return "Other";
        }
        
        return to_hstring(subclass_code);
    }

    const char* get_prog_if_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_if)
    {
        switch (class_code)
        {
            case 0x01:
                switch (subclass_code)
                {
                    case 0x06:
                        switch (prog_if)
                        {
                            case 0x00:
                                return "Vendor Specific Interface";
                            case 0x01:
                                return "AHCI 1.0";
                            case 0x02:
                                return "Serial Storage Bus";
                        }
                }
            case 0x03:
                switch (subclass_code)
                {
                    case 0x00:
                        switch (prog_if)
                        {
                            case 0x00:
                                return "VGA Controller";
                            case 0x01:
                                return "8514-Compatible Controller";
                        }
                }
            case 0x0C:
                switch (subclass_code)
                {
                    case 0x03:
                        switch (prog_if)
                        {
                            case 0x00:
                                return "UHCI Controller";
                            case 0x10:
                                return "OHCI Controller";
                            case 0x20:
                                return "EHCI (USB2) Controller";
                            case 0x30:
                                return "XHCI (USB3) Controller";
                            case 0x80:
                                return "Unspecified";
                            case 0xFE:
                                return "USB Device (Not a Host Controller)";
                        }
                }    
        }
        return to_hstring(prog_if);
    }

    const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code)
    {
        switch (class_code)
        {
            case 0x01:
                return mass_storage_controller_subclass_name(subclass_code);
            case 0x03:
                switch (subclass_code)
                {
                    case 0x00:
                        return "VGA Compatible Controller";
                }
        }
        
        return to_hstring(subclass_code);
    }
}