#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sqlite3.h>

//flip endianness
uint8_t flipEndian(uint8_t n) {
	return n;
}

int flipEndian(int n) {
	return ((n & 0xFF000000) >> 24) |
		((n & 0x00FF0000) >> 8) |
		((n & 0x0000FF00) << 8) |
		((n & 0x000000FF) << 24);
}

std::vector<std::vector<uint8_t>> readMNISTImages(const std::string& imageFilePath,int index,int count) {
	std::ifstream file(imageFilePath, std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Failed to open image file: " << imageFilePath << std::endl;
		return std::vector< std::vector<uint8_t>>();
	}

	// Read the header information
	int magicNumber, numImages, numRows, numCols;
	file.read((char*)&magicNumber, 4);
	magicNumber=flipEndian(magicNumber);
	file.read((char*)&numImages, 4);
	numImages = flipEndian(numImages);
	file.read((char*)&numRows, 4);
	numRows = flipEndian(numRows);
	file.read((char*)&numCols, 4);
	numCols = flipEndian(numCols);
	if (index > numImages) {
		return std::vector<std::vector<uint8_t>>();
	}

	// Read image data
	std::vector<std::vector<uint8_t>> images(count, std::vector<uint8_t>(numRows * numCols));
	file.seekg(16 + index * numRows * numCols, std::ios::beg);
	for (int i = 0; i < count; i++) {
		file.read((char*)images[i].data(), numRows * numCols);
	}

	return images;
}

int GetImageCount(const std::string& imageFilePath) {
	std::ifstream file(imageFilePath, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open image file: " << imageFilePath << std::endl;
		return 0;
	}
	// Read the header information
	int numImages;
	file.seekg(4, std::ios::beg);
	file.read((char*)&numImages, 4);
	return flipEndian(numImages);
}

// Function to read MNIST label data
std::vector<uint8_t> readMNISTLabels(const std::string& labelFilePath,int index,int count) {
	std::ifstream file(labelFilePath, std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Failed to open label file: " << labelFilePath << std::endl;
		return std::vector<uint8_t>();
	}

	// Read the header information
	int magicNumber, numLabels;
	file.read((char*)&magicNumber, 4);
	magicNumber = flipEndian(magicNumber);
	file.read((char*)&numLabels, 4);
	numLabels = flipEndian(numLabels);

	// Read label data
	std::vector<uint8_t> labels(count);
	file.seekg(8 + index, std::ios::beg);
	file.read((char*)labels.data(), count);

	return labels;
}

int GetLabelCount(const std::string& labelFilePath) {
	std::ifstream file(labelFilePath, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open label file: " << labelFilePath << std::endl;
		return 0;
	}
	// Read the header information
	int numLabels;
	file.seekg(4, std::ios::beg);
	file.read((char*)&numLabels, 4);
	return flipEndian(numLabels);
}

// Function to read MNIST data in batches and insert into an SQLite database
void createDatabase(const std::string& imageFilePath, const std::string& labelFilePath, const std::string& databasePath,int batch_size) {
	sqlite3* db;
	int rc = sqlite3_open(databasePath.c_str(), &db);

	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	// Create the MNIST_DATA table
	const char* createTableSQL = "CREATE TABLE MNIST_DATA (id INTEGER PRIMARY KEY AUTOINCREMENT, label INTEGER, image BLOB);";
	rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);

	if (rc) {
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	int numImages = GetImageCount(imageFilePath);
	int numLabels = GetLabelCount(labelFilePath);
	if (numImages != numLabels) {
		std::cerr << "Number of images and labels do not match." << std::endl;
		sqlite3_close(db);
		return;
	}

	// Prepare an SQL statement for batch insertion
	sqlite3_stmt* stmt;

	//calc numeber of batches
	int numBatches = numImages / batch_size;
	int remainder = numImages % batch_size;
	if (remainder > 0) {
		numBatches++;
	}

	// Loop through the batches
	for (int i = 0; i < numBatches; i++)
	{
		if (i != 0) {
			printf("\r");
			//move up by 1 using ansii
			printf("\x1b[1A");
		}
		printf("Processing batch %d of %d\n", i + 1, numBatches);

		// Read a batch of images and labels
		int index = i * batch_size;
		int count = batch_size;
		if (i == numBatches - 1 && remainder > 0) {
			count = remainder;
		}
		std::vector<std::vector<uint8_t>> images = readMNISTImages(imageFilePath, index, count);
		std::vector<uint8_t> labels = readMNISTLabels(labelFilePath, index, count);

		// Prepare the SQL statement
		std::string sql = "INSERT INTO MNIST_DATA (label, image) VALUES (?, ?);";
		rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

		for (int j = 0; j < count; j++) {
			if (j != 0) {
				printf("\r");
			}
			printf("Processing Batch Item %d of %d", j, count);
			// Bind the label to the statement
			sqlite3_bind_int(stmt, 1, labels[j]);
			// Bind the image to the statement
			sqlite3_bind_blob(stmt, 2, images[j].data(), images[j].size(), SQLITE_STATIC);
			// Execute the statement
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE) {
				std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
				sqlite3_close(db);
				return;
			}
		}

		//reset the statement for the next batch
		sqlite3_reset(stmt);
	}
	printf("\n");
	// Finalize the statement
	//sqlite3_finalize(stmt);
}

int main() {
	std::string trainImageFile = "t10k-images.idx3-ubyte";
	std::string trainLabelFile = "t10k-labels.idx1-ubyte";
	std::string databasePath = "mnist_test.db";
	int batchSize = 1000; // Adjust the batch size as needed

	//delete db if exists
	if (std::filesystem::exists(databasePath)) {
		std::filesystem::remove(databasePath);
	}

	createDatabase(trainImageFile, trainLabelFile, databasePath,1000);

	return 0;
}
