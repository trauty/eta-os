#include "acpi.h"

namespace ACPI
{
    void* find_table(SDTHeader* sdt_header, char* signature)
    {
        int entries = (sdt_header->length - sizeof(ACPI::SDTHeader)) / 8; // entries come directly after header

        for (int i = 0; i < entries; i++)
        {
            // casting address of xsdt to uint64 for algebra adding size of header for the end and getting address of new header
            ACPI::SDTHeader* new_sdt_header = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdt_header + sizeof(ACPI::SDTHeader) + (i * 8));

            for (int j = 0; j < 4; j++)
            {
                if (new_sdt_header->signature[j] != signature[j])
                {
                    break;
                }
                if (j == 3) { return new_sdt_header; }
            }
        }

        return 0;
    }
}