#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    wczytajSpisKsiazek();
    wczytajUzytkownikowIZwroty();
    aktualizujListeKsiazek();
    aktualizujListeUzytkownikow();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::wczytajSpisKsiazek() {
    QString path = QCoreApplication::applicationDirPath() + "/../Resources/data/spisksiazek.txt";
    QFile plik(path);
    if (!plik.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Błąd", "Nie można otworzyć pliku:\n" + path);
        return;
    }

    QTextStream in(&plik);
    spisKsiazek.clear();

    while (!in.atEnd()) {
        QString linia = in.readLine();
        QStringList dane = linia.split(",");
        if (dane.size() == 2) {
            Book ks;
            ks.tytul = dane[0].trimmed();
            ks.autor = dane[1].trimmed();
            ks.wypozyczona = false;
            spisKsiazek.append(ks);
        }
    }
    plik.close();
}

void MainWindow::wczytajUzytkownikowIZwroty() {
    QString basePath = QCoreApplication::applicationDirPath() + "/../Resources/data/";

    QFile plik(basePath + "uzytkownicy.txt");
    if (plik.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&plik);
        osoby.clear();

        while (!in.atEnd()) {
            QString linia = in.readLine();
            QStringList dane = linia.split(",");
            if (dane.size() == 2) {
                Person p;
                p.imieNazwisko = dane[0].trimmed();
                p.id = dane[1].trimmed().toInt();
                osoby.append(p);
            }
        }
        plik.close();
    }

    QFile wypozyczenia(basePath + "wypozyczenia.txt");
    if (wypozyczenia.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&wypozyczenia);
        while (!in.atEnd()) {
            QString linia = in.readLine();
            QStringList dane = linia.split(",");
            if (dane.size() == 3) {
                int id = dane[0].toInt();
                Book ks;
                ks.tytul = dane[1].trimmed();
                ks.autor = dane[2].trimmed();
                ks.wypozyczona = true;

                for (auto& osoba : osoby) {
                    if (osoba.id == id) {
                        osoba.ksiazki.append(ks);
                        break;
                    }
                }

                for (auto& b : spisKsiazek) {
                    if (b.tytul == ks.tytul && b.autor == ks.autor) {
                        b.wypozyczona = true;
                        break;
                    }
                }
            }
        }
        wypozyczenia.close();
    }
}

void MainWindow::zapiszUzytkownikow() {
    QString path = QCoreApplication::applicationDirPath() + "/../Resources/data/uzytkownicy.txt";
    QFile plik(path);
    if (plik.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&plik);
        for (const auto& osoba : osoby) {
            out << osoba.imieNazwisko << "," << osoba.id << "\n";
        }
        plik.close();
    }
}

void MainWindow::zapiszWypozyczenia() {
    QString path = QCoreApplication::applicationDirPath() + "/../Resources/data/wypozyczenia.txt";
    QFile plik(path);
    if (plik.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&plik);
        for (const auto& osoba : osoby) {
            for (const auto& ks : osoba.ksiazki) {
                out << osoba.id << "," << ks.tytul << "," << ks.autor << "\n";
            }
        }
        plik.close();
    }
}

void MainWindow::aktualizujListeKsiazek() {
    ui->listaKsiazek->clear();
    for (const auto& k : spisKsiazek) {
        if (!k.wypozyczona) {
            ui->listaKsiazek->addItem(k.tytul + " - " + k.autor);
        }
    }
}

void MainWindow::aktualizujListeUzytkownikow() {
    ui->listaUzytkownikow->clear();
    for (const auto& o : osoby) {
        QString text = o.imieNazwisko + " (ID: " + QString::number(o.id) + ")";
        ui->listaUzytkownikow->addItem(text);
    }
}

void MainWindow::on_dodajOsobeButton_clicked() {
    QString imie = ui->lineEditImieNazwisko->text();
    int id = ui->lineEditID->text().toInt();

    if (imie.isEmpty() || id == 0) {
        QMessageBox::warning(this, "Błąd", "Wprowadź poprawne dane.");
        return;
    }

    for (const auto& osoba : osoby) {
        if (osoba.id == id) {
            QMessageBox::warning(this, "Błąd", "Użytkownik o tym ID już istnieje.");
            return;
        }
    }

    Person p;
    p.imieNazwisko = imie;
    p.id = id;
    osoby.append(p);

    aktualizujListeUzytkownikow();
    zapiszUzytkownikow();

    ui->lineEditImieNazwisko->clear();
    ui->lineEditID->clear();
}

void MainWindow::on_usunUzytkownikaButton_clicked() {
    QListWidgetItem* selected = ui->listaUzytkownikow->currentItem();
    if (!selected) {
        QMessageBox::warning(this, "Brak wyboru", "Nie wybrano użytkownika do usunięcia.");
        return;
    }

    int ret = QMessageBox::question(this, "Potwierdzenie", "Czy na pewno chcesz usunąć tego użytkownika?",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    QString text = selected->text();
    int idStart = text.indexOf("ID: ") + 4;
    int id = text.mid(idStart, text.length() - idStart - 1).toInt();

    for (int i = 0; i < osoby.size(); ++i) {
        if (osoby[i].id == id) {
            osoby.removeAt(i);
            break;
        }
    }

    aktualizujListeUzytkownikow();
    ui->listaWypozyczonych->clear();
    zapiszUzytkownikow();
    zapiszWypozyczenia();
}

void MainWindow::on_listaUzytkownikow_itemClicked(QListWidgetItem *item) {
    ui->listaWypozyczonych->clear();

    QString text = item->text();
    int idStart = text.indexOf("ID: ") + 4;
    int id = text.mid(idStart, text.length() - idStart - 1).toInt();

    for (const auto& osoba : osoby) {
        if (osoba.id == id) {
            for (const auto& ks : osoba.ksiazki) {
                ui->listaWypozyczonych->addItem(ks.tytul + " - " + ks.autor);
            }
            break;
        }
    }
}

void MainWindow::on_wypozyczButton_clicked() {
    QListWidgetItem* selectedKsiazka = ui->listaKsiazek->currentItem();
    QListWidgetItem* selectedOsoba = ui->listaUzytkownikow->currentItem();
    if (!selectedKsiazka || !selectedOsoba) return;

    QString tytul = selectedKsiazka->text().split(" - ").first();
    QString text = selectedOsoba->text();
    int idStart = text.indexOf("ID: ") + 4;
    int id = text.mid(idStart, text.length() - idStart - 1).toInt();

    for (auto& osoba : osoby) {
        if (osoba.id == id) {
            for (auto& ks : spisKsiazek) {
                if (ks.tytul == tytul && !ks.wypozyczona) {
                    ks.wypozyczona = true;
                    osoba.ksiazki.append(ks);
                    break;
                }
            }
            break;
        }
    }

    aktualizujListeKsiazek();
    zapiszWypozyczenia();
    aktualizujListeUzytkownikow();
    ui->listaWypozyczonych->clear();
}

void MainWindow::on_oddajButton_clicked() {
    QListWidgetItem* selectedOsoba = ui->listaUzytkownikow->currentItem();
    QListWidgetItem* selectedKsiazka = ui->listaWypozyczonych->currentItem();
    if (!selectedOsoba || !selectedKsiazka) return;

    QString text = selectedOsoba->text();
    int idStart = text.indexOf("ID: ") + 4;
    int id = text.mid(idStart, text.length() - idStart - 1).toInt();

    QString tytul = selectedKsiazka->text().split(" - ").first();

    for (auto& osoba : osoby) {
        if (osoba.id == id) {
            for (int i = 0; i < osoba.ksiazki.size(); ++i) {
                if (osoba.ksiazki[i].tytul == tytul) {
                    osoba.ksiazki.removeAt(i);
                    break;
                }
            }
            break;
        }
    }

    for (auto& ks : spisKsiazek) {
        if (ks.tytul == tytul) {
            ks.wypozyczona = false;
            break;
        }
    }

    aktualizujListeKsiazek();
    zapiszWypozyczenia();
    ui->listaWypozyczonych->clear();
}

void MainWindow::on_wyswietlButton_clicked() {
    QString info;
    for (const auto& osoba : osoby) {
        info += osoba.imieNazwisko + " (ID: " + QString::number(osoba.id) + ")\n";
        for (const auto& ks : osoba.ksiazki) {
            info += "  - " + ks.tytul + " (" + ks.autor + ")\n";
        }
    }
    QMessageBox::information(this, "Wypożyczenia", info);
}

void MainWindow::on_znajdzButton_clicked() {
    QString szukany = ui->lineEditSzukaj->text().trimmed();

    if (szukany.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Pole wyszukiwania jest puste.");
        return;
    }

    QStringList wyniki;

    for (const auto& ks : spisKsiazek) {
        if (ks.tytul.contains(szukany, Qt::CaseInsensitive) ||
            ks.autor.contains(szukany, Qt::CaseInsensitive)) {
            wyniki << "Książka: " + ks.tytul + " - " + ks.autor;
        }
    }

    for (const auto& osoba : osoby) {
        if (osoba.imieNazwisko.contains(szukany, Qt::CaseInsensitive)) {
            wyniki << "Użytkownik: " + osoba.imieNazwisko + " (ID: " + QString::number(osoba.id) + ")";
            for (const auto& ks : osoba.ksiazki) {
                wyniki << "  -> Wypożyczona: " + ks.tytul + " - " + ks.autor;
            }
        }
    }

    if (!wyniki.isEmpty()) {
        QMessageBox::information(this, "Wyniki wyszukiwania", wyniki.join("\n"));
    } else {
        QMessageBox::warning(this, "Nie znaleziono", "Brak wyników pasujących do \"" + szukany + "\".");
    }
}

