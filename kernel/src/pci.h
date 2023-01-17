#pragma once

#include <stdint.h>

#include "acpi.h"
#include "c_str.h"
#include "basic_renderer.h"
#include "paging/page_table_manager.h"

namespace PCI
{
    struct PCIDeviceHeader
    {
        uint16_t vendor_id;
        uint16_t device_id;
        uint16_t command;
        uint16_t status;
        uint8_t revision_id;
        uint8_t prog_if; // program interface
        uint8_t subclass;
        uint8_t class_code;
        uint8_t cache_line_size;
        uint8_t latency_timer;
        uint8_t header_type;
        uint8_t bist;
    };

    void enumerate_pci(ACPI::MCFGHeader* mcfg);

    extern const char* device_classes[];

    const char* get_vendor_name(uint16_t vendor_id);
    const char* get_device_name(uint16_t vendor_id, uint16_t device_id);
    const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code);
    const char* get_prog_if_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_if);
}