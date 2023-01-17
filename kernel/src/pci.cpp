#include "pci.h"

namespace PCI
{
    void enumerate_function(uint64_t device_addr, uint64_t function)
    {
        uint64_t offset = function << 12;
        uint64_t function_addr = device_addr + offset;
        
        global_ptm.map_mem((void*)function_addr, (void*)function_addr); // identity mapping

        PCIDeviceHeader* pci_device_header = (PCIDeviceHeader*)function_addr;

        if (pci_device_header->device_id == 0) { return; }
        if (pci_device_header->device_id == 0xffff) { return; }

        global_renderer->print(get_vendor_name(pci_device_header->vendor_id));
        global_renderer->print(" / ");
        global_renderer->print(get_device_name(pci_device_header->vendor_id, pci_device_header->device_id));
        global_renderer->print(" / ");
        global_renderer->print(device_classes[pci_device_header->class_code]);
        global_renderer->print(" / ");
        global_renderer->print(get_subclass_name(pci_device_header->class_code, pci_device_header->subclass));
        global_renderer->nextln();
    }
    
    void enumerate_device(uint64_t bus_addr, uint64_t device)
    {
        uint64_t offset = device << 15;
        uint64_t device_addr = bus_addr + offset;
        
        global_ptm.map_mem((void*)device_addr, (void*)device_addr); // identity mapping

        PCIDeviceHeader* pci_device_header = (PCIDeviceHeader*)device_addr;

        if (pci_device_header->device_id == 0) { return; }
        if (pci_device_header->device_id == 0xffff) { return; }

        for (uint64_t function = 0; function < 8; function++) // 8 functions per device
        {
            enumerate_function(device_addr, function);
        }
    }
    
    void enumerate_bus(uint64_t base_addr, uint64_t bus)
    {
        uint64_t offset = bus << 20;
        uint64_t bus_addr = base_addr + offset;
        
        global_ptm.map_mem((void*)bus_addr, (void*)bus_addr); // identity mapping

        PCIDeviceHeader* pci_device_header = (PCIDeviceHeader*)bus_addr;

        if (pci_device_header->device_id == 0) { return; }
        if (pci_device_header->device_id == 0xffff) { return; }

        for (uint64_t device = 0; device < 32; device++) // 32 devices per bus
        {
            enumerate_device(bus_addr, device);
        }
    }

    void enumerate_pci(ACPI::MCFGHeader* mcfg)
    {
        int entries = ((mcfg->header.length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);

        for (int i = 0; i < entries; i++)
        {
            // getting base offset of device configuration
            ACPI::DeviceConfig* new_device_config = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * i));

            for (uint64_t bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++)
            {
                enumerate_bus(new_device_config->base_addr, bus);
            }
        }
    }
}