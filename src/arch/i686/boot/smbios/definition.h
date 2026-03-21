#pragma once

#include <stdint.h>
#include "../../../../kernel/libc/util.h"

// ! SMBIOS STRUCTURE TYPE CONSTANTS
#define SMBIOS_TYPE_BIOS_INFORMATION              0
#define SMBIOS_TYPE_SYSTEM_INFORMATION            1
#define SMBIOS_TYPE_BASEBOARD_INFORMATION         2
#define SMBIOS_TYPE_SYSTEM_ENCLOSURE              3
#define SMBIOS_TYPE_PROCESSOR_INFORMATION         4
#define SMBIOS_TYPE_MEMORY_CONTROLLER_INFORMATION 5
#define SMBIOS_TYPE_MEMORY_MODULE_INFORMATION     6
#define SMBIOS_TYPE_CACHE_INFORMATION             7
#define SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION    8
#define SMBIOS_TYPE_SYSTEM_SLOTS                  9
#define SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY        16
#define SMBIOS_TYPE_MEMORY_DEVICE                17
#define SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFO      18
#define SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDR     19
#define SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDR    20
#define SMBIOS_TYPE_END_OF_TABLE                127

// ! SMBIOS STRUCTURE DEFINITIONS

typedef struct {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
} PACKED smbios_tag_t;

// BIOS Information (Type 0)
typedef struct {
    smbios_tag_t head;
    uint8_t vendor;
    uint8_t bios_version;
    uint16_t bios_starting_address_segment;
    uint8_t bios_release_date;
    uint8_t bios_rom_size;
    uint64_t bios_characteristics;
    uint8_t extension_bytes[2];
    uint8_t system_bios_major_release;
    uint8_t system_bios_minor_release;
    uint8_t embedded_controller_major_release;
    uint8_t embedded_controller_minor_release;
    // Strings follow
} PACKED smbios_bios_information_t;

// System Information (Type 1)
typedef struct {
    smbios_tag_t head;
    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
    uint8_t uuid[16];
    uint8_t wake_up_type;
    uint8_t sku_number;
    uint8_t family;
    // Strings follow
} PACKED smbios_system_information_t;

// Baseboard Information (Type 2)
typedef struct {
    smbios_tag_t head;
    uint8_t manufacturer;
    uint8_t product;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t feature_flags;
    uint8_t location_in_chassis;
    uint16_t chassis_handle;
    uint8_t board_type;
    uint8_t number_of_contained_object_handles;
    // uint16_t contained_object_handles[];
    // Strings follow
} PACKED smbios_baseboard_information_t;

// System Enclosure (Type 3)
typedef struct {
    smbios_tag_t head;
    uint8_t manufacturer;
    uint8_t type;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag_number;
    uint8_t boot_up_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count;
    uint8_t contained_element_record_length;
    // Contained elements, strings follow
} PACKED smbios_system_enclosure_t;

// Processor Information (Type 4)
typedef struct {
    smbios_tag_t head;
    uint8_t socket_designation;
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    // Strings follow
} PACKED smbios_processor_information_t;

// Physical Memory Array (Type 16)
typedef struct {
    smbios_tag_t head;
    uint8_t location;
    uint8_t use;
    uint8_t error_correction;
    uint32_t maximum_capacity;
    uint16_t memory_error_information_handle;
    uint16_t number_of_memory_devices;
    uint64_t extended_maximum_capacity;
} PACKED smbios_physical_memory_array_t;

// Memory Device (Type 17)
typedef struct {
    smbios_tag_t head;
    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;
    uint16_t speed;
    uint8_t manufacturer;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t attributes;
    uint32_t extended_size;
    uint16_t configured_memory_speed;
    uint16_t minimum_voltage;
    uint16_t maximum_voltage;
    uint16_t configured_voltage;
    // Strings follow
} PACKED smbios_memory_device_t;

// SMBIOS Entry Point Structure (32-bit, SMBIOS 2.x)
typedef struct {
    char anchor[4]; // "_SM_"
    uint8_t checksum;
    uint8_t length;
    uint8_t major_version;
    uint8_t minor_version;
    uint16_t max_structure_size;
    uint8_t entry_point_revision;
    uint8_t formatted_area[5];
    char intermediate_anchor[5]; // "_DMI_"
    uint8_t intermediate_checksum;
    uint16_t table_length;
    uint32_t table_address;
    uint16_t number_of_structures;
    uint8_t bcd_revision;
} PACKED smbios_entry_point_t;
