# QFaktury 0.7.1


1. [Opis](#opis)  
2. [Wymagania](#wymagania)  
3. [Instalacja zależności](#instalacja-zależności)  
4. [Instalacja](#instalacja)  
5. [Usuwanie](#usuwanie)  
6. [Rozwój](#rozwój)  
7. [Uwagi](#uwagi)  
8. [Zgłaszanie błędów](#zgłaszanie-błędów)  

<br/>
<br/>

## Opis

Stale rozwijana aplikacja do obsługi faktur. Aktualnie pozwala na zapis, edycję i drukowanie faktur VAT, Brutto, RR, Pro Forma, Rachunku, Korekty i Duplikatu. Dodatkowo pozwala na dodanie kontrahenta oraz towaru bądź usługi do tworzonej faktury. Program oferuje:

- zmianę waluty według aktualnego kursu ze strony NBP
- walidację wprowadzanych danych oraz sprawdzanie sum kontrolnych
- wybór trzech szablonów drukowania
- wydrukowanie listy kontrahentów wraz z danymi
- wybór motywu
- wybór kodowania
- prowadzenie organizera z kalendarzem
- przedstawienie słownie kwoty na fakturze w formacie od dziesiątek po miliardy wraz z wartościami po przecinku
- dodanie własnych elementów do listy wyborów stawek VAT, metod płatności, powodów korekty, sposobu numeracji faktur, dopisku, jednostek
- stworzenie i wczytywanie kopii zapasowej plików konfiguracyjnych oraz głównego katalogu
- dodanie kolejnych sprzedawców/oddziałów
- wysłanie e-mail do kontrahentów z możliwością korzystania z gotowych szablonów
- wystawianie dokumentów RW, WZ
- korzystanie z danych Głównego Urzędu Statystycznego z podaniem numeru NIP


<br/>
<br/>


## Wymagania

- dowolna dystrybucja Linux
- biblioteka Qt w wersji wyższej lub równej 5.0.0
- paczki qt5-base qt5-webengine (dla modułów Qt GUI, Qt Core, Qt Widgets, Qt Print Support, Qt XML, Qt WebEngine, Qt Network)
- paczki kwidgetsaddons, kxmlgui, ki18n, kcoreaddons, karchive (dla modułów KF5 - KCoreAddons, KXmlGui, KI18n, KWidgetsAddons, KArchive)
- cmake w wersji 3.9 lub wyższej (wymagane dla kompilacji)
- extra-cmake-modules w wersji 1.7.0 lub nowszej (wymagane dla kompilacji)
- połączenie z internetem (opcjonalne)
- aktualny czas systemowy (opcjonalne)

<br/>
<br/>


## Instalacja zależności

# Arch Linux

```
sudo pacman -S gksu php cmake qt5-base qt5-webengine extra-cmake-modules kwidgetsaddons kxmlgui ki18n kcoreaddons karchive
```

# Ubuntu

```
sudo apt-get install -y language-pack-en-base
sudo LC_ALL=en_US.UTF-8 add-apt-repository ppa:ondrej/php
sudo apt-get update
sudo apt-get install php7.1 php7.1-common cmake gksu qtbase5-dev qtwebengine5-dev extra-cmake-modules libkf5widgetsaddons-dev libkf5xmlgui-dev libkf5i18n-dev libkf5coreaddons-dev libkf5archive-dev
```

lub 

```
sudo apt-get install software-properties-common python-software-properties
sudo apt-get install php7.1 php7.1-common cmake gksu qtbase5-dev qtwebengine5-dev extra-cmake-modules libkf5widgetsaddons-dev libkf5xmlgui-dev libkf5i18n-dev libkf5coreaddons-dev libkf5archive-dev
```

# Debian Jessie

```
sudo apt-get install apt-transport-https lsb-release ca-certificates
wget -O /etc/apt/trusted.gpg.d/php.gpg https://packages.sury.org/php/apt.gpg
echo "deb https://packages.sury.org/php/ $(lsb_release -sc) main" > /etc/apt/sources.list.d/php.list
sudo apt-get update
sudo apt get install php7.1 cmake gksu qtbase5-dev qtwebengine5-dev extra-cmake-modules libkf5widgetsaddons-dev libkf5xmlgui-dev libkf5i18n-dev libkf5coreaddons-dev libkf5archive-dev
```

# Debian Sid

```
sudo apt get install php7.1 cmake gksu qtbase5-dev qtwebengine5-dev extra-cmake-modules libkf5widgetsaddons-dev libkf5xmlgui-dev libkf5i18n-dev libkf5coreaddons-dev libkf5archive-dev
```

# Fedora

```
wget http://rpms.remirepo.net/fedora/remi-release-25.rpm
sudo dnf install remi-release-25.rpm
sudo dnf install dnf-plugins-core
sudo dnf config-manager --set-enabled remi-php71
sudo dnf install cmake beesu qt5-qtbase-devel qt5-qtwebengine-devel extra-cmake-modules kf5-kwidgetsaddons kf5-kxmlgui kf5-ki18n kf5-kcoreaddons kf5-karchive
```

# OpenSUSE

```
sudo zypper in php7 php7-devel cmake gksu libgksu libqt5-qtbase libqt5-qtwebengine extra-cmake-modules libKF5WidgetsAddons5 libKF5XmlGui5 libKF5I18n5 libKF5CoreAddons5 libKF5Archive5
```

# Linux Mint

```
sudo apt-get install -y language-pack-en-base
sudo LC_ALL=en_US.UTF-8 add-apt-repository ppa:ondrej/php
sudo apt-get update
sudo apt-get install php7.1 cmake gksu qtbase5-dev qtwebengine5-dev extra-cmake-modules libkf5widgetsaddons-dev libkf5xmlgui-dev libkf5i18n-dev libkf5coreaddons-dev libkf5archive-dev
```

<br/>
<br/>


## Instalacja

Wykonujemy pobranie katalogu z plikami w wybranej przez siebie ścieżce i wpisujemy polecenia:

```
git clone -b KF5 https://github.com/juliagoda/qfaktury.git
cd qfaktury
mkdir zbudowane
cd zbudowane
cmake ..
make
sudo make install
```

Jeśli posiadasz Arch Linux bądź pokrewny system (Manjaro) możesz wykonać poniższe kroki:

```
git clone https://github.com/archlinux-lucjan/archlinux-poland.git
cd qfaktury-qt5-git
makepkg -sric
```

<br/>
<br/>

## Usuwanie

W zbudowanym katalogu:

`cat install_manifest.txt | sudo xargs rm`

lub (w Arch Linux), jeśli instalacja nastąpiła z PKGBUILD:

`sudo pacman -Rns qfaktury-qt5-git`

<br/>
<br/>


## Rozwój

Program jest co jakiś czas uaktualniany. Plany jego rozbudowy, wykonane zadania i zadania w trakcie testów znajdują się [na tej stronie](https://github.com/juliagoda/qfaktury/projects/1).


<br/>
<br/>


## Uwagi

1. Osoby posiadające faktury, listy kontrahentów i towarów z wcześniejszych wersji (mniejszych niż 0.7.0), powinny wykonać kopię zapasową i skopiować je do katalogu "~/.local/share/data/elinux"

2. Osoby posiadające faktury z wcześniejszych commit'ów w razie jakichkolwiek niepoprawnych wyników mogą spróbować ponownie zapisać istniejąca fakturę, edytując ją, ponieważ możliwe, że dotychczasowe commit'y uwzględniają zauważone błędy i uzupełnią także pliki XML dodatkowymi danymi, które pozwolą korzystać z pełnej funkcjonalności programu. Zalecane jest także trzymać kopię zapasową plików w innym miejscu.

3. Aktualizacja aktualnego kursu walut następuje co pół godziny, pod warunkiem połączenia z internetem oraz poprawnego ustawienia czasu systemowego

4. Portowanie na KF5 w trakcie


<br/>
<br/>

## Zgłaszanie błędów

W razie zauważonych błędów lub poważnych braków, można stworzyć wątek [w tym dziale](https://github.com/juliagoda/qfaktury/issues), klikając w zielony przycisk "New issue" po prawej stronie, a następnie wprowadzając tytuł i treść.

