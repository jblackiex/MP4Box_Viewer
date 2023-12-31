#include "MP4Box.h"

void	extract_images(QString &line, int &image, int &count)
{
	static QString base64;
	if (!image)
		image++;
	else if (!line.contains("</smpte:image>")) // NOT end of image
		base64 += line;
	else {
		QImage png;
		QByteArray imageData = QByteArray::fromBase64(base64.toLatin1());
		if (png.loadFromData(imageData, "PNG")) // Load image from data with "PNG" format
			if (!png.save("image_00" + QString::number(++count) + ".png", "PNG")) // save image as png
				--count;
		base64.clear(); // reset base64 string
		image = 0;
	}
}

void	print_mdat(QFile &file)
{
	QString line;

	int count = 0;
	int image = 0;

	while (!file.atEnd()) {
		line = file.readLine(); // read line by line now
		if (line.size() > 1)
		{
			if (image || line.contains("imagetype=")) // if found image tag or already found
				extract_images(line, image, count);
			cout << line.toStdString();
		}
	}
	cout << endl << "#-----------------------------------------------------------------------#" << endl <<  "END OF FILE. Scroll up to see the size and type of found Indented Boxes." << endl;
	if (count > 0)
		cout << endl << "\033[1;32m" << "Found " << count << " images in mdat box." << " Saved as image_00X.png" << "\033[0m" << endl;
	else
		cout << endl << "\033[1;31m" << "No images found in mdat box." << "\033[0m" << endl;
	cout << "#-----------------------------------------------------------------------#" << endl;
}

// NEW CODE HERE! //
bool	print_box(QByteArray &prev_box, QByteArray &box, quint32 &size, int &mdat)
{
	static int tab = 2;
	string type = box.right(4).toStdString();
	string restore;

	tab *= type == "moof" || type == "traf" ? 2 : type == "mdat" ? 0 : 1;
	restore = type == "mdat" ? "\r" : ""; // this to print mdat Box ID correctly
	string box_id = restore + "Box ID: "; // \rBox ID: mdat if type == "mdat"
	if (!mdat)
		cout << box_id << type << " of size " << size << endl << setw(tab) << " ";
	else
		cout << "\rMdat content:\n" << prev_box.right(3).toStdString();
	return mdat;
}

bool	save_box(QByteArray &prev_box, QByteArray &box, quint32 &next_byte, quint32 &size, string &type)
{
	// size = 0; 
	if (!(next_byte % 8)) { // if next_byte is a multiple of 8, box type on the right half
		QDataStream first4bytes(box.left(4));
		first4bytes.setByteOrder(QDataStream::BigEndian); // set byte order to Big Endian

		// read a 32 bit integer from the stream
		first4bytes >> size;
	}
	else {
		int index = box.indexOf(type.c_str()); // return index where is type in box
		int find = 4 - index;
		QByteArray lastbytes = prev_box.right(find); // dim is in the last "find" bytes of the found previous box
		if (index) // this means that the box is split between the previous and the current box
		{
			prev_box.replace(5, 3, box.right(find).toStdString().c_str()); // this to print correctly first bytes of mdat box content
			lastbytes.append(box.left(index)); // last part of previus box + first part of current box
		}
		QDataStream dim(lastbytes);
		dim >> size;
		box.replace(4, 4, type.c_str());
	}
	next_byte += type == "moof" || type == "traf" || type == "mdat" ? 8 : size;
	return true;

}

bool	check_box(QByteArray &prev_box, QByteArray &box, quint32 &next_byte, quint32 &size)
{
	int i = 0; // static int i = 0 "solution" if box are in the same order as type[] array in the file O(1) time instead of O(n)
	string found = box.right(4).toStdString();
	string type[] = {"moof", "mfhd", "traf", "tfhd", "trun", "uuid", "mdat"};

	// see if box contains the right type
	while (!box.contains(type[i]) && i < 7)
		i++;
	if (i < 7 && found == type[i]) // see if box contains the right type on the right half of the box
		return save_box(prev_box, box, next_byte, size, type[i]);
	else if (i < 7) // box is not on the right half
		return save_box(prev_box, box, next_byte, size, type[i]);
	return false;
}