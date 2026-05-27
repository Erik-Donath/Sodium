#pragma once

// @brief Architecture-independent kernel entry point.
//        Called by i686_pre_kernel() after all hardware initialisation is done.
//        Driver management, process scheduling, and system services
//        will be added here as the kernel grows.
void k_main(void);
