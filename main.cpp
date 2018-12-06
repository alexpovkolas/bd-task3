#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>


using namespace std;



int main2() {

    ifstream input("input.bin", ios::binary);
    ofstream output("output.bin", ios::binary);
    int n, m;

    input.read((char *)&n, 4);
    input.read((char *)&m, 4);

    int block_size = 40000;

    vector<unsigned char> v(block_size);
    vector<unsigned char> row(block_size);
    vector<unsigned char> acc_buf(block_size);

    int acc = 0, col = 0, row_number = 0;
    if (m < block_size) {
        input.seekg(8 + m * n);
        input.read((char *)v.data(), m);

        int byte_processed = 0;
        while (byte_processed < n*m) {
            int to_read = byte_processed + block_size > n*m ? n * m - byte_processed : block_size;

            input.seekg(8 + byte_processed);
            input.read((char *)row.data(), to_read);
            byte_processed += to_read;

            for (int i = 0; i < to_read; i++) {
                acc += v[col % m] * row[i];

                if ((col + 1) % m == 0) {
                    acc_buf[row_number] = acc;
                    row_number++;
                    acc = 0;
                    col = 0;
                    if (row_number >= block_size) {
                        output.write((char *)acc_buf.data(), block_size);
                        row_number = 0;
                    }
                } else {
                    col++;
                }
            }
        }
    } else {
        for (int i = 0; i < n; i++) {

            int byte_processed = 0;
            acc = 0;

            while (byte_processed < m) {
                int to_read = block_size;
                if (byte_processed + block_size > m) {
                    to_read = m - byte_processed;
                }

                input.seekg(8 + i * m + byte_processed);
                input.read((char *)row.data(), to_read);

                input.seekg(8 + m * n + byte_processed);
                input.read((char *)v.data(), to_read);
                byte_processed += to_read;

                for (int i = 0; i < to_read; i++) {
                    acc += v[i] * row[i];
                }
            }


            acc_buf[row_number] = acc;
            row_number += 1;

            if (row_number >= block_size) {
                output.write((char *)acc_buf.data(), block_size);
                row_number = 0;
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