# YTNoAds

[English](#english) | [Polski](#polski)

---

## English

A lightweight, native macOS desktop client for YouTube built with **SwiftUI** and **WebKit** (`WKWebView`). It blocks ad network requests and automatically skips video ads while preserving persistent login sessions and native macOS window controls.

### Features
- **Network-Level Ad Blocking:** Uses `WKContentRuleList` to block ad servers and tracking endpoints (`doubleclick.net`, `googlesyndication.com`, YouTube ad breaks).
- **Automatic Video Ad Skipping:** Injects a lightweight script to hide banner/overlay ads and fast-forward unskippable video ads automatically.
- **Persistent Session:** Keeps you signed in across app restarts using the default `WKWebsiteDataStore`.
- **Native macOS Integration:** Compact unified toolbar, dynamic video title in the window bar, loading progress indicator, and keyboard shortcuts (`Cmd + [`, `Cmd + ]`, `Cmd + R`, `Cmd + Shift + H`, `Cmd + Shift + O`).
- **External Link Handling:** Opens external links outside YouTube/Google directly in your default system browser.

### Installation
1. Download the latest `.dmg` file from the [**Releases**](../../releases/latest) page.
2. Open the `.dmg` disk image and drag the app into your **Applications** folder.
3. **First launch (Gatekeeper notice):** Because the app is not notarized with a paid Apple Developer ID certificate, macOS may block it on first launch. To open it:
   - Right-click (or `Control`-click) the app in **Applications** and select **Open**, or
   - Run the following command in Terminal:
     ```bash
     xattr -cr /Applications/YTNoAds.app
     ```

### Building from Source
- **Requirements:** macOS with Xcode 15 or newer.
- Clone the repository, open the `.xcodeproj` file in Xcode, ensure **Outgoing Connections (Client)** is enabled under *Signing & Capabilities -> App Sandbox*, and press `Cmd + R`.

---

## Polski

Lekki, natywny klient desktopowy YouTube dla systemu **macOS**, napisany w **SwiftUI** oraz **WebKit** (`WKWebView`). Aplikacja blokuje żądania sieciowe sieci reklamowych oraz automatycznie pomija reklamy wideo, zachowując przy tym sesję logowania i natywną obsługę okien macOS.

### Funkcje
- **Blokowanie reklam na poziomie sieciowym:** Wykorzystuje `WKContentRuleList` do blokowania serwerów reklamowych i skryptów śledzących (`doubleclick.net`, `googlesyndication.com`, bloki reklamowe YouTube).
- **Automatyczne pomijanie reklam wideo:** Wstrzykuje lekki skrypt ukrywający banery oraz automatycznie przewijający i klikający „Pomiń” na reklamach wideo.
- **Zapamiętywanie sesji:** Utrzymuje zalogowane konto między kolejnymi uruchomieniami programu (`WKWebsiteDataStore.default()`).
- **Natywna integracja z macOS:** Kompaktowy pasek narzędzi, dynamiczny tytuł odtwarzanego wideo na belce okna, pasek postępu ładowania oraz skróty klawiszowe (`Cmd + [`, `Cmd + ]`, `Cmd + R`, `Cmd + Shift + H`, `Cmd + Shift + O`).
- **Obsługa linków zewnętrznych:** Linki prowadzące poza ekosystem YouTube/Google są automatycznie otwierane w domyślnej przeglądarce systemowej.

### Instalacja
1. Pobierz najnowszy plik `.dmg` z zakładki [**Releases**](../../releases/latest).
2. Otwórz pobrany obraz dysku `.dmg` i przeciągnij aplikację do folderu **Aplikacje (Applications)**.
3. **Pierwsze uruchomienie (ostrzeżenie Gatekeepera):** Ponieważ aplikacja nie jest podpisana płatnym certyfikatem Apple Developer ID, przy pierwszym uruchomieniu macOS może zablokować jej otwarcie. Aby ją uruchomić:
   - Kliknij ikonę aplikacji w folderze **Aplikacje** prawym przyciskiem myszy (lub z klawiszem `Control`) i wybierz **Otwórz**, albo
   - Wpisz w Terminalu polecenie usuwające atrybut kwarantanny:
     ```bash
     xattr -cr /Applications/YTNoAds.app
     ```

### Kompilacja ze źródeł
- **Wymagania:** macOS oraz Xcode 15 lub nowszy.
- Sklonuj repozytorium, otwórz plik `.xcodeproj` w Xcode, upewnij się, że w zakładce *Signing & Capabilities -> App Sandbox* zaznaczona jest opcja **Outgoing Connections (Client)**, a następnie uruchom projekt skrótem `Cmd + R`.
