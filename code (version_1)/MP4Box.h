#ifndef MP4Box_h
#define MP4Box_h

// Qt library
#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QtEndian>

// std library
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

// function prototypes
bool	print_box(QByteArray &prev_box, QByteArray &box, quint32 &size, int &mdat);
bool	check_box(QByteArray &prev_box, QByteArray &box, quint32 &next_byte, quint32 &size);
void	print_mdat(QFile &file);
#endif
