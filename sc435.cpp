#include <iostream>
#include <fstream>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>

//Seam Carving Class
class SC {
public:

	SC() = default;

	void print(std::vector<std::vector<int>> matrix);
	void get_hdr(std::ifstream& img);
	void pop_pix_mtx(std::ifstream& img);
	void pop_enr_mtx();
	void pop_cume_mtx();
	void find_v_seams();
	auto find_min_col();
	void find_h_seams();
	void remove_v_seams(int v_seams);
	void remove_h_seams(int horizontal_seams);
	void write(std::ofstream& output);

private:

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
		//Check for correct number of arguments
		if (argc != 4)
			throw "Please use the following format. file_name number_of_vertical_seams number_of_horizontal_seams";
		else {
			//Check that number of seams to be removed is positive
			if (atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0)
				throw "Number of vertical and horizontal seams must be greater than zero";
			//File Name
			std::string f_name = argv[1];
			img.open(f_name.c_str(), std::ios::binary);

			//change file name
			f_name.erase(f_name.length() - 4, 4);
			f_name = f_name + "_processed.pgm";
			img_proc.open(f_name.c_str(), std::ios::binary);

			//Error handling for opening and creating files
			if (!img.is_open()) {
				throw "Failed to open file";
			}
			if (!img_proc.is_open()) {
				throw "Failed to create output file";
			}
		}
	}
	catch (const char* err) {
		std::cout << "Error: " << err << std::endl;
	}

	//Create Seam carver object and do the manipulations to the file and output 
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



//Print
void SC::print(std::vector<std::vector<int>> mtx) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++)
			std::cout << mtx[y][x] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
//Gets header from pgm file
void SC::get_hdr(std::ifstream& img) {
	std::string x, y;

	std::getline(img, hdr);
	while (hdr[0] == '#') {
		std::getline(img, hdr);
	}

	std::getline(img, dim);
	
	while (dim[0] == '#') {
		std::getline(img, dim);
	}
	
	std::getline(img, grey_max);
	
	while (grey_max[0] == '#') {
		std::getline(img, grey_max);
	}
	
	const char* chars = dim.c_str();

	while (*chars != ' ') {
		x += *chars++;
	}
	
	for (auto i = dim.size() - x.size() - 1; i < dim.size(); i++) {
		y += *chars++;
	}

	//Get width and height
	w = stoi(x);
	h = stoi(y);
}

//Populate pixel matrix
void SC::pop_pix_mtx(std::ifstream& img) {

	pix_mtx.resize(h);
	
	//Resize to 2d matrix
	for (int i = 0; i < h; i++) {
		pix_mtx[i].resize(w);
	}
	
	//Populate matrix
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			img >> pix_mtx[y][x];
}

//Populate energy matrix
void SC::pop_enr_mtx() {

	enr_mtx.resize(h);
	//Resize to 2d matrix
	for (int i = 0; i < h; i++) {
		enr_mtx[i].resize(w);
	}
	
	//Handles All the different cases for where you are in matrix
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {


			if (y == 0) {

				if (x == 0 && y == 0) {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
				}

				else if (x == w - 1 && y == 0) {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
				}

				else {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
				}
			}


			else if (y == h - 1) {

				if (x == 0 && y == h - 1) {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]);
				}

				else if (x == w - 1 && y == h - 1) {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]);
				}

				else {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]);
				}
			}


			else if (x == 0 || x == w - 1) {


				if (x == 0) {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
				}

				else if (x == w - 1) {
					enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
				}
			}


			else {
				enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1]) + abs(pix_mtx[y][x] - pix_mtx[y - 1][x]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
			}
		}
	}
}

//Populate cumulative energy matrix
void SC::pop_cume_mtx() {
	cume_mtx.resize(h);
	for (int i = 0; i < h; i++)
		cume_mtx[i].resize(w);


	for (int x = 0; x < w; x++) {
		cume_mtx[0][x] = enr_mtx[0][x];
	}

	for (int y = 1; y < h; y++) {
		for (int x = 0; x < w; x++) {

			
			if (x == 0) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(enr_mtx[y - 1][x], enr_mtx[y - 1][x + 1]);
			}
			
			else if (x == w - 1) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(enr_mtx[y - 1][x], enr_mtx[y - 1][x - 1]);
			}

			else {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(std::min(enr_mtx[y - 1][x - 1], enr_mtx[y - 1][x]), enr_mtx[y - 1][x + 1]);
			}
		}
	}
	print(cume_mtx);
}
//Find the vertical seams 
void SC::find_v_seams() {
	std::vector<int>::iterator it;

	it = std::min_element(cume_mtx[h - 1].begin(), cume_mtx[h - 1].end());
	pos[h - 1] = std::distance(std::begin(cume_mtx[h - 1]), it);

	for (int row = h - 1; row > 0; row--) {
		
		if (pos[row] == 0) {
			it = std::min_element(cume_mtx[row - 1].begin(), cume_mtx[row - 1].begin() + 2);
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		
		else if (pos[row] == w - 1) {
			it = std::min_element(cume_mtx[row - 1].begin() + pos[row] - 1, cume_mtx[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		
		else if (w <= 3) {
			it = std::min_element(cume_mtx[row - 1].begin(), cume_mtx[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		
		else {
			it = std::min_element(cume_mtx[row - 1].begin() + pos[row] - 1, cume_mtx[row - 1].begin() + pos[row] + 1);
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}
	}
}

//Removing vertical seams
void SC::remove_v_seams(int v_seams) {
	while (v_seams > 0) {

		pop_enr_mtx();
		pop_cume_mtx();
		find_v_seams();

		for (auto i : pos) {
			pix_mtx[i.first].erase(pix_mtx[i.first].begin() + i.second);
			cume_mtx[i.first].erase(cume_mtx[i.first].begin() + i.second);
		}

		w--;
		v_seams--;
	}

	print(pix_mtx);
}

//Find the min columns
auto SC::find_min_col() {

	std::vector<int> val;
	int column = w - 1;

	for (int row = 0; row < h; row++) {
		val.push_back(cume_mtx[row][column]);
	}

	auto min_elem = std::min_element(val.begin(), val.end());

	for (int row = 0; row < h; row++) {
		if (cume_mtx[row][column] == *min_elem) {
			return cume_mtx[row].begin() + column;
		}
	}
}

//Find horizontal seams 
void SC::find_h_seams() {

	std::vector<std::vector<int>>::iterator it;
	int row;
	std::vector<int> comp;
	auto min_iter = find_min_col();
	pos.clear();


	for (int y = 0; y < h; y++) {
		if (cume_mtx[y].begin() <= min_iter && min_iter < cume_mtx[y].end()) {
			row = y;
		}
	}

	for (int column = w - 1; column > 0; column--) {

		
		if (row == 0) {

		}

		
		else if (row == h - 1) {

		}

		
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