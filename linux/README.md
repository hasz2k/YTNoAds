# YTNoAds (Linux — GTK4 + WebKitGTK)

[English](#english) | [Polski](#polski)

---

## English

A lightweight, native Linux desktop client for YouTube written in **C** using **GTK4** and **WebKitGTK 6.0**. It blocks ad network requests via compiled content filter rules (`WebKitUserContentFilterStore`) and automatically skips video ads (`WebKitUserScript`), while keeping login sessions persistent in SQLite.

### Features
- **Network-Level Ad Blocking:** Uses `WebKitUserContentFilterStore` to block ad servers and tracking endpoints (`doubleclick.net`, `googlesyndication.com`, `googleadservices.com`, YouTube ad breaks).
- **Automatic Video Ad Skipping:** Injects a lightweight script at document start to hide overlay/banner ads and fast-forward unskippable video ads automatically.
- **Persistent Login Session:** Stores cookies in `~/.local/share/ytnoads/cookies.sqlite` across restarts.
- **Native GTK4 HeaderBar & Fullscreen:** Navigation controls (Back, Forward, Reload, Home, Open in External Browser), loading progress bar, dynamic window title, and clean HTML5 video fullscreen handling.
- **Tiny Footprint:** Compiles to a ~30 KB native binary with zero Electron/Chromium overhead.

### Dependencies
- **Arch Linux / CachyOS:**
  ```bash
  sudo pacman -S gtk4 webkitgtk-6.0 gcc pkgconf
  ```
- **Debian / Ubuntu (24.04+):**
  ```bash
  sudo apt install build-essential pkg-config libgtk-4-dev libwebkitgtk-6.0-dev
  ```
- **Fedora:**
  ```bash
  sudo dnf install gcc make pkgconf-pkg-config gtk4-devel webkitgtk6.0-devel
  ```

### Build & Install
```bash
make
./ytnoads

# Optional system-wide installation:
sudo make install
```

### Desktop Entry (Application Menu)
```bash
mkdir -p ~/.local/share/applications
cat << "DESKTOP" > ~/.local/share/applications/ytnoads.desktop
[Desktop Entry]
Name=YTNoAds
Comment=Native YouTube client with ad blocker
Exec=/usr/local/bin/ytnoads
Icon=web-browser
Terminal=false
Type=Application
Categories=AudioVideo;Video;Network;
DESKTOP
```

---

## Polski

Lekki, natywny klient desktopowy YouTube dla systemu **Linux**, napisany w języku **C** z wykorzystaniem **GTK4** oraz **WebKitGTK 6.0**. Blokuje żądania sieciowe serwerów reklamowych za pomocą skompilowanych filtrów treści (`WebKitUserContentFilterStore`) oraz automatycznie pomija reklamy wideo (`WebKitUserScript`), zachowując sesję logowania w bazie SQLite.

### Funkcje
- **Blokowanie reklam na poziomie sieciowym:** Wykorzystuje `WebKitUserContentFilterStore` do blokowania domen reklamowych i skryptów śledzących (`doubleclick.net`, `googlesyndication.com`, `googleadservices.com`, bloki reklamowe YouTube).
- **Automatyczne pomijanie reklam wideo:** Wstrzykuje lekki skrypt przy starcie dokumentu, który ukrywa banery oraz automatycznie przewija i pomija reklamy wideo.
- **Zapamiętywanie sesji logowania:** Zapisuje ciasteczka w `~/.local/share/ytnoads/cookies.sqlite` pomiędzy kolejnymi uruchomieniami.
- **Natywna integracja z GTK4:** Pasek nagłówka z nawigacją (Wstecz, Dalej, Odśwież, Strona główna, Otwórz w przeglądarce), pasek postępu ładowania, dynamiczny tytuł okna oraz obsługa pełnego ekranu HTML5.
- **Minimalny narzut zasobów:** Kompiluje się do natywnej binarki o rozmiarze ok. 30 KB, bez ciężkiego silnika Electron/Chromium.

### Wymagania
- **Arch Linux / CachyOS:**
  ```bash
  sudo pacman -S gtk4 webkitgtk-6.0 gcc pkgconf
  ```
- **Debian / Ubuntu (24.04+):**
  ```bash
  sudo apt install build-essential pkg-config libgtk-4-dev libwebkitgtk-6.0-dev
  ```
- **Fedora:**
  ```bash
  sudo dnf install gcc make pkgconf-pkg-config gtk4-devel webkitgtk6.0-devel
  ```

### Kompilacja i instalacja
```bash
make
./ytnoads

# Opcjonalna instalacja w systemie:
sudo make install
```

### Skrót w menu aplikacji (`.desktop`)
```bash
mkdir -p ~/.local/share/applications
cat << "DESKTOP" > ~/.local/share/applications/ytnoads.desktop
[Desktop Entry]
Name=YTNoAds
Comment=Native YouTube client with ad blocker
Exec=/usr/local/bin/ytnoads
Icon=web-browser
Terminal=false
Type=Application
Categories=AudioVideo;Video;Network;
DESKTOP
```