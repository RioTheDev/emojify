# # Emojify

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GTK4](https://img.shields.io/badge/GTK-4.0-blueviolet.svg)](https://www.gtk.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-20-00599C.svg)](https://isocpp.org/)

**Emojify** is a lightweight, native emoji picker for the Linux desktop. Built with **C++** and **gtkmm-4.0** it provides a fast, responsive, and visually consistent experience for GNOME and other modern desktop environments.

![Main Screenshot](screenshots/main_window.png)

## ✨ Features

- 🚀 **Native Performance**: Written in C++ for near-instant startup and low memory footprint.
- 🎨 **Modern Interface**: Fully supports system dark/light modes and adaptive layouts.
- 🔒 **Privacy Focused**: No tracking, no telemetry, and zero network calls.

## 🛠 Installation

### Flatpak (Recommended)

The easiest way to install Emojify is via Flathub:

```bash
flatpak install flathub xyz.riothedev.emojify
```

### Building from Source

If you prefer to build manually, ensure you have the following dependencies:

- `meson` & `ninja`
- `gcc` or `clang` (with C++20 support)
- `gtkmm-4.0`
- `glibmm-2.68`
- `libadwaita-1`

#### Steps:

1.  **Clone the repository:**

    ```bash
    git clone [https://github.com/riothedev/emojify.git](https://github.com/riothedev/emojify.git)
    cd emojify
    ```

2.  **Setup the build directory:**

    ```bash
    meson setup build
    ```

3.  **Compile and Run:**

    ```bash
    meson compile -C build
    # To run locally with GSettings:
    export GSETTINGS_SCHEMA_DIR=./build/data
    ./build/src/emojify
    ```

4.  **Install system-wide:**
    ```bash
    sudo meson install -C build
    ```

## 🤝 Contributing

Contributions are welcome! Whether it's fixing a bug or adding a feature:

1.  Fork the repo.
2.  Create your feature branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

## 📄 License

Distributed under the **GPL-3.0-or-later** License. See `LICENSE` for more information.

---
