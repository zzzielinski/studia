#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Book {
    QString tytul;
    QString autor;
    bool wypozyczona;
};

struct Person {
    QString imieNazwisko;
    int id;
    QVector<Book> ksiazki;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_dodajOsobeButton_clicked();
    void on_usunUzytkownikaButton_clicked();
    void on_listaUzytkownikow_itemClicked(QListWidgetItem *item);
    void on_wypozyczButton_clicked();
    void on_oddajButton_clicked();
    void on_wyswietlButton_clicked();
    void on_znajdzButton_clicked();

private:
    Ui::MainWindow *ui;
    QVector<Book> spisKsiazek;
    QVector<Person> osoby;

    void wczytajSpisKsiazek();
    void wczytajUzytkownikowIZwroty();
    void zapiszUzytkownikow();
    void zapiszWypozyczenia();
    void aktualizujListeKsiazek();
    void aktualizujListeUzytkownikow();
};

#endif // MAINWINDOW_H
