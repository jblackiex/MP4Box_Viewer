#include "MP4Box.h"

void	read_file(QFile &file)
{
	int mdat = 0;
	quint32 counter = 0;
	quint32 next_byte = 0;
	quint32 size = 0;
	QByteArray box;
	QByteArray prev_box = 0;

	box = file.read(8);
	if (box.size() != 8) {
		qWarning() << "Error: check the file, no box found.";
		return ;
	}
	// read file 8 bytes at a time until i found mdat box
	while (!file.atEnd()) {
	if (counter >= next_byte) { // true when i found next box
		if (check_box(prev_box, box, next_byte, size))
			if (print_box(prev_box, box, size, mdat) || (box.contains("mdat") && ++mdat))
				break ;
	}
	counter += 8; // counter for bytes read
	prev_box = box;
	box = file.read(8);
	}
	// print mdat box content
	if (mdat && print_box(prev_box, box, size, mdat))
		print_mdat(file);
	else
		qDebug() << "no mdat box found.";
}

int main(int ac, char **av)
{
    QCoreApplication a(ac, av);

	QFile file("text0.mp4");
	if (!file.open(QIODevice::ReadOnly)) {
	qWarning() << "Error: file not found. Provide 'text0.mp4' sample file in the same directory as the executable.";
	QCoreApplication::exit(1);
	return 1;
    }
	read_file(file);
	
	QTextStream input(stdin);
    QString userInput;

    // ask user if he wants to quit
    while (1) {
		qDebug() << "Would you like to quit? (yes/no)";
		input >> userInput;
    	if (userInput.toLower() == "yes" || userInput.toLower() == "y") {
        qDebug() << "Quitting... bye!";
		file.close();
		QCoreApplication::exit(0);
		return 0;
    	}
	}
	return a.exec();
}
