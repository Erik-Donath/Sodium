#pragma once

typedef const void* smbios_info_ptr;

void smbios_print(smbios_info_ptr smbios, uint32_t size);
