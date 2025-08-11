#pragma once

typedef const uint8_t* smbios_info_ptr;

void smbios_print(smbios_info_ptr smbios, uint32_t size);
