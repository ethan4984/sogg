#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

namespace mnist {

constexpr size_t label_signature = 0x801;
constexpr size_t image_signature = 0x803;

struct [[gnu::packed]] image_set_hdr {
    uint32_t magic_number;
    uint32_t image_cnt;
    uint32_t row_cnt;
    uint32_t col_cnt;
};

struct [[gnu::packed]] label_set_hdr {
    uint32_t magic_number;
    uint32_t item_cnt;
};

struct image {
    uint8_t *data;
    size_t size;
    size_t col_cnt;
    size_t row_cnt;

    ~image() {
        delete data;
    }
};

class image_set {
public:
    image_set(std::string &path);
    image_set() = default;

    image operator[] (size_t index);

    image_set_hdr hdr;
private:
    std::ifstream file;
};

image image_set::operator[] (size_t index) {
    image ret;

    ret.col_cnt = hdr.col_cnt;
    ret.row_cnt = hdr.row_cnt;
    ret.size = hdr.col_cnt * hdr.row_cnt;
    ret.data = new uint8_t[ret.size];

    size_t position = sizeof(image_set_hdr) + index * ret.size;

    file.seekg(position);
    file.read(reinterpret_cast<char*>(ret.data), ret.size);

    return ret;
}

image_set::image_set(std::string &path) : file(path) {
    if(!file.is_open())
        return;

    file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));

    hdr.magic_number = __builtin_bswap32(hdr.magic_number);
    hdr.image_cnt = __builtin_bswap32(hdr.image_cnt);
    hdr.row_cnt = __builtin_bswap32(hdr.row_cnt);
    hdr.col_cnt = __builtin_bswap32(hdr.col_cnt);

    if(hdr.magic_number != image_signature) {
        std::cout << "image set: invalid magic number" << std::endl;
        exit(-1);
    }
}

class label_set {
public:
    label_set(std::string &path);
    label_set() = default;

    uint8_t operator[] (size_t index);

    label_set_hdr hdr;
private:
    std::ifstream file;
};

uint8_t label_set::operator[] (size_t index) {
    file.seekg(sizeof(label_set_hdr) + index);

    uint8_t label;
    file.read(reinterpret_cast<char*>(&label), 1);

    return label;
}

label_set::label_set(std::string &path) : file(path) {
    if(!file.is_open())
        return;

    file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));

    hdr.magic_number = __builtin_bswap32(hdr.magic_number);
    hdr.item_cnt = __builtin_bswap32(hdr.item_cnt);

    if(hdr.magic_number != label_signature) {
        std::cout << "label set: invalid magic number" << std::endl;
        exit(-1);
    }
}

}

namespace knn {

size_t dist(mnist::image &&train_image, mnist::image &image) {
    size_t sum = 0;

    for(size_t j = 0; j < train_image.size; j++) {
        sum += pow(train_image.data[j] - image.data[j], 2);
    }

    return sqrt(sum);
}

std::vector<std::pair<size_t, size_t>> find_all_distances(mnist::image_set &image_set, mnist::image &image) {
    std::vector<std::pair<size_t, size_t>> ret;

    for(size_t i = 0; i < image_set.hdr.image_cnt; i++) {
        ret.push_back(std::make_pair(dist(image_set[i], image), i));
    }

    return ret;
}

void run(mnist::image_set &train_set, mnist::label_set &label_set, mnist::image_set &image_set, mnist::label_set &label_test, size_t k = 3) {
    for(size_t i = 0; i < image_set.hdr.image_cnt; i++) {
        mnist::image image = image_set[i];

        std::vector<std::pair<size_t, size_t>> distances = find_all_distances(train_set, image);
        std::sort(distances.begin(), distances.end());

        auto prediction = [&]() {
            std::map<size_t, size_t> cnt;

            for(size_t j = 0; j < k; j++) {
                ++cnt[label_set[std::get<1>(distances[i])]];
            }

            return std::max_element(cnt.begin(), cnt.end(),
                    [] (const std::pair<size_t, size_t> &obj0, const std::pair<size_t, size_t> &obj1) -> bool {
                        return obj0.second < obj1.second;
                    });
        } ();

        std::cout << "Predicting " << prediction->first << ", answer: " << unsigned(label_test[i]) << std::endl;
    }
}

}

int main(int argc, char *argv[]) {
    if(argc < 5) {
        std::cout << "Failure to input paths" << std::endl;
        exit(-1);
    }

    std::string test_image_path = argv[1];
    std::string test_label_path = argv[2];
    std::string train_image_path = argv[3];
    std::string train_label_path = argv[4];

    mnist::image_set test_image_set(test_image_path);
    mnist::label_set test_label_set(test_label_path);
    mnist::image_set image_set(train_image_path);
    mnist::label_set label_set(train_label_path);

    knn::run(image_set, label_set, test_image_set, test_label_set);
}
