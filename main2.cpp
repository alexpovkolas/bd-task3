#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>

using namespace std;



int main() {
    ifstream input("input.bin", ios::binary);
    ofstream output("output.bin", std::ios::binary);

    int n, m;

    input.read((char *)&n, 4);
    input.read((char *)&m, 4);


    int block_size = 20000;
    int row_number = 0;
    vector<unsigned char> acc_buf(block_size);

    if (m <= block_size) {

        vector<unsigned char> v(m);
        vector<unsigned char> row(block_size);

        input.seekg(8 + m * n);
        input.read((char*)v.data(), m);

        long long read_bytes = 0;


        unsigned char acc = 0;
        int col = 0;
        while (read_bytes < n*m) {

            int to_read = n*m - read_bytes < block_size ? n*m - read_bytes : block_size;

            input.seekg(8 + read_bytes);
            input.read((char*)row.data(), to_read);
            read_bytes+= to_read;

            int byte_processed = 0;
            while (byte_processed < to_read) {
                int to_calc = to_read - byte_processed < m ? to_read - byte_processed : m;
                to_calc = min(to_calc, m - col);

                for (int i = 0; i < to_calc; ++i) {
                    acc += v[col + i] * row[byte_processed + i];
                }
                byte_processed += to_calc;
                col += to_calc;

                if (col % m == 0) {
                    acc_buf[row_number++] = acc;
                    if (row_number >= block_size) {
                        output.write((char *)acc_buf.data(), block_size);
                        row_number = 0;
                    }
                    acc = 0;
                    col = 0;
                }
            }
        }

    } else {
        vector<unsigned char> v(block_size);
        vector<unsigned char> row(block_size);


        long long read_bytes = 0;
        int chunk_count = 0;
        unsigned char acc = 0;
        int col = 0;
        while (read_bytes < n*m) {

            int to_read = n*m - read_bytes < block_size ? n*m - read_bytes : block_size;
            to_read = min(to_read, m - col);


            input.seekg(8 + m * n + col);
            input.read((char*)v.data(), to_read);

            input.seekg(8 + read_bytes);
            input.read((char*)row.data(), to_read);
            chunk_count++;
            read_bytes += to_read;

            for (int i = 0; i < to_read; ++i) {
                acc += v[i] * row[i];
                col++;

                if (col % m == 0) {
                    acc_buf[row_number++] = acc;

                    if (row_number >= block_size) {
                        output.write((char *)acc_buf.data(), block_size);
                        row_number = 0;
                    }
                    acc = 0;
                    col = 0;
                }
            }
        }
    }

    if (row_number > 0) {
        output.write((char *)acc_buf.data(), row_number);
    }

    input.close();
    output.close();
    return 0;
}