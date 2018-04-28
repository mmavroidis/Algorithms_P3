/*
Mavroidis Mavroidis 
4/27/2018
Final Project
Algorithms
Seam Carving 
*/


#include <fstream>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>


// Seam carving Class
class SC {
public:

	//Constructor and functions
	SC() = default;
	void get_hdr(std::ifstream& img);
	void pop_pix_mtx(std::ifstream& img);
	void pop_enr_mtx();
	void pop_cume_mtx();
	void find_v_seams();
	void pop_cume_h();
	void rotate(int a);
	void find_h_seams();
	void remove_v_seams(int v_seams);
	void remove_h_seams(int h_seams);
	void write(std::ofstream& out);

private:
	std::vector<std::vector<int>> pix_mtx;
	std::vector<std::vector<int>> enr_mtx;
	std::vector<std::vector<int>> cume_mtx;
	std::vector<std::vector<int>> pix_rot;
	std::vector<std::vector<int>> cume_rot;
	std::map<int, int> pos;
	std::string hdr, dim, max_gray;
	int	w, h;
};


//Main Function Loop
int main(int argc, char** argv) {
	std::ifstream img;
	std::ofstream img_proc;
	int v_seams, h_seams;

	try {

		if (argc != 4)
			throw "Please enter the following to run program, file_Name num_vert_seams num_hor_seams";
		else {

			if (atoi(argv[2]) < 0 || atoi(argv[3]) < 0)
				throw "Enter Positive number of seams to remove";
			else {
				v_seams = atoi(argv[2]);
				h_seams = atoi(argv[3]);
			}

			std::string f_name = argv[1];
			img.open(f_name.c_str(), std::ios::binary);
			f_name.erase(f_name.length() - 4, 4);
			f_name = f_name + "_processed.pgm";
			img_proc.open(f_name.c_str(), std::ios::binary);

			// Check to ensure proper io
			if (!img.is_open()) {
				throw "Could not open img file";
			}
			if (!img_proc.is_open()) {
				throw "Could not create out file";
			}
		}
	}
	catch (const char* err) {
		std::cout << "Error: " << err << std::endl;
	}

	//Create the SC object and grab the header attributes
	SC sc;								
	sc.get_hdr(img);			

	//Populate the matricies and remove the seams
	sc.pop_pix_mtx(img);			
	sc.remove_v_seams(v_seams);			
	sc.remove_h_seams(h_seams);		

	//Output to file
	sc.write(img_proc);				
	img.close();
	img_proc.close();

	return 0;
}



// Get header Info
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

	std::getline(img, max_gray);
	while (max_gray[0] == '#') {
		std::getline(img, max_gray);
	}

	const char* chars = dim.c_str();

	while (*chars != ' ') {
		x += *chars++;
	}

	for (int i = dim.size() - x.size() - 1; i < dim.size(); i++) {
		y += *chars++;
	}

	w = stoi(x);
	h = stoi(y);
}

//Populate pixel matrix
void SC::pop_pix_mtx(std::ifstream& img) {

	pix_mtx.resize(h);
	for (int i = 0; i < h; i++) {
		pix_mtx[i].resize(w);
	}

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			img >> pix_mtx[y][x];
		}
	}
}

// Populate energy matrix
void SC::pop_enr_mtx() {

	enr_mtx.resize(h);

	for (int i = 0; i < h; i++) {
		enr_mtx[i].resize(w);
	}

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
				enr_mtx[y][x] = abs(pix_mtx[y][x] - pix_mtx[y][x - 1]) + abs(pix_mtx[y][x] - pix_mtx[y][x + 1])	+ abs(pix_mtx[y][x] - pix_mtx[y - 1][x]) + abs(pix_mtx[y][x] - pix_mtx[y + 1][x]);
			}
		}
	}
}

// Populate cumulative mtx
void SC::pop_cume_mtx() {

	cume_mtx.clear();
	cume_mtx.resize(h);
	
	for (int i = 0; i < h; i++) {
		cume_mtx[i].resize(w);
	}

	for (int x = 0; x < w; x++) {
		cume_mtx[0][x] = enr_mtx[0][x];
	}
	for (int y = 1; y < h; y++) {
		for (int x = 0; x < w; x++) {

		
			if (x == 0) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(cume_mtx[y - 1][x], cume_mtx[y - 1][x + 1]);
			}

		
			else if (x == w - 1) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(cume_mtx[y - 1][x], cume_mtx[y - 1][x - 1]);
			}

		
			else {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(std::min(cume_mtx[y - 1][x - 1], cume_mtx[y - 1][x]), cume_mtx[y - 1][x + 1]);
			}
		}
	}
}

// Find Vertical seams
void SC::find_v_seams() {

	std::vector<int>::iterator it;
	pos.clear();

	it = std::min_element(cume_mtx[h - 1].begin(), cume_mtx[h - 1].end());
	pos[h - 1] = std::distance(std::begin(cume_mtx[h - 1]), it);

	for (int row = h - 1; row > 0; row--) {

		if (pos[row] == 0) {
			it = std::min_element(cume_mtx[row - 1].begin(), cume_mtx[row - 1].begin() + 2);
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}
		
		else if (pos[row] == w - 1) {
			it = std::min_element(cume_mtx[row - 1].end() - 2, cume_mtx[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		else if (w <= 3) {
			it = std::min_element(cume_mtx[row - 1].begin(), cume_mtx[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}

		else {
			it = std::min_element(cume_mtx[row - 1].begin() + pos[row] - 1, cume_mtx[row - 1].begin() + pos[row] + 2);
			pos[row - 1] = std::distance(std::begin(cume_mtx[row - 1]), it);
		}
	}
}

// Remove vertical seams
void SC::remove_v_seams(int v_seams) {

	while (v_seams > 0) {
		pop_enr_mtx();		
		pop_cume_mtx(); 
		find_v_seams();			

		for (auto i : pos) {
			pix_mtx[i.first].erase(pix_mtx[i.first].begin() + i.second);
		}

		w--;
		v_seams--;
	}
}

// Populate cumulative matrix 
void SC::pop_cume_h() {

	cume_mtx.clear();
	cume_mtx.resize(h);
	for (int i = 0; i < h; i++) {
		cume_mtx[i].resize(w);
	}
	
	for (int y = 0; y < h; y++) {
		cume_mtx[y][0] = enr_mtx[y][0];
	}

	for (int x = 1; x < w; x++) {
		for (int y = 0; y < h; y++) {

			if (y == 0) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(cume_mtx[y][x - 1], cume_mtx[y + 1][x - 1]);
			}

			else if (y == h - 1) {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(cume_mtx[y][x - 1], cume_mtx[y - 1][x - 1]);
			}

			else {
				cume_mtx[y][x] = enr_mtx[y][x] + std::min(std::min(cume_mtx[y + 1][x - 1], cume_mtx[y][x - 1]), cume_mtx[y - 1][x - 1]);
			}
		}
	}
}

// Rotate matrix 
void SC::rotate(int a) {

	// 90 c
	if (a == 0) {

		cume_rot.resize(w);
		pix_rot.resize(w);

		for (int i = 0; i < w; i++) {
			cume_rot[i].resize(h);
			pix_rot[i].resize(h);
		}

		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++) {
				cume_rot[i][j] = cume_mtx[h - 1 - j][i];
				pix_rot[i][j] = pix_mtx[h - 1 - j][i];
			}
	}

	// 90 cc
	else if (a == 1) {

		pix_mtx.clear();
		pix_mtx.resize(h);

		for (int i = 0; i < h; i++) {
			pix_mtx[i].resize(w);
		}

		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				pix_mtx[i][j] = pix_rot[j][h - 1 - i];
			}
		}
	}

}

// Find h seams
void SC::find_h_seams() {

	std::vector<int>::iterator it;
	pos.clear();

	it = std::min_element(cume_rot[w - 1].begin(), cume_rot[w - 1].end());
	pos[w - 1] = std::distance(std::begin(cume_rot[w - 1]), it);

	for (int row = w - 1; row > 0; row--) {

		if (pos[row] == 0) {
			it = std::min_element(cume_rot[row - 1].begin(), cume_rot[row - 1].begin() + 2);
			pos[row - 1] = std::distance(std::begin(cume_rot[row - 1]), it);
		}

		else if (pos[row] == h - 1) {
			it = std::min_element(cume_rot[row - 1].end() - 2, cume_rot[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_rot[row - 1]), it);
		}

		else if (h <= 3) {
			it = std::min_element(cume_rot[row - 1].begin(), cume_rot[row - 1].end());
			pos[row - 1] = std::distance(std::begin(cume_rot[row - 1]), it);
		}

		else {
			it = std::min_element(cume_rot[row - 1].begin() + pos[row] - 1, cume_rot[row - 1].begin() + pos[row] + 2);
			pos[row - 1] = std::distance(std::begin(cume_rot[row - 1]), it);
		}
	}
}

// Remove h seams
void SC::remove_h_seams(int h_seams) {

	while (h_seams > 0) {
		pop_enr_mtx();
		pop_cume_h();
		rotate(0);
		find_h_seams();

		for (auto i : pos) {
			pix_rot[i.first].erase(pix_rot[i.first].begin() + i.second);
		}

		h_seams--;
		h--;
		rotate(1);
	}
}

//Output to file 
void SC::write(std::ofstream& out) {

	std::vector<std::vector<int>>::iterator row;
	std::vector<int>::iterator col;

	out << "P2\n";
	out << "#\n";
	out << w << " " << h << "\n";
	out << max_gray << "\n";

	for (row = pix_mtx.begin(); row != pix_mtx.end(); row++) {
		for (col = row->begin(); col != row->end(); col++) {
			out << *col << " ";
		}
		out << "\n";
	}
	out << "\n";
}