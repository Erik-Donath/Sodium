#pragma once

// This is the HAL (Hardware Abstraction Layer). It defines Funktions and Structures that the underlaying Architecture needs to implement.
// This also means that the file will grow or seperate into multiple HAL files in the future.

// Most of the Interactions with the Architecture will go through the DMS(Driver Managment System). The DMS is a System Service that defines a globale Interface for Drivers. There are BUS Drivers that implement Hardware Controllers into a generic Interface. 
// For Example PCI(E) or PS2 (i8042 controller in the i686 Architecure). And there are "virtuale" Drivers. Those do not communicate directly with a Hardware Connection. Instead they will use a BUS. For Example the MF2 Keyboard Driver is defined for PS2 Buses.
// The Driver doesn't care where the Device is connected: It could be connected via the i8042 Controller, or via a USB Bridge or PCI(E) Card.
// That of curse means that the difference between BUS and normal Drivers are the Locations. While normale Drivers life in the high kernel, BUS Drivers life in the Architecture spesific Kernel (called low kernel).
// The DMS will call a HAL function that lifes in the low Kernel to register Arch Spesific Drivers. This means that when Implementing another Architecture the low kernel of that Architecture must only implement
// the basic init setup spesific to this arch and the BUS drivers. A MF2 PS2 Keyboard will then work on that Platform without reimplementing the Driver from Scratch since it isn't spesific to the Structure.
// All Systems that are not Drivers but Arch spesific are labeled as infrastructor or platform spesifc. For Example the GDT implementation is not realy a Driver but a platform spesific Detail.

// The File doesn't contain any Functions yet.