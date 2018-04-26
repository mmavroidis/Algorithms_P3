#include <iostream>
#include <fstream>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>

class SC {
public:
	// Constructor
	SC() = default;

	// Public class members
	void print(std::vector<std::vector<int>> matrix);
	void get_hdr(std::ifstream& img);
	void pop_pix_mtx(std::ifstream& img);
	void pop_enr_mtx();
	void pop_cume_mtx();
	void find_v_seams();
	auto find_min_col();
	void find_h_seams();
	void remove_v_seams(int vertical_seams);
	void remove_h_seams(int horizontal_seams);
	void write(std::ofstream& output);

private:

	// Private class members
	std::vector<std::vector<int>> pix_mtx;
	std::vector<std::vector<int>> enr_mtx;
	std::vector<std::vector<int>> cume_mtx;
	std::map<int, int> pos;
	std::string hdr, dim, grey_max;
	int	w, h;
};


int main(int argc, char** argv) {
	std::ifstream img;
	std::ofstream img_proc;

	try {
		if (argc != 4)
			throw "Usage: PROGRAM f_name NUM_VERTICAL_SEAMS NUM_HORIZONTAL_SEAMS";
		else {

			if (atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0)
				throw "Number of vertical and horizontal seams must be greater than zero";

			std::string f_name = argv[1];
			img.open(f_name.c_str(), std::ios::binary);

			f_name.erase(f_name.length() - 4, 4);
			f_name = f_name + "_processed.pgm";
			img_proc.open(f_name.c_str(), std::ios::binary);

			if (!img.is_open())
				throw "Could not open img file";

			if (!img_proc.is_open())
				throw "Could not create output file";
		}
	}
	catch (const char* err) {
		std::cout << "The application threw an exception: " << err << std::endl;
	}

	SC sc;
	sc.get_hdr(img);
	sc.pop_pix_mtx(img);
	sc.remove_v_seams(atoi(argv[2]));
	sc.remove_h_seams(atoi(argv[3]));
	sc.write(img_proc);

	img.close();
	img_proc.close();

	return 0;
}




void SC::print(std::vector<std::vector<int>> matrix) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++)
			std::cout << matrix[y][x] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void SC::get_hdr(std::ifstream& img) {
	std::string x, y;

	std::getline(img, hdr);
	while (hdr[0] == '#')
		std::getline(img, hdr);

	std::getline(img, dim);
	while (dim[0] == '#')
		std::getline(img, dim);

	std::getline(img, grey_max);
	while (grey_max[0] == '#')
		std::getline(img, grey_max);

	const char* chars = dim.c_str();

	while (*chars != ' ')
		x += *chars++;

	for (auto i = dim.size() - x.size() - 1; i < dim.size(); i++)
		y += *chars++;

	w = stoi(x);
	h = stoi(y);
}

void SC::pop_pix_mtx(std::ifstream& img) {
	// Resize vector
	pix_mtx.resize(h);
	for (int i = 0; i < h; i++)
		pix_mtx[i].resize(w);

	// Fill vector with pixel values
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			img >> pix_mtx[y][x];
}

void SC::pop_enr_mtx() {
	enr_mtx.resize(h);
	for (int i = 0; i < h; i++)
		enr_mtx[i].resize(w);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {

			// Top
			if (y == 0) {
				// Top left
				if (x == 0 && y == 0)
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);

				// Top right
				else if (x == w - 1 && y == 0)
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);

				// Otherwise
				else
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1])
					+ abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
			}

			// Bottom
			else if (y == h - 1) {
				// Bottom left
				if (x == 0 && y == h - 1)
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]);

				// Bottom right
				else if (x == w - 1 && y == h - 1)
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]);

				// Otherwise
				else
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1])
					+ abs(pix_mtx[y][x] - pix_mtx[y - 1][x]);
			}

			// Sides
			else if (x == 0 || x == w - 1) {

				// Left Side
				if (x == 0)
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x])
					+ abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);

				// Right Side
				else if (x == w - 1)
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x])
					+ abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
			}

			// Everything else
			else
				enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1])
				+ abs(pix_mtx[y][x] - pix_mtx[y - 1][x]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
		}
	}
}

void SC::pop_cume_mtx() {
	cume_mtx.resize(h);
	for (int i = 0; i < h; i++)
		cume_mtx[i].resize(w);

	// Copy top row of energy matrix to cumulative matrix
	for (int x = 0; x < w; x++)
		cume_mtx[0][x] = enr_mtx[0][x];

	for (int y = 1; y < h; y++) {
		for (int x = 0; x < w; x++) {

			// Left side
			if (x == 0) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(enr_mtx[y - 1][x], enr_mtx[y - 1][x + 1]);
			}

			// Right side
			else if (x == w - 1) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(enr_mtx[y - 1][x], enr_mtx[y - 1][x - 1]);
			}

			// Everything else
			else {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(std::min(enr_mtx[y - 1][x - 1], enr_mtx[y - 1][x]), enr_mtx[y - 1][x + 1]);
			}
		}
	}
	print(cume_mtx);
}

void SC::find_v_seams() {
	std::vector<int>::iterator it;

	it = std::min_element(cume_mtx[h - 1].begin(), cume_mtx[h - 1].end());
	pos[h - 1] = std::distance(std::begin(cume_mtx[h - 1]), it);

	for (int row = h - 1; row > 0; row--) {
		// Left side
		if (pos[row] == 0) {
			it = std::min_element(cume_mtx[row - 1].begin(), cume_mtx[row - 1].begin() + 2);
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		// Right side
		else if (pos[row] == w - 1) {
			it = std::min_element(cume_mtx[row - 1].begin() + pos[row] - 1, cume_mtx[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		// Special case
		else if (w <= 3) {
			it = std::min_element(cume_mtx[row - 1].begin(), cume_mtx[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		// Otherwise
		else {
			it = std::min_element(cume_mtx[row - 1].begin() + pos[row] - 1, cume_mtx[row - 1].begin() + pos[row] + 1);
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}
	}
}

void SC::remove_v_seams(int vertical_seams) {
	while (vertical_seams > 0) {
		pop_enr_mtx();
		pop_cume_mtx();
		find_v_seams();

		for (auto i : pos) {
			pix_mtx[i.first].erase(pix_mtx[i.first].begin() + i.second);
			cume_mtx[i.first].erase(cume_mtx[i.first].begin() + i.second);
		}

		w--;
		vertical_seams--;
	}

	print(pix_mtx);
}

//Resume from here

auto SC::find_min_col() {
	std::vector<int> values;
	int column = w - 1;

	for (int row = 0; row < h; row++) {
		values.push_back(cume_mtx[row][column]);
	}

	auto min_elem = std::min_element(values.begin(), values.end());

	for (int row = 0; row < h; row++) {
		if (cume_mtx[row][column] == *min_elem) {
			return cume_mtx[row].begin() + column;
		}
	}
}

void SC::find_h_seams() {
	std::vector<std::vector<int>>::iterator it;
	int row;
	std::vector<int> comparitor;
	auto min_iter = find_min_col();
	pos.clear();

	// Find the row of the last column that holds the smallest value
	for (int y = 0; y < h; y++) {
		if (cume_mtx[y].begin() <= min_iter && min_iter < cume_mtx[y].end()) {
			row = y;
		}
	}

	for (int column = w - 1; column > 0; column--) {

		// Top
		if (row == 0) {

		}

		// Bottom
		else if (row == h - 1) {

		}

		// Special case
		else if (h <= 3) {

		}

		// Otherwise
		else {

		}
	}
}

void SC::remove_h_seams(int horizontal_seams) {
	while (horizontal_seams > 0) {
		pop_enr_mtx();
		pop_cume_mtx();
		find_h_seams();

		for (auto i : pos) {
			pix_mtx[i.first].erase(pix_mtx[i.first].begin() + i.second);
			cume_mtx[i.first].erase(cume_mtx[i.first].begin() + i.second);
		}

		h--;
		horizontal_seams--;
	}

	print(pix_mtx);
}

void SC::write(std::ofstream& output) {
	std::vector<std::vector<int>>::iterator row;
	std::vector<int>::iterator column;

	output << "P2\n";
	output << w << " " << h << "\n";
	output << grey_max;

	for (row = pix_mtx.begin(); row != pix_mtx.end(); row++) {
		for (column = row->begin(); column != row->end(); column++) {
			output << *column << " ";
		}
		output << "\n";
	}
	output << "\n";
}