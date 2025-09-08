window.onload = function () {
    var emulator = new V86({
        screen_container: document.getElementById("emulator"),
        wasm_path: "v86.wasm",
        memory_size: 64 * 1024 * 1024,
        bios: { url: "bios/seabios.bin" },
        vga_bios: { url: "bios/vgabios.bin" },
        cdrom: { url: "iso/Sodium.iso" },
        autostart: true,
        default_screen: "screen"
    });
};
